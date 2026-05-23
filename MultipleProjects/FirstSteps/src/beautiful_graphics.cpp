#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

struct Particle {
    float x, y;
    float vx, vy;
    float radius;
    Uint8 r, g, b;
    float alpha;
    float flamePhase;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    int width, height;
    float brightnessCycle;
    float backgroundBrightness;

public:
    float getBackgroundBrightness() const { return backgroundBrightness; }
    ParticleSystem(int w, int h, int count) : width(w), height(h), brightnessCycle(0.0f), backgroundBrightness(1.0f) {
        std::srand(std::time(nullptr));
        particles.reserve(count);
        for (int i = 0; i < count; ++i) {
            particles.push_back(createParticle());
        }
    }

    Particle createParticle() {
        Particle p;
        p.x = std::rand() % width;
        p.y = std::rand() % height;
        p.vx = (std::rand() % 200 - 100) / 100.0f;
        p.vy = (std::rand() % 200 - 100) / 100.0f;
        p.radius = 40 + std::rand() % 30;

        float hue = std::rand() % 360;
        hslToRgb(hue, 0.7f, 0.6f, p.r, p.g, p.b);
        p.alpha = 0.6f + (std::rand() % 40) / 100.0f;
        p.flamePhase = std::rand() % 100 / 100.0f * 6.28f;

        return p;
    }

