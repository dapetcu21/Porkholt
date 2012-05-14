#include "PHX11.h"
#include "PHWindowing.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <GL/gl.h>
#include <GL/glx.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef GLvoid (*glXSwapIntervalFunc) (GLint);
glXSwapIntervalFunc glXSwapInterval = 0;

static bool PHXResEn = false;
void PHXSetResolution(int & w, int & h, int & r, bool enable, bool cache);
 
static bool PHXErrorOccurred = false;
static int (*PHXOldHandler)(Display*, XErrorEvent*);
static Display * PHXDisplay = NULL;

static set<string> PHGLXExtList;
static void PHXInitExtensionList(Display * display)
{
    const char * ext = (char*) glXQueryExtensionsString(display, DefaultScreen( display ));
    while (ext && ext[0])
    {
        const char * pos = strpbrk(ext, " ");
        if (!pos)
            pos = ext+strlen(ext);
            PHGLXExtList.insert(string(ext,pos-ext));
            if (!pos[0])
                ext = pos;
            else
                ext = pos+1;
    }
}

static bool PHXHasExtension(const string & s)
{
    return (PHGLXExtList.count(s));
}

static int PHXErrorHandler( Display *dpy, XErrorEvent *ev )
{
    if (dpy != PHXDisplay) 
        return PHXOldHandler(dpy, ev);
    PHXErrorOccurred = true;
    return 0;
}

static Colormap PHXCmap;
static GLXContext PHXContext;
static Window PHXWindow;
static int PHXRefreshRate;

int PHXGetScreenSize(int &width, int &height, int &rate)
{
    int num_sizes;
    Rotation original_rotation;

    Display *dpy = PHXDisplay;
    Window root = PHXWindow;
    XRRScreenSize *xrrs = XRRSizes(dpy, 0, &num_sizes);
    XRRScreenConfiguration *conf = XRRGetScreenInfo(dpy, root);
    SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);
    rate = XRRConfigCurrentRate(conf);

    width=xrrs[original_size_id].width;
    height=xrrs[original_size_id].height;
} 

void PHXCreateContext(const string & title, int & resX, int & resY, int & refresh, int fstype, int flags)
{
    Display * display = XOpenDisplay(0);
 
    if ( !display )
        throw string("Failed to open X display");
 
    int visual_attribs[] =
    {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , True,
        //GLX_SAMPLE_BUFFERS  , 1,
        //GLX_SAMPLES         , 4,
        None
    };
 
    int glx_major, glx_minor;
 
    // FBConfigs were added in GLX version 1.3.
    if ( !glXQueryVersion( display, &glx_major, &glx_minor ) || 
        ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
    {
        XCloseDisplay(display);
        throw string( "Invalid GLX version" );
    }

    int fbcount;
    GLXFBConfig *fbc = glXChooseFBConfig( display, DefaultScreen( display ), 
                                        visual_attribs, &fbcount );
    if ( !fbc )
    {
        XCloseDisplay(display);
        throw string("Failed to retrieve a framebuffer config");
    }
  
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
    int i;
    for ( i = 0; i < fbcount; i++ )
    {
        XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
        if ( vi )
        {
           int samp_buf, samples;
            glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
 
            if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
                best_fbc = i, best_num_samp = samples;
            if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
                worst_fbc = i, worst_num_samp = samples;
        }
        XFree( vi );
    }
 
    GLXFBConfig bestFbc = fbc[ best_fbc ];
    XFree( fbc );
 
    XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
    XSetWindowAttributes swa;
    Colormap cmap;
    swa.colormap = cmap = PHXCmap = XCreateColormap( display,
                                            RootWindow( display, vi->screen ), 
                                            vi->visual, AllocNone );
    swa.background_pixmap = None ;
    swa.border_pixel      = 0;
    #define PHXEventMask StructureNotifyMask | ButtonMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask
    swa.event_mask        = PHXEventMask; 
    Window win = XCreateWindow( display, RootWindow( display, vi->screen ), 
                                0, 0, resX, resY, 0, vi->depth, InputOutput, 
                                vi->visual, 
                                CWBorderPixel|CWColormap|CWEventMask, &swa );
    PHXWindow = win;
    XFree( vi );
    if ( !win )
    {
        XCloseDisplay(display);
        throw string("Failed to create window.\n");
    }
 
    PHXDisplay = display;
    
    Atom wmDelete=XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, win, &wmDelete, 1); 
    
    PHXSetTitle(title);
    PHXSetVideoMode(resX, resY, refresh, 0, (flags & PHWResizable)!=0);
    XMapRaised(display, win);
    XSync(display, False);
    PHXSetVideoMode(resX, resY, refresh, fstype, (flags & PHWResizable)!=0);
    
    PHXInitExtensionList(display);
 
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );
 
    GLXContext ctx = 0;
    PHXErrorOccurred = false;
    PHXOldHandler = XSetErrorHandler(&PHXErrorHandler);
 
    if (!PHXHasExtension("GLX_ARB_create_context") || !glXCreateContextAttribsARB)
    {
        ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
    }
    else
    {
        int context_attribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            None
        };
 
        ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
 
        XSync( display, False );
        if ( PHXErrorOccurred || !ctx )
        {
            context_attribs[1] = 1;
            context_attribs[3] = 0;
 
            PHXErrorOccurred = false;
 
            ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs );
        }
    }

    PHXContext = ctx;
 
    XSync(display, False );
    XSetErrorHandler(PHXOldHandler );
 
    if (PHXErrorOccurred || !ctx)
    {
        if (ctx)
            glXDestroyContext(display, ctx);
        XDestroyWindow(display, win);
        XFreeColormap(display, cmap);
        XCloseDisplay(display);
        
        throw("Failed to create an OpenGL context");
    }
    
    glXMakeCurrent(display, win, ctx);
}

