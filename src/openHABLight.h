#pragma once

#include "openHABItem.h"

class openHABLight : public openHABItem{
public:
    openHABLight(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel );
    virtual ~openHABLight();

    bool SendData( unsigned char *data) override;

    std::string GetType() const override {return "Light";}

    void setLightOnRGB( uint8_t r, uint8_t g, uint8_t b);
    void setLightOnHSV( int hue, int saturation, int brightness);
    void setLightOff();

private:
    uint8_t m_r;
    uint8_t m_g;
    uint8_t m_b;

    void outputData( uint8_t r ,uint8_t g ,uint8_t b );
    void RGBtoHSIV(float fR, float fG, float fB, float& fH, float& fSI, float& fSV,float& fI, float& fV);
};