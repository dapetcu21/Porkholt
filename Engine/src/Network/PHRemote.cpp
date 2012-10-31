/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHRemote.h>
#include <Porkholt/Core/PHAccelInterface.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEventHandler.h>

PHRemote::PHRemote(PHGameManager * gameManager)
{
    setPort("2221");
    me = gameManager;
}

bool PHRemote::acceptsSignature(uint8_t signature)
{
    return (signature == 0xAC);
}

void PHRemote::recievedPacket(uint8_t signature, const URField * const * fields, int nrfields)
{
    if (signature == 0xAC)
    {
        int state = -1;
        PHPoint pnt;
        uint32_t x=0,y=0,w=1,h=1;
        void * id = NULL;
        
        for (int i=0; i<nrfields; i++)
        {
            uint8_t tag = fields[i]->tag();
            if (tag==0x02)
                id = (void*)(size_t)fields[i]->asInt32();
            if (tag==0x03)
                state = fields[i]->asInt8();
            if (tag==0x04)
                x = fields[i]->asInt32();
            if (tag==0x05)
                y = fields[i]->asInt32();
            if (tag==0x06)
                w = fields[i]->asInt32();
            if (tag==0x07)
                h = fields[i]->asInt32();
            if (tag==0x08)
            {
                PHEventHandler * eh = me->eventHandler();
                pnt.x = ((ph_float)(int32_t)x)/w * 2 - 1;
                pnt.y = (1-((ph_float)(int32_t)y)/h) * 2 - 1;
                switch (state) {
                    case 0:
                    {
                        eh->touchDown(pnt, id);
                        break;
                    }
                    case 1:
                    {
                        eh->touchUp(pnt, id);
                        break;
                    }
                    case 2:
                    {
                        eh->touchCancelled(pnt, id);
                        break;
                    }
                    case 3:
                    {
                        eh->touchMoved(pnt, id);
                        break;
                    }
                }
            }
            if (tag==0x01)
            {
                PHAcceleration accel;
                if (fields[i]->length()<sizeof(uint64_t)*3) continue;
                int64_t * arr = (int64_t*)fields[i]->asInt64s();
                accel.x = arr[0]/1048576.0f;
                accel.y = arr[1]/1048576.0f;
                accel.z = arr[2]/1048576.0f;
                PHAccelInterface::setAcceleration(accel);
                delete[] arr;
            }
        }
    }
}
