#include "SurfaceTP.h"

#include <limits>

#include "../../../../../Client.h"

SurfaceTP::SurfaceTP() : Module("SurfaceTP", "Teleport to the highest surface", Category::WORLD) {}

void SurfaceTP::onEnable() {
    Game.DisplayClientMessage("%s[Surface] Enabled", MCTF::GREEN);

    lerping = false;
    LocalPlayer* localPlayer = Game.getLocalPlayer();
    if(!localPlayer)
        return;

    BlockSource* region = Game.getClientInstance()->getRegion();
    if(!region)
        return;

    Vec3<float> playerPos = localPlayer->getPos();
    int highestY = -1;

    for(int y = 60; y <= 256; y++) {
        BlockPos basePos(playerPos.x, y, playerPos.z);
        Block* block = region->getBlock(basePos);
        if(!block || !block->blockLegacy)
            continue;

        int blockId = block->blockLegacy->blockId;
        if(blockId == 0 || (blockId >= 8 && blockId <= 11))  
            continue;

        if(!block->blockLegacy->mSolid)
            continue;

        Block* above1 = region->getBlock(BlockPos(playerPos.x, y + 1, playerPos.z));
        Block* above2 = region->getBlock(BlockPos(playerPos.x, y + 2, playerPos.z));
        Block* above3 = region->getBlock(BlockPos(playerPos.x, y + 3, playerPos.z));
        if(!above1 || !above2 || !above3)
            continue;

        if(above1->blockLegacy->blockId == 0 && above2->blockLegacy->blockId == 0 &&
           above3->blockLegacy->blockId == 0) {
            highestY = y;
        }
    }

    if(highestY != -1) {
        targetPos = Vec3<float>(playerPos.x + 0.5f, highestY + 1.0f, playerPos.z + 0.5f);
        lerping = true;
    } else {
        Game.DisplayClientMessage("%s[SurfaceTP] No valid surface found", MCTF::RED);
        this->setEnabled(false);
    }
}

void SurfaceTP::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer || !lerping)
        return;

    Vec3<float> currentPos = localPlayer->getPos();
    float distance = currentPos.dist(targetPos);

    float stepSize = 1.8f;  

    if(distance <= stepSize) {
        localPlayer->teleportTo(targetPos, true, 0, 0, false);
        Game.DisplayClientMessage("%s[Surface] Teleported to the top", MCTF::GREEN);
        lerping = false;
        this->setEnabled(false);
        return;
    }

    Vec3<float> delta = targetPos.sub(currentPos);
    Vec3<float> step(delta.x / distance * stepSize, delta.y / distance * stepSize,
                     delta.z / distance * stepSize);
    Vec3<float> nextPos = currentPos.add(step);

    localPlayer->teleportTo(nextPos, true, 0, 0, false);
}

void SurfaceTP::onDisable() {
    lerping = false;
    Game.DisplayClientMessage("%s[Surface] Disabled", MCTF::WHITE);
}
