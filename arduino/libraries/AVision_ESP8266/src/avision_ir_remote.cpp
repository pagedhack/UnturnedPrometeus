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

#include <avision_debug.h>
#include <avision_ir_remote.h>

using namespace AVision;

void IRremote::onReceive(IRcallbackFunction action)
{
    onReceiveIRCodeAction = action;
}
void IRremote::onReceiveEvent(char* event, IReventCallbackFunction callback)
{
    if (event_index < MAX_IR_EVENTS)
    {
        IRevents[event_index].event = (char *)malloc(strlen(event) + 1);
        strcpy(IRevents[event_index].event, (char *)event);
        IRevents[event_index].callback = callback;
        event_index++;
    }
}

void IRremote::registerCodeEvent(uint64_t code, char *event)
{
    if (action_index < MAX_IR_ACTIONS)
    {
        IRactions[action_index].code = code;
        IRactions[action_index].event = (char *)malloc(strlen(event) + 1);
        strcpy(IRactions[action_index].event, (char *)event);
        action_index++;
    }
}
void IRremote::resetCodeEvents()
{
    for (size_t a = 0; a < action_index; a++)
    {
        IRactions[a].code = 0;
        free(IRactions[a].event);
    }
    action_index = 0;
}

void IRremote::loop()
{
    decode_results results;
    if (irrecv->decode(&results) && (results.decode_type >= 0) && (results.decode_type < 1024))
    {

        if (!results.repeat)
        {
            last_code = results.value;
        }

        bool actioned = false;
        for (size_t i = 0; i < action_index; i++)
        {
            if (IRactions[i].code == last_code)
            {
                dbgln("IR event: " + String(IRactions[i].event));
                for (size_t e = 0; e < event_index; e++)
                {
                    if (strcmp(IRevents[e].event, IRactions[i].event) == 0)
                    {
                        IRevents[e].callback(results.repeat);
                        actioned = true;
                    }
                }
            }
        }

        if (onReceiveIRCodeAction != nullptr)
        {
            onReceiveIRCodeAction(results);
        }
    }
}

void IRremote::init(int pin)
{
    irrecv = new IRrecv(pin, IR_BUFFER_SIZE, IR_RECEIVE_TIMEOUT, true);

    assert(irutils::lowLevelSanityCheck() == 0);
#if DECODE_HASH
    // Ignore messages with less than minimum on or off pulses.
    irrecv->setUnknownThreshold(IR_UNKNOWN_SIZE);
#endif // DECODE_HASH

    irrecv->setTolerance(IR_TOLERANCE); // Override the default tolerance.
    irrecv->enableIRIn();               // Start the receiver

    dbgln("IRremote initialised");
}
void IRremote::init()
{
    init(IR_RECEIVE_PIN);
}

IRremote::IRremote()
{
}

IRremote::~IRremote()
{
}
