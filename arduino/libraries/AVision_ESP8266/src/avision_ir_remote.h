/*******************************************************************************
*
* (c) 2021 Copyright A-Vision Software
*
* File description :        IR Remote control
*
* Created by       :        Arnold Velzel
* Created on       :        26-09-2021
*
*******************************************************************************/
#include <avision_system.h>

// https://github.com/crankyoldgit/IRremoteESP8266
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

#ifndef __IR_REMOTE_H__
#define __IR_REMOTE_H__

typedef void (*IRcallbackFunction)(decode_results results);
typedef void (*IReventCallbackFunction)(int repeat);

#define LEGACY_TIMING_INFO false
#define IR_RECEIVE_PIN D4
#define IR_BUFFER_SIZE 1024
#define IR_RECEIVE_TIMEOUT 50
#define IR_UNKNOWN_SIZE 12
#define IR_TOLERANCE 25

#define MAX_IR_ACTIONS 50
#define MAX_IR_EVENTS 50
struct IR_event_struct
{
    int code;
    char *event;
};
struct IR_event_action_struct
{
    char *event;
    IReventCallbackFunction callback;
};

namespace AVision
{

    class IRremote
    {
        private:
            struct IR_event_struct IRactions[MAX_IR_ACTIONS];
            struct IR_event_action_struct IRevents[MAX_IR_EVENTS];

            uint8 action_index = 0;
            uint8 event_index = 0;
            uint64_t last_code = 0;
            IRcallbackFunction onReceiveIRCodeAction = nullptr;

            IRrecv *irrecv;

        public:
            // Event on receiving any IR signal/code
            void onReceive(IRcallbackFunction action);
            // Event when a specific event (IR code) is received
            void onReceiveEvent(char *event, IReventCallbackFunction callback);
            // Note! Any specific event is triggered BEFORE the generic onReceive is triggered

            // Register what event to trigger on a specified code
            void registerCodeEvent(uint64_t code, char *event);
            void resetCodeEvents();

            void loop();
            void init(int pin);
            void init();

            IRremote();
            ~IRremote();
    };

}

#endif
