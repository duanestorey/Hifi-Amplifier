#ifndef __STATE_H__
#define __STATE_H__

#include <stdio.h>
#include <stdlib.h>
#include "channelsel.h"

class AmplifierState {
public:
    AmplifierState() : mAudioType( AUDIO_ANALOG ), mState( STATE_INIT ), mInput( INPUT_6CH ), mSpeakerConfig( AUDIO_2_CH ), mCurrentVolume( 55 ), mSamplingRate( 48000 ), mConnected( false ), mEnhancement( false ) {}

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
    } ;

    enum {
        AUDIO_2_CH,
        AUDIO_2_DOT_1,
        AUDIO_5_DOT_1,
    };

    // Volume goes from 0-79
    uint8_t mState;
    uint8_t mInput;
    uint8_t mSpeakerConfig;
    uint8_t mCurrentVolume;
    uint32_t mSamplingRate;
    bool mConnected;
    bool mEnhancement;
};

#endif