#pragma once

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <functional>
#include <Cfx/Threading/Micro/Promise.h>
#include "Game/Client/player.h"
#include "Game/World/Chunk/Chunk.h"

namespace Game::SyncService {
    using WorldHdc = uintptr_t;
    using ChunkHdc = uintptr_t;
    using PlayerHdc = uintptr_t;
    using EntityHdc = uintptr_t;
    using TileEntityHdc = uintptr_t;

    struct IClientNoStateApi {
        [[nodiscard]] virtual Future<int> Ping() const = 0;
        [[nodiscard]] virtual Future<std::string> Description() const = 0;
        [[nodiscard]] virtual Future<std::vector<std::string>> GetPluginRequirementAndEnvJsons() const = 0;
    };

    struct WorldBasicInfo {
        int Tick, Day, DayTime;
        int ServerLoadDistance, MaxViewDistance, CurrentViewDistance, MaxTEDistance, CurrentTEDistance;
        int SkyLightPatternId;
        int CurrentSkyLight;
    };

    struct IClientApi {
        // Calls
        [[nodiscard]] virtual Future<PlayerHdc> Login(const std::string& token) = 0;
        [[nodiscard]] virtual Future<std::vector<std::pair<int, std::string>>> GetServerBlockPalette() = 0;
        [[nodiscard]] virtual Future<std::string> SyncAuxDataJson(const std::string& data) = 0;
        [[nodiscard]] virtual Future<std::vector<std::string>> SyncAuxUpdateSlot() = 0;
        [[nodiscard]] virtual Future<ChunkHdc> GetChunk(const Int3& position) = 0;
        virtual void AuxDataTx(int slot, uint8_t* data, int length) = 0;
        // Event Hooks
        virtual void SetOnAuxUpdateRx(std::function<void(int, uint8_t*, int)> function) = 0;
        virtual void SetOnPlayerJoinWorld(std::function<void(WorldHdc world)> function) = 0;
        virtual void SetOnPlayerLeaveWorld(std::function<void(WorldHdc world, bool isDisconnect)> function) = 0;
        // Visit Data Held By HDC
        // TODO: Use readonly const views instead of full object
        [[nodiscard]] virtual Player GetHdcPlayer(PlayerHdc hdc) = 0;
        [[nodiscard]] virtual Chunk& GetHdcChunk(ChunkHdc hdc) = 0;
        // Engine Synchronized Info
        [[nodiscard]] virtual PlayerHdc GetThisPlayer() const noexcept = 0;
        virtual void SetThisPlayerMovementCommand(const Double3& unitVec) noexcept = 0;
        virtual void SetThisPlayerViewVector(const Double3& unitVec) noexcept = 0;
        virtual void SetPlayerInteractionState(int button, bool down) noexcept = 0;
        // NOTE: The following two are tick volatile. Should always call get before use.
        [[nodiscard]] virtual Double3 GetThisPlayerMovementCommand() const noexcept = 0;
        [[nodiscard]] virtual Double3 GetThisPlayerViewVector() const noexcept = 0;
        // Basic World Info
        [[nodiscard]] virtual WorldHdc GetCurrentWorld() const noexcept = 0;
        // NOTE: The following one is tick volatile. Should always call get before use.
        [[nodiscard]] virtual const WorldBasicInfo& GetCurrentWorldBasicInfo() const noexcept = 0;
        // List Updated Entity (for evaluation A/V effects)
        [[nodiscard]] virtual std::vector<EntityHdc> PollUpdatedEntities() noexcept = 0;
        // List Updated Tile Entities in range
        [[nodiscard]] virtual std::vector<TileEntityHdc> PollUpdatedTileEntities() noexcept = 0;
        // Poll Updated Chunks for visual updates (updated means BlockState or Lighting update)
        [[nodiscard]] virtual std::vector<ChunkHdc> PoolUpdatedChunksSortedOfCount(int count) = 0;
    };
}
