#include "fpp-pch.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstring>
#include <execution>
#include <istream>
#include <ostream>
#include <iostream>
#include <thread>
#include <vector>

#include <unistd.h>
#include <termios.h>
#include <chrono>
#include <thread>
#include <cmath>

#include <httpserver.hpp>
#include "common.h"
#include "settings.h"
#include "Plugin.h"
#include "Plugins.h"
#include "log.h"
#include "MultiSync.h"

#if __has_include("channeloutput/ChannelOutputSetup.h")
#include "channeloutput/ChannelOutputSetup.h"
#elif __has_include("channeloutput/channeloutput.h")
#include "channeloutput/channeloutput.h"
#endif

#include "fppversion_defines.h"

#include "commands/Commands.h"

#include "openHABLight.h"
#include "openHABSwitch.h"
#include "openHABItem.h"

class openHABPlugin : public FPPPlugin, public httpserver::http_resource {
private:
    //std::vector<std::unique_ptr <TPLinkItem>> _TPLinkOutputs;
    //Json::Value config;

public:
    openHABPlugin() : FPPPlugin("fpp-openHAB") {
        LogInfo(VB_PLUGIN, "Initializing openHAB Plugin\n");
        //readFiles();
        registerCommand();
    }
    virtual ~openHABPlugin() {
        //_TPLinkOutputs.clear();
    }

    class openHABSetSwitchCommand : public Command {
    public:
        openHABSetSwitchCommand(openHABPlugin *p) : Command("openHAB Set Switch"), plugin(p) {
            args.push_back(CommandArg("IP", "string", "openHAB IP Address"));
            args.push_back(CommandArg("port", "int", "Port").setRange(1, 50000).setDefaultValue("8080"));
            args.push_back(CommandArg("item", "string", "openHAB Item"));
            args.push_back(CommandArg("state", "bool", "Set Switch On or Off").setDefaultValue("true"));
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            std::string ipAddress = "";
            uint16_t port = 8080;
            std::string item = "";
            bool bulbOn = true;
             if (args.size() >= 1) {
                ipAddress = args[0];
            }
            if (args.size() >= 2) {
                port = std::stoi(args[1]);
            }
            if (args.size() >= 3) {
                item = args[2];
            }
            if (args.size() >= 4) {
                bulbOn = args[3]=="true";
            }
            plugin->SetSwitchState(ipAddress, port, item, bulbOn);
            return std::make_unique<Command::Result>("openHAB Switch Set");
        }
        openHABPlugin *plugin;
    };

    class openHABSetLightRGBCommand : public Command {
    public:
        openHABSetLightRGBCommand(openHABPlugin *p) : Command("TPLink Set Light RGB"), plugin(p) {
            args.push_back(CommandArg("IP", "string", "openHAB IP Address"));
            args.push_back(CommandArg("port", "int", "Port").setRange(1, 50000).setDefaultValue("8080"));
            args.push_back(CommandArg("item", "string", "openHAB Item"));
            args.push_back(CommandArg("r", "int", "Red").setRange(0, 255).setDefaultValue("255"));
            args.push_back(CommandArg("g", "int", "Green").setRange(0, 255).setDefaultValue("255"));
            args.push_back(CommandArg("b", "int", "Blue").setRange(0, 255).setDefaultValue("255"));
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            std::string ipAddress = "";
            uint16_t port = 8080;
            std::string item = "";
            uint8_t r = 255;
            uint8_t g = 255;
            uint8_t b = 255;
            if (args.size() >= 1) {
                ipAddress = args[0];
            }
            if (args.size() >= 2) {
                port = std::stoi(args[1]);
            }
            if (args.size() >= 3) {
                item = args[2];
            }
            if (args.size() >= 4) {
                r = std::stoi(args[3]);
            }
            if (args.size() >= 5) {
                g = std::stoi(args[4]);
            }
            if (args.size() >= 6) {
                b = std::stoi(args[5]);
            }

            plugin->SetLightOnRGB(ipAddress, port, item, r, g, b);
            return std::make_unique<Command::Result>("openHAB Light RGB Set");
        }
        openHABPlugin *plugin;
    };

