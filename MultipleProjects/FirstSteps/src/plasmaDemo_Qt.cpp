#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVector>
#include <QPainter>
#include <QString>

#include <QOpenGLShader>

struct ShaderPair {
    const char *name;
    const char *vert;
    const char *frag;
};

class PlasmaWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit PlasmaWidget(QWidget *parent = nullptr);
    ~PlasmaWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void compileCurrentEffect();
    GLuint compileShader(GLenum type, const char *source);
    void renderQuad();
    void setupShaders();
    void paintOverlay(QPainter &p);
    QString paletteName() const;

    QTimer m_timer;
    QElapsedTimer m_elapsed;

    int m_effect = 0;
    int m_palette = 0;
    bool m_paused = false;

    GLuint m_program = 0;
    int u_time = -1;
    int u_resolution = -1;
    int u_palette = -1;

    int m_frameCount = 0;
    int m_fps = 0;
    QElapsedTimer m_fpsTimer;

    QVector<ShaderPair> m_effects;
};

static const char *vshader = R"(
attribute vec2 a_pos;
varying vec2 v_uv;
void main() {
    v_uv = a_pos * 0.5 + 0.5;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)";

// ── Effect 0: Classic Plasma ──
static const char *plasma_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float v = 0.0;
    v += sin((p.x * 3.0 + t * 0.8) * 2.0);
    v += sin((p.y * 4.0 + t * 0.6) * 2.0);
    v += sin(((p.x + p.y) * 2.5 + t * 0.5) * 2.0);
    v += sin(length(p) * 5.0 - t * 1.2);

    float cx = p.x + 0.5 * sin(t * 0.3);
    float cy = p.y + 0.5 * cos(t * 0.4);
    v += sin(atan(p.y - cy, p.x - cx) * 6.0 + t * 1.5) * 0.5;

    v = v / 5.0 + 0.5;
    float hue = v, sat = 0.9, val = 0.9;

    if (u_palette == 0) {
        hue = fract(hue + t * 0.05);
        sat = 0.9;
        val = 0.8 + 0.2 * sin(v * 6.28 + t);
    } else if (u_palette == 1) {
        hue = fract(v * 2.0 + t * 0.03);
        sat = 0.7 + 0.3 * sin(v * 8.0 + t * 0.7);
    } else if (u_palette == 2) {
        hue = 0.55 + 0.15 * sin(v * 6.28 + t * 0.4);
        sat = 0.5 + 0.5 * v;
        val = 0.5 + 0.5 * (1.0 - abs(v - 0.5) * 2.0);
    } else {
        vec3 rgb = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 2.1, 4.2) + t * 0.3);
        gl_FragColor = vec4(rgb * 0.9, 1.0);
        return;
    }
    gl_FragColor = vec4(hsv2rgb(vec3(hue, sat, val)), 1.0);
}
)";

// ── Effect 1: Fire ──
static const char *fire_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

