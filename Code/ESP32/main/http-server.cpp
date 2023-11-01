#include "http-server.h"

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"


#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
#include "esp_spiffs.h"

#include "debug.h"
#include <string>


HTTP_Server::HTTP_Server( Queue *queue ) : mQueue( queue ), mServerHandle( 0 ) {

}

esp_err_t send_web_response( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_MAIN, req );
}

esp_err_t poweroff( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_POWEROFF, req );
}

esp_err_t poweron( httpd_req_t *req )
{
    HTTP_Server *server = (HTTP_Server *)req->user_ctx;
    return server->handleResponse( HTTP_Server::SERVER_POWERON, req );
}


esp_err_t 
HTTP_Server::handleResponse( uint8_t requestType, httpd_req_t *req ) {
    switch( requestType ) {
        case HTTP_Server::SERVER_POWEROFF:
            mQueue->add( Message::MSG_POWEROFF );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN );
            break;
        case HTTP_Server::SERVER_POWERON:
            mQueue->add( Message::MSG_POWERON );
            httpd_resp_set_hdr( req, "Location", "/");
            return httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN );
            break;
        case HTTP_Server::SERVER_MAIN:
            return httpd_resp_send(req, mMainPage.c_str(), HTTPD_RESP_USE_STRLEN );
            break;
        default:
            return httpd_resp_send(req, mMainPage.c_str(), HTTPD_RESP_USE_STRLEN );
            break;
    }

}

void
HTTP_Server::start() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    mServerHandle = 0;

    if ( httpd_start( &mServerHandle, &config ) == ESP_OK ) { 
        AMP_DEBUG_I( "Web server started" );

        httpd_uri_t uri_get = {0};
        uri_get.uri = "/";
        uri_get.method = HTTP_GET;
        uri_get.handler = send_web_response;
        uri_get.user_ctx = this;
        httpd_register_uri_handler( mServerHandle, &uri_get );

        uri_get.uri = "/poweroff";
        uri_get.handler = poweroff;
        httpd_register_uri_handler( mServerHandle, &uri_get );
        uri_get.uri = "/poweron";
        uri_get.handler = poweron;
        httpd_register_uri_handler( mServerHandle, &uri_get );

                
        esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
        };

        esp_err_t ret = esp_vfs_spiffs_register(&conf);

        if (ret == ESP_OK) {
            AMP_DEBUG_I( ".. file system mounted" );

            FILE *f = fopen( "/spiffs/index.html", "rt" );
            if ( f ) {
                char s[1024];

                while ( !feof( f ) ) {
                    fgets( s, 1024, f );

                    if ( strlen( s ) ) {
                        mMainPage = mMainPage + s;
                    }
                }
            }
        }
    }


    
}

void 
HTTP_Server::stop() {

}