    void drawCandle(SDL_Renderer* renderer, float cx, float cy, float size, float brightness, float flameP, Uint8 cr, Uint8 cg, Uint8 cb) {
        float flameFlicker = 0.8f + 0.2f * std::sin(flameP * 8.0f) * std::sin(flameP * 13.0f);
        float candleWidth = size * 0.3f;
        float candleHeight = size * 0.6f;
        float flameHeight = size * 0.5f * flameFlicker;
        float flameWidth = size * 0.15f;

        int baseX = static_cast<int>(cx);
        int baseY = static_cast<int>(cy);

        SDL_SetRenderDrawColor(renderer,
            static_cast<Uint8>(cr * brightness),
            static_cast<Uint8>(cg * brightness),
            static_cast<Uint8>(cb * brightness),
            255);
        SDL_Rect body = { baseX - static_cast<int>(candleWidth/2), baseY - static_cast<int>(candleHeight), 
                          static_cast<int>(candleWidth), static_cast<int>(candleHeight) };
        SDL_RenderFillRect(renderer, &body);

        SDL_SetRenderDrawColor(renderer,
            static_cast<Uint8>(cr * 0.3f * brightness),
            static_cast<Uint8>(cg * 0.3f * brightness),
            static_cast<Uint8>(cb * 0.3f * brightness),
            255);
        SDL_RenderDrawRect(renderer, &body);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        
        int wickX = baseX;
        int wickY = baseY - static_cast<int>(candleHeight);
        SDL_SetRenderDrawColor(renderer,
            static_cast<Uint8>(30 * brightness),
            static_cast<Uint8>(20 * brightness),
            static_cast<Uint8>(10 * brightness),
            255);
        SDL_RenderDrawLine(renderer, wickX, wickY, wickX, wickY - static_cast<int>(size * 0.1f));

        int flameBaseY = wickY - static_cast<int>(size * 0.1f);
        int flameTipY = flameBaseY - static_cast<int>(flameHeight);

        for (int y = flameTipY; y <= flameBaseY; ++y) {
            float t = 1.0f - static_cast<float>(flameBaseY - y) / flameHeight;
            int halfWidth = static_cast<int>(flameWidth * std::sqrt(t) * (0.8f + 0.2f * std::sin(flameP * 10.0f + y * 0.1f)));
            for (int x = -halfWidth; x <= halfWidth; ++x) {
                float dist = std::sqrt(static_cast<float>(x*x) / (halfWidth*halfWidth + 1) + 
                                     std::pow((y - flameBaseY) / flameHeight, 2));
                if (dist < 1.0f) {
                    Uint8 outerR = static_cast<Uint8>(std::min(255.0f, cr * 1.2f * brightness));
                    Uint8 outerG = static_cast<Uint8>(std::min(255.0f, cg * brightness));
                    Uint8 outerB = static_cast<Uint8>(std::min(255.0f, cb * 0.3f * brightness));
                    SDL_SetRenderDrawColor(renderer, outerR, outerG, outerB, 
                        static_cast<Uint8>(255 * (1.0f - dist * 0.5f)));
                    SDL_RenderDrawPoint(renderer, wickX + x, y);
                }
            }
        }

        for (int y = flameTipY + static_cast<int>(flameHeight*0.3f); y <= flameBaseY - static_cast<int>(flameHeight*0.2f); ++y) {
            float t = 1.0f - static_cast<float>(flameBaseY - y) / (flameHeight * 0.5f);
            int halfWidth = static_cast<int>(flameWidth * 0.6f * std::sqrt(t));
            for (int x = -halfWidth; x <= halfWidth; ++x) {
                Uint8 innerR = static_cast<Uint8>(std::min(255.0f, cr * 1.5f * brightness));
                Uint8 innerG = static_cast<Uint8>(std::min(255.0f, cg * 1.2f * brightness));
                Uint8 innerB = static_cast<Uint8>(std::min(255.0f, cb * 0.5f * brightness));
                SDL_SetRenderDrawColor(renderer, innerR, innerG, innerB, 255);
                SDL_RenderDrawPoint(renderer, wickX + x, y);
            }
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }

    void hslToRgb(float h, float s, float l, Uint8& r, Uint8& g, Uint8& b) {
        float c = (1 - std::abs(2 * l - 1)) * s;
        float x = c * (1 - std::abs(std::fmod(h / 60.0f, 2) - 1));
        float m = l - c / 2;

        float r_, g_, b_;
        if (h < 60) { r_ = c; g_ = x; b_ = 0; }
        else if (h < 120) { r_ = x; g_ = c; b_ = 0; }
        else if (h < 180) { r_ = 0; g_ = c; b_ = x; }
        else if (h < 240) { r_ = 0; g_ = x; b_ = c; }
        else if (h < 300) { r_ = x; g_ = 0; b_ = c; }
        else { r_ = c; g_ = 0; b_ = x; }

        r = static_cast<Uint8>((r_ + m) * 255);
        g = static_cast<Uint8>((g_ + m) * 255);
        b = static_cast<Uint8>((b_ + m) * 255);
    }

    void update() {
        for (auto& p : particles) {
            p.x += p.vx;
            p.y += p.vy;

            if (p.x < 0) { p.x = 0; p.vx *= -1; }
            if (p.x > width) { p.x = static_cast<float>(width); p.vx *= -1; }
            if (p.y < 0) { p.y = 0; p.vy *= -1; }
            if (p.y > height) { p.y = static_cast<float>(height); p.vy *= -1; }

            p.vx += (std::rand() % 100 - 50) / 800.0f;
            p.vy += (std::rand() % 100 - 50) / 800.0f;

            float maxSpeed = 1.0f;
            float speed = std::sqrt(p.vx * p.vx + p.vy * p.vy);
            if (speed > maxSpeed) {
                p.vx = (p.vx / speed) * maxSpeed;
                p.vy = (p.vy / speed) * maxSpeed;
            }
        }

        for (size_t i = 0; i < particles.size(); ++i) {
            for (size_t j = i + 1; j < particles.size(); ++j) {
                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dist = std::sqrt(dx * dx + dy * dy);
                float minDist = particles[i].radius + particles[j].radius + 50;

                if (dist < minDist && dist > 0) {
                    float force = (minDist - dist) / minDist * 0.01f;
                    float fx = (dx / dist) * force;
                    float fy = (dy / dist) * force;
                    particles[i].vx -= fx;
                    particles[i].vy -= fy;
                    particles[j].vx += fx;
                    particles[j].vy += fy;
                }
            }
        }
        brightnessCycle += 0.05f;
        backgroundBrightness = 1.5f + 0.7f * (0.5f + 0.5f * std::sin(brightnessCycle));
        
        for (auto& p : particles) {
            p.flamePhase += 0.05f;
        }
    }

    void draw(SDL_Renderer* renderer) 
    {
        float brightness = 0.3f + 1.7f * (0.5f + 0.5f * std::sin(brightnessCycle));

        for (size_t i = 0; i < particles.size(); ++i) 
        {
            for (size_t j = i + 1; j < particles.size(); ++j) 
            {
                float dx = particles[j].x - particles[i].x;
                float dy = particles[j].y - particles[i].y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < 400) {
                    float distFactor = std::max(0.0f, 1.0f - dist / 400.0f);
                    float alpha = distFactor * 0.6f;
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
                    SDL_SetRenderDrawColor(renderer,
                        static_cast<Uint8>(180 * brightness),
                        static_cast<Uint8>(120 * brightness),
                        static_cast<Uint8>(50 * brightness),
                        static_cast<Uint8>(std::min(255.0f, alpha * 255)));
                    
                    int x1 = static_cast<int>(particles[i].x);
                    int y1 = static_cast<int>(particles[i].y);
                    int x2 = static_cast<int>(particles[j].x);
                    int y2 = static_cast<int>(particles[j].y);
                    
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                    SDL_RenderDrawLine(renderer, x1+1, y1, x2+1, y2);
                    SDL_RenderDrawLine(renderer, x1, y1+1, x2, y2+1);
                }
            }
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        for (const auto& p : particles) 
        {
            drawCandle(renderer, p.x, p.y, p.radius, brightness, p.flamePhase, p.r, p.g, p.b);
        }
    } 
};

class BackgroundParticle {
public:
    float x, y;
    float phase;
    float size;
    int colorType;
};

class GradientBackground {
private:
    int width, height;
    float brightness;
    std::vector<BackgroundParticle> stars;
    float globalPhase;

public:
    GradientBackground(int w, int h) : width(w), height(h), brightness(1.0f), globalPhase(0.0f) {
        std::srand(std::time(nullptr));
        for (int i = 0; i < 80; ++i) {
            BackgroundParticle s;
            s.x = std::rand() % w;
            s.y = std::rand() % h;
            s.phase = std::rand() % 1000 / 1000.0f * 6.28f;
            s.size = 2 + (std::rand() % 30) / 10.0f;
            s.colorType = std::rand() % 3;
            stars.push_back(s);
        }
    }

