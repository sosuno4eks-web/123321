#pragma once
#include "../../ModuleBase/Module.h"

class EnemyTP : public Module {
   public:
    float range = 20;
    int delay = 0;
    int Ticks = 0;
    Vec3<float> prevPos;
    bool eny = false;
    int Odelay = 0;
    float y = -1.f;
    bool us = true;
    bool works;
   std::vector<Actor*> targetList69;

    EnemyTP();
    virtual void onEnable() override;
    virtual void onDisable() override;
    virtual void onNormalTick(LocalPlayer* gm) override;
};