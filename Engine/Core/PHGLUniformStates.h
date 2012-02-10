//
//  PHGLUniformStates.h
//  Porkholt
//
//  Created by Marius Petcu on 2/5/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGLUNIFORMSTATES_H
#define PHGLUNIFORMSTATES_H

#include "PHMain.h"

class PHGLShaderProgram;
class PHGLUniformStates : public PHObject
{
    //TODO: array support
    enum types {
        nullType = 0,
        intType,
        floatType,
        int2Type,
        float2Type,
        int3Type,
        float3Type,
        int4Type,
        float4Type,
        matrixType
    };
    struct uniform {
        int type;
        union
        {
            GLint intValue[4];
            GLfloat floatValue[16];
        };
        
        map<PHGLShaderProgram*,int> shaders;
        string name;
        int index;
        int active;
        
        void setValue(GLint v) { type = intType; intValue[0] = v; }
        void setValue(GLint v0, GLint v1) { type = int2Type; intValue[0] = v0; intValue[1] = v1; }
        void setValue(GLint v0, GLint v1, GLint v2) { type = int3Type; intValue[0] = v0; intValue[1] = v1; intValue[2] = v2; }
        void setValue(GLint v0, GLint v1, GLint v2, GLint v3) { type = int4Type; intValue[0] = v0; intValue[1] = v1; intValue[2] = v2; intValue[3] = v3; }
        
        void setValue(GLfloat v) { type = floatType; floatValue[0] = v; }
        void setValue(GLfloat v0, GLfloat v1) { type = float2Type; floatValue[0] = v0; floatValue[1] = v1; }
        void setValue(GLfloat v0, GLfloat v1, GLfloat v2) { type = float3Type; floatValue[0] = v0; floatValue[1] = v1; floatValue[2] = v2; }
        void setValue(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { type = float4Type; floatValue[0] = v0; floatValue[1] = v1; floatValue[2] = v2; floatValue[3] = v3; }
        
        void setValue(const PHVector2 & p) { type = float2Type; floatValue[0] = p.x; floatValue[1] = p.y; }
        void setValue(const PHVector3 & p) { type = float3Type; floatValue[0] = p.x; floatValue[1] = p.y; floatValue[2] = p.z; }
        void setValue(const PHColor & p) { type = float4Type; floatValue[0] = p.r; floatValue[1] = p.g; floatValue[2] = p.b; floatValue[3] = p.a; }
        void setValue(const PHRect & p) { type = float4Type; floatValue[0] = p.x; floatValue[1] = p.y; floatValue[2] = p.width; floatValue[3] = p.height; }
        
        void setValue(const PHMatrix & m) { type = matrixType; memcpy(floatValue, m.floats(), sizeof(GLfloat)*16); }
        
        uniform & operator = (GLint v) { setValue(v); return (*this); }
        uniform & operator = (GLfloat v) { setValue(v); return (*this); }
        uniform & operator = (const PHVector2 & p) { setValue(p); return (*this); }
        uniform & operator = (const PHVector3 & p) { setValue(p); return (*this); }
        uniform & operator = (const PHColor & p) { setValue(p); return (*this); }
        uniform & operator = (const PHRect & p) { setValue(p); return (*this); }
        uniform & operator = (const PHMatrix & p) { setValue(p); return (*this); }
        
        void apply(PHGLShaderProgram * shader);
        GLint location(PHGLShaderProgram * shader);
    };
    
protected:
    map<string,uniform*> stringMap;
    map<int,uniform*> intMap;
    int autogen;
    
    uniform & _insert(const string & name, int index);
    
public:
    
    PHGLUniformStates();
    ~PHGLUniformStates();
    
    uniform & operator[] (const string & name);
    uniform & operator[] (int index);
    
    uniform & at(const string & name) { return (*this)[name]; }
    uniform & at(int index) { return (*this)[index]; }
    
    void erase(const uniform & u);
    uniform & insert(const string & name, int index);
    
    void apply(PHGLShaderProgram * shader);
};

#endif
