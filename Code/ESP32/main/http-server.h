#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "queue.h"
#include <esp_http_server.h>
#include <string>

class HTTP_Server {
public:
    HTTP_Server( Queue *queue );

    enum {
        SERVER_MAIN,
        SERVER_POWEROFF,
        SERVER_POWERON,
        SERVER_VOLUME_UP,
        SERVER_VOLUME_DOWN,
        SERVER_VOLUME_UP_5,
        SERVER_VOLUME_DOWN_5,
        SERVER_INPUT_6CH,
        SERVER_INPUT_STREAMER,
        SERVER_INPUT_TV,
        SERVER_INPUT_VINYL,
        SERVER_INPUT_GAME
    };

    void start();
    void stop();

    esp_err_t handleResponse( uint8_t requestType, httpd_req_t *req );
protected:
    Queue *mQueue;
    httpd_handle_t mServerHandle;
    std::string mMainPage;
};

#endif