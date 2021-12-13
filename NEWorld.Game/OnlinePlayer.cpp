#include "OnlinePlayer.h"
#include "Renderer/Renderer.h"

std::map<SkinID, std::pair<VBOID, vtxCount>> playerSkins;
std::vector<OnlinePlayer> players;

void OnlinePlayer::GenVAOVBO(int skinID) {
    return;
}

void OnlinePlayer::buildRenderIfNeed() {
    if (VBO == 0 || vtxs == 0) {
        const auto iter = playerSkins.find(_skinID);
        if (iter != playerSkins.end()) {
            VBO = iter->second.first;
            vtxs = iter->second.second;
        } else {
            VBO = 0;
            vtxs = 0;
            GenVAOVBO(_skinID);
            playerSkins[_skinID] = std::make_pair(VBO, vtxs);
        }
    }
}

void OnlinePlayer::render() const {
    glDisable(GL_CULL_FACE);
    glNormal3f(0, 0, 0);
    glColor4f(1.0, 1.0, 1.0, 0.5);
    glBindTexture(GL_TEXTURE_2D, _skinID == 0 ? DefaultSkin : _skinID);
    glEnable(GL_CULL_FACE);
}
