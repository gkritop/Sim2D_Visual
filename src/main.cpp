// ====================================================================================
// Project: sim2d_raylib
// 
// What is this?
// A compact, beginner-friendly 2D simulation sandbox built on raylib. It visualizes
// grid-based "heat" diffusion and simple interactions in real time.
//
// How to run (generic):
//   mkdir build && cd build
//   cmake .. && cmake --build . && ./sim2d_raylib
//
// ====================================================================================

#include <raylib.h>
#include <vector>
#include <array>
#include <algorithm>
#include <string>

#include "hv/heat.hpp"
#include "hv/wave.hpp"
#include "hv/life.hpp"
#include "hv/color.hpp"

// Simulation modes available
enum class Mode { Menu, Heat, Wave, Life };

// Draws a simple text-based menu
static void draw_menu(int W, int H) {
    BeginDrawing();
    
    ClearBackground(BLACK);
    DrawText("sim2d-visual", W/2 - MeasureText("sim2d-visual", 40)/2, 40, 40, RAYWHITE);
    DrawText("Choose a simulation:", 60, 120, 24, RAYWHITE);
    DrawText("1) Heat equation (explicit)", 80, 160, 22, RAYWHITE);
    DrawText("2) Wave equation (leapfrog)", 80, 190, 22, RAYWHITE);
    DrawText("3) Game of Life", 80, 220, 22, RAYWHITE);
    DrawText("ESC to quit", 80, 260, 20, GRAY);
    
    EndDrawing();
}

