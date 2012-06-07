/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHTCPConsole.h"

#define INIT ctrlc(0), lastc(0)

PHTCPConsole::PHTCPConsole(uint16_t port) : PHTCPServer(port), INIT {}
PHTCPConsole::PHTCPConsole(const string & port) : PHTCPServer(port), INIT {}

bool PHTCPConsole::initConnection()
{
    string welcome(
"     ▄▄▄▄▀▀▀▀▀▀▀▀▄▄▄▄▄▄\r\n\
    █░░░░▒▒▒▒▒▒▒▒▒▒▒▒░░▀▀▄\r\n\
   █░░░▒▒▒▒▒▒░░░░░░░░▒▒▒░░█\r\n\
  █░░░░░░▄██▀▄▄░░░░░▄▄▄░░░█\r\n\
 ▀▒▄▄▄▒░█▀▀▀▀▄▄█░░░██▄▄█░░░█\r\n\
█▒█▒▄░▀▄▄▄▀░░░░░░░░█░░░▒▒▒▒▒█\r\n\
█▒█░█▀▄▄░░░░░█▀░░░░▀▄░░▄▀▀▀▄▒█\r\n\
 █▀▄░█▄░█▀▄▄░▀░▀▀░▄▄▀░░░░█░░█\r\n\
  █░░▀▄▀█▄▄░█▀▀▀▄▄▄▄▀▀█▀██░█\r\n\
   █░░██░░▀█▄▄▄█▄▄█▄████░█\r\n\
    █░░░▀▀▄░█░░░█░███████░█\r\n\
     ▀▄░░░▀▀▄▄▄█▄█▄█▄█▄▀░░█\r\n\
       ▀▄▄░▒▒▒▒░░░░░░░░░░█\r\n\
          ▀▀▄▄░▒▒▒▒▒▒▒▒▒▒░█\r\n\
             ▀▄▄▄▄▄░░░░░█\r\n\
Welcome to Porkholt Console. Problem?\r\n> ");
    interpretAsCommand = suboptions = option_cmd = esc = false;
    terminalEscape = terminalEscapeNumber = 0;
    waiting_for_response.clear();
    suboption_buffer.clear();
    echo = sgo = false;
    cursor = 0;
    cli = incompleteCLI = "";
    history.clear();
    history_pointer = 0;
    telnetSendOptionNegotiation(cmd_WILL, opt_echo);
    telnetSendOptionNegotiation(cmd_WILL, opt_suppress_go_ahead);
    return sendBytes((const uint8_t *)welcome.c_str(), welcome.size());
}

void PHTCPConsole::telnetSubOptionNegotiation()
{
    
}

void PHTCPConsole::telnetSendOptionNegotiation(uint8_t cmd, uint8_t option)
{
    uint8_t b[] = { cmd_IAC, cmd, option };
    sendBytes(b, 3);
    waiting_for_response.insert(option);
}

void PHTCPConsole::telnetOptionNegotiation(uint8_t cmd, uint8_t option)
{
    bool wfr = (waiting_for_response.count(option) != 0);
#define response(x,y) if (wfr) { telnetSendOptionNegotiation(x, y); waiting_for_response.erase(y); }
        
    switch (option) {
        case opt_echo:
        {
            if (cmd == cmd_DO)
            {
                echo = true;
                response(cmd_WILL, opt_echo);
            }
            if (cmd == cmd_DONT)
            {
                echo = false;
                response(cmd_WONT, opt_echo);
            }
            if (cmd == cmd_WONT || cmd == cmd_WILL)
            {
                response(cmd_DONT, opt_echo);
            }
            break;
        }
        case opt_suppress_go_ahead:
        {
            if (cmd == cmd_DO)
            {
                sgo = true;
                response(cmd_WILL, opt_suppress_go_ahead);
            }
            if (cmd == cmd_DONT)
            {
                sgo = false;
                response(cmd_WONT, opt_suppress_go_ahead);
            }
            if (cmd == cmd_WONT || cmd == cmd_WILL)
            {
                response(cmd_DONT, opt_suppress_go_ahead);
            }
            break;
        }
        default:
        {
            if (cmd == cmd_WILL || cmd == cmd_WONT)
                telnetSendOptionNegotiation(cmd_DONT,option);
            if (cmd == cmd_DO || cmd == cmd_DONT)
                telnetSendOptionNegotiation(cmd_WONT,option);
            break;
        }
    }
}

