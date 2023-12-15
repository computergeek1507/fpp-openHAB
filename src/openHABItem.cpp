#include "openHABItem.h"

#include <stdlib.h>
#include <cstdint>
#include <thread>
#include <cmath>

#include <arpa/inet.h>  // for inet_pton
#include <netinet/in.h> // for sockaddr_in, htons
#include <netinet/tcp.h>
#include <stdint.h>     // for uint16_t, uint8_t, uint32_t
#include <stdio.h>      // for printf
#include <sys/socket.h> // for AF_INET, connect, send, socket, SOCK_STREAM
#include <unistd.h>     // for close, read
#include <string>       // for string
#include <cstring>      // for ??? memcpy, memset, strncpy


#include <iostream>
#include <istream>
#include <ostream>

openHABItem::openHABItem(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel) :
    m_ipAddress(ip),
    m_item(item),
    m_port(port),
    m_startChannel(startChannel),
    m_seqCount(0),
    m_unreachable(false),
    m_issending(false)
{
}

openHABItem::~openHABItem() {

}

std::string openHABItem::sendCmd( std::string const& message) {
    try {
       //curl -X POST --header "Content-Type: text/plain" --header "Accept: application/json" -d "OFF" "http://{openHAB_IP}:8080/rest/items/My_Item"
        std::string repURL = "http://" + m_ipAddress + ":" + std::to_string(m_port) + "/rest/items/" + m_item ;//+ "/state";
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1L);
        curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, message.size());
        curl_easy_setopt(m_curl, CURLOPT_COPYPOSTFIELDS, message.c_str());
        //curl_easy_setopt(m_curl, CURLOPT_PUT, 1L);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: text/plain");

        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers); 

        CURLcode status = curl_easy_perform(m_curl);
        if (status != CURLE_OK) {
            m_unreachable = true;
            LogInfo(VB_PLUGIN, "failed to send curl command\n");
        }
    }
    catch(std::exception const& ex) {
        LogInfo(VB_PLUGIN, "Error %s \n", ex.what());
    }
    return std::string("");
}

