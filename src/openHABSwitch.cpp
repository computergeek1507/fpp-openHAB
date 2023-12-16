#include "openHABSwitch.h"

#include "fpp-pch.h"
#include "common.h"
#include "settings.h"

#include <stdlib.h>
#include <cstdint>
#include <thread>
#include <cmath>

openHABSwitch::openHABSwitch(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel) :
    openHABItem(ip,port,item,startChannel),
    m_w(0)
{
}

openHABSwitch::~openHABSwitch() {

}

bool openHABSwitch::SendData( unsigned char *data) {
    try
    {
        if(m_unreachable){
            return false;
        }

        if(m_startChannel == 0){
            return false;
        }

        uint8_t w = data[m_startChannel - 1];

        if(w == m_w ) {
            if(m_seqCount < 1201) {
                ++ m_seqCount;
                return true;
            }
        }
        m_seqCount=0;
        m_w = w;

        std::thread t(&openHABSwitch::outputData, this, w );
        t.detach();
        //outputData(w );
        return true;
    }
    catch(std::exception const& ex)
    {
        m_unreachable = true;
        LogInfo(VB_PLUGIN, "Error %s \n",ex.what());
    }
    return false;
}

void openHABSwitch::outputData( uint8_t w ) {
    if(w >= 127){
        setSwitchOn();
    } else {
        setSwitchOff();
    }
}

void openHABSwitch::setSwitchOn() {
    sendCmd("ON");
}

void openHABSwitch::setSwitchOff() {
    sendCmd("OFF");
}




