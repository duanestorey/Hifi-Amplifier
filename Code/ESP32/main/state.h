#ifndef __STATE_H__
#define __STATE_H__

#include <stdio.h>
#include <stdlib.h>
#include "channelsel.h"

class AmplifierState {
public:
    AmplifierState() :mAudioType( AUDIO_DOLBY ), mState( STATE_INIT ), mInput( INPUT_UNDEFINED ), mCurrentVolume( 20 ), mSamplingRate( 48000 ), mConnected( false ) {}

    enum {
        INPUT_UNDEFINED = 0,
        INPUT_STEREO_1 = ChannelSel::INPUT_STEREO_1,
        INPUT_STEREO_2 = ChannelSel::INPUT_STEREO_2,
        INPUT_STEREO_3 = ChannelSel::INPUT_STEREO_3,
        INPUT_STEREO_4 = ChannelSel::INPUT_STEREO_4,
        INPUT_6CH = ChannelSel::INPUT_6CH,
    };

    enum {
        AUDIO_ANALOG,
        AUDIO_PCM,
        AUDIO_DOLBY,
        AUDIO_DTS
    } mAudioType;

    enum {
        STATE_INIT,
        STATE_PLAYING,
        STATE_MUTED,
        STATE_SLEEPING,
        STATE_UPDATING,
        STATE_ERROR_MINOR,
        STATE_ERROR_MAJOR
    } mState;

    // Volume goes from 0-79
    uint8_t mInput;
    uint8_t mCurrentVolume;
    uint32_t mSamplingRate;
    bool mConnected;
};

#endif