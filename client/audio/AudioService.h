#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <string>
#include <atomic>

class AudioService {
public:
    AudioService();
    ~AudioService();

    void playBackgroundMusic(const std::string& filePath);
    void stopMusic();

private:
    std::atomic<bool> isMusicPlaying;
    std::string currentMusicFile;
};

#endif //AUDIOSERVICE_H
