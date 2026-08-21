// ============================================================
// Fantasy 3D - Crystal Kingdoms (Shaders + Bloom Edition)
// ============================================================
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>

// ============================================================
// Constants
// ============================================================
static const int   SCREEN_W   = 1280;
static const int   SCREEN_H   = 720;
static const float PI         = 3.14159265358979f;
static const int   TERRAIN_GR = 60;
static const float TERRAIN_CS = 5.0f;
static const float TERRAIN_H  = 8.0f;

// FBO / GL constants not in GL/gl.h
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER            0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0      0x8CE0
#endif
#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT       0x8D00
#endif
#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER           0x8D41
#endif
#ifndef GL_DEPTH_COMPONENT24
#define GL_DEPTH_COMPONENT24      0x81A6
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE   0x8CD5
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE          0x812F
#endif
#ifndef GL_TEXTURE1
#define GL_TEXTURE1               0x84C1
#endif

// ============================================================
// Particle
// ============================================================
struct Particle {
    float px, py, pz;
    float vx, vy, vz;
    float life, r, g, b, size;
};

// ============================================================
// GL Function Pointer Types
// ============================================================
typedef GLuint (*PFN_glCreateShader)(GLenum);
typedef void   (*PFN_glShaderSource)(GLuint, GLsizei, const GLchar**, const GLint*);
typedef void   (*PFN_glCompileShader)(GLuint);
typedef void   (*PFN_glGetShaderiv)(GLuint, GLenum, GLint*);
typedef void   (*PFN_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void   (*PFN_glDeleteShader)(GLuint);
typedef GLuint (*PFN_glCreateProgram)(void);
typedef void   (*PFN_glAttachShader)(GLuint, GLuint);
typedef void   (*PFN_glLinkProgram)(GLuint);
typedef void   (*PFN_glGetProgramiv)(GLuint, GLenum, GLint*);
typedef void   (*PFN_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void   (*PFN_glUseProgram)(GLuint);
typedef void   (*PFN_glDeleteProgram)(GLuint);
typedef GLint  (*PFN_glGetUniformLocation)(GLuint, const GLchar*);
typedef void   (*PFN_glUniform1i)(GLint, GLint);
typedef void   (*PFN_glUniform1f)(GLint, GLfloat);
typedef void   (*PFN_glUniform2f)(GLint, GLfloat, GLfloat);
typedef void   (*PFN_glUniform3f)(GLint, GLfloat, GLfloat, GLfloat);
typedef void   (*PFN_glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat*);
typedef void   (*PFN_glBindAttribLocation)(GLuint, GLuint, const GLchar*);
typedef void   (*PFN_glGenFramebuffers)(GLsizei, GLuint*);
typedef void   (*PFN_glBindFramebuffer)(GLenum, GLuint);
typedef void   (*PFN_glDeleteFramebuffers)(GLsizei, const GLuint*);
typedef void   (*PFN_glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef GLenum (*PFN_glCheckFramebufferStatus)(GLenum);
typedef void   (*PFN_glGenRenderbuffers)(GLsizei, GLuint*);
typedef void   (*PFN_glBindRenderbuffer)(GLenum, GLuint);
typedef void   (*PFN_glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei);
typedef void   (*PFN_glFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint);
typedef void   (*PFN_glDeleteRenderbuffers)(GLsizei, const GLuint*);
typedef void   (*PFN_glEnableVertexAttribArray)(GLuint);
typedef void   (*PFN_glDisableVertexAttribArray)(GLuint);
typedef void   (*PFN_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);

// ============================================================
// GL Function Pointers
// ============================================================
static PFN_glCreateShader            p_glCreateShader            = nullptr;
static PFN_glShaderSource            p_glShaderSource            = nullptr;
static PFN_glCompileShader           p_glCompileShader           = nullptr;
static PFN_glGetShaderiv             p_glGetShaderiv             = nullptr;
static PFN_glGetShaderInfoLog        p_glGetShaderInfoLog        = nullptr;
static PFN_glDeleteShader            p_glDeleteShader            = nullptr;
static PFN_glCreateProgram           p_glCreateProgram           = nullptr;
static PFN_glAttachShader            p_glAttachShader            = nullptr;
static PFN_glLinkProgram             p_glLinkProgram             = nullptr;
static PFN_glGetProgramiv            p_glGetProgramiv            = nullptr;
static PFN_glGetProgramInfoLog       p_glGetProgramInfoLog       = nullptr;
static PFN_glUseProgram              p_glUseProgram              = nullptr;
static PFN_glDeleteProgram           p_glDeleteProgram           = nullptr;
static PFN_glGetUniformLocation      p_glGetUniformLocation      = nullptr;
static PFN_glUniform1i               p_glUniform1i               = nullptr;
static PFN_glUniform1f               p_glUniform1f               = nullptr;
static PFN_glUniform2f               p_glUniform2f               = nullptr;
static PFN_glUniform3f               p_glUniform3f               = nullptr;
static PFN_glUniformMatrix4fv        p_glUniformMatrix4fv        = nullptr;
static PFN_glBindAttribLocation      p_glBindAttribLocation      = nullptr;
static PFN_glGenFramebuffers         p_glGenFramebuffers         = nullptr;
static PFN_glBindFramebuffer         p_glBindFramebuffer         = nullptr;
static PFN_glDeleteFramebuffers      p_glDeleteFramebuffers      = nullptr;
static PFN_glFramebufferTexture2D    p_glFramebufferTexture2D    = nullptr;
static PFN_glCheckFramebufferStatus  p_glCheckFramebufferStatus  = nullptr;
static PFN_glGenRenderbuffers        p_glGenRenderbuffers        = nullptr;
static PFN_glBindRenderbuffer        p_glBindRenderbuffer        = nullptr;
static PFN_glRenderbufferStorage     p_glRenderbufferStorage     = nullptr;
static PFN_glFramebufferRenderbuffer p_glFramebufferRenderbuffer = nullptr;
static PFN_glDeleteRenderbuffers     p_glDeleteRenderbuffers     = nullptr;
static PFN_glEnableVertexAttribArray p_glEnableVertexAttribArray = nullptr;
static PFN_glDisableVertexAttribArray p_glDisableVertexAttribArray = nullptr;
static PFN_glVertexAttribPointer     p_glVertexAttribPointer     = nullptr;

static bool loadGLFunctions() {
    #define LOAD(name) p_##name = (PFN_##name)SDL_GL_GetProcAddress(#name)
    LOAD(glCreateShader);
    LOAD(glShaderSource);
    LOAD(glCompileShader);
    LOAD(glGetShaderiv);
    LOAD(glGetShaderInfoLog);
    LOAD(glDeleteShader);
    LOAD(glCreateProgram);
    LOAD(glAttachShader);
    LOAD(glLinkProgram);
    LOAD(glGetProgramiv);
    LOAD(glGetProgramInfoLog);
    LOAD(glUseProgram);
    LOAD(glDeleteProgram);
    LOAD(glGetUniformLocation);
    LOAD(glUniform1i);
    LOAD(glUniform1f);
    LOAD(glUniform2f);
    LOAD(glUniform3f);
    LOAD(glUniformMatrix4fv);
    LOAD(glBindAttribLocation);
    LOAD(glGenFramebuffers);
    LOAD(glBindFramebuffer);
    LOAD(glDeleteFramebuffers);
    LOAD(glFramebufferTexture2D);
    LOAD(glCheckFramebufferStatus);
    LOAD(glGenRenderbuffers);
    LOAD(glBindRenderbuffer);
    LOAD(glRenderbufferStorage);
    LOAD(glFramebufferRenderbuffer);
    LOAD(glDeleteRenderbuffers);
    LOAD(glEnableVertexAttribArray);
    LOAD(glDisableVertexAttribArray);
    LOAD(glVertexAttribPointer);
    #undef LOAD
    if (!p_glCreateShader || !p_glCreateProgram || !p_glUseProgram ||
        !p_glGenFramebuffers || !p_glBindFramebuffer) {
        fprintf(stderr, "loadGLFunctions: critical functions missing\n");
        return false;
    }
    return true;
}

// ============================================================
// Shader Sources
// ============================================================
static const char* postVertSrc =
    "#version 120\n"
    "attribute vec2 aPos;\n"
    "varying vec2 vTC;\n"
    "void main(){vTC=aPos*0.5+0.5;gl_Position=vec4(aPos,0,1);}\n";

static const char* waterVertSrc =
    "#version 120\n"
    "attribute vec3 aPos;\n"
    "uniform mat4 uMVP;\n"
    "uniform float uTime;\n"
    "varying vec3 vWP;\n"
    "void main(){\n"
    "  vec3 p=aPos;\n"
    "  float w=sin(p.x*0.3+uTime*1.5)*0.25+cos(p.z*0.25+uTime*1.1)*0.18\n"
    "         +sin((p.x+p.z)*0.15+uTime*0.8)*0.12;\n"
    "  p.y+=w;vWP=p;\n"
    "  gl_Position=uMVP*vec4(p,1);\n"
    "}\n";

static const char* waterFragSrc =
    "#version 120\n"
    "varying vec3 vWP;\n"
    "uniform float uTime;\n"
    "uniform vec3 uCamPos;\n"
    "vec3 hsv2rgb(vec3 c){\n"
    "  vec4 K=vec4(1,2./3.,1./3.,3.);\n"
    "  vec3 p=abs(fract(c.xxx+K.xyz)*6.-K.www);\n"
    "  return c.z*mix(K.xxx,clamp(p-K.xxx,0.,1.),c.y);\n"
    "}\n"
    "void main(){\n"
    "  float hue=fract(vWP.x*0.008+vWP.z*0.008+uTime*0.08);\n"
    "  vec3 base=hsv2rgb(vec3(hue,0.5,0.65));\n"
    "  float dx=cos(vWP.x*0.3+uTime*1.5)*0.3*0.25\n"
    "          +cos(vWP.x*0.7+uTime*2.1)*0.1*0.15\n"
    "          +sin(vWP.x*1.2+uTime*3.0)*0.05*0.08;\n"
    "  float dz=-sin(vWP.z*0.25+uTime*1.1)*0.25*0.18\n"
    "           -sin(vWP.z*0.6+uTime*1.8)*0.12*0.12\n"
    "           -cos(vWP.z*1.0+uTime*2.7)*0.06*0.07;\n"
    "  vec3 N=normalize(vec3(-dx,1,-dz));\n"
    "  vec3 V=normalize(uCamPos-vWP);\n"
    "  float fres=pow(1.-max(dot(V,N),0.),3.);\n"
    "  vec3 L=normalize(vec3(0.3,0.8,0.5));\n"
    "  float spec=pow(max(dot(N,normalize(V+L)),0.),120.)*1.2;\n"
    "  float spec2=pow(max(dot(N,normalize(V+normalize(vec3(-0.5,0.6,-0.3)))),0.),40.)*0.4;\n"
    "  vec3 col=base+spec*0.9+fres*0.3+spec2*vec3(0.6,0.8,1.0);\n"
    "  float a=0.6+fres*0.35;\n"
    "  gl_FragColor=vec4(col*1.4,a);\n"
    "}\n";

static const char* extractFragSrc =
    "#version 120\n"
    "varying vec2 vTC;\n"
    "uniform sampler2D uTex;\n"
    "uniform float uThresh;\n"
    "void main(){\n"
    "  vec4 c=texture2D(uTex,vTC);\n"
    "  float b=dot(c.rgb,vec3(0.2126,0.7152,0.0722));\n"
    "  gl_FragColor=b>uThresh?vec4(c.rgb*(b-uThresh)*2.5,1):vec4(0,0,0,1);\n"
    "}\n";

static const char* blurFragSrc =
    "#version 120\n"
    "varying vec2 vTC;\n"
    "uniform sampler2D uTex;\n"
    "uniform vec2 uDir;\n"
    "uniform vec2 uSize;\n"
    "void main(){\n"
    "  vec2 o=uDir/uSize;\n"
    "  vec4 c=vec4(0);\n"
    "  c+=texture2D(uTex,vTC-4.*o)*0.0162;\n"
    "  c+=texture2D(uTex,vTC-3.*o)*0.0540;\n"
    "  c+=texture2D(uTex,vTC-2.*o)*0.1216;\n"
    "  c+=texture2D(uTex,vTC-1.*o)*0.1945;\n"
    "  c+=texture2D(uTex,vTC    )*0.2270;\n"
    "  c+=texture2D(uTex,vTC+1.*o)*0.1945;\n"
    "  c+=texture2D(uTex,vTC+2.*o)*0.1216;\n"
    "  c+=texture2D(uTex,vTC+3.*o)*0.0540;\n"
    "  c+=texture2D(uTex,vTC+4.*o)*0.0162;\n"
    "  gl_FragColor=c;\n"
    "}\n";

static const char* compositeFragSrc =
    "#version 120\n"
    "varying vec2 vTC;\n"
    "uniform sampler2D uScene;\n"
    "uniform sampler2D uBloom;\n"
    "uniform float uStr;\n"
    "void main(){\n"
    "  vec3 s=texture2D(uScene,vTC).rgb;\n"
    "  vec3 b=texture2D(uBloom,vTC).rgb;\n"
    "  vec3 c=s+b*uStr;\n"
    "  c=clamp(c*1.2,0.0,1.0);\n"
    "  vec2 uv=vTC*2.-1.;\n"
    "  c*=1.-dot(uv*0.2,uv*0.2);\n"
    "  gl_FragColor=vec4(c,1);\n"
    "}\n";

// ============================================================
// Global State
// ============================================================
static SDL_Window*    gWindow     = nullptr;
static SDL_GLContext   gGL         = nullptr;
static bool           gRunning    = true;
static Uint32         gTicks      = 0;
static float          gDelta      = 0.0f;
static int            gFPS        = 0;
static int            gFrameCnt   = 0;
static Uint32         gFPSTimer   = 0;
static int            gWinW       = SCREEN_W;
static int            gWinH       = SCREEN_H;

static float gCarX      = 0.0f;
static float gCarZ      = 0.0f;
static float gCarAngle  = 0.0f;
static float gCarSpeed  = 0.0f;
static float gCamDist   = 22.0f;
static float gCamHeight = 10.0f;
static float gCamAngleH = 0.0f;
static float gCamAngleV = 0.3f;
static bool  gCamFree   = false;
static bool  gBloomOn   = true;
static float gSteerAngle= 0.0f;
static float gWheelSpin = 0.0f;

static const Uint8* gKeys = nullptr;
static const int NUM_PARTICLES = 500;
    static Particle gParticles[500];

// Shader programs
static bool  gShadersOK      = false;
static GLuint gWaterProg     = 0;
static GLuint gExtractProg   = 0;
static GLuint gBlurProg      = 0;
static GLuint gCompositeProg = 0;
static GLuint gPostVert      = 0;

// FBO
static GLuint gSceneFBO     = 0;
static GLuint gSceneTex     = 0;
static GLuint gSceneDepthRB = 0;
static GLuint gPPFBO[2]     = {0,0};
static GLuint gPPTex[2]     = {0,0};

// ============================================================
// Utility
// ============================================================
static float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static void hsv2rgb(float h, float s, float v, float& r, float& g, float& b) {
    int   i = (int)(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default: r = v; g = t; b = p; break;
    }
}

static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = p_glCreateShader(type);
    p_glShaderSource(s, 1, &src, nullptr);
    p_glCompileShader(s);
    GLint ok = 0;
    p_glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        p_glGetShaderInfoLog(s, 512, nullptr, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
        p_glDeleteShader(s);
        return 0;
    }
    return s;
}

static GLuint linkProgram(GLuint vs, GLuint fs) {
    GLuint p = p_glCreateProgram();
    p_glAttachShader(p, vs);
    p_glAttachShader(p, fs);
    p_glBindAttribLocation(p, 0, "aPos");
    p_glLinkProgram(p);
    GLint ok = 0;
    p_glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        p_glGetProgramInfoLog(p, 512, nullptr, log);
        fprintf(stderr, "Program link error: %s\n", log);
        p_glDeleteProgram(p);
        return 0;
    }
    return p;
}

static GLuint createPostProgram(const char* fragSrc) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, postVertSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!vs || !fs) { return 0; }
    GLuint p = linkProgram(vs, fs);
    p_glDeleteShader(vs);
    p_glDeleteShader(fs);
    return p;
}

