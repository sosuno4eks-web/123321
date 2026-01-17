#include <Windows.h>
#include <psapi.h>
#include <wininet.h>

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>

#include "../evhihewhq.h"
#include "Client/Client.h"
#include "Client/Managers/CommandManager/CommandManager.h"
#include "Client/Managers/HooksManager/HookManager.h"
#include "Client/Managers/ModuleManager/ModuleManager.h"
#include "Client/Managers/ModuleManager/Modules/Category/Client/SearchBox.h"
#include "Utils/RenderUtil.h"
#include "json.hpp"


std::string version = "v1";

using json = nlohmann::json;

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "psapi.lib")

CRITICAL_SECTION cs;

std::string parseSoundLink(const std::string& json) {
    const std::string key = "\"sound_link\"";
    size_t pos = json.find(key);
    if(pos == std::string::npos)
        return "";


    pos = json.find(':', pos);
    if(pos == std::string::npos)
        return "";


    pos = json.find('"', pos);
    if(pos == std::string::npos)
        return "";

    size_t start = pos + 1;
    size_t end = json.find('"', start);
    if(end == std::string::npos)
        return "";

    return json.substr(start, end - start);
}

static const std::string WEBHOOK_URL =
    "https://discord.com/api/webhooks/1404099194175619203/"
    "FktBiiN76dGTrkSdwqNUDXAmJSjQrqDaw9k6GKvTi6el5wNwPIp8PYYUBdIZTyc41bzx";


#include "../Libs\miniaudio\miniaudio.h"
#include "..\Utils\NetworkUtil.h"

void ma_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);
}


void downloadAndPlay(const std::string& url) {
    std::string data;
    if(!NetworkUtil::httpGet(url, data))
        return;

    std::string path = (std::filesystem::temp_directory_path() / "sound.mp3").string();
    std::ofstream f(path, std::ios::binary);
    f.write(data.data(), data.size());
    f.close();

    ma_result result;
    ma_decoder decoder;
    result = ma_decoder_init_file(path.c_str(), NULL, &decoder);
    if(result != MA_SUCCESS)
        return;

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = decoder.outputFormat;
    config.playback.channels = decoder.outputChannels;
    config.sampleRate = decoder.outputSampleRate;
    config.dataCallback = ma_data_callback;
    config.pUserData = &decoder;

    ma_device device;
    if(ma_device_init(NULL, &config, &device) != MA_SUCCESS ||
       ma_device_start(&device) != MA_SUCCESS) {
        ma_decoder_uninit(&decoder);
        return;
    }
}






std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

