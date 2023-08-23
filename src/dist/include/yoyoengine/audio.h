#ifndef AUDIO_H
#define AUDIO_H

// counter for audio chunks
extern int totalChunks;

// initialize audio components for engine
void initAudio();

// play a sound by its filename path and specify number of loops (-1 for looping)
void playSound(const char *pFilename, int channel, int loops);

// set a specific (or all channels if passed -1) volume level 0-128 
void setVolume(int channel, int volume);

// shut down all audio systems and free all memory assosciated
void shutdownAudio();

#endif