void main() {
    vec2 uv = v_uv;
    float t = u_time;

    float v = 0.0;
    for (int i = 0; i < 5; i++) {
        float fi = float(i);
        float x = uv.x + sin(t * 0.5 + fi * 1.7) * 0.15;
        float y = uv.y + cos(t * 0.3 + fi * 2.3) * 0.1;
        float d = length(vec2(x - 0.5, y - 0.5 + fi * 0.15));
        v += (1.0 - smoothstep(0.0, 0.4, d)) * (0.5 + 0.5 * sin(t * 2.0 + fi));
    }

    float flicker = 0.8 + 0.2 * sin(t * 5.0 + uv.x * 20.0);
    v = clamp(v * 0.35 * flicker, 0.0, 1.0);
    v = pow(v, 0.6);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.1, 0.0, 0.0), vec3(1.0, 0.3, 0.0), v);
        col = mix(col, vec3(1.0, 0.9, 0.3), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 1) {
        col = mix(vec3(0.0, 0.0, 0.1), vec3(0.0, 0.5, 1.0), v);
        col = mix(col, vec3(0.5, 1.0, 1.0), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 2) {
        col = mix(vec3(0.0, 0.1, 0.0), vec3(0.0, 1.0, 0.3), v);
        col = mix(col, vec3(0.8, 1.0, 0.3), smoothstep(0.5, 1.0, v));
    } else {
        col = mix(vec3(0.1), vec3(0.8, 0.3, 0.8), v);
        col = mix(col, vec3(1.0, 0.8, 1.0), smoothstep(0.5, 1.0, v));
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 2: Ripples / Water ──
static const char *ripple_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float d = length(p);
    float wave = sin(d * 8.0 - t * 2.0) * 0.5 + 0.5;
    wave += sin(d * 15.0 + t * 1.5 + p.x * 3.0) * 0.25;
    wave += sin(d * 25.0 - t * 3.0 + p.y * 4.0) * 0.125;
    wave = wave / 1.875;

    float ripple = sin(atan(p.y, p.x) * 6.0 + d * 10.0 - t * 1.5) * 0.3 + 0.5;

    float v = wave * 0.7 + ripple * 0.3;
    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.1, 0.2), vec3(0.0, 0.5, 0.8), v);
        col = mix(col, vec3(0.5, 0.8, 1.0), smoothstep(0.6, 1.0, v));
    } else if (u_palette == 1) {
        col = hsv2rgb(vec3(fract(0.6 + v * 0.3 + t * 0.02), 0.7, 0.5 + v * 0.5));
    } else if (u_palette == 2) {
        col = mix(vec3(0.05, 0.05, 0.05), vec3(1.0, 0.6, 0.0), v);
        col = mix(col, vec3(1.0, 1.0, 0.5), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 1.0, 2.0) + t * 0.2);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 3: Mandelbrot ──
static const char *mandel_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float ar = u_resolution.x / u_resolution.y;
    vec2 uv = (v_uv - 0.5) * vec2(ar, 1.0);

    float zoom = 1.0 + 0.5 * sin(u_time * 0.1);
    float rot = u_time * 0.05;
    float cs = cos(rot), sn = sin(rot);
    vec2 z = vec2(uv.x * cs - uv.y * sn, uv.x * sn + uv.y * cs) * zoom;
    z += vec2(-0.5, 0.0);

    vec2 c = z;
    float it = 0.0;
    for (int i = 0; i < 128; i++) {
        if (dot(z, z) > 4.0) break;
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        it += 1.0;
    }
    float v = it / 128.0;

    vec3 col;
    if (u_palette == 0) {
        col = hsv2rgb(vec3(fract(v * 3.0 + u_time * 0.02), 0.8, v * 0.5 + 0.5));
    } else if (u_palette == 1) {
        col = 0.5 + 0.5 * cos(v * 4.0 + vec3(0.0, 1.0, 2.0));
    } else if (u_palette == 2) {
        col = mix(vec3(0.05, 0.0, 0.1), vec3(1.0, 0.5, 0.0), v);
    } else {
        col = mix(vec3(0.0, 0.1, 0.0), vec3(0.0, 1.0, 0.5), v);
        col = mix(col, vec3(1.0, 1.0, 0.5), smoothstep(0.6, 1.0, v));
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 4: Tunnel ──
static const char *tunnel_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float a = atan(p.y, p.x);
    float r = length(p);

    float tunnel = sin(r * 20.0 - t * 2.0) * 0.5 + 0.5;
    tunnel += sin(a * 8.0 + t * 1.0) * 0.3;
    tunnel += sin((a + r) * 10.0 + t * 0.5) * 0.2;
    tunnel = tunnel / 1.5;

    float v = clamp(tunnel, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.0, 0.1), vec3(0.3, 0.0, 0.5), v);
        col = mix(col, vec3(1.0, 0.5, 0.0), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 1) {
        col = mix(vec3(0.0, 0.1, 0.0), vec3(0.0, 0.8, 0.3), v);
        col = mix(col, vec3(0.5, 1.0, 0.5), smoothstep(0.6, 1.0, v));
    } else if (u_palette == 2) {
        col = mix(vec3(0.05, 0.0, 0.1), vec3(0.8, 0.0, 0.8), v);
        col = mix(col, vec3(1.0, 0.6, 1.0), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 2.1, 4.2) + t * 0.2);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 5: Aurora ──
