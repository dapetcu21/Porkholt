//
//  PHRemote.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef PHREMOTE_H
#define PHREMOTE_H

#include "URServer.h"

class PHMainEvents;

class PHRemote : public URServer
{
public:
    PHRemote();
    
    virtual bool acceptsSignature(uint8_t signature);
	virtual void recievedPacket(uint8_t signature, const URField * const * fields, int nrfields); 
};

#endif