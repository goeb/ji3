#ifndef __SoundManager_hpp
#define __SoundManager_hpp

#include <iostream>
using namespace std;

typedef enum {
    ITEM_CHANNEL, // sound channel for normal items
    DING_CHANNEL // sound channel for the 'ding' sound
} ChannelId;

class SoundManager
{
    private :
        static int itemChannel; // used by SDL mixer
        static int dingChannel; // used by SDL mixer
    public :
        static void init();
        static void playSound(const string & filename, ChannelId channelId);
        static void close();
};

#endif
