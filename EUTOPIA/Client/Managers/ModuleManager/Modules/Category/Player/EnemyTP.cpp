#include "EnemyTP.h"
#include "../../../ModuleManager.h"
#include <Minecraft/TargetUtil.h>
//made by yo zeppo bitch
float Speed = 10.f;
EnemyTP::EnemyTP() : Module("EnemyTP", "Teleports you to Players", Category::PLAYER) {
    registerSetting(new BoolSetting("Use Blink", "Use teleportation blink", &us, false));
    registerSetting(new SliderSetting<float>("Range", "Teleportation search range", &range, 30.f, 1.f, 500.f));
    registerSetting(new SliderSetting<int>("Delay", "Delay between teleports", &delay, 5, 0, 50));
    registerSetting(new SliderSetting<float>("Y TP", "Y level offset for teleport", &y, 0.f, -6.f, 10.f));
    registerSetting(new SliderSetting<float>("Speed", "Speed Of the Teleport", &Speed, 10.f, 0.f, 10.f));
}


void EnemyTP::onEnable() {
    auto blink = ModuleManager::getModule<NoPacket>();
    auto player = Game.getLocalPlayer();
    if(us) {
        if(!blink->isEnabled())
            blink->setEnabled(true);
    }
    prevPos = player->getPos();
    auto as = ModuleManager::getModule<Timer>();
    if(!as->isEnabled())
        as->setEnabled(false);
}
bool sortBySex(Actor* a1, Actor* a2) {
    Vec3<float> lpPos = Game.getLocalPlayer()->getPos();
    return ((a1->getPos().dist(lpPos)) < (a2->getPos().dist(lpPos)));
}
void EnemyTP::onNormalTick(LocalPlayer* gm) {
    auto player = Game.getLocalPlayer();


    targetList69.clear();
    for(auto& entity : Game.getLocalPlayer()->level->getRuntimeActorList()) {
        if(TargetUtil::isTargetValid(entity, false)) {
            if(TargetUtil::sortByDist(Game.getLocalPlayer(), entity) <= range)
                targetList69.push_back(entity);
        }
    }

    auto blink = ModuleManager::getModule<NoPacket>();

    if(targetList69.empty()) {
        Ticks = 0;
        if(blink && blink->isEnabled())
            blink->setEnabled(false);
        Game.DisplayClientMessage("%s[EnemyTP] : %sWarning: %sTarget Not Detected within range",
                                  MCTF::RED, MCTF::RED, MCTF::GREEN);
        setEnabled(false);
        return;
    }

    std::sort(targetList69.begin(), targetList69.end(), sortBySex);

    Odelay++;
    Ticks++;
    if(Odelay < delay)
        return;

    Vec3<float> goal = targetList69[0]->getPos();
    goal.y += y;

    if((goal.x >= 1094) || (goal.z >= 1094) || (goal.x <= 55) || (goal.z <= 55)) {
        Game.DisplayClientMessage("[%sEnemyTP] : %sWarning: Border/Ghost target. Cancelling.",
                                  MCTF::RED, MCTF::GREEN);
        if(blink && blink->isEnabled())
            blink->setEnabled(false);
        setEnabled(false);
        return;
    }

    Vec3<float> cur = player->getPos();
    Vec3<float> delta = goal.sub(cur);
    float dist = cur.dist(goal);

    if(dist <= Speed) {
        player->teleportTo(goal, /*shouldStopRiding*/ true, /*a*/ 0,
                                                     /*b*/ 0, /*keepVelocity*/ false);
        Game.DisplayClientMessage("[%sEnemyTP] : %sTeleported to player", MCTF::RED,
                                  MCTF::GREEN);
        if(blink && blink->isEnabled())
            blink->setEnabled(false);
        setEnabled(false);
        Odelay = 0;
        return;
    } else {
        Vec3<float> step =
            Vec3<float>(delta.x / dist * Speed, delta.y / dist * Speed, delta.z / dist * Speed);
        Vec3<float> nextPos = cur.add(step);
        player->teleportTo(nextPos, /*shouldStopRiding*/ true, /*a*/ 0,
                                                     /*b*/ 0, /*keepVelocity*/ false);
        Odelay = delay; 
                        
    }
}

void EnemyTP::onDisable() {
    auto blink = ModuleManager::getModule<NoPacket>();
    Game.DisplayClientMessage(
        "[%sEnemyTP] : %sDisabled", MCTF::RED, MCTF::GREEN);
    Game.getClientInstance()->minecraft->minecraftTimer = (float*)20;
    Ticks = 0;
    if(blink->isEnabled())
        blink->setEnabled(false);
}