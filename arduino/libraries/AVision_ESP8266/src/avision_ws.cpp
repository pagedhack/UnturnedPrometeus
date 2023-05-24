/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        Websocket communications
*
* Created by       :        Arnold Velzel
* Created on       :        28-09-2021
*
*******************************************************************************/

#include <avision_debug.h>
#include <avision_ws.h>

using namespace AVision;

void wsCommunication::sendString(int num, String data)
{
    if (queueSize < MAX_SEND_QUEUE) {
        send_queue[queueSize].num = num;
        send_queue[queueSize].data = (char *)malloc(data.length() + 1);
        strcpy(send_queue[queueSize].data, data.c_str());
        queueSize++;
    }
}
void wsCommunication::sendData(int num, uint64_t data)
{
    sendString(num, String(data));
}
void wsCommunication::sendEndpointString(String endpoint, String data)
{
    for (size_t c = 0; c < MAX_WEBSOCKET_CLIENTS; c++)
    {
        String client_endpoint = String(clients[c].endpoint);
        if (clients[c].connected && client_endpoint == endpoint)
        {
            sendString(clients[c].num, data);
        }
    }
}
void wsCommunication::sendEndpointData(String endpoint, uint64_t data)
{
    sendEndpointString(endpoint, String(data));
}

byte wsCommunication::client_num(byte num)
{
    for (size_t c = 0; c < MAX_WEBSOCKET_CLIENTS; c++)
    {
        if (clients[c].num == num && clients[c].connected)
        {
            return clients[c].num;
        }
    }
}
char *wsCommunication::client_endpoint(byte num)
{
    for (size_t c = 0; c < MAX_WEBSOCKET_CLIENTS; c++)
    {
        if (clients[c].num == num && clients[c].connected)
        {
            return clients[c].endpoint;
        }
    }
}

void wsCommunication::event(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_CONNECTED)
    {
        bool accepted = false;
        for (size_t c = 0; c < MAX_WEBSOCKET_CLIENTS && !accepted; c++)
        {
            if (!clients[c].connected)
            {
                clients[c].num = num;
                clients[c].connected = true;
                clients[c].endpoint = (char *)malloc(length + 1);
                strcpy(clients[c].endpoint, (char *)payload);
                accepted = true;
            }
        }

        if (accepted)
        {
            clientsConnected++;
            server->sendTXT(num, "CONNECTED");
            if (clientConnectedCallback != nullptr)
            {
                String response = clientConnectedCallback(num, String((char *)payload));
                if (response != "") {
                    server->sendTXT(num, response);
                }
            }
        }
        else
        {
            // Auto disconnect
            server->sendTXT(num, "OVERLOAD");
            server->disconnect(num);
        }
        return;
    }

    if (type == WStype_DISCONNECTED)
    {
        bool accepted = false;
        for (size_t c = 0; c < MAX_WEBSOCKET_CLIENTS && !accepted; c++)
        {
            if (clients[c].num == num)
            {
                clients[c].num = 0;
                clients[c].connected = false;
                free(clients[c].endpoint);
                accepted = true;
            }
        }
        clientsConnected--;

        if (clientDisconnectedCallback != nullptr)
        {
            clientDisconnectedCallback(num);
        }
        return;
    }

    if (type == WStype_TEXT)
    {
        char *endpoint = client_endpoint(num);
        if (clientDataReceivedCallback != nullptr)
        {
            String response = clientDataReceivedCallback(num, String(endpoint), String((char *)payload));
            if (response != "") {
                server->sendTXT(num, response);
            } else {
                server->sendTXT(num, "OK");
            }
        } else {
            server->sendTXT(num, "OK");
        }
        return;
    }
}

void wsCommunication::onClientConnected(callbackFunctionWebsocketConnect callback)
{
    clientConnectedCallback = callback;
}
void wsCommunication::onClientDisconnected(callbackFunctionInt callback)
{
    clientDisconnectedCallback = callback;
}
void wsCommunication::onClientDataReceived(callbackFunctionWebsocketData callback)
{
    clientDataReceivedCallback = callback;
}

void wsCommunication::loop()
{
    if (queueSize > 0) {
        for (size_t q = 0; q < queueSize; q++)
        {
            int num = send_queue[q].num;
            if (num < 0) {
                // broadcast
                server->broadcastTXT(send_queue[q].data);
            } else {
                server->sendTXT(num, send_queue[q].data);
            }
            free(send_queue[q].data);
        }
        queueSize = 0;    
    }

    server->loop();
}
void wsCommunication::init(WebSocketsServerCore::WebSocketServerEvent clockCommunicationEvent)
{

    for (size_t c = 0; c < MAX_WEBSOCKET_CLIENTS; c++)
    {
        clients[c].num = 0;
        clients[c].endpoint = nullptr;
        clients[c].connected = false;
    }

    server = new WebSocketsServer(WEBSOCKET_PORT);
    server->begin();
    server->onEvent(clockCommunicationEvent);
    server->enableHeartbeat(2000, 2000, 5);
    running = true;
    dbgln("WS communication initialised");
}

wsCommunication::wsCommunication()
{
    running = false;
    queueSize = 0;
    clientsConnected = 0;

    clientConnectedCallback = nullptr;
    clientDisconnectedCallback = nullptr;
    clientDataReceivedCallback = nullptr;
}

wsCommunication::~wsCommunication()
{
}