static const char *aurora_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float v = 0.0;
    for (int i = 0; i < 6; i++) {
        float fi = float(i);
        float x = p.x * 2.0 + sin(t * 0.25 + fi * 1.3) * 1.5;
        float y = p.y * 3.0 + cos(t * 0.2 + fi * 0.9) * 1.2;
        float d = length(vec2(x, y));
        v += (1.0 - smoothstep(0.4, 1.8, d)) * (0.4 + 0.4 * sin(t * 0.4 + fi * 2.5));
    }
    v = clamp(v * 0.22, 0.0, 1.0);
    v = pow(v, 0.7);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.02, 0.05), vec3(0.0, 0.8, 0.5), v);
        col = mix(col, vec3(0.2, 1.0, 0.3), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 1) {
        col = mix(vec3(0.05, 0.0, 0.05), vec3(1.0, 0.2, 0.6), v);
        col = mix(col, vec3(1.0, 0.8, 0.4), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 2) {
        float h = 0.55 + 0.25 * sin(v * 6.28 + t * 0.1);
        col = hsv2rgb(vec3(h, 0.55, 0.3 + v * 0.7));
    } else {
        col = 0.5 + 0.5 * cos(v * 4.0 + vec3(0.0, 0.5, 1.5) + t * 0.15);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 6: Kaleidoscope ──
static const char *kaleido_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float a = atan(p.y, p.x);
    float r = length(p);

    float segs = 8.0;
    float aa = mod(a, 6.2832 / segs);
    aa = abs(aa - 3.1416 / segs);
    aa = aa * segs / 3.1416;

    vec2 q = vec2(aa, r + 0.2 * sin(t * 0.5));
    q.x += 0.1 * sin(q.y * 10.0 + t);
    q.y += 0.1 * sin(q.x * 8.0 + t * 0.7);

    float v = sin(q.x * 14.0 + t) * 0.5 + 0.5;
    v += sin(q.y * 10.0 - t * 0.8) * 0.3;
    v += sin((q.x + q.y) * 6.0 + t * 0.6) * 0.2;
    v = v / 1.5;
    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = hsv2rgb(vec3(fract(v * 2.0 + t * 0.03), 0.8, 0.5 + v * 0.5));
    } else if (u_palette == 1) {
        col = mix(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.1, 0.3), v);
        col = mix(col, vec3(1.0, 1.0, 0.5), smoothstep(0.4, 1.0, v));
    } else if (u_palette == 2) {
        col = mix(vec3(0.0, 0.0, 0.1), vec3(0.0, 0.6, 0.8), v);
        col = mix(col, vec3(0.8, 1.0, 0.8), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 1.5, 3.0) + t * 0.4);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 7: Spiral Galaxy ──
