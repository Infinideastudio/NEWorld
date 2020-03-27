#include "Network/ProtocolBase.h"
#include "LoginPackets.g.h"

namespace Game::Protocol::LoginPackets {
    void LoginStart::Process(Network::StateBase& state) {}

    void EncryptionResponse::Process(Network::StateBase& state) {}

    void Plugin::Process(Network::StateBase& state) {}

    void EncryptionRequest::Process(Network::StateBase& state) {}

    void LoginSuccess::Process(Network::StateBase& state) {}

    void SetCompression::Process(Network::StateBase& state) {}

    void RequestPlugin::Process(Network::StateBase& state) {}
}