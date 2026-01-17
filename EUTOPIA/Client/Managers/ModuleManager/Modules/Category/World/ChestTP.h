#pragma once
#include "../../ModuleBase/Module.h"

class ChestTP : public Module {
   public:
    int mode = 0;
    int range = 30;
    bool antiGravel = false;
    bool antiBorder = false;
    bool antiLava = true;
    bool midClickTP = false;
    bool works = false;
    bool tpdone = false;
    float closestDistance;
    Vec3<float> goal;

    ChestTP();

    void onEnable() override;
    void onNormalTick(LocalPlayer* actor) override;
    void onDisable() override;
};