    class openHABSetLightOffCommand : public Command {
    public:
        openHABSetLightOffCommand(openHABPlugin *p) : Command("openHAB Set Light Off"), plugin(p) {
            args.push_back(CommandArg("IP", "string", "openHAB IP Address"));
            args.push_back(CommandArg("port", "int", "Port").setRange(1, 50000).setDefaultValue("8080"));
            args.push_back(CommandArg("item", "string", "openHAB Item"));
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            std::string ipAddress = "";
            uint16_t port = 8080;
            std::string item = "";
            if (args.size() >= 1) {
                ipAddress = args[0];
            }
            if (args.size() >= 2) {
                port = std::stoi(args[1]);
            }
            if (args.size() >= 3) {
                item = args[2];
            }

            plugin->SetLightOff(ipAddress, port, item);
            return std::make_unique<Command::Result>("openHAB Light Off Set");
        }
        openHABPlugin *plugin;
    };

    void registerCommand() {
        CommandManager::INSTANCE.addCommand(new openHABSetSwitchCommand(this));
        CommandManager::INSTANCE.addCommand(new openHABSetLightRGBCommand(this));
        CommandManager::INSTANCE.addCommand(new openHABSetLightOffCommand(this));
    }

    //virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request &req) override {
    //    std::string v = getTopics();
    //    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(v, 200));
    //}
    
//#if FPP_MAJOR_VERSION < 4 || FPP_MINOR_VERSION < 1
 //   virtual void modifyChannelData(int ms, uint8_t *seqData) override {
//#else
    virtual void modifySequenceData(int ms, uint8_t *seqData) override {
//#endif
        try
        {
            //sendChannelData(seqData);
        }
        catch(std::exception const& ex)
        {
            std::cout << ex.what();
        }
    }

    virtual void playlistCallback(const Json::Value &playlist, const std::string &action, const std::string &section, int item) {
        if (settings["Start"] == "PlaylistStart" && action == "start") {
            //EnableTPLinkItems();
        }  
    }

    void SetSwitchState(std::string const& ip, uint16_t port, std::string const& item, bool state) {

        auto SetSwState = [ip,port,state](std::string const& __item)
        {
            openHABSwitch openSwitch(ip, port, __item, 1);
            if(state){
                openSwitch.setRelayOn();
            } else{
                openSwitch.setRelayOff();
            }
        };
        if(item.find(",") != std::string::npos) {
            auto items = split(item, ',');
            for(auto const& item_ : items) {
               SetSwState(item_);
            }
        } else {
            SetSwState(item);
        }
    }

    void SetLightOnRGB(std::string const& ip, uint16_t port, std::string const& item, uint8_t r, uint8_t g, uint8_t b ) {
        auto SetLightState = [ip,port,r, g, b](std::string const& __item)
        {
            openHABLight openLight(ip, port, __item ,1);
            openLight.setLightOnRGB(r, g, b);
        };
        if(item.find(",") != std::string::npos) {
            auto items = split(item, ',');
            for(auto const& item_ : items) {
               SetLightState(item_);
            }
        } else {
            SetLightState(item);
        }
    }

    void SetLightOff(std::string const& ip, uint16_t port, std::string const& item) {
        auto SetLightState = [ip,port](std::string const& __item)
        {
            openHABLight openLight(ip, port, __item ,1);
            openLight.setLightOff();
        };
        if(item.find(",") != std::string::npos) {
            auto items = split(item, ',');
            for(auto const& item_ : items) {
               SetLightState(item_);
            }
        } else {
            SetLightState(item);
        }
    }
};


extern "C" {
    FPPPlugin *createPlugin() {
        return new openHABPlugin();
    }
}
