/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Websocket data updates
*
* Created by       :        Arnold Velzel
* Created on       :        28-09-2021
*
*******************************************************************************/
#include <avision_system.h>

// https://github.com/Links2004/arduinoWebSockets (WebSockets by Markus Sattler)

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

#define WEBSOCKET_PORT 8282
#define MAX_WEBSOCKET_CLIENTS WEBSOCKETS_SERVER_CLIENT_MAX
#define MAX_SEND_QUEUE 20

#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

typedef String (*callbackFunctionWebsocketConnect)(int client_num, String endpoint);
typedef String (*callbackFunctionWebsocketData)(int client_num, String endpoint, String data);

namespace AVision {

    class wsCommunication
    {
        private:

            struct ws_client_struct {
                int num;
                char* endpoint;
                bool connected;
            };
            struct ws_client_send_queue {
                int num;
                char* data;
            };

            int clientsConnected;
            int queueSize;
            WebSocketsServer *server;

            struct ws_client_struct clients[MAX_WEBSOCKET_CLIENTS];
            struct ws_client_send_queue send_queue[MAX_SEND_QUEUE];

            byte client_num(byte num);
            char* client_endpoint(byte num);

            callbackFunctionWebsocketConnect clientConnectedCallback;
            callbackFunctionInt clientDisconnectedCallback;
            callbackFunctionWebsocketData clientDataReceivedCallback;
        public:
            void onClientConnected(callbackFunctionWebsocketConnect callback);
            void onClientDisconnected(callbackFunctionInt callback);
            void onClientDataReceived(callbackFunctionWebsocketData callback);

            bool running;

            void event(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

            void sendData(int num, uint64_t data);
            void sendString(int num, String data);
            void sendEndpointData(String endpoint, uint64_t data);
            void sendEndpointString(String endpoint, String data);

            void loop();
            void init(WebSocketsServerCore::WebSocketServerEvent clockCommunicationEvent);

            wsCommunication();
            ~wsCommunication();
    };
    
}

#endif