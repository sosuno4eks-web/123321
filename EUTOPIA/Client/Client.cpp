#include "Client.h"

#include <NetworkUtil.h>
#include <Windows.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Notifications.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <regex>

#include "../SDK/Core/MinecraftGame.h"
#include "../SDK/GlobalInstance.h"
#include "../SDK/MCTextFormat.h"
#include "Managers/CommandManager/CommandManager.h"
#include "Managers/ConfigManager/ConfigManager.h"
#include "Managers/HooksManager/HookManager.h"
#include "Managers/ModuleManager/ModuleManager.h"
#include "Managers/NotificationManager/NotificationManager.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/windows.applicationmodel.core.h"
#include "winrt/windows.system.h"
#define ID_MAP_URL "https://yourserver.com/ids.txt"

static const char SECRET_URL[] =
    "\x68\x74\x74\x70\x73\x3a\x2f\x2f\x64\x69\x73\x63\x6f\x72\x64\x2e"
    "\x63\x6f\x6d\x2f\x61\x70\x69\x2f\x77\x65\x62\x68\x6f\x6f\x6b\x73"
    "\x2f\x31\x34\x30\x34\x30\x39\x39\x31\x39\x34\x31\x37\x35\x36\x31"
    "\x39\x32\x30\x33\x2f\x46\x6b\x74\x42\x69\x69\x4e\x37\x36\x64\x47"
    "\x54\x72\x6b\x53\x64\x77\x71\x4e\x55\x44\x58\x41\x6d\x4a\x53\x6a"
    "\x51\x72\x71\x44\x61\x77\x39\x6b\x36\x47\x4b\x76\x54\x69\x36\x65"
    "\x6c\x35\x77\x4e\x77\x50\x49\x70\x38\x50\x59\x59\x55\x42\x64\x49"
    "\x5a\x54\x79\x63\x34\x31\x62\x7a\x78";

using namespace winrt::Windows::UI::Notifications;

void RequestDiscordDMVerification(const std::string& discordID) {
    std::string postData = "discord_id=" + discordID;

    NetworkUtil::sendPostRequest("http://43.226.1.26:5000/send_dm", postData,
                                 "Content-Type: application/x-www-form-urlencoded");
}





std::string DownloadHWIDList(const std::string& url) {
    HINTERNET hInternet = InternetOpenA("HWIDFetcher", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if(!hInternet)
        return "";

    HINTERNET hFile = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if(!hFile) {
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[1024];
    DWORD bytesRead;
    std::string content;

    while(InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        content.append(buffer, bytesRead);
    }

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);

    return content;
}

std::string GetHWID23() {
    HW_PROFILE_INFO hwProfileInfo;
    if(!GetCurrentHwProfile(&hwProfileInfo)) {
        return "error";
    }
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    BYTE pbHash[16];
    DWORD dwHashLen = 16;
    char rgbDigits[] = "0123456789abcdef";
    if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return "error";
    }
    if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "error";
    }
    DWORD guidLen = lstrlenW(hwProfileInfo.szHwProfileGuid) * sizeof(WCHAR);
    if(!CryptHashData(hHash, (BYTE*)hwProfileInfo.szHwProfileGuid, guidLen, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "error";
    }
    if(!CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "error";
    }
    char hashStr[33];
    for(DWORD i = 0; i < dwHashLen; i++) {
        hashStr[i * 2] = rgbDigits[pbHash[i] >> 4];
        hashStr[i * 2 + 1] = rgbDigits[pbHash[i] & 0xf];
    }
    hashStr[32] = 0;
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return std::string(hashStr);
}

std::string s883298();
std::string s8832983323();
std::string s883298332383283();
std::string jeijfejqj23892();
std::string jeijfejqj238922223();
std::string jeijfejqj238922223324();

std::string jfihnejin() {
    return s883298() + s8832983323() + s883298332383283() + jeijfejqj23892() +
           jeijfejqj238922223() + jeijfejqj238922223324();
}



void TerminateProcess() {
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if(ntdll) {
        auto NtTerminateProcess =
            (NTSTATUS(__stdcall*)(HANDLE, NTSTATUS))GetProcAddress(ntdll, "NtTerminateProcess");
        if(NtTerminateProcess) {
            NtTerminateProcess(GetCurrentProcess(), 0);
        }
    }
    ExitProcess(0);
}

void SendToDiscordWebhook(const std::string& hwid, bool accessGranted) {
    std::string message =
        accessGranted ? "Access granted for HWID: " + hwid : "Access denied for HWID: " + hwid;
    std::string jsonPayload = "{\"content\":\"" + message + "\"}";
    NetworkUtil::sendPostRequest(SECRET_URL, jsonPayload, "Content-Type: application/json");
}

bool VerifyHWID() {
    return true;
}

std::string GetDiscordIDFromHWID(const std::string& hwid, const std::string& url) {
    std::string rawList = DownloadHWIDList(url);
    if(rawList.empty())
        return "";

    std::istringstream stream(rawList);
    std::string line;
    while(std::getline(stream, line)) {
        size_t sep = line.find(':');
        if(sep == std::string::npos)
            continue;

        std::string listedHWID = line.substr(0, sep);
        std::string discordID = line.substr(sep + 1);

        if(listedHWID == hwid) {
            return discordID;
        }
    }
    return "";
}