static void computeMVP(float* out) {
    float mv[16], proj[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++) {
            out[c*4+r] = 0;
            for (int k = 0; k < 4; k++)
                out[c*4+r] += proj[k*4+r] * mv[c*4+k];
        }
}

// ============================================================
// Terrain (NO river, NO collision blocking)
// ============================================================
static float terrainHeight(float wx, float wz) {
    float h = 0.0f;
    h += sinf(wx*0.03f)*cosf(wz*0.04f)*TERRAIN_H;
    h += sinf(wx*0.07f+1.3f)*cosf(wz*0.05f+0.7f)*TERRAIN_H*0.4f;
    h += sinf(wx*0.13f+2.1f)*cosf(wz*0.11f+3.2f)*TERRAIN_H*0.15f;
    h += cosf(wx*0.02f-wz*0.02f)*TERRAIN_H*0.3f;
    return h;
}

// ============================================================
// Drawing Helpers
// ============================================================
static void drawBox(float w, float h, float d) {
    float hw=w*0.5f, hh=h*0.5f, hd=d*0.5f;
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    glVertex3f(-hw,-hh,hd); glVertex3f(hw,-hh,hd);
    glVertex3f(hw,hh,hd);   glVertex3f(-hw,hh,hd);
    glNormal3f(0,0,-1);
    glVertex3f(hw,-hh,-hd); glVertex3f(-hw,-hh,-hd);
    glVertex3f(-hw,hh,-hd); glVertex3f(hw,hh,-hd);
    glNormal3f(0,1,0);
    glVertex3f(-hw,hh,hd);  glVertex3f(hw,hh,hd);
    glVertex3f(hw,hh,-hd);  glVertex3f(-hw,hh,-hd);
    glNormal3f(0,-1,0);
    glVertex3f(-hw,-hh,-hd); glVertex3f(hw,-hh,-hd);
    glVertex3f(hw,-hh,hd);   glVertex3f(-hw,-hh,hd);
    glNormal3f(1,0,0);
    glVertex3f(hw,-hh,hd);  glVertex3f(hw,-hh,-hd);
    glVertex3f(hw,hh,-hd);  glVertex3f(hw,hh,hd);
    glNormal3f(-1,0,0);
    glVertex3f(-hw,-hh,-hd); glVertex3f(-hw,-hh,hd);
    glVertex3f(-hw,hh,hd);   glVertex3f(-hw,hh,-hd);
    glEnd();
}

