/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

PHColor ccolor;
float cdepth;
int cstencil;
set<string> extensions;
bool parsedExtensions;
static bool phglinited;
int openGLVersionMajor,openGLVersionMinor,glslVersion;
string glslHeader;
bool openGLCaps[PHGLNumberCapabilities];


void initPHGL();
void loadCapabilities();

public:

bool hasExtension(const string & ext);
bool isGLES() { return openGLCaps[PHGLCapabilityOpenGLES]; }
bool useShaders() { return openGLCaps[PHGLCapabilityShaders]; }
int openGLMajorVersion() { return openGLVersionMajor; }
int openGLMinorVersion() { return openGLVersionMinor; }
int openGLSLVersion() { return glslVersion; }
bool hasCapability(int cap) { return openGLCaps[cap]; }

void setClearColor(const PHColor & c);
void setDepthClearValue(float val);
void setStencilClearValue(int val);
const PHColor & clearColor() { return ccolor; }
float depthClearValue() { return cdepth; }
int stencilClearValue() { return cstencil; }

enum {
    colorBuffers = 1<<0,
    depthBuffer = 1<<1,
    stencilBuffer = 1<<2
};
void clearBuffers(int mask);
void clearColorBuffers() { clearBuffers(colorBuffers); }
void clearDepthBuffer() { clearBuffers(depthBuffer); }
void clearStencilBuffer() { clearBuffers(stencilBuffer); }