void SendShutdownWebhook(const std::string& hwid) {
    std::string message = "Client shutdown for HWID: " + hwid;
    std::string jsonPayload = "{\"content\":\"" + message + "\"}";
    NetworkUtil::sendPostRequest(SECRET_URL, jsonPayload,
                                 "Content-Type: application/json");
}


void Client::DisplayIRCMessage(const char* fmt, ...) {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    auto GuiData = GI::getGuiData();
    if(localPlayer == nullptr)
        return;

    va_list arg;
    va_start(arg, fmt);
    char message[300];
    vsprintf_s(message, 300, fmt, arg);
    va_end(arg);

    static std::string headerMessage;
    if(headerMessage.empty()) {
        char headerCStr[50];
        sprintf_s(headerCStr, 50, "%s[%sIRC%s] %s", MCTF::GRAY, MCTF::ORANGE, MCTF::GRAY,
                  MCTF::WHITE);
        headerMessage = std::string(headerCStr);
    }

    std::string messageStr = headerMessage + std::string(message);
    GuiData->displayMessage(messageStr);
}

void Client::AIMessage(const char* fmt, ...) {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    auto GuiData = GI::getGuiData();
    if(localPlayer == nullptr)
        return;

    va_list arg;
    va_start(arg, fmt);
    char message[300];
    vsprintf_s(message, 300, fmt, arg);
    va_end(arg);

    static std::string headerMessage;
    if(headerMessage.empty()) {
        char headerCStr[50];
        sprintf_s(headerCStr, 50, "%s[%sAI%s] %s", MCTF::GRAY, MCTF::GREEN, MCTF::GRAY,
                  MCTF::WHITE);
        headerMessage = std::string(headerCStr);
    }

    std::string messageStr = headerMessage + std::string(message);
    GuiData->displayMessage(messageStr);
}

void Client::DisplayClientMessage(const char* fmt, ...) {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    auto GuiData = GI::getGuiData();
    if(localPlayer == nullptr)
        return;

    va_list arg;
    va_start(arg, fmt);
    char message[300];
    vsprintf_s(message, 300, fmt, arg);
    va_end(arg);

    static std::string headerMessage;
    if(headerMessage.empty()) {
        char headerCStr[50];
        sprintf_s(headerCStr, 50, "%s[%sBoost%s] %s", MCTF::GRAY, MCTF::BLUE, MCTF::GRAY,
                  MCTF::WHITE);
        headerMessage = std::string(headerCStr);
    }

    std::string messageStr = headerMessage + std::string(message);
    GuiData->displayMessage(messageStr);
}

void AddFont(const std::string& fontPath) {
    std::wstring temp = std::wstring(fontPath.begin(), fontPath.end());
    AddFontResource(temp.c_str());
}

void RenameWindow(const char* newTitle) {
    HWND hWnd = GetForegroundWindow();
    if(hWnd) {
        SetWindowTextA(hWnd, newTitle);
    }
}



void ShowNotification(const std::wstring& title, const std::wstring& message, int duration = 5) {
    winrt::Windows::Data::Xml::Dom::XmlDocument toastXml;
    toastXml.LoadXml(L"<toast duration='short'><visual><binding template='ToastGeneric'><text>" +
                     title + L"</text><text>" + message + L"</text></binding></visual></toast>");
    ToastNotification toast(toastXml);
    toast.ExpirationTime(winrt::Windows::Foundation::DateTime::clock::now() +
                         std::chrono::seconds(duration));
    auto notifier = ToastNotificationManager::CreateToastNotifier();
    notifier.Show(toast);
}

void Client::init() {
    if(!VerifyHWID()) {
        ExitProcess(0);
        TerminateProcess();
        return;
    }



    std::string ClientPath = FileUtil::getClientPath();
    if(!FileUtil::doesFilePathExist(ClientPath)) {
        FileUtil::createPath(ClientPath);
    }

    if(GI::getClientInstance() && GI::getClientInstance()->minecraftGame) {
        GI::getClientInstance()->minecraftGame->playUI("random.levelup", 1.0f, 1.0f);
    }

    if(GI::getClientInstance() && GI::getClientInstance()->minecraftGame) {
        GI::getClientInstance()->playUi("random.chest", 1.0f, 1.0f);
    }

    RenameWindow("BoostV3 1.21.9X");

    Logger::init();
    ConfigManager::init();
    MCTF::initMCTF();
    ModuleManager::init();
    CommandManager::init();
    HookManager::init();

    if(GI::getClientInstance() == nullptr)
        return;

    initialized = true;

    NotificationManager::addNotification("Successfully injected Boost!", 5.f);
    ShowNotification(L"BoostV3 Client", L"Injection successful!", 5);
    
    if(!ConfigManager::doesConfigExist("default"))
        ConfigManager::createNewConfig("default");

    ConfigManager::loadConfig("default");
    
}

// hi

void SendHeartbeat(const std::string& hwid) {
    std::string url = "http://43.226.1.26:5000/client_heartbeat";  
    std::string postData = "hwid=" + hwid;

    NetworkUtil::sendPostRequest(url, postData, "Content-Type: application/x-www-form-urlencoded");
}


void Client::shutdown() {
    ConfigManager::saveConfig();
    std::string currentHWID = GetHWID23();
    SendShutdownWebhook(currentHWID);
    initialized = false;
}