static const char *galaxy_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float a = atan(p.y, p.x);
    float r = length(p);

    float arms = 3.0;
    float spiral = sin(a * arms - r * 10.0 + t * 0.4) * 0.5 + 0.5;
    float core = 1.0 - smoothstep(0.0, 0.3, r);
    float dust = 0.5 + 0.5 * sin(r * 12.0 - t * 0.3 + a * 2.0);

    float v = spiral * 0.5 + core * 0.4 + dust * 0.1;
    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.0, 0.05), vec3(0.3, 0.1, 0.5), v);
        col = mix(col, vec3(1.0, 0.7, 0.3), smoothstep(0.4, 0.9, v));
    } else if (u_palette == 1) {
        col = hsv2rgb(vec3(fract(0.6 + v * 0.4 + t * 0.01), 0.8, v * 0.5 + 0.5));
    } else if (u_palette == 2) {
        col = mix(vec3(0.0, 0.02, 0.1), vec3(0.0, 0.3, 0.8), v);
        col = mix(col, vec3(0.5, 0.8, 1.0), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 5.0 + vec3(0.0, 1.0, 2.0) + t * 0.2);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 8: Lightning ──
static const char *lightning_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float v = 0.0;

    float bolt = 1.0 - abs(p.x * 3.0 + sin(p.y * 20.0 + t * 5.0) * 0.1 + sin(p.y * 40.0 + t * 8.0) * 0.05);
    bolt = smoothstep(0.0, 0.08, bolt);
    v += bolt;

    for (int i = 0; i < 5; i++) {
        float fi = float(i);
        float xoff = -0.3 + fi * 0.15 + sin(t * 2.0 + fi * 3.0) * 0.1;
        float yoff = 0.3 + fi * 0.1;
        vec2 bp = p - vec2(xoff, yoff);
        float glow = exp(-length(bp) * 5.0) * 0.4;
        float spark = 1.0 - abs(bp.x * 8.0 + sin(bp.y * 30.0 + t * 10.0 + fi * 5.0) * 0.05);
        spark = smoothstep(0.0, 0.04, spark);
        v += (glow + spark * 0.3) * (0.5 + 0.5 * sin(t * 3.0 + fi));
    }

    float glow = exp(-length(p - vec2(0.0, 0.3)) * 8.0) * 0.3;
    v += glow;

    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0), vec3(0.3, 0.5, 1.0), v * 2.0);
        col = mix(col, vec3(1.0), smoothstep(0.4, 1.0, v));
    } else if (u_palette == 1) {
        col = mix(vec3(0.0), vec3(0.8, 0.2, 0.8), v * 2.0);
        col = mix(col, vec3(1.0, 0.8, 1.0), smoothstep(0.4, 1.0, v));
    } else if (u_palette == 2) {
        col = mix(vec3(0.0), vec3(0.0, 0.8, 0.3), v * 2.0);
        col = mix(col, vec3(0.5, 1.0, 0.5), smoothstep(0.4, 1.0, v));
    } else {
        col = mix(vec3(0.0), vec3(0.8, 0.0, 0.0), v * 2.0);
        col = mix(col, vec3(1.0, 0.8, 0.3), smoothstep(0.4, 1.0, v));
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 9: Hex Grid ──
static const char *hexgrid_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float s = 0.15;
    vec2 q = p / s;
    float row = floor(q.y);
    float colIdx = floor(q.x + 0.5 * mod(row, 2.0));
    vec2 cell = vec2(colIdx, row);
    if (mod(row, 2.0) > 0.5) cell.x += 0.5;
    vec2 diff = q - cell;

    float d = length(diff);
    float fill = 1.0 - smoothstep(0.25, 0.45, d);

    float cellPhase = dot(cell, vec2(1.2, 0.8)) + t;
    float pulse = 0.5 + 0.5 * sin(cellPhase);

    float v = fill * pulse + fill * 0.3;

    float edge = smoothstep(0.35, 0.45, d) - smoothstep(0.45, 0.5, d);
    float edgeGlow = edge * (0.5 + 0.5 * sin(cellPhase * 2.0));
    v += edgeGlow * 0.5;

    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.0, 0.05), vec3(0.2, 0.6, 1.0), v);
        col = mix(col, vec3(0.8, 1.0, 1.0), smoothstep(0.6, 1.0, v));
    } else if (u_palette == 1) {
        col = hsv2rgb(vec3(fract(0.05 + v * 0.5 + t * 0.02), 0.7, 0.3 + v * 0.7));
    } else if (u_palette == 2) {
        col = mix(vec3(0.05, 0.0, 0.05), vec3(1.0, 0.2, 0.4), v);
        col = mix(col, vec3(1.0, 0.8, 0.4), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 1.0, 2.0) + t * 0.3);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 10: Lava ──