bool PHXStillRunning;

void PHWRender();
void PHWMouseDown(int button, int x, int y);
void PHWMouseUp(int button, int x, int y);
void PHWMouseMoved(int x, int y);
void PHWReconfigure(int w, int h);

Bool PHTruePred(Display * display, XEvent * event, XPointer arg)
{
    return True;
}


void PHXMainLoop()
{
    PHXStillRunning = true;
    glXMakeCurrent(PHXDisplay, PHXWindow, PHXContext);
    bool restoreRes = false;
    int w,h,r;
    while (PHXStillRunning)
    {
        XEvent evt;
        while (XCheckIfEvent(PHXDisplay, &evt, PHTruePred, NULL))
        {
            switch (evt.type)
            {
                case ButtonPress:
                case ButtonRelease:
                {
                    int b = evt.xbutton.button;
                    if (b>=4 && b<=7)
                    {
                        break;
                    } 
                    if (b==3)
                        b=2;
                    else if (b==2)
                        b=3;
                    else if (b>7)
                        b-=4;
                    if (evt.type == ButtonPress)
                        PHWMouseDown(b, evt.xbutton.x, evt.xbutton.y);
                    else
                        PHWMouseUp(b, evt.xbutton.x, evt.xbutton.y);
                    break;
                }
                case MotionNotify:
                {
                    PHWMouseMoved(evt.xmotion.x, evt.xmotion.y);
                    break;
                }
                case ConfigureNotify:
                {
                    PHWReconfigure(evt.xconfigure.width, evt.xconfigure.height);
                    break;
                }
                case ClientMessage:
                {
                    PHXTerminate();
                    return;
                }
                /*case FocusOut:
                {
                    //if (evt.xfocus.mode != NotifyNormal) continue;
                    if (PHXResEn)
                    {
                        restoreRes = true;
                        PHXSetResolution(w, h, r, false, true);
                    }
                    PHLog("focusOut: %d",evt.xfocus.mode);
                }
                case FocusIn:
                { 
                    //if (evt.xfocus.mode != NotifyNormal) continue;
                    if (restoreRes)
                    {
                        restoreRes = false;
                        PHXSetResolution(w, h, r, true, true);
                    }
                    PHLog("focusIn %d",evt.xfocus.mode);
                }*/
            }
        }

        PHWRender();
        glFinish();
        glXSwapBuffers(PHXDisplay, PHXWindow);
       
    }
}

