#define NOMINMAX
#include "TpMine.h"
// zep fix fixed by dani lol
#include <cfloat>
#include <cmath>

bool lapis = false;
bool AntiSuffocation = false;

static inline int max_i(int a, int b) {
    return a > b ? a : b;
}

struct IntList {
    int data[64];
    int n;
    IntList() : n(0) {}
    void push(int v) {
        if(n < 64)
            data[n++] = v;
    }
    bool contains(int v) const {
        for(int i = 0; i < n; ++i)
            if(data[i] == v)
                return true;
        return false;
    }
};

TpMine::TpMine() : Module("TpMine", "Teleport to ores", Category::WORLD) {
    registerSetting(new SliderSetting<int>("Range", "Search range for ores", &range, 30, 3, 300));
    registerSetting(new BoolSetting("AntiGravel", "Avoid ores under gravel", &antiGravel, false));
    registerSetting(new BoolSetting("AntiBorder", "Avoid ores near borders", &antiBorder, false));
    registerSetting(new BoolSetting("AntiLava", "Avoid ores near lava", &antiLava, true));
    registerSetting(new BoolSetting("Diamond", "Target Diamond ore", &dim, false));
    registerSetting(new BoolSetting("Iron", "Target Iron ore", &iron, false));
    registerSetting(new BoolSetting("Coal", "Target Coal ore", &coal, false));
    registerSetting(new BoolSetting("Gold", "Target Gold ore", &gold, false));
    registerSetting(new BoolSetting("Emerald", "Target Emerald ore", &emerald, false));
    registerSetting(new BoolSetting("AncientDebris", "Target Ancient Debris", &ancient, false));
    registerSetting(new BoolSetting("Redstone", "Target Redstone ore", &redstone, false));
    registerSetting(new BoolSetting("Quartz", "Target Quartz ore", &quartz, false));
    registerSetting(new BoolSetting("Lapis", "Target Lapis ore", &lapis, false));
    registerSetting(new BoolSetting("Spawners", "Target Spawners", &spawners, false));
    registerSetting(new BoolSetting("Custom", "Enable custom block ID", &cuOre, false));
    registerSetting(new SliderSetting<int>("CustomID", "Custom Block ID", &cuID, 1, 0, 252));
    registerSetting(new SliderSetting<int>("Vein Size", "Min cluster size", &clusterSize, 1, 1, 9));
}