std::string fetchUrlContent(const std::string& url) {
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

std::thread soundThread([]() {
    std::string lastLink;
    while(true) {
        std::string resp = fetchUrlContent("http://43.226.1.26:5000/get_sound");
        resp = trim(resp);

        std::string soundLink = parseSoundLink(resp);

        if(!soundLink.empty() && soundLink != lastLink) {
            lastLink = soundLink;
            downloadAndPlay(soundLink);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
});

std::string makeEmbedPayload(const std::string& title, const std::string& description,
                             int color = 16711680) {
    return "{\"embeds\":[{\"title\":\"" + title + "\",\"description\":\"" + description +
           "\",\"color\":" + std::to_string(color) + "}]}";
}

void SendDiscordEmbed(const std::string& payloadJson) {
    HINTERNET hInternet = InternetOpenA("WebhookSender", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if(!hInternet)
        return;

    URL_COMPONENTSA urlComp{sizeof(urlComp)};
    char host[256] = {0}, path[2048] = {0};
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = _countof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = _countof(path);
    InternetCrackUrlA(WEBHOOK_URL.c_str(), 0, 0, &urlComp);

    HINTERNET hConnect = InternetConnectA(hInternet, host, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                                          INTERNET_SERVICE_HTTP, 0, 0);
    if(!hConnect) {
        InternetCloseHandle(hInternet);
        return;
    }

    HINTERNET hRequest =
        HttpOpenRequestA(hConnect, "POST", path, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if(!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    const std::string headers = "Content-Type: application/json\r\n";
    HttpSendRequestA(hRequest, headers.c_str(), -1, (LPVOID)payloadJson.c_str(),
                     static_cast<DWORD>(payloadJson.size()));

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}



std::string GetHWIDD() {
    HW_PROFILE_INFO hwProfileInfo;
    if(!GetCurrentHwProfile(&hwProfileInfo))
        return "error";

    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    BYTE pbHash[16];
    DWORD dwHashLen = 16;
    char rgbDigits[] = "0123456789abcdef";

    if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        return "error";
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

int GetLoadedModuleCount() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    HANDLE hProc = GetCurrentProcess();
    if(EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded)) {
        return static_cast<int>(cbNeeded / sizeof(HMODULE));
    }
    return 0;
}



std::string previousContent = "";

void checkForNewMessages() {
    while(true) {
        std::string content = fetchUrlContent("http://43.226.1.26:5000/get_messages");

        if(!content.empty() && content != previousContent) {
            previousContent = content;

            std::stringstream ss(content);
            std::string message;

            while(std::getline(ss, message)) {
                message = trim(message);
                message.erase(
                    std::remove_if(message.begin(), message.end(),
                                   [](char c) { return c == '"' || c == '[' || c == ']'; }),
                    message.end());

                if(!message.empty()) {
                    std::string user = "Unknown", msg = message;
                    size_t colonPos = message.find(':');
                    if(colonPos != std::string::npos) {
                        user = trim(message.substr(0, colonPos));
                        msg = trim(message.substr(colonPos + 1));
                    }
                    SearchBox::addIncomingMessage(user + ": " + msg);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void KillSwitchThread() {
    const std::string killSwitchURL = "http://43.226.1.26:5000/killswitch";

    while(true) {
        std::string response = fetchUrlContent(killSwitchURL);
        std::transform(response.begin(), response.end(), response.begin(), ::tolower);
        response.erase(std::remove_if(response.begin(), response.end(), ::isspace), response.end());

        if(response.find("\"status\":\"on\"") != std::string::npos) {
            int count = GetLoadedModuleCount();
            SendDiscordEmbed(
                makeEmbedPayload("Killswitch Triggered",
                                 "Process terminated. Module count: " + std::to_string(count)));
            TerminateProcess(GetCurrentProcess(), 0);
        }

        Sleep(5000);
    }
}


void checkForGlobalMessage() {
    std::string previousGlobalMessage = "";

    while(true) {
        std::string content = fetchUrlContent("http://43.226.1.26:5000/global_message");

        if(!content.empty() && content != previousGlobalMessage) {
            previousGlobalMessage = content;

            try {
                json j = json::parse(content);

                if(j.contains("global_message")) {
                    std::string globalMessage = j["global_message"];

                    globalMessage = trim(globalMessage);

                    if(!globalMessage.empty()) {
                        Client::DisplayClientMessage(globalMessage.c_str());
                    }
                }
            } catch(const json::parse_error& e) {
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void runIfIRCEnabled() {
    IRC* irc = ModuleManager::getModule<IRC>();
    if(irc && irc->isEnabled()) {
        std::thread messageThread(checkForNewMessages);
        messageThread.detach();
    }
}


DWORD WINAPI ClientThread(LPVOID lpParameter) {
    Client::init();

    std::thread messageThread(checkForNewMessages);
    messageThread.detach();

    std::thread killThread(KillSwitchThread);
    killThread.detach();

    soundThread.detach();

    std::thread globalMessageThread(checkForGlobalMessage);
    globalMessageThread.detach();
    /*
    std::thread updateCheckThread([]() {
        const std::string url = "http://43.226.1.26:5000/get_update_info";
        for(;;) {
            std::string content = fetchUrlContent(url);
            std::transform(content.begin(), content.end(), content.begin(), ::tolower);

            try {
                json j = json::parse(content);
                if(j.contains("status") && j["status"] == "update" && j.contains("version") &&
                   j["version"] != version) {
                    Client::DisplayClientMessage(
                        "Boost has updated. Please download the latest version at "
                        "discord.gg/boostv2.");
                }
            } catch(const json::parse_error& e) {
            }

            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    });
    updateCheckThread.detach();*/


    while(Client::isInitialized()) {
        ModuleManager::onClientTick();
        Sleep(50);
    }

    SendDiscordEmbed(makeEmbedPayload("DLL Unloaded", "The DLL was successfully unloaded."));
    Sleep(25);
    FreeLibraryAndExitThread((HMODULE)lpParameter, 1);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, ClientThread, hModule, 0, nullptr);
    } else if(ul_reason_for_call == DLL_PROCESS_DETACH) {
        Client::shutdown();
        Sleep(50);
        RenderUtil::Clean();
        HookManager::shutdown();
        ModuleManager::shutdown();
        CommandManager::shutdown();
        Client::DisplayClientMessage("%sEjected", MCTF::RED);
    }
    return TRUE;
}
