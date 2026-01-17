#pragma once
#include "../../ModuleBase/Module.h"

class NameTags : public Module {
   private:
    float opacity = 0.4f;
    bool showPops = false;
    bool showItems = true;
    bool showDura = true;
    bool showSelf = true;
    int underlineMode = 0;
    bool dynamicSize = false;
    float tagSize = 0.70f;
    bool showHealthBar = true;
    bool showAbsorption = true;
    UIColor friendColor = (0, 255, 255, 255);

   public:
    NameTags();
    virtual void onD2DRender() override;
    virtual void onMCRender(MinecraftUIRenderContext* renderCtx) override;
};