// ============================================================
// Terrain Drawing (improved coloring)
// ============================================================
static void terrainVertexColor(float h, float wx, float wz, float time,
                               float& r, float& g, float& b) {
    if (h > 10.0f) {
        float snow = clampf((h-10.0f)*0.12f,0,1);
        float sparkle = sinf(wx*0.5f+time*2.0f)*sinf(wz*0.5f+time*1.7f)*0.08f;
        r = 0.85f+snow*0.15f+sparkle; g = 0.88f+snow*0.12f+sparkle; b = 0.95f+snow*0.05f;
    } else if (h > 5.0f) {
        float t = (h-5.0f)/5.0f;
        hsv2rgb(0.72f+t*0.1f, 0.3f+t*0.2f, 0.55f+t*0.25f, r,g,b);
    } else if (h > -1.0f) {
        float t = (h+1.0f)/6.0f;
        float flower = sinf(wx*0.3f+time*0.5f)*sinf(wz*0.3f+time*0.3f);
        if (flower > 0.75f) {
            hsv2rgb(fmodf(wx*0.1f+time*0.1f,1.0f), 0.7f, 0.8f, r,g,b);
        } else {
            r = 0.15f+t*0.25f+sinf(wx*0.05f+time*0.3f)*0.05f;
            g = 0.42f+t*0.25f+sinf(wz*0.04f+time*0.2f)*0.05f;
            b = 0.12f+t*0.08f;
        }
    } else if (h > -3.0f) {
        float t = (-h-1.0f)/2.0f;
        hsv2rgb(0.78f+sinf(time*0.5f)*0.08f, 0.5f+t*0.2f, 0.42f+t*0.25f, r,g,b);
    } else {
        float glow = sinf(time*2.0f+h)*0.12f;
        r = 0.9f+glow; g = 0.25f; b = 0.08f;
    }
}