void PHXTerminate()
{
    PHXStillRunning = false;
}

void PHXSetTitle(const string & s)
{
    XStoreName(PHXDisplay, PHXWindow, s.c_str());
}

void PHXDestroyContext()
{
    int w=800, h=600, r=60;
    PHXSetResolution(w, h, r, false, false);

    Display * display = PHXDisplay;
    if (!display) return;
    PHXDisplay = NULL;

    GLXContext ctx = PHXContext;
    Colormap cmap = PHXCmap;
    Window win = PHXWindow;

    glXMakeCurrent( display, 0, 0 );
    glXDestroyContext( display, ctx );
 
    XFreeColormap(display, cmap);
    XDestroyWindow(display, win);
    XCloseDisplay(display);
}

void PHXSetVSync(bool vsync)
{
    GLint iInterval = (vsync ? 1 : 0);
    
    Display * display = glXGetCurrentDisplay();
    
#define glXGetProc(x) glXGetProcAddressARB((const GLubyte *)(x))

    if (PHXHasExtension("GLX_SGI_swap_control")) 
        glXSwapInterval = (glXSwapIntervalFunc)glXGetProc("glXSwapIntervalSGI");
    else if (PHXHasExtension("GLX_EXT_swap_control")) 
        glXSwapInterval = (glXSwapIntervalFunc)glXGetProc("glXSwapIntervalEXT");
    else if (PHXHasExtension("GLX_MESA_swap_control")) 
        glXSwapInterval = (glXSwapIntervalFunc)glXGetProc("glXSwapIntervalMESA");
    else if (PHXHasExtension("GLX_OML_sync_control")) 
        glXSwapInterval = (glXSwapIntervalFunc)glXGetProc("glXSwapIntervalOML");
    else
    {
        PHLog("Unable to switch sync-to-vblank on.\n");
        return;
    }
      
    glXSwapInterval(iInterval);
}

typedef struct {
    unsigned long   flags;
    unsigned long   functions;
    unsigned long   decorations;
    long            inputMode;
    unsigned long   status;
} Hints;

void PHXSetResizable(bool r, int width, int height)
{
    XSizeHints xhints;
    xhints.flags = r?0:(PMinSize | PMaxSize);
    xhints.min_width = xhints.max_width = width;
    xhints.min_height = xhints.max_height = height;
    XSetWMNormalHints(PHXDisplay, PHXWindow, &xhints);
}

void PHXResizeWindow(int w, int h, int sw, int sh, int unmap)
{
        Window r;
        int xr, yr;
        unsigned int wr, hr;
        unsigned int br;
        unsigned int dr;
        XGetGeometry(PHXDisplay, PHXWindow, &r, &xr, &yr, &wr, &hr, &br, &dr);
        if ((w == wr) && (h == hr)) return;
        if (unmap)
        {
            XEvent xev;
            Atom wm_state = XInternAtom(PHXDisplay, "_NET_WM_STATE", False);
            Atom fullscreen = XInternAtom(PHXDisplay, "_NET_WM_STATE_FULLSCREEN", False);

            memset(&xev, 0, sizeof(xev));
            xev.type = ClientMessage;
            xev.xclient.window = PHXWindow;
            xev.xclient.message_type = wm_state;
            xev.xclient.format = 32;
            bool isFs = (w==sw && h==sh); 
            xev.xclient.data.l[0] = isFs?1:0;
            xev.xclient.data.l[1] = fullscreen;
            xev.xclient.data.l[2] = 0;

            XSendEvent(PHXDisplay, DefaultRootWindow(PHXDisplay), False,
            SubstructureNotifyMask, &xev);
        } else
        {
            XResizeWindow(PHXDisplay, PHXWindow, w, h); 
        }
        XMapRaised(PHXDisplay, PHXWindow);
}