static const char *lava_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float v = 0.0;

    for (int i = 0; i < 8; i++) {
        float fi = float(i);
        vec2 pos = vec2(
            0.5 + 0.4 * sin(fi * 2.1 + t * 0.3),
            0.5 + 0.4 * cos(fi * 1.7 + t * 0.4 + fi * 0.5)
        );
        pos = (pos - 0.5) * vec2(ar, 1.0);
        float d = distance(p, pos);
        float bubble = smoothstep(0.2, 0.0, d) * (0.5 + 0.5 * sin(t * 2.0 + fi * 3.0));
        v += bubble;
    }

    float flow = sin(p.x * 5.0 + t * 0.5) * 0.5 + 0.5;
    flow += sin(p.y * 7.0 + t * 0.3 + p.x * 2.0) * 0.3;
    flow += sin((p.x + p.y) * 4.0 + t * 0.7) * 0.2;
    flow = flow / 1.5;

    float crackle = 0.5 + 0.5 * sin(p.x * 50.0 + p.y * 50.0 + t * 10.0);
    crackle = smoothstep(0.0, 0.1, crackle);

    v = v * 0.4 + flow * 0.5 + crackle * 0.1 * flow;
    v = clamp(v, 0.0, 1.0);
    v = pow(v, 0.8);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.1, 0.0, 0.0), vec3(0.8, 0.1, 0.0), v);
        col = mix(col, vec3(1.0, 0.8, 0.1), smoothstep(0.4, 1.0, v));
    } else if (u_palette == 1) {
        col = mix(vec3(0.0, 0.0, 0.1), vec3(0.0, 0.3, 0.8), v);
        col = mix(col, vec3(0.3, 1.0, 0.8), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 2) {
        col = mix(vec3(0.0, 0.1, 0.0), vec3(0.3, 0.8, 0.0), v);
        col = mix(col, vec3(0.8, 1.0, 0.3), smoothstep(0.5, 1.0, v));
    } else {
        col = mix(vec3(0.1, 0.0, 0.1), vec3(0.8, 0.2, 0.8), v);
        col = mix(col, vec3(1.0, 0.7, 1.0), smoothstep(0.4, 1.0, v));
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 11: Ocean Waves ──
static const char *ocean_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float v = 0.0;
    for (int i = 0; i < 6; i++) {
        float fi = float(i);
        float freq = 3.0 + fi * 1.5;
        float amp = 0.5 - fi * 0.07;
        float dirx = sin(fi * 1.3);
        float diry = cos(fi * 0.9);
        float phase = sin(p.x * freq * dirx + p.y * freq * diry
                          + t * (0.8 + fi * 0.2) + fi * 2.0);
        v += phase * amp;
    }
    v = v / 2.0 + 0.5;
    v = clamp(v, 0.0, 1.0);

    float foam = smoothstep(0.7, 0.9, v);
    v += foam * 0.3;
    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.1, 0.2), vec3(0.0, 0.4, 0.6), v);
        col = mix(col, vec3(0.6, 0.9, 1.0), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 1) {
        col = hsv2rgb(vec3(fract(0.7 + v * 0.2 + t * 0.01), 0.6, 0.3 + v * 0.7));
    } else if (u_palette == 2) {
        col = mix(vec3(0.1, 0.05, 0.0), vec3(0.8, 0.4, 0.0), v);
        col = mix(col, vec3(1.0, 0.9, 0.4), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 4.0 + vec3(0.0, 0.5, 1.5) + t * 0.1);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 12: Infinite Tunnel ──
static const char *inf_tunnel_frag = R"(
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

float hash21(vec2 p) {
    p = fract(p * vec2(234.34, 435.345));
    p += dot(p, p + 19.19);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = hash21(i);
    float b = hash21(i + vec2(1.0, 0.0));
    float c = hash21(i + vec2(0.0, 1.0));
    float d = hash21(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

vec3 hsv(float h, float s, float v) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(vec3(h) + K.xyz) * 6.0 - K.www);
    return v * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), s);
}

