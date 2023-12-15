#pragma once

#include "openHABItem.h"

#include <string>

class openHABSwitch : public openHABItem{
public:
    openHABSwitch(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel );
    virtual ~openHABSwitch();

    bool SendData( unsigned char *data) override;

    std::string setRelayOn();
    std::string setRelayOff();

    std::string GetType() const override {return "Switch";}

private:
    uint8_t m_w;
    void outputData( uint8_t w );

};