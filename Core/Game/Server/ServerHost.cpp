#include <wangle/bootstrap/ServerBootstrap.h>
#include <wangle/channel/OutputBufferingHandler.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/codec/ByteToMessageDecoder.h>
#include <cstring>
#include "Network/Packet.h"
#include "Network/VarIntHelper.h"
#include "Network/PacketMultiplexer.h"

namespace Game::Server {
    using namespace folly;
    using namespace wangle;
    using Network::Packet;
    using Network::VarIntHelper;

    using PacketPipeline = Pipeline<IOBufQueue&, Packet>;

    class FrameDecoder : public ByteToMessageDecoder<Packet> {
    public:
        bool decode(Context* ctx, IOBufQueue& buf, Packet& packet, size_t&) override {
            auto front = buf.front();
            if (!front) return false;
            io::Cursor c(front);
            int payloadLen;
            auto bytes = c.peekBytes();
            auto offset = bytes.data();
            if (VarIntHelper::TryLoadAdv(offset, offset+bytes.size(), payloadLen)) {
                const auto vSize = offset-bytes.data();
                if (buf.chainLength()>=(vSize+payloadLen)) {
                    buf.trimStart(vSize);
                    packet = Coalesce(buf.front(), payloadLen);
                    buf.trimStart(payloadLen);
                    return true;
                }
            }
            return false;
        }
    private:
        static Packet Coalesce(const IOBuf* buf, const int payloadLen) {
            Packet out = Packet(Packet(payloadLen+8), payloadLen);
            auto done = 0;
            while (done<payloadLen) {
                const auto l = buf->length();
                std::memcpy(out.Data()+done, buf->data(), l);
                done += l;
            }
            return out;
        }
    };

    class PacketHandler : public Handler<Packet, int, Packet, std::unique_ptr<IOBuf>>,
                          public Network::PacketMultiplexer {
    public:
        using Context = typename Handler<Packet, int, Packet, std::unique_ptr<IOBuf>>::Context;

        explicit PacketHandler(PacketPipeline::Ptr pipeline)
                :mPipeline(std::move(pipeline)) {
            mPipeline->setOwner(this);
        }

        void read(Context* ctx, Packet buf) override { Inbound(std::move(buf)); }

        Future<Unit> write(Context* ctx, Packet msg) override {
            const auto vSize = VarIntHelper::GetSize(msg.Size());
            // although we can eliminate this, but it is not trivial
            auto buf = IOBuf::copyBuffer(msg.Data(), msg.Size(), vSize);
            buf->prepend(vSize);
            VarIntHelper::Write(buf->writableData(), msg.Size());
            return ctx->fireWrite(std::move(buf));
        }

        void Outbound(Packet&& packet) override {
            write(getContext(), std::move(packet)).thenError([this](Future<Unit> fut) {
                mPipeline->close();
            });
        }
    private:
        PacketPipeline::Ptr mPipeline;
    };

    // where we define the chain of handlers for each message received
    class PacketPipelineFactory : public PipelineFactory<PacketPipeline> {
    public:
        PacketPipeline::Ptr newPipeline(std::shared_ptr<AsyncTransportWrapper> sock) override {
            auto pipeline = PacketPipeline::create();
            pipeline->addBack(AsyncSocketHandler(std::move(sock)));
            pipeline->addBack(OutputBufferingHandler());
            pipeline->addBack(FrameDecoder());
            pipeline->addBack(PacketHandler(pipeline));
            pipeline->finalize();
            return pipeline;
        }
    };
}