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
        SERVER_POWERON
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