#include "Network/ProtocolBase.h"
#include "LoginPackets.g.h"

namespace Game::Network::Protocol::LoginPackets {
    void LoginStart::Process(States::StateBase& state) {}

    void EncryptionResponse::Process(States::StateBase& state) {}

    void Plugin::Process(States::StateBase& state) {}

    void EncryptionRequest::Process(States::StateBase& state) {}

    void LoginSuccess::Process(States::StateBase& state) {}

    void SetCompression::Process(States::StateBase& state) {}

    void RequestPlugin::Process(States::StateBase& state) {}
}