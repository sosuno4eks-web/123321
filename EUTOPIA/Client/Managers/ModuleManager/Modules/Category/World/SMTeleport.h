#pragma once
#include "../../ModuleBase/Module.h"

class SMTeleport : public Module {
   private:
    float speed = 7.5f;
    float arrivalThreshold = 1.0f;
    bool timerSet = false;

    Vec3<float> targetPos = Vec3<float>(0.f, 0.f, 0.f);

   public:
    SMTeleport()
        : Module("SMTeleport", "Smoothly teleports to a target using setPos()", Category::WORLD) {
        registerSetting(
            new SliderSetting<float>("Zoomy", "Movement speed", &speed, 1.5f, 0.1f, 3.f));
        registerSetting(new SliderSetting<float>("Threshold", "Arrival distance", &arrivalThreshold,
                                                 1.0f, 0.1f, 5.0f));
    }

    void onEnable() override {
        auto player = Game.getLocalPlayer();
        if(player)
            player->displayClientMessage("Use the command .setPos <x> <y> <z> to set a target!");

        Minecraft* mc = Game.getClientInstance()->minecraft;
        if(mc != nullptr) {
            *mc->minecraftTimer = 725000.f;
            *mc->minecraftRenderTimer = 725000.f;
            timerSet = true;
        }
    }

    void onDisable() override {
        Minecraft* mc = Game.getClientInstance()->minecraft;
        if(mc != nullptr && timerSet) {
            *mc->minecraftTimer = 20.f;
            *mc->minecraftRenderTimer = 20.f;
        }
        timerSet = false;

        auto player = Game.getLocalPlayer();
        if(player)
            player->stateVector->mVelocity = Vec3<float>(0.f, 0.f, 0.f);
    }

    void onNormalTick(LocalPlayer* localPlayer) override {
        auto player = Game.getLocalPlayer();
        if(!player)
            return;

        player->getStateVectorComponent()->mVelocity = Vec3<float>(0.f, 0.f, 0.f);
        Vec3<float> pos = player->getPos();
        Vec3<float> delta = targetPos.sub(pos);

        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        if(distance < arrivalThreshold) {
            player->displayClientMessage("Arrived near target!");
            setEnabled(false);
            return;
        }
        // player->getStateVectorComponent()->mVelocity

        float invDist = 1.0f / distance;
        Vec3<float> dir(delta.x * invDist, delta.y * invDist, delta.z * invDist);

        Vec3<float> newPos(pos.x + dir.x * speed, pos.y + dir.y * speed, pos.z + dir.z * speed);
        player->teleportTo(newPos, true, 1, 1, true);
    }

    void setTargetPos(Vec3<float> pos) {
        targetPos = pos;
        auto player = Game.getLocalPlayer();
        if(player) {
            std::ostringstream ss;
            ss << "Target position set to: X" << pos.x << " Y" << pos.y << " Z" << pos.z;
            player->displayClientMessage(ss.str());
        }
    }
};
