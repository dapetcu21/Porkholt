/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTCPSERVER_H
#define PHTCPSERVER_H

#include <Porkholt/Core/PHMain.h>

class PHTCPServer : public PHObject
{
private:
    int state;
    int sock;
    list<int> listeningSockets;
    string port;
    vector<uint8_t> write_buffer;
    bool will_buffer_writes;
    
public:
    enum states
    {
        disconnectedState,
        listeningState,
        connectedState
    };
    int currentState() { return state; }
    
    void execute();
    bool sendBytes(const uint8_t * buffer, size_t count);
    
    PHTCPServer(uint16_t port);
    PHTCPServer(const string & port);
    ~PHTCPServer();

protected:
    virtual void recievedBytes(uint8_t * bytes, size_t size) = 0;
    virtual void recievedOutOfBandBytes(uint8_t * bytes, size_t size);
    virtual bool initConnection() = 0;
    
    virtual void clientConnected();
    virtual void clientDisconnected();
};

#endif