static void drawTerrain(float cx, float cz, float time) {
    int baseX = (int)(cx / TERRAIN_CS);
    int baseZ = (int)(cz / TERRAIN_CS);
    int half = TERRAIN_GR / 2;
    int verts = TERRAIN_GR + 1;

    glDisable(GL_LIGHTING);

    float lx=0.3f, ly=0.8f, lz=0.5f;
    float llen=sqrtf(lx*lx+ly*ly+lz*lz);
    lx/=llen; ly/=llen; lz/=llen;

    float h[65*65];
    float cr[65*65], cg_[65*65], cb[65*65];
    float nx[65*65], ny[65*65], nz_[65*65];

    for(int gz=0; gz<verts; gz++){
        for(int gx=0; gx<verts; gx++){
            float wx=(baseX+gx-half)*TERRAIN_CS;
            float wz=(baseZ+gz-half)*TERRAIN_CS;
            float hh=terrainHeight(wx,wz);
            h[gz*verts+gx]=hh;
            terrainVertexColor(hh,wx,wz,time, cr[gz*verts+gx], cg_[gz*verts+gx], cb[gz*verts+gx]);
        }
    }

    for(int gz=0; gz<verts; gz++){
        for(int gx=0; gx<verts; gx++){
            float hL = gx>0      ? h[gz*verts+gx-1] : h[gz*verts+gx];
            float hR = gx<verts-1? h[gz*verts+gx+1] : h[gz*verts+gx];
            float hD = gz>0      ? h[(gz-1)*verts+gx] : h[gz*verts+gx];
            float hU = gz<verts-1? h[(gz+1)*verts+gx] : h[gz*verts+gx];
            float nnx = hL - hR;
            float nnz = hD - hU;
            float nny = 2.0f * TERRAIN_CS;
            float len=sqrtf(nnx*nnx+nny*nny+nnz*nnz)+0.001f;
            nx[gz*verts+gx]=nnx/len; ny[gz*verts+gx]=nny/len; nz_[gz*verts+gx]=nnz/len;
        }
    }

    for(int gz=0; gz<half+half; gz++){
        glBegin(GL_TRIANGLES);
        for(int gx=0; gx<half+half; gx++){
            int i00=gz*verts+gx, i10=gz*verts+gx+1, i01=(gz+1)*verts+gx, i11=(gz+1)*verts+gx+1;

            float ndotl0=nx[i00]*lx+ny[i00]*ly+nz_[i00]*lz;
            if(ndotl0<0)ndotl0=-ndotl0; float sh0=0.45f+0.55f*ndotl0;
            float ndotl1=nx[i10]*lx+ny[i10]*ly+nz_[i10]*lz;
            if(ndotl1<0)ndotl1=-ndotl1; float sh1=0.45f+0.55f*ndotl1;
            float ndotl2=nx[i01]*lx+ny[i01]*ly+nz_[i01]*lz;
            if(ndotl2<0)ndotl2=-ndotl2; float sh2=0.45f+0.55f*ndotl2;
            float ndotl3=nx[i11]*lx+ny[i11]*ly+nz_[i11]*lz;
            if(ndotl3<0)ndotl3=-ndotl3; float sh3=0.45f+0.55f*ndotl3;

            glColor3f(cr[i00]*sh0,cg_[i00]*sh0,cb[i00]*sh0);
            glVertex3f((baseX+gx-half)*TERRAIN_CS,h[i00],(baseZ+gz-half)*TERRAIN_CS);
            glColor3f(cr[i10]*sh1,cg_[i10]*sh1,cb[i10]*sh1);
            glVertex3f((baseX+gx+1-half)*TERRAIN_CS,h[i10],(baseZ+gz-half)*TERRAIN_CS);
            glColor3f(cr[i01]*sh2,cg_[i01]*sh2,cb[i01]*sh2);
            glVertex3f((baseX+gx-half)*TERRAIN_CS,h[i01],(baseZ+gz+1-half)*TERRAIN_CS);

            glColor3f(cr[i10]*sh1,cg_[i10]*sh1,cb[i10]*sh1);
            glVertex3f((baseX+gx+1-half)*TERRAIN_CS,h[i10],(baseZ+gz-half)*TERRAIN_CS);
            glColor3f(cr[i11]*sh3,cg_[i11]*sh3,cb[i11]*sh3);
            glVertex3f((baseX+gx+1-half)*TERRAIN_CS,h[i11],(baseZ+gz+1-half)*TERRAIN_CS);
            glColor3f(cr[i01]*sh2,cg_[i01]*sh2,cb[i01]*sh2);
            glVertex3f((baseX+gx-half)*TERRAIN_CS,h[i01],(baseZ+gz+1-half)*TERRAIN_CS);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

// ============================================================
// Terrain Glow Veins (additive blended points)
// ============================================================
static void drawTerrainVeins(float cx, float cz, float time) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    srand(33333);
    for (int i = 0; i < 150; i++) {
        float vx = ((float)rand()/RAND_MAX-0.5f)*250.0f+cx;
        float vz = ((float)rand()/RAND_MAX-0.5f)*250.0f+cz;
        float vy = terrainHeight(vx,vz);
        if (vy < -2.5f) continue;

        float pulse = 0.4f+0.6f*sinf(time*2.0f+i*1.7f);
        float hue = fmodf((float)i/150.0f+time*0.06f,1.0f);
        float vr,vg,vb;
        hsv2rgb(hue, 0.9f, 1.0f, vr,vg,vb);

        glPointSize(4.0f+pulse*5.0f);
        glBegin(GL_POINTS);
        glColor4f(vr*pulse, vg*pulse, vb*pulse, 0.5f+pulse*0.4f);
        glVertex3f(vx, vy+0.15f, vz);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============================================================
// Water (Shader-Based)
// ============================================================
static void drawWater(float cx, float cz, float time) {
    if (!gShadersOK) return;

    float waterVerts[] = {
        cx-150.0f, -3.0f, cz-150.0f,
        cx+150.0f, -3.0f, cz-150.0f,
        cx+150.0f, -3.0f, cz+150.0f,
        cx-150.0f, -3.0f, cz+150.0f
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    p_glUseProgram(gWaterProg);
    float mvp[16];
    computeMVP(mvp);
    p_glUniformMatrix4fv(p_glGetUniformLocation(gWaterProg,"uMVP"),1,GL_FALSE,mvp);
    p_glUniform1f(p_glGetUniformLocation(gWaterProg,"uTime"),time);

    float carY = terrainHeight(gCarX,gCarZ)+1.5f;
    p_glUniform3f(p_glGetUniformLocation(gWaterProg,"uCamPos"),
                  gCamFree ? gCarX : gCarX-sinf(gCarAngle*PI/180.0f)*gCamDist,
                  gCamFree ? gCamHeight : carY+gCamHeight,
                  gCamFree ? gCarZ : gCarZ-cosf(gCarAngle*PI/180.0f)*gCamDist);

    p_glEnableVertexAttribArray(0);
    p_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,waterVerts);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    p_glDisableVertexAttribArray(0);

    p_glUseProgram(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

// ============================================================
// Wheel
// ============================================================
static void drawWheel(float x, float y, float z, float spin) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(90,0,1,0);
    glRotatef(spin,0,0,1);
    int segs=12; float r=0.45f, w=0.3f;
    glColor3f(0.1f,0.1f,0.12f);
    glBegin(GL_QUAD_STRIP);
    for(int i=0;i<=segs;i++){
        float a=(float)i/segs*2*PI;
        float ca=cosf(a),sa=sinf(a);
        glNormal3f(ca,sa,0);
        glVertex3f(ca*r,sa*r,w);
        glVertex3f(ca*r,sa*r,-w);
    }
    glEnd();
    glColor3f(0.15f,0.15f,0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,0,1);
    glVertex3f(0,0,w);
    for(int i=0;i<=segs;i++){
        float a=(float)i/segs*2*PI;
        glVertex3f(cosf(a)*r,sinf(a)*r,w);
    }
    glEnd();
    glPopMatrix();
}

// ============================================================
// Car
// ============================================================
static void drawCar(float x, float y, float z, float angle, float time) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(angle,0,1,0);
    float pulse = 0.85f+0.15f*sinf(time*3.0f);

    glColor3f(0.1f*pulse,0.02f,0.3f);
    drawBox(2.0f,0.7f,4.2f);
    glPushMatrix();
    glTranslatef(0,0.55f,-0.2f);
    glColor3f(0.15f*pulse,0.0f,0.4f);
    drawBox(1.7f,0.55f,2.4f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.85f,-0.3f);
    glColor3f(0.2f,0.4f,0.8f);
    drawBox(1.55f,0.45f,2.1f);
    glPopMatrix();

    float hg = 0.8f+0.2f*sinf(time*5.0f);
    glColor3f(0.9f*hg,0.85f*hg,0.1f);
    glPushMatrix(); glTranslatef(0.65f,-0.1f,2.15f); drawBox(0.35f,0.18f,0.05f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.65f,-0.1f,2.15f); drawBox(0.35f,0.18f,0.05f); glPopMatrix();
    glColor3f(0.8f,0.0f,0.1f);
    glPushMatrix(); glTranslatef(0.65f,-0.1f,-2.15f); drawBox(0.3f,0.15f,0.05f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.65f,-0.1f,-2.15f); drawBox(0.3f,0.15f,0.05f); glPopMatrix();

    float neon = 0.5f+0.5f*sinf(time*4.0f);
    glColor3f(0.0f,0.8f*neon,1.0f*neon);
    glPushMatrix(); glTranslatef(0,-0.36f,0); drawBox(2.1f,0.1f,4.3f); glPopMatrix();
    glColor3f(1.0f*neon,0.0f,0.8f*neon);
    glPushMatrix(); glTranslatef(0,0.35f,1.6f); drawBox(1.8f,0.04f,0.8f); glPopMatrix();
    glPushMatrix(); glTranslatef(0,0.35f,-1.8f); drawBox(1.6f,0.04f,0.5f); glPopMatrix();

    drawWheel( 1.05f,-0.35f, 1.4f,gWheelSpin);
    drawWheel(-1.05f,-0.35f, 1.4f,gWheelSpin);
    drawWheel( 1.05f,-0.35f,-1.4f,gWheelSpin);
    drawWheel(-1.05f,-0.35f,-1.4f,gWheelSpin);
    glPopMatrix();
}

// ============================================================
// Tree
// ============================================================
static void drawTree(float x, float z, float time) {
    float y = terrainHeight(x,z);
    if (y < -1.0f) return;
    glPushMatrix();
    glTranslatef(x,y,z);
    float hue = fmodf(x*0.01f+z*0.01f,1.0f);
    float tr,tg,tb;
    hsv2rgb(hue,0.3f,0.3f,tr,tg,tb);
    glColor3f(tr,tg,tb);
    drawBox(0.4f,3.0f,0.4f);
    float lr=0,lg=0,lb=0;
    for(int i=0;i<3;i++){
        float ly=1.5f+i*1.2f, s=2.0f-i*0.5f;
        float lH=fmodf(hue+i*0.1f+time*0.05f,1.0f);
        hsv2rgb(lH,0.7f,0.5f+i*0.1f,lr,lg,lb);
        glPushMatrix();
        glTranslatef(0,ly,0);
        glColor3f(lr,lg,lb);
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0,1,0);
        glVertex3f(0,s,0);
        for(int j=0;j<=10;j++){
            float a=(float)j/10.0f*2*PI;
            glNormal3f(cosf(a)*0.5f,0.5f,sinf(a)*0.5f);
            glVertex3f(cosf(a)*s*0.5f,0,sinf(a)*s*0.5f);
        }
        glEnd();
        glPopMatrix();
    }
    float gp=0.5f+0.4f*sinf(time*2.0f+x*0.3f+z*0.3f);
    glColor4f(lr*gp*1.5f,lg*gp*1.5f,lb*gp*1.5f,0.8f);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glVertex3f(0,3.5f,0);
    glEnd();
    glPopMatrix();
}

// ============================================================
// Crystal
// ============================================================
static void drawCrystal(float x, float z, float height, float cr, float cg, float cb, float time) {
    float y = terrainHeight(x,z);
    glPushMatrix();
    glTranslatef(x,y,z);
    float pulse = 0.6f+0.4f*sinf(time*2.5f+x*0.5f);
    float base=0.8f, tip=0.05f;
    int sides=6;

    glColor3f(cr*pulse*1.2f,cg*pulse*1.2f,cb*pulse*1.2f);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glVertex3f(0,height,0);
    for(int i=0;i<=sides;i++){
        float a=(float)i/sides*2*PI;
        glNormal3f(cosf(a)*0.7f,0.3f,sinf(a)*0.7f);
        glVertex3f(cosf(a)*tip,0,sinf(a)*tip);
    }
    glEnd();

    float band=0.6f+0.4f*sinf(time*3.0f+height);
    glColor3f(cr*band,cg*band,cb*band);
    glBegin(GL_QUAD_STRIP);
    for(int i=0;i<=sides;i++){
        float a=(float)i/sides*2*PI;
        float ca=cosf(a),sa=sinf(a);
        float wobble=1.0f+0.1f*sinf(time*4.0f+a*2.0f+height);
        glNormal3f(ca*0.7f,0.3f,sa*0.7f);
        glVertex3f(ca*base*wobble,0,sa*base*wobble);
        glVertex3f(ca*tip,height,sa*tip);
    }
    glEnd();

    float gr=cr*pulse*2.0f, gg=cg*pulse*2.0f, gb=cb*pulse*2.0f;
    gr=gr>1?1:gr; gg=gg>1?1:gg; gb=gb>1?1:gb;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glColor4f(gr,gg,gb,0.5f*pulse);
    glPointSize(40.0f*pulse);
    glBegin(GL_POINTS);
    glVertex3f(0,height*0.5f,0);
    glEnd();
    glPointSize(24.0f*pulse);
    glBegin(GL_POINTS);
    glVertex3f(0,height,0);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ============================================================
// Mushroom
// ============================================================
static void drawMushroom(float x, float z, float time) {
    float y = terrainHeight(x,z);
    if (y < -1.0f) return;
    glPushMatrix();
    glTranslatef(x,y,z);
    glColor3f(0.3f,0.25f,0.2f);
    drawBox(0.15f,0.8f,0.15f);
    float hue=fmodf(x*0.02f+z*0.02f+time*0.1f,1.0f);
    float mr,mg,mb;
    hsv2rgb(hue,0.8f,0.7f,mr,mg,mb);
    float gp=0.7f+0.3f*sinf(time*3.0f+x+z);
    glColor3f(mr*gp,mg*gp,mb*gp);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glVertex3f(0,0.9f,0);
    for(int i=0;i<=10;i++){
        float a=(float)i/10.0f*2*PI;
        glNormal3f(cosf(a)*0.3f,0.7f,sinf(a)*0.3f);
        glVertex3f(cosf(a)*0.5f,0.7f,sinf(a)*0.5f);
    }
    glEnd();
    glColor4f(mr,mg,mb,0.4f*gp);
    glPointSize(10.0f*gp);
    glBegin(GL_POINTS);
    glVertex3f(0,1.0f,0);
    glEnd();
    glPopMatrix();
}

// ============================================================
// Floating Island
// ============================================================
static void drawFloatingIsland(float x, float y, float z, float size, float time) {
    float bob = sinf(time*0.5f+x*0.1f)*1.5f;
    glPushMatrix();
    glTranslatef(x,y+bob,z);
    int segs=10;
    float hue=fmodf(x*0.005f+z*0.005f,1.0f);
    float ir,ig,ib;
    hsv2rgb(hue,0.4f,0.35f,ir,ig,ib);
    glColor3f(ir,ig,ib);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glVertex3f(0,0.5f,0);
    for(int i=0;i<=segs;i++){
        float a=(float)i/segs*2*PI;
        glVertex3f(cosf(a)*size,0,sinf(a)*size);
    }
    glEnd();
    hsv2rgb(fmodf(hue+0.3f,1.0f),0.7f,0.45f,ir,ig,ib);
    glColor3f(ir,ig,ib);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glVertex3f(0,0.6f,0);
    for(int i=0;i<=segs;i++){
        float a=(float)i/segs*2*PI;
        glVertex3f(cosf(a)*size*0.85f,0.4f,sinf(a)*size*0.85f);
    }
    glEnd();
    glColor3f(0.35f,0.25f,0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glVertex3f(0,-size*0.6f,0);
    for(int i=segs;i>=0;i--){
        float a=(float)i/segs*2*PI;
        glNormal3f(cosf(a)*0.5f,-0.5f,sinf(a)*0.5f);
        glVertex3f(cosf(a)*size,0,sinf(a)*size);
    }
    glEnd();

    float eg=0.5f+0.5f*sinf(time*1.5f+x+z);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glColor4f(0.3f,0.8f*eg,1.0f*eg,0.3f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for(int i=0;i<8;i++){
        float a=(float)i/8*2*PI;
        glVertex3f(cosf(a)*size,0.1f,sinf(a)*size);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    drawTree(x*0.7f,z*0.9f,time);
    drawTree(x*0.7f+2.0f,z*0.9f+1.5f,time);
    drawMushroom(x*0.5f+1.0f,z*0.5f-1.0f,time);
    glPopMatrix();
}

// ============================================================
// Sky
// ============================================================
static void drawSky(float time) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float t=time*0.05f;
    glBegin(GL_QUADS);
    float r1=0.18f+sinf(t)*0.10f, g1=0.08f+sinf(t+1)*0.06f, b1=0.38f+sinf(t+2)*0.15f;
    glColor3f(r1,g1,b1); glVertex2f(-1,-0.2f); glVertex2f(1,-0.2f);
    float r2=0.22f+sinf(t*1.3f)*0.10f, g2=0.08f+sinf(t*1.3f+0.5f)*0.05f, b2=0.50f+sinf(t*1.3f+1)*0.15f;
    glColor3f(r2,g2,b2); glVertex2f(1,0.3f); glVertex2f(-1,0.3f);
    float r3=0.28f+sinf(t*0.7f)*0.10f, g3=0.08f+sinf(t*0.7f+1.5f)*0.06f, b3=0.60f+sinf(t*0.7f+3)*0.15f;
    glColor3f(r3,g3,b3); glVertex2f(-1,0.3f); glVertex2f(1,0.3f);
    float r4=0.12f+sinf(t*0.5f)*0.06f, b4=0.30f+sinf(t*0.5f+2)*0.10f;
    glColor3f(r4,0.04f,b4); glVertex2f(1,1); glVertex2f(-1,1);
    glEnd();

    srand(42);
    glPointSize(5.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POINTS);
    for(int i=0;i<250;i++){
        float sx=((float)rand()/RAND_MAX)*2-1;
        float sy=((float)rand()/RAND_MAX)*0.7f+0.3f;
        float br=0.2f+0.8f*fabsf(sinf(time*2.0f+i*1.7f)*sinf(time*3.7f+i*2.3f));
        float sh=(float)rand()/RAND_MAX;
        float sr,sg,sb;
        hsv2rgb(sh,0.4f,br*1.2f,sr,sg,sb);
        float glow=br>0.6f?br*1.8f:br;
        glColor3f(sr*glow,sg*glow,sb*glow);
        glVertex2f(sx,sy);
    }
    glEnd();
    glDisable(GL_BLEND);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for(int band=0;band<8;band++){
        float bBaseY=0.15f+band*0.09f;
        float bH=fmodf(time*0.06f+band*0.18f,1.0f);
        float br,bg,bb;
        hsv2rgb(bH,0.7f,1.0f,br,bg,bb);
        glBegin(GL_QUADS);
        for(int j=0;j<30;j++){
            float xo=((float)j/30)*2.6f-1.3f;
            float wave=sinf(time*0.7f+xo*2.0f+band*1.5f)*0.08f;
            float wave2=sinf(time*1.3f+xo*4.0f+band*0.8f)*0.05f;
            float curtainH=0.14f+0.1f*sinf(time*0.5f+j*0.7f+band);
            float fadeH=1.0f-fabsf(xo);
            if(fadeH<0) fadeH=0;
            float fadeV;
            float shimmer=0.5f+0.5f*sinf(time*3.0f+j*1.1f+band*2.0f);
            float stripe=0.5f+0.5f*sinf(time*2.0f+xo*15.0f+band*3.0f);
            float intensity=fadeH*shimmer*stripe;
            float aBase=0.28f*intensity;
            float xW=0.015f;
            fadeV=1.0f;
            glColor4f(br*intensity,bg*intensity,bb*intensity,aBase*fadeV);
            glVertex2f(xo-xW,bBaseY+wave+curtainH);
            glVertex2f(xo+xW,bBaseY+wave2+curtainH);
            fadeV=0.3f;
            glColor4f(br*intensity,bg*intensity,bb*intensity,aBase*fadeV);
            glVertex2f(xo+xW,bBaseY+wave2);
            glVertex2f(xo-xW,bBaseY+wave);
        }
        glEnd();
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPointSize(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ============================================================
// Particles
// ============================================================
static void drawParticles() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for(int i=0;i<NUM_PARTICLES;i++){
        Particle& p=gParticles[i];
        float a=p.life>1?1:p.life;
        glColor4f(p.r,p.g,p.b,a*0.9f);
        glVertex3f(p.px,p.py,p.pz);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void updateParticles(float cx, float cz) {
    for(int i=0;i<NUM_PARTICLES;i++){
        Particle& p=gParticles[i];
        p.px+=p.vx; p.py+=p.vy; p.pz+=p.vz;
        p.life-=gDelta*0.1f;
        if(p.life<=0||p.py>60){
            p.px=cx+((float)rand()/RAND_MAX-0.5f)*200;
            p.py=0.5f+(float)rand()/RAND_MAX*40;
            p.pz=cz+((float)rand()/RAND_MAX-0.5f)*200;
            p.vx=((float)rand()/RAND_MAX-0.5f)*0.03f;
            p.vy=0.01f+(float)rand()/RAND_MAX*0.05f;
            p.vz=((float)rand()/RAND_MAX-0.5f)*0.03f;
            p.life=0.6f+(float)rand()/RAND_MAX*0.4f;
            float pH=(float)rand()/RAND_MAX;
            hsv2rgb(pH,0.8f,0.9f,p.r,p.g,p.b);
        }
    }
}

static void initParticles() {
    for(int i=0;i<NUM_PARTICLES;i++){
        Particle& p=gParticles[i];
        p.px=((float)rand()/RAND_MAX-0.5f)*200;
        p.py=2.0f+(float)rand()/RAND_MAX*35;
        p.pz=((float)rand()/RAND_MAX-0.5f)*200;
        p.vx=((float)rand()/RAND_MAX-0.5f)*0.03f;
        p.vy=(float)rand()/RAND_MAX*0.04f;
        p.vz=((float)rand()/RAND_MAX-0.5f)*0.03f;
        p.life=(float)rand()/RAND_MAX;
        float pH=(float)rand()/RAND_MAX;
        hsv2rgb(pH,0.8f,0.9f,p.r,p.g,p.b);
    }
}

// ============================================================
// Lava Pools
// ============================================================
static void drawLavaPools(float cx, float cz, float time) {
    int bx=(int)(cx/30), bz=(int)(cz/30);
    for(int gz=-2;gz<=2;gz++){
        for(int gx=-2;gx<=2;gx++){
            float px=(bx+gx)*30+15, pz=(bz+gz)*30+15;
            float h=terrainHeight(px,pz);
            if(h>-2.0f) continue;
            float ly=h+0.1f;
            float pulse=0.7f+0.3f*sinf(time*2.0f+px*0.1f);
            int segs=16; float rad=6.0f;
            glColor4f(pulse,0.3f*pulse,0,0.9f);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(px,ly,pz);
            for(int i=0;i<=segs;i++){
                float a=(float)i/segs*2*PI;
                float ripple=0.85f+0.15f*sinf(time*4.0f+a*3.0f);
                float lh=fmodf(a/(2*PI)+time*0.3f,1.0f);
                float lr,lg,lb;
                hsv2rgb(lh,1.0f,1.0f,lr,lg,lb);
                glColor4f(lr*pulse,lg*pulse*0.5f,lb*0.1f,0.7f);
                glVertex3f(px+cosf(a)*rad*ripple,ly+0.05f,pz+sinf(a)*rad*ripple);
            }
            glEnd();
            glPointSize(14.0f*pulse);
            glBegin(GL_POINTS);
            glColor4f(1,0.6f,0.1f,0.7f*pulse);
            glVertex3f(px,ly+0.3f,pz);
            glEnd();
        }
    }
}

// ============================================================
// Glow Orbs
// ============================================================
static void drawGlowOrbs(float cx, float cz, float time) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    srand(55555);
    for(int i=0;i<30;i++){
        float ox=((float)rand()/RAND_MAX-0.5f)*300+cx;
        float oy=2.0f+(float)rand()/RAND_MAX*25;
        float oz=((float)rand()/RAND_MAX-0.5f)*300+cz;
        float pulse=0.5f+0.5f*sinf(time*2.0f+i*2.3f);
        float oH=fmodf((float)i/30+time*0.05f,1.0f);
        float or_,og,ob;
        hsv2rgb(oH,0.8f,0.9f,or_,og,ob);
        glPointSize(25.0f*pulse);
        glBegin(GL_POINTS);
        glColor4f(or_*pulse,og*pulse,ob*pulse,0.7f*pulse);
        glVertex3f(ox,oy+sinf(time+i)*0.5f,oz);
        glEnd();
        glPointSize(12.0f*pulse);
        glBegin(GL_POINTS);
        glColor4f(1,1,1,0.4f*pulse);
        glVertex3f(ox,oy+sinf(time+i)*0.5f,oz);
        glEnd();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============================================================
// Magic Trail
// ============================================================
static void drawMagicPath(float cx, float cz, float time) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    float rad=gCarAngle*PI/180.0f;
    for(int i=0;i<30;i++){
        float t=(float)i/30;
        float px=cx+sinf(rad+PI)*t*60;
        float pz=cz+cosf(rad+PI)*t*60;
        float py=terrainHeight(px,pz);
        if(py<-2.0f) continue;
        float tH=fmodf(t*0.5f+time*0.3f,1.0f);
        float tr,tg,tb;
        hsv2rgb(tH,0.7f,0.8f,tr,tg,tb);
        glPointSize(12.0f*(1-t*0.5f));
        glBegin(GL_POINTS);
        glColor4f(tr,tg,tb,(1-t)*0.7f);
        glVertex3f(px,py+0.2f,pz);
        glEnd();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ============================================================
// HUD
// ============================================================
static void renderHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,gWinW,gWinH,0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);

    float barW=200,barH=12,margin=15;
    glColor4f(0,0,0,0.5f);
    glBegin(GL_QUADS);
    glVertex2f(margin-2,margin-2); glVertex2f(margin+barW+2,margin-2);
    glVertex2f(margin+barW+2,margin+barH+2); glVertex2f(margin-2,margin+barH+2);
    glEnd();
    float sf=clampf(fabsf(gCarSpeed)/45.0f,0,1);
    float sH=(1-sf)*0.33f;
    float sr,sg,sb;
    hsv2rgb(sH,0.9f,0.9f,sr,sg,sb);
    glColor4f(sr,sg,sb,0.9f);
    glBegin(GL_QUADS);
    glVertex2f(margin,margin); glVertex2f(margin+barW*sf,margin);
    glVertex2f(margin+barW*sf,margin+barH); glVertex2f(margin,margin+barH);
    glEnd();

    float fy=margin+barH+8;
    glColor4f(0,0,0,0.5f);
    glBegin(GL_QUADS);
    glVertex2f(margin-2,fy-2); glVertex2f(margin+barW+2,fy-2);
    glVertex2f(margin+barW+2,fy+barH+2); glVertex2f(margin-2,fy+barH+2);
    glEnd();
    float ff=clampf((float)gFPS/120,0,1);
    glColor4f(0,0.9f,0.4f,0.9f);
    glBegin(GL_QUADS);
    glVertex2f(margin,fy); glVertex2f(margin+barW*ff,fy);
    glVertex2f(margin+barW*ff,fy+barH); glVertex2f(margin,fy+barH);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// Input (NO collision blocking)
// ============================================================
static void handleInput() {
    gKeys = SDL_GetKeyboardState(nullptr);
    float accel=0;
    if(gKeys[SDL_SCANCODE_W]||gKeys[SDL_SCANCODE_UP])    accel=40;
    if(gKeys[SDL_SCANCODE_S]||gKeys[SDL_SCANCODE_DOWN])  accel=-25;
    if(gKeys[SDL_SCANCODE_LSHIFT]) accel*=2;
    gCarSpeed+=accel*gDelta;
    gCarSpeed*=(1-1.5f*gDelta);
    if(fabsf(gCarSpeed)<0.1f&&accel==0) gCarSpeed*=(1-5*gDelta);
    float maxSpd=45;
    if(gCarSpeed>maxSpd) gCarSpeed=maxSpd;
    if(gCarSpeed<-maxSpd*0.6f) gCarSpeed=-maxSpd*0.6f;

    float turnSpeed=180;
    if(gKeys[SDL_SCANCODE_A]||gKeys[SDL_SCANCODE_LEFT])
        gSteerAngle=-turnSpeed*gDelta;
    else if(gKeys[SDL_SCANCODE_D]||gKeys[SDL_SCANCODE_RIGHT])
        gSteerAngle=turnSpeed*gDelta;
    else
        gSteerAngle*=0.8f;

    float sf=clampf(fabsf(gCarSpeed)/10,0.15f,1.0f);
    gCarAngle+=gSteerAngle*sf;

    float rad=gCarAngle*PI/180.0f;
    gCarX+=sinf(rad)*gCarSpeed*gDelta;
    gCarZ+=cosf(rad)*gCarSpeed*gDelta;

    // NO collision blocking - drive everywhere!

    gWheelSpin+=gCarSpeed*120*gDelta;

    if(gCamFree){
        if(gKeys[SDL_SCANCODE_I]) gCamAngleH+=1.5f*gDelta;
        if(gKeys[SDL_SCANCODE_K]) gCamAngleH-=1.5f*gDelta;
        if(gKeys[SDL_SCANCODE_J]) gCamAngleV+=1.0f*gDelta;
        if(gKeys[SDL_SCANCODE_L]) gCamAngleV-=1.0f*gDelta;
        gCamAngleV=clampf(gCamAngleV,-0.3f,1.2f);
    }
    float tc=terrainHeight(gCarX,gCarZ);
    float liftY=tc>0?tc*0.15f:0;
    if(tc>3.0f) liftY=0;
    gCamHeight=10.0f+liftY;
}

// ============================================================
// FBO Management
// ============================================================
static void cleanupFBOs() {
    if(gSceneFBO){
        p_glDeleteFramebuffers(1,&gSceneFBO);
        p_glDeleteRenderbuffers(1,&gSceneDepthRB);
        glDeleteTextures(1,&gSceneTex);
        gSceneFBO=0;
    }
    for(int i=0;i<2;i++){
        if(gPPFBO[i]){
            p_glDeleteFramebuffers(1,&gPPFBO[i]);
            glDeleteTextures(1,&gPPTex[i]);
            gPPFBO[i]=0;
        }
    }
}

static bool setupFBOs() {
    cleanupFBOs();
    int w=gWinW, h=gWinH, hw=w/2, hh=h/2;

    p_glGenFramebuffers(1,&gSceneFBO);
    p_glBindFramebuffer(GL_FRAMEBUFFER,gSceneFBO);
    glGenTextures(1,&gSceneTex);
    glBindTexture(GL_TEXTURE_2D,gSceneTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    p_glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,gSceneTex,0);
    p_glGenRenderbuffers(1,&gSceneDepthRB);
    p_glBindRenderbuffer(GL_RENDERBUFFER,gSceneDepthRB);
    p_glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,w,h);
    p_glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,gSceneDepthRB);
    if(p_glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
        fprintf(stderr,"Scene FBO incomplete\n");
        cleanupFBOs(); return false;
    }

    for(int i=0;i<2;i++){
        p_glGenFramebuffers(1,&gPPFBO[i]);
        p_glBindFramebuffer(GL_FRAMEBUFFER,gPPFBO[i]);
        glGenTextures(1,&gPPTex[i]);
        glBindTexture(GL_TEXTURE_2D,gPPTex[i]);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,hw,hh,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        p_glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,gPPTex[i],0);
        if(p_glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){
            fprintf(stderr,"Ping-pong FBO %d incomplete\n",i);
            cleanupFBOs(); return false;
        }
    }
    p_glBindFramebuffer(GL_FRAMEBUFFER,0);
    fprintf(stderr,"FBOs created: %dx%d scene, %dx%d blur\n",w,h,hw,hh);
    return true;
}

// ============================================================
// Bloom Post-Processing
// ============================================================
static void renderBloom() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float quad[] = {-1,-1, 1,-1, 1,1, -1,1};
    p_glEnableVertexAttribArray(0);
    p_glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,quad);

    // 1. Brightness extract
    p_glBindFramebuffer(GL_FRAMEBUFFER,gPPFBO[0]);
    glViewport(0,0,gWinW/2,gWinH/2);
    p_glUseProgram(gExtractProg);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,gSceneTex);
    p_glUniform1i(p_glGetUniformLocation(gExtractProg,"uTex"),0);
    p_glUniform1f(p_glGetUniformLocation(gExtractProg,"uThresh"),0.30f);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    // 2. Blur passes
    p_glUseProgram(gBlurProg);
    p_glUniform1i(p_glGetUniformLocation(gBlurProg,"uTex"),0);
    p_glUniform2f(p_glGetUniformLocation(gBlurProg,"uSize"),(float)(gWinW/2),(float)(gWinH/2));
    for(int i=0;i<6;i++){
        p_glBindFramebuffer(GL_FRAMEBUFFER,gPPFBO[1]);
        glViewport(0,0,gWinW/2,gWinH/2);
        glBindTexture(GL_TEXTURE_2D,gPPTex[0]);
        p_glUniform2f(p_glGetUniformLocation(gBlurProg,"uDir"),1.0f,0.0f);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);

        p_glBindFramebuffer(GL_FRAMEBUFFER,gPPFBO[0]);
        glViewport(0,0,gWinW/2,gWinH/2);
        glBindTexture(GL_TEXTURE_2D,gPPTex[1]);
        p_glUniform2f(p_glGetUniformLocation(gBlurProg,"uDir"),0.0f,1.0f);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    }

    // 3. Composite
    p_glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,gWinW,gWinH);
    p_glUseProgram(gCompositeProg);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,gSceneTex);
    p_glUniform1i(p_glGetUniformLocation(gCompositeProg,"uScene"),0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,gPPTex[0]);
    p_glUniform1i(p_glGetUniformLocation(gCompositeProg,"uBloom"),1);
    p_glUniform1f(p_glGetUniformLocation(gCompositeProg,"uStr"),1.3f);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);

    p_glDisableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    p_glUseProgram(0);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// Init
// ============================================================
static bool init() {
    fprintf(stderr,"init: SDL_Init...\n"); fflush(stderr);
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        fprintf(stderr,"SDL init failed: %s\n",SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

    fprintf(stderr,"init: CreateWindow...\n"); fflush(stderr);
    gWindow = SDL_CreateWindow("Fantasy 3D - Crystal Kingdoms",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
        SCREEN_W,SCREEN_H,
        SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(!gWindow){
        fprintf(stderr,"Window failed: %s\n",SDL_GetError());
        return false;
    }

    fprintf(stderr,"init: CreateContext...\n"); fflush(stderr);
    gGL = SDL_GL_CreateContext(gWindow);
    if(!gGL){
        fprintf(stderr,"GL context failed: %s\n",SDL_GetError());
        return false;
    }

    SDL_GL_SetSwapInterval(1);
    fprintf(stderr,"OpenGL: %s\n",glGetString(GL_VERSION));
    fprintf(stderr,"Renderer: %s\n",glGetString(GL_RENDERER));
    fprintf(stderr,"GLSL: %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
    fflush(stderr);

    gShadersOK = loadGLFunctions();
    if(gShadersOK){
        fprintf(stderr,"Compiling shaders...\n"); fflush(stderr);
        GLuint vs = compileShader(GL_VERTEX_SHADER, waterVertSrc);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, waterFragSrc);
        if(vs && fs){
            gWaterProg = linkProgram(vs, fs);
            p_glDeleteShader(vs);
            p_glDeleteShader(fs);
        }
        gExtractProg  = createPostProgram(extractFragSrc);
        gBlurProg     = createPostProgram(blurFragSrc);
        gCompositeProg = createPostProgram(compositeFragSrc);

        if(!gWaterProg||!gExtractProg||!gBlurProg||!gCompositeProg){
            fprintf(stderr,"Some shaders failed, disabling post-processing\n");
            gShadersOK = false;
        } else {
            fprintf(stderr,"All shaders compiled OK\n");
        }
    }
    if(gShadersOK){
        if(!setupFBOs()){
            fprintf(stderr,"FBO setup failed, disabling post-processing\n");
            gShadersOK = false;
        }
    }
    fflush(stderr);

    srand((unsigned)time(nullptr));
    initParticles();
    gTicks = SDL_GetTicks();
    gFPSTimer = gTicks;
    return true;
}

// ============================================================
// Close
// ============================================================
static void close() {
    cleanupFBOs();
    if(gWaterProg)     p_glDeleteProgram(gWaterProg);
    if(gExtractProg)   p_glDeleteProgram(gExtractProg);
    if(gBlurProg)      p_glDeleteProgram(gBlurProg);
    if(gCompositeProg) p_glDeleteProgram(gCompositeProg);
    if(gGL)     SDL_GL_DeleteContext(gGL);
    if(gWindow) SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

// ============================================================
// Main Entry Point
// ============================================================
void manage_three_dee(void) {
    if(!init()){ close(); return; }

    while(gRunning){
        Uint32 now=SDL_GetTicks();
        gDelta=(now-gTicks)/1000.0f;
        if(gDelta>0.1f) gDelta=0.016f;
        gTicks=now;
        gFrameCnt++;
        if(now-gFPSTimer>=1000){ gFPS=gFrameCnt; gFrameCnt=0; gFPSTimer=now; }

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) gRunning=false;
            if(e.type==SDL_KEYDOWN){
                if(e.key.keysym.sym==SDLK_ESCAPE) gRunning=false;
                if(e.key.keysym.sym==SDLK_f) gCamFree=!gCamFree;
                if(e.key.keysym.sym==SDLK_b) gBloomOn=!gBloomOn;
                if(e.key.keysym.sym==SDLK_r){
                    gCarX=0;gCarZ=0;gCarAngle=0;gCarSpeed=0;
                }
            }
            if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_SIZE_CHANGED){
                gWinW=e.window.data1; gWinH=e.window.data2;
                glViewport(0,0,gWinW,gWinH);
                if(gShadersOK){
                    if(!setupFBOs()){
                        fprintf(stderr,"FBO recreation failed\n");
                    }
                }
            }
        }

        float time=(float)SDL_GetTicks()/1000.0f;
        handleInput();

        // ---- Render scene to FBO ----
        if(gShadersOK)
            p_glBindFramebuffer(GL_FRAMEBUFFER,gSceneFBO);

        glClearColor(0.05f,0.02f,0.12f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(65.0,(double)gWinW/gWinH,0.5,500.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float rad=gCarAngle*PI/180.0f;
        float carY=terrainHeight(gCarX,gCarZ)+1.5f;
        float camX,camY,camZ,lookX,lookY,lookZ;
        if(gCamFree){
            float cH=cosf(gCamAngleH),sH=sinf(gCamAngleH);
            float cV=cosf(gCamAngleV),sV=sinf(gCamAngleV);
            camX=gCarX+cH*cV*gCamDist;
            camY=gCamHeight+sV*gCamDist;
            camZ=gCarZ+sH*cV*gCamDist;
            lookX=gCarX; lookY=carY; lookZ=gCarZ;
        } else {
            camX=gCarX+(-sinf(rad))*gCamDist;
            camZ=gCarZ+(-cosf(rad))*gCamDist;
            camY=carY+gCamHeight;
            lookX=gCarX+sinf(rad)*5; lookY=carY+1; lookZ=gCarZ+cosf(rad)*5;
        }
        gluLookAt(camX,camY,camZ,lookX,lookY,lookZ,0,1,0);

        // Lighting
        float amb[]={0.6f,0.55f,0.55f,1};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
        float p0[]={gCarX,carY+10,gCarZ,1};
        float d0[]={1,0.95f,0.9f,1};
        glLightfv(GL_LIGHT0,GL_POSITION,p0);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,d0);
        glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.2f);
        glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.004f);
        glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.0008f);

        float lH1=fmodf(time*0.05f,1.0f);
        float l1r,l1g,l1b;
        hsv2rgb(lH1,0.4f,0.6f,l1r,l1g,l1b);
        float p1[]={gCarX+60,25,gCarZ+60,1};
        float d1[]={l1r,l1g,l1b,1};
        glLightfv(GL_LIGHT1,GL_POSITION,p1);
        glLightfv(GL_LIGHT1,GL_DIFFUSE,d1);
        glEnable(GL_LIGHT1);

        float lH2=fmodf(time*0.03f+0.5f,1.0f);
        float l2r,l2g,l2b;
        hsv2rgb(lH2,0.5f,0.5f,l2r,l2g,l2b);
        float p2[]={gCarX-50,20,gCarZ-40,1};
        float d2[]={l2r,l2g,l2b,1};
        glLightfv(GL_LIGHT2,GL_POSITION,p2);
        glLightfv(GL_LIGHT2,GL_DIFFUSE,d2);
        glEnable(GL_LIGHT2);

        float fH=fmodf(time*0.03f,1.0f);
        float fr,fg,fb;
        hsv2rgb(fH,0.3f,0.15f,fr,fg,fb);
        float fogC[]={fr,fg,fb,1};
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE,GL_EXP2);
        glFogfv(GL_FOG_COLOR,fogC);
        glFogf(GL_FOG_DENSITY,0.001f);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);

        // --- Draw Scene ---
        drawSky(time);
        drawTerrain(gCarX,gCarZ,time);

        // Water (shader-based)
        drawWater(gCarX,gCarZ,time);

        drawTerrainVeins(gCarX,gCarZ,time);
        drawLavaPools(gCarX,gCarZ,time);
        drawCar(gCarX,carY+0.8f,gCarZ,gCarAngle,time);
        drawMagicPath(gCarX,gCarZ,time);
        updateParticles(gCarX,gCarZ);
        drawParticles();

        srand(12345);
        for(int i=0;i<25;i++){
            float tx=((float)rand()/RAND_MAX-0.5f)*300+gCarX;
            float tz=((float)rand()/RAND_MAX-0.5f)*300+gCarZ;
            drawTree(tx,tz,time);
        }
        srand(88888);
        for(int i=0;i<20;i++){
            float mx=((float)rand()/RAND_MAX-0.5f)*200+gCarX;
            float mz=((float)rand()/RAND_MAX-0.5f)*200+gCarZ;
            drawMushroom(mx,mz,time);
        }
        srand(99999);
        for(int i=0;i<15;i++){
            float cx_=((float)rand()/RAND_MAX-0.5f)*400+gCarX;
            float cz_=((float)rand()/RAND_MAX-0.5f)*400+gCarZ;
            float h=(float)rand()/RAND_MAX;
            float cr,cg,cb;
            hsv2rgb(h,0.8f,0.9f,cr,cg,cb);
            drawCrystal(cx_,cz_,4+(float)rand()/RAND_MAX*10,cr,cg,cb,time);
        }
        srand(77777);
        for(int i=0;i<6;i++){
            drawFloatingIsland(
                ((float)rand()/RAND_MAX-0.5f)*500+gCarX,
                25+(float)rand()/RAND_MAX*30,
                ((float)rand()/RAND_MAX-0.5f)*500+gCarZ,
                3+(float)rand()/RAND_MAX*5,time);
        }
        drawGlowOrbs(gCarX,gCarZ,time);

        // If no shaders or bloom disabled, draw directly to screen
        if(!gShadersOK || !gBloomOn){
            renderHUD();
            SDL_GL_SwapWindow(gWindow);
            continue;
        }

        // Unbind FBO, apply bloom, then HUD
        p_glBindFramebuffer(GL_FRAMEBUFFER,0);
        renderBloom();
        renderHUD();
        SDL_GL_SwapWindow(gWindow);
    }
    close();
}