void PHTCPConsole::echoBytes(const char * s, size_t sz)
{
    if (sz <=0) return;
    if (echo && sgo)
        sendBytes((uint8_t*)s, sz);
}

void PHTCPConsole::bell()
{
    sendBytes((uint8_t*)"\007", 1);
}

void PHTCPConsole::rawByte(uint8_t C)
{
    char c = (char)C;
    if (terminalEscape)
    {
        switch (c) {
            case 'A':
            case 'B':
            {
                if (terminalEscapeNumber == 0)
                    terminalEscapeNumber = 1;
                int last = history_pointer;
                if (c=='A')
                    history_pointer-=terminalEscapeNumber;
                else
                    history_pointer+=terminalEscapeNumber;
                terminalEscape = 0;
                if (cli.size()==0)
                {
                    if (history_pointer<0)
                        history_pointer = 0;
                    if (history_pointer>history.size())
                        history_pointer = history.size();
                    if (history_pointer != last)
                    {
                        if (history_pointer==history.size())
                            incompleteCLI = "";
                        else
                            incompleteCLI = history[history_pointer];
                        int lc = cursor;
                        cursor = incompleteCLI.size();
                        for (int i=0; i<lc; i++)
                            echoBytes("\010", 1);
                        echoBytes(incompleteCLI.c_str(), cursor);
                        for (int i=cursor; i<lc; i++)
                            echoBytes(" ", 1);
                        for (int i=cursor; i<lc; i++)
                            echoBytes("\010", 1);
                    } else
                        bell();
                }
                break;
            }
            case 'D':
                if (terminalEscapeNumber == 0)
                    terminalEscapeNumber = 1;
                for (int i=0; i<terminalEscapeNumber; i++)
                {
                    if (cursor)
                    {
                        cursor--;
                        echoBytes("\010", 1);
                    } else 
                    {
                        bell();
                        break;
                    }
                }
                terminalEscape = 0;
                break;
            case 'C':
                if (terminalEscapeNumber == 0)
                    terminalEscapeNumber = 1;
                for (int i=0; i<terminalEscapeNumber; i++)
                {
                    if (cursor<incompleteCLI.size())
                    {
                        echoBytes(incompleteCLI.c_str()+cursor, 1);
                        cursor++;
                    } else
                    {
                        bell();
                        break;
                    }
                }
                terminalEscape = 0;
                break;
            case '~':
                if (terminalEscapeNumber == 3)
                {
                    if (cursor>=incompleteCLI.size())
                        bell();
                    else 
                    {
                        incompleteCLI.erase(cursor,1);
                        echoBytes(incompleteCLI.c_str()+cursor, incompleteCLI.size()-cursor);
                        echoBytes(" ", 1);
                        for (int i=0; i<incompleteCLI.size()-cursor+1; i++)
                            echoBytes("\010", 1);
                    }
                }
                terminalEscape = 0;
                break;
            default:
                if (c<='9' && c>='0')
                    terminalEscapeNumber = terminalEscapeNumber*10 + c - '0';
                else
                    terminalEscape = 0;
                break;
        }
        return;
    }
    if (esc && c == '[')
    {
        terminalEscape = 1;
        terminalEscapeNumber = 0;
        esc = false;
        return;
    }
    esc = false;
    if (c == '\0' || 
        c == '\t' || 
        c == '\r' || 
        c == '\n' || 
        c == '\004' || //ctrl + d 
        c == '\003' || //ctrl + c
        c == '\177' || //backspace
        c == '\033' //terminal escape
        )
    {
        esc = (c == '\033');
        if (c == '\t')
            autoCompleteCLI();
        if (c == '\r')
        {
            if (cli.size())
                cli.append(" ");
            cli.append(incompleteCLI);
            incompleteCLI = "";
            echoBytes("\r\n",2);
            cursor = 0;
            if (pharseCLI())
            {
                if (cli.size())
                {
                    history.push_back(cli);
                    history_pointer = history.size();
                }
                cli = "";
                sendBytes((uint8_t*)"> ", 2);
            } else {
                sendBytes((uint8_t*)">> ", 3);
            }
        }
        if (c == '\004' || c == '\003')
        {
            if (cli.size())
                cli.append(" ");
            cli.append(incompleteCLI);
            if (cli.size())
            {
                history.push_back(cli);
                history_pointer = history.size();
            }
            cli = "";
            incompleteCLI = "";
            cursor = 0;
            sendBytes((const uint8_t *)"\r\n> ", 4);
        }   
        if (c == '\177')
        {
            cursor--;
            if (cursor<0)
            {
                cursor = 0;
                bell();
            } else {
                if (cursor == incompleteCLI.size())
                    bell();
                else 
                {
                    echoBytes("\010", 1);
                    incompleteCLI.erase(cursor,1);
                    echoBytes(incompleteCLI.c_str()+cursor, incompleteCLI.size()-cursor);
                    echoBytes(" ", 1);
                    for (int i=0; i<incompleteCLI.size()-cursor+1; i++)
                        echoBytes("\010", 1);
                }
            }
        }
    } else
    {
        int l = incompleteCLI.size();
        incompleteCLI.insert(cursor,&c,1);
        echoBytes(incompleteCLI.c_str()+cursor, l-cursor+1);
        for (int i=0; i<l-cursor; i++)
            echoBytes("\010", 1);
        cursor++;
    }
}

