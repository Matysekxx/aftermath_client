#include "AudioService.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <filesystem>
#include <fstream>

#pragma comment(lib, "winmm.lib")

namespace fs = std::filesystem;

AudioService::AudioService() : isMusicPlaying(false) {}

AudioService::~AudioService() {
    stopMusic();
}

void AudioService::playBackgroundMusic(const std::string& filePath) {
    if (isMusicPlaying) {
        stopMusic();
    }

    std::string absPath = fs::absolute(filePath).string();

    if (!fs::exists(filePath)) {
        std::cerr <<"[AUDIO ERROR] File not found: " + absPath + "\n";
        return;
    }

    currentMusicFile = absPath;
    isMusicPlaying = true;

    mciSendString("close bgm", NULL, 0, NULL);

    std::string command = "open \"" + currentMusicFile + "\" type mpegvideo alias bgm";
    MCIERROR err = mciSendString(command.c_str(), NULL, 0, NULL);
    if (err != 0) {
        command = "open \"" + currentMusicFile + "\" alias bgm";
        mciSendString(command.c_str(), NULL, 0, NULL);
    }

    mciSendString("play bgm from 0 repeat", NULL, 0, NULL);
}

void AudioService::stopMusic() {
    if (isMusicPlaying) {
        mciSendString("stop bgm", NULL, 0, NULL);
        mciSendString("close bgm", NULL, 0, NULL);
        isMusicPlaying = false;
    }
}