int main() {
    const int NX = 256, NY = 256;
    const int SCALE = 2;
    const int W = NX*SCALE, H = NY*SCALE;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
// --- Window & renderer bootstrap ---
    
    InitWindow(W, H, "sim2d-visual");
    SetTargetFPS(120);

    Image img = GenImageColor(NX, NY, BLACK);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    hv::Heat2D heat(NX, NY);
    heat.alpha = 0.2; 
    heat.dt = std::min(1e-4, heat.stable_dt_max()*0.9);

    hv::Wave2D wave(NX, NY);
    wave.c = 1.0; 
    wave.dt = std::min(1e-3, wave.stable_dt_max()*0.9);

    hv::Life2D life(NX, NY); 
    life.randomize(0.15);

    Mode mode = Mode::Menu;
    bool paused = false;
    int colormap = 1; // 0 gray, 1 fire, 2 blue-red

// === Main loop: update + draw, until the window closes ===
    while (!WindowShouldClose()) {
        if (mode == Mode::Menu) {
            if (IsKeyPressed(KEY_ONE)) mode = Mode::Heat;
            if (IsKeyPressed(KEY_TWO)) mode = Mode::Wave;
            if (IsKeyPressed(KEY_THREE)) mode = Mode::Life;
            if (IsKeyPressed(KEY_ESCAPE)) break;
            draw_menu(GetScreenWidth(), GetScreenHeight());
            continue;
        }

        // Common controls
        if (IsKeyPressed(KEY_SPACE)) paused = !paused;
        if (IsKeyPressed(KEY_R)) {
            if (mode==Mode::Heat) heat.reset();
            else if (mode==Mode::Wave) wave.reset();
            else if (mode==Mode::Life) life.clear();
        }
        if (IsKeyPressed(KEY_ONE)) colormap = 0;
        if (IsKeyPressed(KEY_TWO)) colormap = 1;
        if (IsKeyPressed(KEY_THREE)) colormap = 2;
        if (IsKeyPressed(KEY_M)) mode = Mode::Menu;
        if (IsKeyPressed(KEY_S)) {
            Image shot = LoadImageFromTexture(tex); 
            ExportImage(shot, "frame.png"); 
            UnloadImage(shot); 
        }

        // Mode-specific tweaks
        if (mode==Mode::Heat) {
            if (IsKeyPressed(KEY_UP)) heat.alpha *= 1.2;
            if (IsKeyPressed(KEY_DOWN)) heat.alpha /= 1.2;
            if (IsKeyPressed(KEY_RIGHT)) heat.dt *= 1.2;
            if (IsKeyPressed(KEY_LEFT))  heat.dt /= 1.2;
            
        } else if (mode==Mode::Wave) {
            if (IsKeyPressed(KEY_UP)) wave.c *= 1.1;
            if (IsKeyPressed(KEY_DOWN)) wave.c /= 1.1;
            if (IsKeyPressed(KEY_RIGHT)) wave.dt *= 1.2;
            if (IsKeyPressed(KEY_LEFT))  wave.dt /= 1.2;
            
        } else if (mode==Mode::Life) {
            if (IsKeyPressed(KEY_C)) life.clear();
            if (IsKeyPressed(KEY_N)) life.step(); // single-step
            if (IsKeyPressed(KEY_P)) life.randomize(0.15); // repopulate
        }

        // Painting
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            int winW = GetScreenWidth(), winH = GetScreenHeight();
            float sx = (float)NX / (float)winW;
            float sy = (float)NY / (float)winH;
            int ix = (int)(m.x * sx);
            int iy = (int)(m.y * sy);
            
            if (mode==Mode::Heat) heat.paint(ix, iy, 6, 0.5f);
            else if (mode==Mode::Wave) wave.paint(ix, iy, 6, 0.5f);
            else if (mode==Mode::Life) life.toggle(ix, iy, 2);
        }

        // Step
        if (!paused) {
            if (mode==Mode::Heat) {
                double mx = heat.stable_dt_max();
                int sub = 1; double subdt = heat.dt;
                if (heat.dt > mx) { sub = std::min(10, (int)std::ceil(heat.dt/mx)); subdt = heat.dt/sub; }
                double old = heat.dt;
                
                for (int i=0;i<sub;++i){ heat.dt=subdt; heat.step(); }
                heat.dt=old;
                
            } else if (mode==Mode::Wave) {
                double mx = wave.stable_dt_max();
                int sub = 1; double subdt = wave.dt;
                
                if (wave.dt > mx) { sub = std::min(10, (int)std::ceil(wave.dt/mx)); subdt = wave.dt/sub; }
                double old = wave.dt;
                
                for (int i=0;i<sub;++i){ wave.dt=subdt; wave.step(); }
                wave.dt=old;
                
            } else if (mode==Mode::Life) {
                life.step();
            }
        }

        // Pixels
        std::vector<unsigned char> pixels((std::size_t)NX*(std::size_t)NY*4);

        if (mode==Mode::Heat) {
            float vmax = 1e-6f;
            
            for (float v : heat.u) vmax = std::max(vmax, v);
            float inv = 1.0f / vmax;
            
            for (std::size_t j=0;j<heat.g.ny;++j)
                
                for (std::size_t i=0;i<heat.g.nx;++i) {
                    std::size_t k = heat.g.idx(i,j);
                    float t = std::clamp(heat.u[k]*inv, 0.0f, 1.0f);
                    auto c = (colormap==0?hv::gray(t):(colormap==1?hv::fire(t):hv::blue_red(t)));
                    pixels[4*k+0]=c[0]; pixels[4*k+1]=c[1]; pixels[4*k+2]=c[2]; pixels[4*k+3]=c[3];
                }
        } else if (mode==Mode::Wave) {
            float vmax = 1e-6f;
            
            for (float v : wave.u) vmax = std::max(vmax, std::abs(v));
            float inv = 0.5f / vmax;
            
            for (std::size_t j=0;j<wave.g.ny;++j)
                
                for (std::size_t i=0;i<wave.g.nx;++i) {
                    std::size_t k = wave.g.idx(i,j);
                    float t = std::clamp(0.5f + wave.u[k]*inv, 0.0f, 1.0f);
                    auto c = (colormap==0?hv::gray(t):(colormap==1?hv::fire(t):hv::blue_red(t)));
                    pixels[4*k+0]=c[0]; pixels[4*k+1]=c[1]; pixels[4*k+2]=c[2]; pixels[4*k+3]=c[3];
                }
        } else if (mode==Mode::Life) {
            
            for (std::size_t j=0;j<life.ny;++j)
                
                for (std::size_t i=0;i<life.nx;++i) {
                    std::size_t k = j*life.nx + i;
                    float t = life.a[k] ? 1.0f : 0.0f;
                    auto c = (colormap==0?hv::gray(t):(colormap==1?hv::fire(t):hv::blue_red(t)));
                    pixels[4*k+0]=c[0]; pixels[4*k+1]=c[1]; pixels[4*k+2]=c[2]; pixels[4*k+3]=c[3];
                }
        }
        UpdateTexture(tex, pixels.data());

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(tex, {0,0,(float)tex.width,(float)tex.height},
                            {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                            {0,0}, 0.0f, WHITE);

        // HUD
        int margin = 12;
        int pad = 6;
        int lineH = 20;
        int y = margin + pad;

        // Dynamic HUD height
        int hudHeight = (mode == Mode::Life) ? 3*lineH + 2*pad : 4*lineH + 2*pad;
        DrawRectangle(margin, margin, GetScreenWidth() - 2*margin, hudHeight, Fade(BLACK, 0.55f));

        if (mode == Mode::Heat) {
            double dtmax = heat.stable_dt_max();
            DrawText(TextFormat("HEAT   alpha=%.4f   dt=%.6g   (<= %.6g stable)", heat.alpha, heat.dt, dtmax),
                     margin+10, y, 18, RAYWHITE);
            y += lineH + pad;
            DrawText("Mouse: paint    Space: pause    R: reset", margin+10, y, 16, RAYWHITE);
            y += lineH;
            DrawText("1/2/3: colormap    S: save    M: menu    Arrows: alpha/dt +/-", margin+10, y, 16, RAYWHITE);
            
            if (heat.dt > dtmax*1.05)
                DrawText("WARNING: dt above stability -> using substeps", margin+10, y+lineH, 16, RED);

        } else if (mode == Mode::Wave) {
            double dtmax = wave.stable_dt_max();
            DrawText(TextFormat("WAVE   c=%.4f   dt=%.6g   (<= %.6g stable)", wave.c, wave.dt, dtmax),
                     margin+10, y, 18, RAYWHITE);
            y += lineH + pad;
            DrawText("Mouse: add displacement    Space: pause    R: reset", margin+10, y, 16, RAYWHITE);
            y += lineH;
            DrawText("1/2/3: colormap    S: save    M: menu    Arrows: c/dt +/-", margin+10, y, 16, RAYWHITE);
            
            if (wave.dt > dtmax*1.05)
                DrawText("WARNING: dt above stability -> using substeps", margin+10, y+lineH, 16, RED);

        } else if (mode == Mode::Life) {
            DrawText("LIFE", margin+10, y, 18, RAYWHITE);
            y += lineH + pad;
            DrawText("Mouse: toggle cells    Space: run/pause    R: clear", margin+10, y, 16, RAYWHITE);
            y += lineH;
            DrawText("P: randomize    N: single-step    1/2/3: colormap    M: menu", margin+10, y, 16, RAYWHITE);
        }

// --- Drawing phase ends ---
        EndDrawing();
    }

    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