mat2 rot2d(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

void main() {
    vec2 uv = (v_uv - 0.5) * vec2(u_resolution.x / u_resolution.y, 1.0);

    vec3 rd = normalize(vec3(uv, 0.8));
    vec3 ro = vec3(0.0, 0.0, -1.0);

    float t = u_time;

    rd.xy *= rot2d(t * 0.05);
    rd.xz *= rot2d(sin(t * 0.1) * 0.1);

    vec3 p = ro;
    vec3 col = vec3(0.0);
    float stepDist = 0.0;
    float radius = 0.0;

    for (float i = 0.0; i < 120.0; i++) {
        radius = length(p);

        vec3 p_log = vec3(
            log(radius) - t * 0.8,
            exp(0.8 - p.z / radius) - 1.0,
            atan(p.y, p.x) + t * 0.4
        );

        vec2 texUV = vec2(p_log.x, p_log.z * 0.1591);
        float noiseVal = noise(texUV);

        float s = 1.0;
        float e = p_log.y + noiseVal * 0.1;

        for (int j = 0; j < 8; j++) {
            vec3 sampling = p_log.yzz * s;
            e += dot(sin(sampling) - 0.5, 0.8 - sin(p_log.zxx * s)) / s * 0.3;
            s *= 2.0;
        }

        stepDist = e;

        float intensity = min(stepDist * s, 0.7 - stepDist) / 35.0;
        intensity = clamp(intensity, 0.0, 1.0);

        float hue = fract(p_log.z * 0.1591 + p_log.x * 0.05 + noiseVal * 0.1 + t * 0.05);
        float sat = 0.65 + noiseVal * 0.35;

        vec3 spectralColor = hsv(hue, sat, intensity);

        float scattering = 1.0 / (1.0 + stepDist * stepDist * 40.0);
        col += spectralColor * (0.5 + noiseVal) * scattering * (1.0 - i / 120.0);

        p += rd * max(stepDist * radius * 0.18, 0.002);

        if (radius > 25.0) break;
    }

    col = mix(col, vec3(0.005, 0.002, 0.01), 1.0 - exp(-0.01 * radius * radius));

    col = pow(col, vec3(0.4545));

    col = col * col * (3.0 - 2.0 * col);

    col *= 1.25 - length(uv) * 0.65;

    if (u_palette == 1) {
        col *= vec3(1.3, 0.7, 0.3);
    } else if (u_palette == 2) {
        col *= vec3(0.3, 0.6, 1.3);
    } else if (u_palette == 3) {
        float gray = dot(col, vec3(0.299, 0.587, 0.114));
        col = vec3(gray);
    }

    gl_FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
)";




// ── Effect 13: Wormhood ──
static const char *wormhood_frag = R"(
const int MAX_STEPS = 200;
const int NUM_SPHERES = 12;

uniform float u_time;
uniform vec2 u_resolution;
varying vec2 v_uv;

float sphere(vec3 pos, float radius, vec3 smpl)
{
    return length(pos - smpl) - radius;
}

float plane(vec3 dir, float offset, vec3 smpl)
{
    return dot(dir, smpl) + offset;
}

float dfDist(vec3 smpl)
{
    float T1 = 10.0;
    float T2 = 2.0 * T1;
    
    float result = 10000.0;
    
    smpl.y += sin(smpl.z * 0.2 + u_time) * sin(u_time * 1.33)
              + sin(smpl.x * 0.3 + u_time) * sin(u_time * 3.22)
              + sin(smpl.x * 0.5 + smpl.z * 0.22 + u_time) * sin(u_time * 2.22 + smpl.z * 0.1);
    float o = floor((smpl.z + T1) / T2);
    smpl.x += o * 7.0;
    smpl.xz = mod(smpl.xz + T1, T2) - T1;
    
    for (int i = 0; i < NUM_SPHERES; i++)
    {
        float t = float(i) / float(NUM_SPHERES);
        float n = t + u_time * 0.25 + o * 0.5;
        vec3 pos = vec3(sin(n * 5.0) * 5.0, cos(n * 3.0) * 9.0, cos(n * 2.0) * 3.0 + 5.0);
        float radius = 2.0 + sin(t * 20.0 + o * 5.0) * 1.0;
        result = min(result, sphere(pos, radius, smpl));
    }
    
    result = min(result, plane(vec3(0, -1, 0), 10.0, smpl));    
    result = min(result, plane(vec3(0, 1, 0), 10.0, smpl));    
    
    return result;
}

vec3 dfNormal(vec3 smpl)
{
    const float E = 0.04;
    
    float d0 = dfDist(smpl);
    float dX = dfDist(smpl + vec3(E, 0, 0));
    float dY = dfDist(smpl + vec3(0, E, 0));
    float dZ = dfDist(smpl + vec3(0, 0, E));
    
    return normalize(vec3(dX - d0, dY - d0, dZ - d0));
}

float dfOcclusion(vec3 smpl, vec3 normal)
{
    float N = 1.0;
    return clamp(dfDist(smpl + normal * N) / N, 0.0, 1.0);
}

float trace(inout vec3 pos, vec3 dir, out vec3 normal)
{
    int steps = 0;
    for (int i = 0; i < MAX_STEPS; i++)
    {
        steps++;
        float d = dfDist(pos);
        pos += d * dir * 1.0;
        
        if (d < 0.001)
        {
            break;
        }
    }
    
    normal = dfNormal(pos);
    return float(steps) / float(MAX_STEPS);
}

void main()
{
    vec3 opos = vec3(4.5,sin(u_time * 0.4) * 3.0 + 2.0,-7.0 + u_time * 3.0);
    vec3 pos = opos;
    vec2 fc = v_uv * u_resolution;
    vec3 dir = normalize(vec3((fc.x - u_resolution.x * 0.5) / u_resolution.y, fc.y / u_resolution.y - 0.5, 1.0));
    vec3 normal;
    
    float steps = trace(pos, dir, normal);
    float occ = dfOcclusion(pos, normal);
    float fogAmt = 1.0 - exp(-distance(opos, pos) * 0.01);
    vec3 fogCol = vec3(0.2, 0.14, 0.18);
    
    vec3 diffuse = vec3(0.4, 0.5, 0.6) * dot(normal, normalize(vec3(1.0, 0.3, -1.0)));
    vec3 ambient = vec3(0.4, 0.2, 0.1);
    vec3 color = (ambient + diffuse) * vec3(1.0 - steps) + pow(1.0 - occ, 1.5) * vec3(1.0, 0.9, 0.8) * 0.8;
    
    
    color = mix(color, fogCol, fogAmt);
    color = (1.0 - exp(-color * 1.5)) * 1.3;
    gl_FragColor = vec4(color, 1.0);
}
)";

