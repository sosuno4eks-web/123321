#define NOMINMAX
#include "WaterTP.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

WaterTP::WaterTP() : Module("WaterTP", "", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "", &range, 30, 1, 300));
}

void WaterTP::onEnable() {
    tpdone = false;
    works = false;

    LocalPlayer* localPlayer = Game.getLocalPlayer();
    if(!localPlayer) {
        this->setEnabled(false);
        return;
    }

    BlockSource* region = Game.getClientInstance()->getRegion();
    if(!region) {
        this->setEnabled(false);
        return;
    }



    Vec3<float> playerPos = localPlayer->getPos();
    closestDistance = std::numeric_limits<float>::max();
    goal = Vec3<float>(0, 0, 0);

    int minX = std::max((int)playerPos.x - range, 0);
    int maxX = (int)playerPos.x + range;
    int minZ = std::max((int)playerPos.z - range, 0);
    int maxZ = (int)playerPos.z + range;

    for(int x = minX; x <= maxX; x++) {
        for(int y = 60; y <= 70; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                BlockPos pos(x, y, z);
                Block* block = region->getBlock(pos);

                if(!block || !block->blockLegacy ||
                   (block->blockLegacy->blockId != 8 && block->blockLegacy->blockId != 9))
                    continue;

                Block* above = region->getBlock(BlockPos(x, y + 1, z));
                if(above && above->blockLegacy && above->blockLegacy->blockId != 0)
                    continue;

                float dist = playerPos.dist(Vec3<float>(x, y, z));
                if(dist < closestDistance) {
                    closestDistance = dist;
                    goal = Vec3<float>(x + 0.5f, y + 1.0f, z + 0.5f);
                }
            }
        }
    }

    if(closestDistance < std::numeric_limits<float>::max()) {
        works = true;
    } else {
        this->setEnabled(false);
    }
}

void WaterTP::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer || !works || tpdone)
        return;

    Vec3<float> currentPos = localPlayer->getPos();
    float distance = currentPos.dist(goal);

    if(distance <= 0.25f) {
        tpdone = true;
        this->setEnabled(false);
        return;
    }

    float lerpFactor = 0.95f;
    Vec3<float> newPos = currentPos.lerpTo(goal, lerpFactor);
    Game.DisplayClientMessage(
        "%s[WaterTP] Moving towards goal with lerp factor %.2f. NewPos: (%.2f, %.2f, %.2f)",
        MCTF::WHITE, lerpFactor, newPos.x, newPos.y, newPos.z);
    localPlayer->teleportTo(newPos, true, 0, 0, false);
}

void WaterTP::onDisable() {
    tpdone = true;
}
