#include "ColorsMod.h"
#include <Windows.h>
#include <psapi.h>
#include <wininet.h>

#include <algorithm>
#include <string>
#include "../../Utils/TimerUtil.h"

std::string fart(const std::string& url) {
    std::string content;
    HINTERNET hInternet = InternetOpenA("Fetcher", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if(!hInternet)
        return content;

    HINTERNET hFile = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if(!hFile) {
        InternetCloseHandle(hInternet);
        return content;
    }

    char buffer[1024];
    DWORD bytesRead = 0;
    while(InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        content.append(buffer, bytesRead);
    }

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);

    return content;
}

Colors* g_Colors = new Colors();

static std::string previousContent = "";
static std::chrono::steady_clock::time_point lastCheckTime = std::chrono::steady_clock::now();
const std::chrono::minutes checkInterval(3); 

void checkForUpdate() {
 
    auto now = std::chrono::steady_clock::now();


    if(std::chrono::duration_cast<std::chrono::minutes>(now - lastCheckTime) >= checkInterval) {
        std::string url = "https://pastebin.com/raw/zQysmDYy";
        std::string content = fart(url);
        std::transform(content.begin(), content.end(), content.begin(), ::tolower);

        if(content != previousContent) {
            if(content.find("update") != std::string::npos) {
                GI::DisplayClientMessage(
                    "Your version of boost is out of date! Please download the latest version at "
                    "discord.gg/boostv2.");
            }
            previousContent = content;
        }


        lastCheckTime = now;
    }
}

Colors::Colors() : Module("Colors", "change the color of the client.", Category::CLIENT) {
    registerSetting(new EnumSetting("Mode", "Color mode", {"Single", "RGB"}, &this->modeValue, 0));
    registerSetting(new SliderSetting<float>("Brightness", "controls rainbow brightness",
                                             &this->brightness, 1.f, 0.f, 1.f));
    registerSetting(
        new SliderSetting<int>("Separation", "changes separation.", &separation, 125, 0, 255));
    this->mainColor = UIColor(50, 205, 50, 255);
    this->midColor = UIColor(0, 255, 255, 255);
    registerSetting(new ColorSetting("Start color", "used in Single mode", &this->mainColor,
                                     this->mainColor, true));
}



bool Colors::isEnabled() {
    return true;
}
void Colors::setEnabled(bool enabled) {}
bool Colors::isVisible() {
    return false;
}
UIColor Colors::getColor(int index) const {
    float time = TimerUtil::getCurrentMs() / 1000.0f;
    switch(this->modeValue) {
        case 0:
            return this->mainColor;
        case 1:
            return ColorUtil::getRainbowColor2(5.f, 1.f, brightness, index);
        default:
            return this->mainColor;
    }
}
void Colors::onNormalTick(LocalPlayer* localPlayer) {
    checkForUpdate();
}