vector<short> PHXRates;
vector<SizeID> PHXSizes;
const vector<PHWVideoMode> & PHXVideoModes()
{
    static bool init = false;
    static vector<PHWVideoMode> v;
    if (init) return v;
    init = true;

    Display * dpy = PHXDisplay;
    Window root = XRootWindow(dpy, 0);
    XRRScreenConfiguration * conf = XRRGetScreenInfo(dpy, root);


    v.push_back(PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed));
    v.push_back(PHWVideoMode(800, 600, 60, PHWVideoMode::FullscreenWindowed));
    PHXRates.push_back(60);
    PHXRates.push_back(60);
    PHXSizes.push_back(0);
    PHXSizes.push_back(0);

    int n;
    XRRScreenSize * xrrs = XRRSizes(dpy, 0, &n);
    for (int i=0; i<n; i++)
    {
        int m;
        short * vv = XRRConfigRates (conf, i, &m);
        for (int j=0; j<m; j++)
        {
            v.push_back(PHWVideoMode(xrrs[i].width, xrrs[i].height, vv[j], PHWVideoMode::Fullscreen));
            PHXRates.push_back(vv[j]);
            PHXSizes.push_back(i);
        }
    }

    return v;
}


void PHXSetResolution(int & w, int & h, int & r, bool enable, bool cached)
{
    static SizeID orig_sizeID, last_sizeID;
    static short orig_rate, last_rate;
    static Rotation orig_rotation;
    SizeID sID;
    short rate;
    Rotation rot;

    if (!enable && !PHXResEn) return;

    Display * dpy = PHXDisplay;
    Window root = XRootWindow(dpy, 0);
    XRRScreenConfiguration * conf = XRRGetScreenInfo(dpy, root); 
    
    sID = orig_sizeID;
    rate = orig_rate;
    
    if (enable)
    {
        if (!PHXResEn)
        {
            orig_rate = XRRConfigCurrentRate(conf);
            orig_sizeID = XRRConfigCurrentConfiguration(conf, &orig_rotation);
        }
        
        if (cached)
        {
            sID = last_sizeID;
            rate = last_rate;
        } else {
            const vector<PHWVideoMode> & v = PHXVideoModes();
            size_t n = v.size();

            int delta = 0x3f3f3f3f;
            for (size_t i = 2; i<n; i++)
            {
                int d = abs(v[i].height-h)+abs(v[i].width-w)+abs(v[i].refresh-r);
                if (d<delta)
                {
                    delta = d;
                    sID = PHXSizes[i];
                    rate = PHXRates[i];
                    w = v[i].width; 
                    h = v[i].height;
                    r = rate;
                }
            }

            last_sizeID = sID;
            last_rate = rate;
        }
    }; 

    XRRSetScreenConfigAndRate(dpy, conf, root, sID, orig_rotation, rate, CurrentTime);
    PHXResEn = enable;
}

void PHXSetVideoMode(int & width, int & height, int & r, int fs, bool resize)
{
    static int lastFs = 0;
    Display * display = PHXDisplay;
    Window window = PHXWindow;

    int swidth, sheight, sr;
    int ox = 0, oy = 0;
    if (fs!=2)
    {
        PHXGetScreenSize(swidth, sheight, sr);
        if (fs == 1)
        {
            width = swidth;
            height = sheight;
        }
        if (fs == 0)
        {
            ox = (swidth-width)/2;
            oy = (sheight-height)/2;
        }
        r = sr;
    }
    
    if (fs!=0)
        PHXSetResizable(true, width, height);
    PHXSetResolution(width, height, r, (fs==2), false);
    if (fs==2)
    {
        swidth = width;
        sheight = height;
    }
    PHXResizeWindow(width, height, swidth, sheight, fs!=lastFs);
    if (fs==0)
        PHXSetResizable(resize, width, height);
    lastFs = fs;
}
