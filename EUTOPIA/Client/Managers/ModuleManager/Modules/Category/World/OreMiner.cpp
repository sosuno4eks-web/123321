#pragma once
#include "OreMiner.h"

#include "../../../ModuleManager.h"
#include "../../ModuleBase/Module.h"
#include "../Player/PacketMine.h"

bool stone = false;
static bool quartz = false;
static bool lapis = false;

OreMiner::OreMiner() : Module("OreMiner", "Mines the ores around you", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "Scan radius", &range, 3, 1, 12));
    registerSetting(new BoolSetting("Diamond", "Break Diamond", &diamond, true));
    registerSetting(new BoolSetting("Iron", "Break Iron", &iron, false));
    registerSetting(new BoolSetting("Gold", "Break Gold", &gold, false));
    registerSetting(new BoolSetting("Coal", "Break Coal", &coal, false));
    registerSetting(new BoolSetting("Lapis", "Break Lapis", &lapis, false));
    registerSetting(new BoolSetting("Quartz", "Break Quartz", &quartz, false));
    registerSetting(new BoolSetting("Redstone", "Break Redstone", &redstone, false));
    registerSetting(new BoolSetting("Emerald", "Break Emerald", &emerald, false));
    registerSetting(new BoolSetting("AncientDebris", "Break Ancient Debris", &ancient, false));
}

void OreMiner::onEnable() {}

void OreMiner::onNormalTick(LocalPlayer* localPlayer) {
    if(!localPlayer)
        return;

    GameMode* gm = localPlayer->gamemode;
    if(!gm)
        return;

    PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
    if(!packetMine || !packetMine->isEnabled())
        packetMine->setEnabled(true);

    BlockSource* region = GI::getClientInstance()->getRegion();
    if(!region)
        return;

    Vec3<float> playerPos = localPlayer->getPos();
    BlockPos base((int)playerPos.x, (int)playerPos.y + 1, (int)playerPos.z);

    std::vector<int> ores;
    if(diamond)
        ores.push_back(56);
    if(iron)
        ores.push_back(15);
    if(gold)
        ores.push_back(14);
    if(coal)
        ores.push_back(16);
    if(quartz)
        ores.push_back(153);
    if(lapis)
        ores.push_back(21);
    if(redstone)
        ores.push_back(74);
    if(emerald)
        ores.push_back(129);
    if(ancient)
        ores.push_back(526);
    if(stone) {
        ores.insert(ores.end(), {1, 4,  48, 98,  113, 114, 155, 491, 486, 487, 112, 3,
                                 2, 12, 13, 243, 110, 87,  88,  89,  45,  103, 18});
    }

    float closestDist = FLT_MAX;
    BlockPos closestOre(0, 0, 0);
    for(int x = -range; x <= range; x++) {
        for(int y = -range; y <= range; y++) {
            for(int z = -range; z <= range; z++) {
                BlockPos pos = base.add2(x, y, z);
                Block* blk = region->getBlock(pos);
                if(!blk || !blk->blockLegacy)
                    continue;
                int id = blk->blockLegacy->blockId;
                if(std::find(ores.begin(), ores.end(), id) == ores.end())
                    continue;
                float dist = playerPos.dist(pos.CastTo<float>());
                if(dist < closestDist) {
                    closestDist = dist;
                    closestOre = pos;
                }
            }
        }
    }
    if(closestDist == FLT_MAX)
        return;

    Vec2<float> rot = playerPos.CalcAngle(closestOre.toFloat().add2(0.5f, 0.5f, 0.5f));

    auto* rotComp = localPlayer->getActorRotationComponent();
    auto* headComp = localPlayer->getActorHeadRotationComponent();
    auto* bodyComp = localPlayer->getMobBodyRotationComponent();
    if(rotComp) {
        rotComp->mYaw = rot.y;
        rotComp->mPitch = rot.x;
        rotComp->mOldYaw = rot.y;
        rotComp->mOldPitch = rot.x;
    }
    if(headComp)
        headComp->mHeadRot = rot.y;
    if(bodyComp)
        bodyComp->yBodyRot = rot.y;

    if(gm->destroyProgress <= 0.f || packetMine->getBreakPos() != closestOre) {
        bool b = false;
        gm->startDestroyBlock(closestOre, 0, b);
        packetMine->mineBlock(closestOre, 0);
    }
}

void OreMiner::onDisable() {
    PacketMine* packetMine = ModuleManager::getModule<PacketMine>();
    if(packetMine)
        packetMine->setEnabled(false);
}