// ── Implementation ──

PlasmaWidget::PlasmaWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat fmt;
    fmt.setVersion(2, 1);
    fmt.setProfile(QSurfaceFormat::CompatibilityProfile);
    fmt.setSwapInterval(1);
    setFormat(fmt);  // <-- dieser Widget verwendet 2.1 Compatibility

    setMinimumSize(400, 300);
    resize(1920, 1200);
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    m_timer.setInterval(16);
    m_elapsed.start();
    m_fpsTimer.start();

    setupShaders();
}

PlasmaWidget::~PlasmaWidget()
{
    makeCurrent();
    if (m_program)
        glDeleteProgram(m_program);
    doneCurrent();
}

void PlasmaWidget::setupShaders()
{
    m_effects = {
        { "Plasma",       vshader,       plasma_frag   },
        { "Fire",         vshader,       fire_frag     },
        { "Ripples",      vshader,       ripple_frag   },
        { "Mandelbrot",   vshader,       mandel_frag   },
        { "Tunnel",       vshader,       tunnel_frag   },
        { "Aurora",       vshader,       aurora_frag   },
        { "Kaleidoscope", vshader,       kaleido_frag  },
        { "Spiral Galaxy",vshader,       galaxy_frag   },
        { "Lightning",    vshader,       lightning_frag},
        { "Hex Grid",     vshader,       hexgrid_frag  },
        { "Lava",         vshader,       lava_frag     },
        { "Ocean Waves",  vshader,       ocean_frag    },
        { "Inf. Tunnel",  vshader,       inf_tunnel_frag},
        { "Wormhood",     vshader,       wormhood_frag},
    };
}

void PlasmaWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    compileCurrentEffect();
    m_timer.start();
}

void PlasmaWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void PlasmaWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_program)
        return;

    glUseProgram(m_program);

    float time = m_elapsed.elapsed() / 1000.0f;
    if (m_paused)
        time = m_elapsed.elapsed() / 1000.0f;

    glUniform1f(u_time, time);
    glUniform2f(u_resolution, (float)width(), (float)height());
    glUniform1i(u_palette, m_palette);

    renderQuad();

    glUseProgram(0);

    // Overlay
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        paintOverlay(p);
    }

    // FPS counter (once per second)
    m_frameCount++;
    if (m_fpsTimer.elapsed() >= 1000) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer.restart();
    }
}

