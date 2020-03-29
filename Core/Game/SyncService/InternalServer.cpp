#include "InternalServer.h"
#include "Game/World/world.h"

namespace  {
    using namespace Game::SyncService;

    class InternalServer : public IClientApi {
    public:
        [[nodiscard]] Future<PlayerHdc> Login(const std::string& token) override;
        [[nodiscard]] Future<std::vector<std::pair<int, std::string>>> GetServerBlockPalette() override;
        [[nodiscard]] Future<std::string> SyncAuxDataJson(const std::string& data) override;
        [[nodiscard]] Future<std::vector<std::string>> SyncAuxUpdateSlot() override;
        [[nodiscard]] Future<ChunkHdc> GetChunk(const Int3& position) override;
        void AuxDataTx(int slot, uint8_t* data, int length) override;
        void SetOnAuxUpdateRx(std::function<void(int, uint8_t*, int)> function) override;
        void SetOnPlayerJoinWorld(std::function<void(WorldHdc)> function) override;
        void SetOnPlayerLeaveWorld(std::function<void(WorldHdc, bool)> function) override;
        [[nodiscard]] Player GetHdcPlayer(PlayerHdc hdc) override;
        [[nodiscard]] Chunk& GetHdcChunk(ChunkHdc hdc) override;
        [[nodiscard]] PlayerHdc GetThisPlayer() const noexcept override;
        void SetThisPlayerMovementCommand(const Double3& unitVec) noexcept override;
        void SetThisPlayerViewVector(const Double3& unitVec) noexcept override;
        void SetPlayerInteractionState(int button, bool down) noexcept override;
        [[nodiscard]] Double3 GetThisPlayerMovementCommand() const noexcept override;
        [[nodiscard]] Double3 GetThisPlayerViewVector() const noexcept override;
        [[nodiscard]] WorldHdc GetCurrentWorld() const noexcept override;
        [[nodiscard]] const WorldBasicInfo& GetCurrentWorldBasicInfo() const noexcept override;
        [[nodiscard]] std::vector<EntityHdc> PollUpdatedEntities() noexcept override;
        [[nodiscard]]std::vector<TileEntityHdc> PollUpdatedTileEntities() noexcept override;
        [[nodiscard]]std::vector<ChunkHdc> PoolUpdatedChunksSortedOfCount(int count) override;
    private:
        std::vector<Player> mPlayers;
        WorldManager mWorlds;
    };

    Future<PlayerHdc> InternalServer::Login(const std::string& token) {
        // TODO: Properly Authorize the player based on the token
        return Future<PlayerHdc>();
    }

    Future<std::vector<std::pair<int, std::string>>> InternalServer::GetServerBlockPalette() {
        return Future<std::vector<std::pair<int, std::string>>>();
    }

    Future<std::string> InternalServer::SyncAuxDataJson(const std::string& data) {
        return Future<std::string>();
    }

    Future<std::vector<std::string>> InternalServer::SyncAuxUpdateSlot() {
        return Future<std::vector<std::string>>();
    }

    Future<ChunkHdc> InternalServer::GetChunk(const Int3& position) {
        return Future<ChunkHdc>();
    }

    void InternalServer::AuxDataTx(int slot, uint8_t* data, int length) {

    }

    void InternalServer::SetOnAuxUpdateRx(std::function<void(int, uint8_t*, int)> function) {

    }

    void InternalServer::SetOnPlayerJoinWorld(std::function<void(WorldHdc)> function) {

    }

    void InternalServer::SetOnPlayerLeaveWorld(std::function<void(WorldHdc, bool)> function) {

    }

    Player InternalServer::GetHdcPlayer(PlayerHdc hdc) {
        return Player(0);
    }

    Chunk& InternalServer::GetHdcChunk(ChunkHdc hdc) {
        throw std::runtime_error("not implemented");
    }

    PlayerHdc InternalServer::GetThisPlayer() const noexcept {
        return 0;
    }

    void InternalServer::SetThisPlayerMovementCommand(const Double3& unitVec) noexcept {

    }

    void InternalServer::SetThisPlayerViewVector(const Double3& unitVec) noexcept {

    }

    void InternalServer::SetPlayerInteractionState(int button, bool down) noexcept {

    }

    Double3 InternalServer::GetThisPlayerMovementCommand() const noexcept {
        return Double3();
    }

    Double3 InternalServer::GetThisPlayerViewVector() const noexcept {
        return Double3();
    }

    WorldHdc InternalServer::GetCurrentWorld() const noexcept {
        return 0;
    }

    const WorldBasicInfo& InternalServer::GetCurrentWorldBasicInfo() const noexcept {
        throw std::runtime_error("not implemented");
    }

    std::vector<EntityHdc> InternalServer::PollUpdatedEntities() noexcept {
        return std::vector<EntityHdc>();
    }

    std::vector<TileEntityHdc> InternalServer::PollUpdatedTileEntities() noexcept {
        return std::vector<TileEntityHdc>();
    }

    std::vector<ChunkHdc> InternalServer::PoolUpdatedChunksSortedOfCount(int count) {
        return std::vector<ChunkHdc>();
    }
}