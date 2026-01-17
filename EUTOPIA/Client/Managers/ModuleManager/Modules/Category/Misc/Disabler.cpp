#include "Disabler.h"
#include <../SDK/NetWork/Packets/PlayerAuthInputPacket.h>
#include <Windows.h>  



Disabler::Disabler() : Module("DisablerLB", "Disable the anticheat", Category::MISC) {
    registerSetting(new EnumSetting("Mode", "change mode", {"SML", "SM", "Skywars", "SMNew"}, &Mode, 0));
}

std::string Disabler::getModeText() {
    static char textStr[15];
    sprintf_s(textStr, 15, "Lifeboat");
    return std::string(textStr);
}

static __int64 ms;
static DWORD lastMS = GetTickCount();
static __int64 timeMS = -1;
static DWORD getCurrentMs() {
    return GetTickCount();
}

static __int64 getElapsedTime() {
    return getCurrentMs() - ms;
}

static void resetTime() {
    lastMS = getCurrentMs();
    timeMS = getCurrentMs();
}

static bool hasTimedElapsed(__int64 time, bool reset) {
    if(getCurrentMs() - lastMS > time) {
        if(reset)
            resetTime();
        return true;
    }
    return false;
}

void Disabler::onSendPacket(Packet* packet) {
    if(Mode == 0 &&
       (packet->getName() == "PlayerAuthInputPacket" || packet->getName() == "MovePlayerPacket")) {
        auto* paip = (PlayerAuthInputPacket*)packet;
        auto* mpp = (MovePlayerPacket*)packet;
        if(paip) {
            float perc = static_cast<float>(paip->mClientTick % 3) / 3.0f;
            float targetY = (perc < 0.5f) ? 0.02f : -0.02f;
            paip->mPos.y = Math::lerp(paip->mPos.y, paip->mPos.y + targetY, perc);
            paip->mMove.y = -(1.0f / 3.0f);
            if(paip->mClientTick % 3 == 0) {
                paip->mInputData |= static_cast<AuthInputAction>(
                    1ULL << static_cast<int>(PlayerAuthInputPacket::InputData::StartJumping));
            }
            paip->mInputData |= static_cast<AuthInputAction>(
                1ULL << static_cast<int>(PlayerAuthInputPacket::InputData::Jumping));
        }
        if(mpp) {
            float perc = static_cast<float>(mpp->mTick % 3) / 3.0f;
            float targetY = (perc < 0.5f) ? 0.02f : -0.02f;
            mpp->mPos.y = Math::lerp(mpp->mPos.y, mpp->mPos.y + targetY, perc);
            mpp->mOnGround = true;
        }
    }
    if(Mode == 1 &&
       (packet->getName() == "PlayerAuthInputPacket" || packet->getName() == "MovePlayerPacket")) {
        auto* paip = (PlayerAuthInputPacket*)packet;
        auto* mpp = (MovePlayerPacket*)packet;
        mpp->mTeleportTick = 0;
        mpp->mRuntimeId = 0;
        paip->TicksAlive = 0;
    }

    if(Mode == 2 &&
       (packet->getName() == "PlayerAuthInputPacket" || packet->getName() == "MovePlayerPacket")) {
        auto* paip = (PlayerAuthInputPacket*)packet;
        auto* mpp = (MovePlayerPacket*)packet;
        mpp->mTeleportTick = 0;
        mpp->mRuntimeId = 0;
        paip->TicksAlive = 0;
    }

    if(Mode == 3 &&
       (packet->getName() == "PlayerAuthInputPacket" || packet->getName() == "MovePlayerPacket")) {
        auto* paip = (PlayerAuthInputPacket*)packet;
        auto* mpp = (MovePlayerPacket*)packet;
        if(paip) {
            float phase = static_cast<float>(paip->mClientTick % 4) / 4.0f;
            float offset = (phase < 0.5f) ? 0.025f : -0.025f;
            paip->mPos.y += offset * phase;
            paip->mMove.y = -(1.0f / 4.0f);
            if(paip->TicksAlive % 4 == 0) {
                paip->mInputData |= static_cast<AuthInputAction>(
                    1ULL << static_cast<int>(PlayerAuthInputPacket::InputData::StartJumping));
            }
        }
        if(mpp) {
            float phase = static_cast<float>(mpp->mTick % 4) / 4.0f;
            float offset = (phase < 0.5f) ? 0.025f : -0.025f;
            mpp->mPos.y += offset * phase;
            mpp->mOnGround = true;
            mpp->mTeleportTick = (mpp->mTick % 10 == 0) ? 0 : 1;
            mpp->mRuntimeId = (mpp->mTick % 5 == 0) ? 0 : mpp->mRuntimeId;
        }
    }
}
