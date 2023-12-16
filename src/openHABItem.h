#pragma once

#include <stdlib.h>
#include <list>
#include <string>
#include <atomic>

#include "log.h"

#include <curl/curl.h>

class openHABItem {
public:
    openHABItem(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel );
    virtual ~openHABItem();

    std::string GetIPAddress() const { return m_ipAddress; }
    unsigned int GetStartChannel() const { return m_startChannel; }

    void EnableOutput() { m_unreachable = false; m_issending = false; }

    virtual bool SendData(unsigned char *data) = 0;

    virtual std::string GetType() const = 0;

protected:
    std::string m_ipAddress;
    std::string m_item;
    uint16_t m_port;
    unsigned int m_startChannel;
    unsigned int m_seqCount;

    std::atomic<bool> m_unreachable;
    std::atomic<bool> m_issending;

    CURL *m_curl;

    void sendCmd( std::string const& message);

private:

};