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

int stencilF, stencilRef;
unsigned int stencilMask, stencilWMask;
int stencilOpSF, stencilOpDF, stencilOpDP;

void initPHGL();
void loadCapabilities();

public:

bool hasExtension(const string & ext);
bool isGLES() { return openGLCaps[PHGLCapabilityOpenGLES]; }
#if   defined(PH_GLES1_ONLY)
bool useShaders() { return false; }
#elif defined(PH_GLES2_ONLY)
bool useShaders() { return true; }
#else
bool useShaders() { return openGLCaps[PHGLCapabilityShaders]; }
#endif
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

enum stencilFunc {
    stencilAlways = 0,
    stencilNever,
    stencilLess,
    stencilLEqual,
    stencilGreater,
    stencilGEqual,
    stencilEqual,
    stencilNotEqual,
};

enum stencilOp {
    stencilKeep = 0,
    stencilZero,
    stencilReplace,
    stencilIncrement,
    stencilIncrementWarp,
    stencilDecrement,
    stencilDecrementWarp,
    stencilInvert
};

void setStencilFunc(enum stencilFunc func, int ref, unsigned int mask = ((unsigned int)-1));
void setStencilOp(enum stencilOp sfail, enum stencilOp dpfail, enum stencilOp dppass);
void setStencilMask(unsigned int mask);

