#pragma once
#include <string>

class AudioManager {
public:
    bool init();
    void shutdown();
    void playSound(const std::string& name) { (void)name; }
    void playMusic(const std::string& name) { (void)name; }
    void stopMusic() {}
    void playFireballSound() { /* stub: play fireball sound */ }
};
