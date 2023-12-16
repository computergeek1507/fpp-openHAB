#include "openHABItem.h"

#include <stdlib.h>
#include <cstdint>
#include <thread>
#include <cmath>

openHABItem::openHABItem(std::string const& ip, uint16_t port, std::string const& item, unsigned int startChannel) :
    m_ipAddress(ip),
    m_item(item),
    m_port(port),
    m_startChannel(startChannel),
    m_seqCount(0),
    m_unreachable(false),
    m_issending(false),
    m_curl(nullptr)
{
    m_curl = curl_easy_init();
}

openHABItem::~openHABItem() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
}

void openHABItem::sendCmd(std::string const& message) {
    try {
       //curl -X POST --header "Content-Type: text/plain" --header "Accept: application/json" -d "OFF" "http://{openHAB_IP}:8080/rest/items/My_Item"
        std::string repURL = "http://" + m_ipAddress + ":" + std::to_string(m_port) + "/rest/items/" + m_item ;//+ "/state";
        LogInfo(VB_PLUGIN, "URL %s \n", repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 5L);
        curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, message.c_str());
        curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);

        struct curl_slist *headers = nullptr;
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
}

