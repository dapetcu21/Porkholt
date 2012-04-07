//
//  PHTCPServer.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 12/7/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTCPServer.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

void PHTCPServer::execute()
{
    struct addrinfo hints, *res;
	int stat;
	int resu;
	string ip;
	
    if (state == disconnectedState)
    {
        string port("1337");
        PHLog("Binding to port %s...",port.c_str());
        memset(&hints,0,sizeof(hints));
        hints.ai_family=AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if ((stat=getaddrinfo(NULL,port.c_str(),&hints,&res)))
        {
            PHLog("getaddrinfo() failed: %s",gai_strerror(stat));
        } else
        {
            for (struct addrinfo * p = res; p; p = p->ai_next)
            {
                int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
                if (sock!=-1)
                {
                    resu=bind(sock, p->ai_addr, p->ai_addrlen);
                    if (resu!=-1)
                    {
                        if (listen(sock, 128)!=-1)
                        {
                            if (fcntl(sock, F_SETFL, O_NONBLOCK)!=-1)
                                listeningSockets.push_back(sock);
                            else
                                close(sock);
                        } else
                            close(sock);
                    } else
                        close(sock);
                }
            }
            freeaddrinfo(res);
        }
        
        if (!listeningSockets.empty())
        {
            state = listeningState;
            PHLog("Listening...");
        }
        else
            PHLog("Could not bind to port %s on any interface",port.c_str());
    }
    if (state == listeningState)
    {
        struct sockaddr_storage their_addr;
        socklen_t addrsize = sizeof their_addr;
        list<int>::iterator nx;
        for (list<int>::iterator i = nx = listeningSockets.begin(); i!=listeningSockets.end(); i=nx)
        {
            nx++;
            sock = accept(*i,(struct sockaddr *)&their_addr, &addrsize);
            if (sock == -1)
            {
                if (errno != EWOULDBLOCK && errno !=EAGAIN)
                {
                    PHLog("Can't accept client");
                    close(*i);
                    listeningSockets.erase(i);
                }
            } else {
                PHLog("Accepted client...");
                write_buffer.clear();
                if (!initConnection())
                {
                    PHLog("Client disconnected");
                    close(sock);
                    sock = -1;
                }
                else
                {
                    if (fcntl(sock, F_SETFL, O_NONBLOCK)==-1)
                    {
                        PHLog("Could not set socket as non-blocking");
                        close(sock);
                        sock = -1;
                    }
                    else
                        state = connectedState;
                    
                }
            }
            if (sock != -1)
                break;
        }
        if (state == connectedState)
        {
            for (list<int>::iterator i = listeningSockets.begin(); i!=listeningSockets.end(); i++)
                close(*i);
            listeningSockets.clear();
            clientConnected();
        } else {
            if (listeningSockets.empty())
            {
                PHLog("All listening sockets did not last");
                state = disconnectedState;
            }
        }
    }
    if (state == connectedState)
    {
        while (1)
        {
            uint8_t buffer[128];
            size_t bytes = recv(sock, buffer, 128, 0);
            if (bytes == -1 && errno == EAGAIN)
                break;
            if (bytes == 0 || bytes == -1)
            {
                clientDisconnected();
                PHLog("Client disconnected");
                close(sock);
                sock = -1;
                state = disconnectedState;
                write_buffer.clear();
                break;
            }
            will_buffer_writes = true;
            recievedBytes(buffer,bytes);
            bytes = recv(sock, buffer, 128, MSG_OOB);
            if (bytes != -1 && bytes>0)
                recievedOutOfBandBytes(buffer,bytes);
            will_buffer_writes = false;
            if (write_buffer.size())
            {
                fcntl(sock, F_SETFL, 0);
                send(sock, &write_buffer.front(), write_buffer.size(), 0);
                fcntl(sock, F_SETFL, O_NONBLOCK);
                write_buffer.clear();
            }
        }
    }
}

void PHTCPServer::clientConnected()
{
    
}

void PHTCPServer::clientDisconnected()
{
    
}


void PHTCPServer::recievedOutOfBandBytes(uint8_t * bytes, size_t size)
{
    
}

PHTCPServer::PHTCPServer(uint16_t prt) : sock(-1), state(disconnectedState), will_buffer_writes(false)
{
    char s[10];
    sprintf(s, "%u",(unsigned int)prt);
    port = s;
}

PHTCPServer::PHTCPServer(const string & prt) : sock(-1), state(disconnectedState), port(prt), will_buffer_writes(false)
{
    
}

bool PHTCPServer::sendBytes(const uint8_t * buffer, size_t size)
{
    if (will_buffer_writes)
    {
        size_t osz = write_buffer.size();
        write_buffer.resize(osz+size);
        memcpy((&write_buffer.front())+osz, buffer, size);
        return true;
    }
    fcntl(sock, F_SETFL, 0);
    size_t res = send(sock, buffer, size, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    return (res != -1);
}

PHTCPServer::~PHTCPServer()
{
    for (list<int>::iterator i = listeningSockets.begin(); i!=listeningSockets.end(); i++)
        close(*i);
    listeningSockets.clear();
    if (sock!=-1)
        close(sock);
}