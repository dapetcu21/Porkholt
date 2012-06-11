/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHREMOTE_H
#define PHREMOTE_H

#include "URServer.h"

class PHGameManager;

class PHRemote : public URServer
{
private:
    PHGameManager * me;
public:
    PHRemote(PHGameManager * gameManager);
    
    virtual bool acceptsSignature(uint8_t signature);
	virtual void recievedPacket(uint8_t signature, const URField * const * fields, int nrfields); 
};

#endif
