/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTCPCONSOLE_H
#define PHTCPCONSOLE_H

#include "PHTCPServer.h"

class PHTCPConsole : public PHTCPServer
{
protected:
    string cli,incompleteCLI;
    int ctrlc;
    char lastc;
    bool interpretAsCommand;
    void recievedBytes(uint8_t * bytes, size_t size);
    void echoBytes(const char * s, size_t sz);
    void processByte(uint8_t c);
    void rawByte(uint8_t c);
    void bell();
    bool initConnection();
    
    virtual void autoCompleteCLI() = 0;
    virtual bool pharseCLI() = 0;
    
    enum telnet_commands
    {
        cmd_SE = 240,
        cmd_NOP = 241,
        cmd_DM = 242,
        cmd_BRK = 243,
        cmd_IP = 244,
        cmd_AO = 245,
        cmd_AYT = 246,
        cmd_EC = 247,
        cmd_EL = 248,
        cmd_GA = 249,
        cmd_SB = 250,
        cmd_WILL = 251,
        cmd_WONT = 252,
        cmd_DO = 253,
        cmd_DONT = 254,
        cmd_IAC = 255
    };
    
    enum telnet_options
    {
        opt_suppress_go_ahead = 3,
        opt_echo = 1
    };
    
    bool suboptions,option_cmd,echo,sgo,esc;
    int terminalEscapeNumber,terminalEscape;
    vector<uint8_t> suboption_buffer;
    multiset<uint8_t> waiting_for_response;
    vector<string> history;
    int history_pointer;
    
    int cursor;
    
    void telnetSubOptionNegotiation();
    void telnetOptionNegotiation(uint8_t cmd, uint8_t option);
    void telnetSendOptionNegotiation(uint8_t cmd, uint8_t option);
public:
    PHTCPConsole(uint16_t port);
    PHTCPConsole(const string & port);
};

#endif
