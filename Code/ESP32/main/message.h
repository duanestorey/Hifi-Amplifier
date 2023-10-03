#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdio.h>
#include <stdlib.h>

class Message {
public:
    Message() : mMessageType( MSG_NONE ), mParam( 0 ) {}

    enum MessageType {
        MSG_NONE = 0,
        MSG_TIMER,
        MSG_WIFI_INIT,
        MSG_WIFI_CONNECTED,
        MSG_VOLUME_UP,
        MSG_VOLUME_DOWN,
        MSG_DISPLAY_DONE_INIT,
        MSG_DISPLAY_SHOULD_UPDATE,
        MSG_WIFI_UPDATE,
        MSG_DECODER_IRQ,
        MSG_BUTTON_POWER_PRESS,
        MSG_VOLUME_POWER_PRESS,
        MSG_INPUT_POWER_PRESS
    };

    Message( MessageType t ) : mMessageType( t ), mParam( 0 ) {}
    Message( MessageType t, uint32_t param ) : mMessageType( t ), mParam( param ) {}

    MessageType mMessageType;
    uint32_t mParam;
};

#endif