void TpMine::onEnable() {
    GI::DisplayClientMessage("%s[TpMine] Enabled", MCTF::WHITE);
    tpdone = false;
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer)
        return;
    BlockSource* region = GI::getClientInstance()->getRegion();
    if(!region)
        return;
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
    IntList targetOres;
    if(coal)
        targetOres.push(16);
    if(iron)
        targetOres.push(15);
    if(gold)
        targetOres.push(14);
    if(redstone)
        targetOres.push(74);
    if(dim)
        targetOres.push(56);
    if(emerald)
        targetOres.push(129);
    if(ancient)
        targetOres.push(526);
    if(quartz)
        targetOres.push(153);
    if(lapis)
        targetOres.push(21);
    if(spawners)
        targetOres.push(52);
    if(cuOre)
        targetOres.push(cuID);
    for(int x = minX; x <= maxX; x++) {
        for(int y = 5; y <= 128; y++) {
            for(int z = minZ; z <= maxZ; z++) {
                BlockPos blockPos(x, y, z);
                Block* block = region->getBlock(blockPos);
                if(!block || !block->blockLegacy)
                    continue;
                int blockId = block->blockLegacy->blockId;
                if(!targetOres.contains(blockId))
                    continue;
                int clusterCount = 0;
                bool clusterInvalid = false;
                for(int dx = -1; dx <= 1 && !clusterInvalid; dx++) {
                    for(int dy = -1; dy <= 1 && !clusterInvalid; dy++) {
                        for(int dz = -1; dz <= 1 && !clusterInvalid; dz++) {
                            BlockPos neighborPos(x + dx, y + dy, z + dz);
                            Block* neighborBlock = region->getBlock(neighborPos);
                            if(!neighborBlock || !neighborBlock->blockLegacy)
                                continue;
                            if(neighborBlock->blockLegacy->blockId != blockId)
                                continue;
                            clusterCount++;
                            if(antiGravel) {
                                Block* aboveBlock = region->getBlock(
                                    BlockPos(neighborPos.x, neighborPos.y + 1, neighborPos.z));
                                if(aboveBlock && aboveBlock->blockLegacy &&
                                   aboveBlock->blockLegacy->blockId == 13) {
                                    clusterInvalid = true;
                                    break;
                                }
                            }
                            if(antiBorder) {
                                if(neighborPos.x > 1090 || neighborPos.x < 50 ||
                                   neighborPos.z > 1090 || neighborPos.z < 50) {
                                    clusterInvalid = true;
                                    break;
                                }
                            }
                            if(antiLava) {
                                for(int lx = -10; lx <= 10 && !clusterInvalid; lx++) {
                                    for(int ly = -10; ly <= 10 && !clusterInvalid; ly++) {
                                        for(int lz = -10; lz <= 10 && !clusterInvalid; lz++) {
                                            BlockPos lp = neighborPos.add2(lx, ly, lz);
                                            Block* lb = region->getBlock(lp);
                                            if(lb && lb->blockLegacy) {
                                                int lavaId = lb->blockLegacy->blockId;
                                                if(lavaId == 10 || lavaId == 11) {
                                                    clusterInvalid = true;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if(clusterInvalid || clusterCount < clusterSize)
                    continue;
                float dist = playerPos.dist(Vec3<float>(x, y, z));
                if(dist < closestDistance) {
                    closestDistance = dist;
                    goal = Vec3<float>(x + 0.5f, y - 2.f, z + 0.5f);
                }
            }
        }
    }
    if(closestDistance < FLT_MAX) {
        works = true;
        GI::DisplayClientMessage("%s[TpMine] Found block", MCTF::WHITE);
    } else {
        GI::DisplayClientMessage("%s[TpMine] No ores found", MCTF::RED);
        this->setEnabled(false);
        Minecraft* mc2 = GI::getClientInstance()->minecraft;
        *mc2->minecraftTimer = 20.0f;
    }
}

void TpMine::onNormalTick(LocalPlayer* actor) {
    LocalPlayer* lp = GI::getLocalPlayer();
    if(!lp || !works)
        return;
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
                if((id == 10 || id == 11) && antiLava)
                    return true;
            }
        }
        return false;
    };
    static float lastD = 1e9f;
    static int stuckTicks = 0;
    static int lifeTicks = 0;
    Vec3<float> cur = lp->getPos();
    float d = cur.dist(goal);
    if(d <= 1.2f) {
        Vec3<float> tp = isAir(goal) ? goal : Vec3<float>(goal.x, goal.y + 1.0f, goal.z);
        lp->teleportTo(tp, false, 0, 0, false);
        this->setEnabled(false);
        return;
    }
    bool mid = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
    float step = (d <= 2.0f)     ? 0.9f
                 : (d <= 13.0f)  ? 0.55f
                 : (d <= 20.0f)  ? 0.4f
                 : (d <= 100.0f) ? 0.09f
                                 : 0.06f;
    if(mid && midClickTP)
        step = 0.35f;
    Vec3<float> next = cur.lerpTo(goal, step);
    if(next.dist(goal) <= 1.2f) {
        if(pathLava(cur, goal)) {
            GI::DisplayClientMessage("%s[TpMine] Lava detected", MCTF::RED);
            this->setEnabled(false);
            return;
        }
        Vec3<float> tp = isAir(goal) ? goal : Vec3<float>(goal.x, goal.y + 1.0f, goal.z);
        lp->teleportTo(tp, false, 0, 0, false);
        this->setEnabled(false);
        return;
    }
    if(pathLava(cur, next)) {
        GI::DisplayClientMessage("%s[TpMine] Lava detected", MCTF::RED);
        this->setEnabled(false);
        return;
    }
    lp->teleportTo(next, false, 0, 0, false);
    if(d >= lastD - 0.01f)
        ++stuckTicks;
    else
        stuckTicks = 0;
    lastD = d;
    if(++lifeTicks >= 240 || stuckTicks >= 10) {
        this->setEnabled(false);
        return;
    }
}

void TpMine::onDisable() {
    tpdone = true;
    Minecraft* mc = GI::getClientInstance()->minecraft;
    *mc->minecraftTimer = 20.0f;
    GI::DisplayClientMessage("%s[TpMine] Disabled", MCTF::WHITE);
}