    void update() {
        globalPhase += 0.03f;
        for (auto& s : stars) {
            s.phase += 0.05f + (std::rand() % 100) / 5000.0f;
        }
    }

    void setBrightness(float b) { brightness = b; }

    void drawStar(SDL_Renderer* renderer, int cx, int cy, float size, int colorType, float intensity) {
        float glowRadius = size * 3.0f;
        float coreRadius = size * 0.5f;
        float spikeLength = size * 2.5f;
        float spikeWidth = size * 0.15f;

        Uint8 r, g, b;
        if (colorType == 0) {
            r = 255; g = 255; b = 255;
        } else if (colorType == 1) {
            r = 180; g = 200; b = 255;
        } else {
            r = 255; g = 240; b = 180;
        }

        int coreR = static_cast<int>(r * intensity * brightness);
        int coreG = static_cast<int>(g * intensity * brightness);
        int coreB = static_cast<int>(b * intensity * brightness);

        for (int dy = -static_cast<int>(glowRadius); dy <= static_cast<int>(glowRadius); ++dy) {
            for (int dx = -static_cast<int>(glowRadius); dx <= static_cast<int>(glowRadius); ++dx) {
                float dist = std::sqrt(dx*dx + dy*dy);
                if (dist <= glowRadius) {
                    float alpha = intensity * 150 * (1.0f - dist / glowRadius);
                    if (alpha > 5) {
                        SDL_SetRenderDrawColor(renderer,
                            std::min(255, coreR),
                            std::min(255, coreG),
                            std::min(255, coreB),
                            static_cast<int>(alpha));
                        SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                    }
                }
            }
        }

        for (int i = 0; i < 8; ++i) {
            float angle = i * 3.14159f / 4.0f;
            bool isLongSpike = (i % 2 == 0);
            float len = isLongSpike ? spikeLength : spikeLength * 0.4f;
            float w = isLongSpike ? spikeWidth : spikeWidth * 0.8f;

            for (float t = 0; t <= 1.0f; t += 0.1f) {
                float px = cx + std::cos(angle) * len * t;
                float py = cy + std::sin(angle) * len * t;
                float width = w * (1.0f - t * 0.7f);
                float spikeAlpha = intensity * 255 * (1.0f - t * 0.5f);

                for (float dt = -width; dt <= width; dt += 1.0f) {
                    float perpAngle = angle + 3.14159f / 2.0f;
                    int drawX = static_cast<int>(px + std::cos(perpAngle) * dt);
                    int drawY = static_cast<int>(py + std::sin(perpAngle) * dt);

                    SDL_SetRenderDrawColor(renderer,
                        std::min(255, coreR),
                        std::min(255, coreG),
                        std::min(255, coreB),
                        static_cast<int>(spikeAlpha));
                    SDL_RenderDrawPoint(renderer, drawX, drawY);
                }
            }
        }

        for (int dy = -static_cast<int>(coreRadius); dy <= static_cast<int>(coreRadius); ++dy) {
            for (int dx = -static_cast<int>(coreRadius); dx <= static_cast<int>(coreRadius); ++dx) {
                if (dx*dx + dy*dy <= coreRadius * coreRadius) {
                    SDL_SetRenderDrawColor(renderer,
                        std::min(255, coreR),
                        std::min(255, coreG),
                        std::min(255, coreB),
                        255);
                    SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
                }
            }
        }
    }

    void draw(SDL_Renderer* renderer) {
        for (int y = 0; y < height; ++y) {
            float ratio = static_cast<float>(y) / height;
            int r = static_cast<int>(5 * brightness);
            int g = static_cast<int>(5 * brightness);
            int b = static_cast<int>(15 + ratio * 40 * brightness);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        for (const auto& s : stars) {
            float intensity = 0.2f + 0.8f * (0.5f + 0.5f * std::sin(s.phase + globalPhase));
            drawStar(renderer, static_cast<int>(s.x), static_cast<int>(s.y), s.size, s.colorType, intensity);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
};

int manage_beautiful_graphics(void) 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL Init failed: %s", SDL_GetError());
        return 1;
    }

    int width = 900;
    int height = 700;
    SDL_Window* window = SDL_CreateWindow(
        "Candle Symphony",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    GradientBackground bg(width, height);
    ParticleSystem particles(width, height, 15);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    width = event.window.data1;
                    height = event.window.data2;
                    SDL_RenderSetLogicalSize(renderer, 900, 700);
                }
            }
        }

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderClear(renderer);
        particles.update();
        bg.setBrightness(particles.getBackgroundBrightness());
        bg.update();
        bg.draw(renderer);
        particles.draw(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
