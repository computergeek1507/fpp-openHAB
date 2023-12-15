#include "openHABLight.h"

#include <stdlib.h>
#include <cstdint>
#include <thread>
#include <cmath>

#include <arpa/inet.h>  // for inet_pton
#include <netinet/in.h> // for sockaddr_in, htons
#include <stdint.h>     // for uint16_t, uint8_t, uint32_t
#include <stdio.h>      // for printf
#include <sys/socket.h> // for AF_INET, connect, send, socket, SOCK_STREAM
#include <unistd.h>     // for close, read
#include <string>       // for string
#include <cstring>      // for ??? memcpy, memset, strncpy


#include <iostream>
#include <istream>
#include <ostream>

openHABLight::openHABLight(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel) :
    openHABItem(ip,port,item,startChannel),
    m_r(0),
    m_g(0),
    m_b(0)
{
}

openHABLight::~openHABLight() {

}

bool openHABLight::SendData( unsigned char *data) {
    try
    {
        if(m_unreachable){
            return false;
        }

        if(m_startChannel == 0){
            return false;
        }

        uint8_t r = data[m_startChannel - 1];
        uint8_t g = data[m_startChannel];
        uint8_t b = data[m_startChannel + 1];

        if(r == m_r && g == m_g && b == m_b) {
            if(m_seqCount < 1201) {
                ++ m_seqCount;
                return true;
            }
        }
        m_seqCount=0;
        m_r = r;
        m_g = g;
        m_b = b;

        std::thread t(&openHABLight::outputData, this, r, g, b );
        t.detach();
        //outputData(r, g, b );
        return true;
    }
    catch(std::exception const& ex)
    {
        m_unreachable = true;
        LogInfo(VB_PLUGIN, "Error %s \n",ex.what());
    }
    return false;
}

void openHABLight::outputData( uint8_t r ,uint8_t g ,uint8_t b ) {
    setLightOnRGB(r,g,b);
}

void openHABLight::RGBtoHSIV(float fR, float fG, float fB, float& fH, float& fSI, float& fSV,float& fI, float& fV) {
    float M  = std::max(std::max(fR, fG), fB);
    float m = std::min(std::min(fR, fG), fB);
    float c = M-m;
    fV = M;
    //fL = (1.0/2.0)*(M+m);
    fI = (1.0/3.0)*(fR+fG+fB);
  
    if(c==0) {
        fH = 0.0;
        fSI = 0.0;
    }
    else {
        if(M==fR) {
            fH = fmod(((fG-fB)/c), 6.0);
        }
        else if(M==fG) {
            fH = (fB-fR)/c + 2.0;
        }
        else if(M==fB) {
            fH = (fR-fG)/c + 4.0;
        }
        fH *=60.0;
        if(fI!=0) {
            fSI = 1.0 - (m/fI);
        }
        else {
            fSI = 0.0;
        }
    }

    fSV = M == 0 ? 0 : (M - m) / M;

    if(fH < 0.0){
        fH += 360.0;
    }
}

std::string openHABLight::setLightOnRGB( uint8_t r, uint8_t g, uint8_t b, int color_Temp, int period) {
    float h,si,sv,i,v;

    RGBtoHSIV(r/255,g/255,b/255,h,si,sv,i,v);
    
    int ih = (h);
    //int isi = (si*100);
    int isv = (sv*100);
    //int ii = (i*100);
    int iv = (v*100);
    return setLightOnHSV(ih, isv, iv, color_Temp, period);
}

std::string openHABLight::setLightOnHSV( int hue, int saturation, int brightness, int color_Temp, int period) {
    const std::string cmd =  std::to_string(hue) + "," + std::to_string(saturation) + "," + std::to_string(brightness);
    return sendCmd(cmd);
}

std::string openHABLight::setLightOff(){

    return sendCmd("OFF");
}