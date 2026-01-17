#define NOMINMAX
#include "ChestTP.h"
#include <cfloat>
#include <cmath>
static inline int max_i(int a, int b) { return a > b ? a : b; }
bool lifeboat = false;
ChestTP::ChestTP() : Module("ChestTP", "Teleport to chests", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "Search range for chests", &range, 30, 3, 150));
    registerSetting(new BoolSetting("AntiBorder", "Avoid chests near borders", &antiBorder, false));
    registerSetting(new BoolSetting("AntiLava", "Avoid chests near lava", &antiLava, true));
    registerSetting(new BoolSetting("Lifeboat", "Adjusted for lifeboat", &lifeboat, false));
}
void ChestTP::onEnable() {
    works = false;
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer) return;
    BlockSource* region = GI::getClientInstance()->getRegion();
    if(!region) return;
    closestDistance = FLT_MAX;
    goal = Vec3<float>(0, 0, 0);
    Minecraft* mc = GI::getClientInstance()->minecraft;
    *mc->minecraftTimer = 10700.0f;
    Vec3<float> p = localPlayer->getPos();
    AABB tiny({p.x - 0.15f, p.y, p.z - 0.15f}, {p.x + 0.15f, p.y + 1.2f, p.z + 0.15f});
    localPlayer->setAABB(tiny);
    Vec3<float> playerPos = localPlayer->getPos();
    int minX = max_i((int)playerPos.x - range, 0);
    int maxX = (int)playerPos.x + range;
    int minZ = max_i((int)playerPos.z - range, 0);
    int maxZ = (int)playerPos.z + range;
    int startY = lifeboat ? 60 : 5;
    int endY = lifeboat ? 70 : 255;
    for(int x = minX; x <= maxX; x++) {
        for(int y = startY; y <= endY; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                BlockPos blockPos(x, y, z);
                Block* block = region->getBlock(blockPos);
                if(!block || !block->blockLegacy) continue;
                int id = block->blockLegacy->blockId;
                if(id != 54 && id != 130 && id != 146) continue;
                BlockPos abovePos(x, y + 1, z);
                Block* aboveBlock = region->getBlock(abovePos);
                if(!aboveBlock || !aboveBlock->blockLegacy || aboveBlock->blockLegacy->blockId != 0) continue;
                if(antiBorder) {
                    if(blockPos.x > 1090 || blockPos.x < 50 || blockPos.z > 1090 || blockPos.z < 50) continue;
                }
                if(antiLava) {
                    bool lavaNear = false;
                    for(int lx = -10; lx <= 10 && !lavaNear; lx++) {
                        for(int ly = -10; ly <= 10 && !lavaNear; ly++) {
                            for(int lz = -10; lz <= 10 && !lavaNear; lz++) {
                                BlockPos lp = blockPos.add2(lx, ly, lz);
                                Block* lb = region->getBlock(lp);
                                if(lb && lb->blockLegacy) {
                                    int lavaId = lb->blockLegacy->blockId;
                                    if(lavaId == 10 || lavaId == 11) { lavaNear = true; break; }
                                }
                            }
                        }
                    }
                    if(lavaNear) continue;
                }
                float dist = playerPos.dist(Vec3<float>(x, y, z));
                if(dist < closestDistance) {
                    closestDistance = dist;
                    goal = Vec3<float>(x + 0.5f, y + (lifeboat ? 0.f : -2.f), z + 0.5f);
                }
            }
        }
    }
    if(closestDistance < FLT_MAX) works = true; else { this->setEnabled(false); *mc->minecraftTimer = 20.0f; }
}
void ChestTP::onNormalTick(LocalPlayer* actor) {
    LocalPlayer* lp = GI::getLocalPlayer();
    if(!lp || !works) return;
    auto isAir = [&](const Vec3<float>& p) {
        BlockPos bp((int)p.x, (int)p.y, (int)p.z);
        Block* b = GI::getClientInstance()->getRegion()->getBlock(bp);
        return b && b->blockLegacy && b->blockLegacy->blockId == 0;
    };
    auto pathLava = [&](const Vec3<float>& a, const Vec3<float>& b) {
        int steps = 8;
        for(int i = 1; i <= steps; i++) {
            float t = i / (float)steps;
            Vec3<float> p = a.lerpTo(b, t);
            BlockPos bp((int)p.x, (int)p.y, (int)p.z);
            Block* bl = GI::getClientInstance()->getRegion()->getBlock(bp);
            if(bl && bl->blockLegacy) {
                int id = bl->blockLegacy->blockId;
                if((id == 10 || id == 11) && antiLava) return true;
            }
        }
        return false;
    };
    Vec3<float> cur = lp->getPos();
    float d = cur.dist(goal);
    if(d <= 1.2f) { Vec3<float> tp = isAir(goal) ? goal : Vec3<float>(goal.x, goal.y + 1.0f, goal.z); lp->teleportTo(tp, false, 0, 0, false); this->setEnabled(false); return; }
    float step = (d <= 2.0f) ? 0.95f : (d <= 13.0f) ? 0.8f : (d <= 20.0f) ? 0.7f : (d <= 100.0f) ? 0.3f : 0.25f;
    Vec3<float> next = cur.lerpTo(goal, step);
    if(pathLava(cur, next)) { this->setEnabled(false); return; }
    lp->teleportTo(next, false, 0, 0, false);
}
void ChestTP::onDisable() {
    Minecraft* mc = GI::getClientInstance()->minecraft;
    *mc->minecraftTimer = 20.0f;
}