void PlasmaWidget::paintOverlay(QPainter &p)
{
    QFont font("DejaVu Sans", 12);
    p.setFont(font);

    // Background strip
    QRect strip(0, 0, width(), 26);
    p.fillRect(strip, QColor(0, 0, 0, 160));

    // Text
    p.setPen(Qt::white);
    QString text = QString("Effect [%1/%2]: %3  |  Palette: %4  |  FPS: %5")
        .arg(m_effect + 1)
        .arg(m_effects.size())
        .arg(m_effects[m_effect].name)
        .arg(paletteName())
        .arg(m_fps);

    if (m_paused)
        text += "  |  PAUSED";

    p.drawText(strip.adjusted(8, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, text);

    // Help text at bottom
    QFont small("DejaVu Sans", 10);
    p.setFont(small);
    QString help = "Click: next effect  |  ←→: palette  |  ↑↓: effect  |  Space: pause";
    QRect helpRect(0, height() - 22, width(), 22);
    p.fillRect(helpRect, QColor(0, 0, 0, 120));
    p.drawText(helpRect.adjusted(8, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, help);
}

void PlasmaWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    makeCurrent();
    if (m_program)
        glDeleteProgram(m_program);
    m_program = 0;
    m_effect = (m_effect + 1) % m_effects.size();
    compileCurrentEffect();
    doneCurrent();
}

void PlasmaWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        makeCurrent();
        m_palette = (m_palette + 3) % 4;
        glUseProgram(m_program);
        glUniform1i(u_palette, m_palette);
        glUseProgram(0);
        doneCurrent();
        break;
    case Qt::Key_Right:
        makeCurrent();
        m_palette = (m_palette + 1) % 4;
        glUseProgram(m_program);
        glUniform1i(u_palette, m_palette);
        glUseProgram(0);
        doneCurrent();
        break;
    case Qt::Key_Up:
        makeCurrent();
        if (m_program)
            glDeleteProgram(m_program);
        m_program = 0;
        m_effect = (m_effect + 1) % m_effects.size();
        compileCurrentEffect();
        doneCurrent();
        break;
    case Qt::Key_Down:
        makeCurrent();
        if (m_program)
            glDeleteProgram(m_program);
        m_program = 0;
        m_effect = (m_effect + m_effects.size() - 1) % m_effects.size();
        compileCurrentEffect();
        doneCurrent();
        break;
    case Qt::Key_Space:
        m_paused = !m_paused;
        if (!m_paused)
            m_elapsed.restart();
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
    }
}

void PlasmaWidget::compileCurrentEffect()
{
    const auto &e = m_effects[m_effect];
    GLuint vs = compileShader(GL_VERTEX_SHADER, e.vert);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, e.frag);

    if (!vs || !fs)
        return;

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glBindAttribLocation(m_program, 0, "a_pos");
    glLinkProgram(m_program);

    GLint linked;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char buf[1024];
        glGetProgramInfoLog(m_program, sizeof(buf), nullptr, buf);
        qWarning("Link error [%s]: %s", e.name, buf);
        glDeleteProgram(m_program);
        m_program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    u_time = glGetUniformLocation(m_program, "u_time");
    u_resolution = glGetUniformLocation(m_program, "u_resolution");
    u_palette = glGetUniformLocation(m_program, "u_palette");
}

GLuint PlasmaWidget::compileShader(GLenum type, const char *source)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &source, nullptr);
    glCompileShader(sh);

    GLint compiled;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char buf[1024];
        glGetShaderInfoLog(sh, sizeof(buf), nullptr, buf);
        qWarning("Shader compile (%s): %s",
                 type == GL_VERTEX_SHADER ? "vertex" : "fragment", buf);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

void PlasmaWidget::renderQuad()
{
    static const float verts[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
    };
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
}

QString PlasmaWidget::paletteName() const
{
    switch (m_palette) {
    case 0: return "Rainbow";
    case 1: return "Warm";
    case 2: return "Cool";
    default: return "RGB";
    }
}

int manage_plasmaDemo_Qt(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PlasmaWidget w;
    w.setWindowTitle("Qt6 Plasma");
    w.show();

    return app.exec();
}

#include "plasmaDemo_Qt.moc"