void PHTCPConsole::processByte(uint8_t c)
{
    bool raw_data = false;
    if (interpretAsCommand)
    {
        switch (c)
        {
            case cmd_SE:
                suboptions = false;
                telnetSubOptionNegotiation();
                break;
            case cmd_NOP:
                break;
            case cmd_SB:
                suboptions = true;
                suboption_buffer.clear();
                break;
            case cmd_WILL:
            case cmd_WONT:
            case cmd_DO:
            case cmd_DONT:
                option_cmd = true;
                break;
            case cmd_IAC:
                raw_data = true;
                break;
                
            case cmd_DM:
            case cmd_BRK:
            case cmd_IP:
            case cmd_AO:
            case cmd_AYT:
            case cmd_EC:
            case cmd_EL:
            case cmd_GA:
            default:
                PHLog("unknown/unsupported telnet command: %d",(int)c);
                break;
        }
        interpretAsCommand = false;
    } else
    {
        if (c==cmd_IAC)
            interpretAsCommand = true;
        else
        {
            if (option_cmd)
            {
                telnetOptionNegotiation((uint8_t)lastc,c);
                option_cmd = false;
            }
            else
                if (suboptions)
                    suboption_buffer.push_back(c);
            else
                raw_data = true;
        }
    }
    if (raw_data)
        rawByte(c);
    lastc = c;
}

void PHTCPConsole::recievedBytes(uint8_t * bytes, size_t size)
{
    for (size_t i = 0; i<size; i++)
        processByte(bytes[i]);
}

/*void PHTCPConsole::recievedBytes(uint8_t * bytes, size_t size)
{
    static char ctrl_c[5] = {0xff, 0xf4, 0xff, 0xfd, 0x06};
    for (size_t i = 0; i<size; i++)
    {
        char c = (char)bytes[i];
        if (c == '\0' || c == '\r' || c == '\n' || c == '\t' || c=='\004' ||
            c==ctrl_c[0] || c==ctrl_c[1] || c==ctrl_c[2] || c==ctrl_c[3] || c==ctrl_c[4]
            )
        {
            cli.append((char*)bytes, i);
            bytes = bytes+i+1;
            size-=i+1;
            i = -1;
            if (c=='\t')
                autoCompleteCLI();
            if ((c=='\n' && lastc!='\r') || c=='\r')
                if (pharseCLI())
                {
                    cli = "";
                    sendBytes((const uint8_t *)"> ", 2);
                } else
                {
                    cli.append("\n");
                    sendBytes((const uint8_t *)">> ", 3);
                }
            if (c==ctrl_c[0] && ctrlc==0)
                ctrlc++;
            else
            if (c==ctrl_c[1] && ctrlc==1)
                ctrlc++;
            else
            if (c==ctrl_c[2] && ctrlc==2)
                ctrlc++;
            else
            if (c==ctrl_c[3] && ctrlc==3)
                ctrlc++;
            else
            if (c==ctrl_c[4] && ctrlc==4)
                ctrlc++;
            else
            if (ctrlc==5)
            {
                ctrlc = 0;
                cli = "";
                sendBytes((const uint8_t *)"\r\n> ", 4);
            } else
                ctrlc = 0;
            if (c=='\004')
            {
                cli = "";
                sendBytes((const uint8_t *)"\r\n> ", 4);
            }   
        } else
            ctrlc = 0;
        lastc = c;
    }
}*/
