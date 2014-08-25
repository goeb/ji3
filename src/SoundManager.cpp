#include <fstream>

#include <SDL/SDL_mixer.h>
#include <SDL/SDL.h>

#include "SoundManager.hpp"
#include "Logger.hpp"


int SoundManager::itemChannel = -1;
int SoundManager::dingChannel = -1;

void SoundManager::init()
{
    SDL_Init(SDL_INIT_AUDIO);

    if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048) < 0)
    {
        LOG_ERROR("Error: Couldn't set 44100 Hz 16-bit audio, Reason: " << SDL_GetError());
    }
    itemChannel = -1;
    dingChannel = -1;

}



void SoundManager::playSound(const string & filename, ChannelId channelId)
{
    LOG_DEBUG("SoundManager::playSound()...filename=" << filename);

    // try opening the file
    ifstream file(filename.c_str(), ifstream::in | ifstream::binary);
    if (!file.fail())
    {
        file.close();
        
        LOG_DEBUG("Loading: " << filename);

        // get parameters of audio file (number of channels, etc.)
        Mix_Music *sound = 0;
        Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
        //sound = Mix_LoadMUS(filename.c_str());
    
        if (!chunk)
        {
            LOG_ERROR("Error: could not load file: " << filename << ", error=" << SDL_GetError());
        }
        else
        {
            Mix_PlayChannel(-1, chunk, 0);
#if 0
            int *channel;
            if (channelId == DING_CHANNEL) channel = &dingChannel;
            else channel = &itemChannel;
                
            LOG_DEBUG("Playing: " << filename);
            //Mix_HaltMusic(); // halt previous sound
                                
            Mix_PlayMusic(sound, 0); // -1 for any channel
#endif
            LOG_DEBUG("Play completed");
        }
    }
}

void SoundManager::close()
{
    Mix_CloseAudio();
}
