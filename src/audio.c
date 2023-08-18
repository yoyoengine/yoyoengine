/*
    AUDIO TODO:
    - allow channel to be specified or auto assigned
    - if game wants to auto assign a channel, they do it through a special function that gives them
      the channel int it was assigned to for tracking purposes (this probably wont happen because game has list of tracks
      it wants to play on already anyways and will interrupt itself)
    - threading audio so it doesnt block the game
*/

#include <stdio.h>

#include <SDL2/SDL_mixer.h>

#include "lib/engine.h"
#include "lib/logging.h"

// define the max number of audio channels
#define MAX_CHANNELS 16

// create array to hold audio chunks in memory
Mix_Chunk *pChunks[MAX_CHANNELS] = { NULL };

// counter for total chunks (used in debug)
int totalChunks = 0;

// function to initialize audio system
void initAudio(){
    // opens the mixer to the format specified
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) 
    {
        // catch failure
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        logMessage(error, buffer);
    }
    // allocate our desired max channels to the mixer
    Mix_AllocateChannels(MAX_CHANNELS);

    // debug: acknowledge audio initialization
    logMessage(info, "Audio initialized.\n");
}

// function to free audio chunk from memory by channel
void free_audio_chunk(int channel) {
    // if the channel is invalid
    if (channel < 0 || channel >= MAX_CHANNELS) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Invalid channel (%d) to free audio chunk.\n", channel);
        logMessage(error, buffer);
        return; // pass
    }

    // if there is something in the channel we want to free from
    if (pChunks[channel] != NULL) {
        // free the chunk indexed from our chunks at the channel specified
        Mix_FreeChunk(pChunks[channel]);
        
        // set the channel the chunk resided in to NULL
        pChunks[channel] = NULL;
        totalChunks--;
    }
}

// function allowing a sound to be played on a channel by filename
// param "chan" is channel
void playSound(const char *pFilename, int chan, int loops) {
    // open our filename into a chunk
    Mix_Chunk *pSound = Mix_LoadWAV(pFilename);
    
    // if opening failed
    if (pSound == NULL) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Error loading audio file: %s\n", Mix_GetError());
        logMessage(error, buffer);
        return; // alarm in console and pass
    }
    
    // attempt to play the chunk on the channel,
    // returns which channel it was assigned to
    int channel = Mix_PlayChannel(chan, pSound, loops); 
    
    // if playing failed (assigned channel -1)
    if (channel == -1) {
        char buffer[100];
        snprintf(buffer, sizeof(buffer),  "Error playing audio file: %s\n", Mix_GetError());
        logMessage(error, buffer);
        Mix_FreeChunk(pSound);
        return; // alarm in console, free the allocated chunk and pass
    }

    // if the channel assigned was out of bounds
    if(channel < 0 || channel >= MAX_CHANNELS){
        logMessage(error, "Error: channel index out of bounds\n");
        Mix_FreeChunk(pSound);
        return; // free the allocated chunk and pass
    }

    // put our channel identifier into the chunks
    pChunks[channel] = pSound;

    // increment total chunks
    totalChunks++;

    // Free audio memory when channel finishes
    Mix_ChannelFinished(free_audio_chunk);
}

// set a specific (or all channels if passed -1) volume level 0-128 
void setVolume(int channel, int volume){
    char buffer[100];
    snprintf(buffer, sizeof(buffer),  "Setting volume of channel %d to %d.\n",channel,volume);
    logMessage(debug, buffer);
    Mix_Volume(channel,volume);
}

// shut down all audio systems and free all audio chunks
void shutdownAudio(){
    // Halt all playing channels
    Mix_HaltChannel(-1);
    logMessage(debug, "Halted playing all channels.\n");


    // Free all audio chunks in the chunks array
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (pChunks[i] != NULL) {
            Mix_FreeChunk(pChunks[i]);
            pChunks[i] = NULL;
        }
    }

    // Close the audio mixer
    Mix_CloseAudio();
    logMessage(info, "Mixer closed.\n");
}