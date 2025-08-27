// Wave equation simulation (leapfrog method)
#pragma once
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>

namespace hv {

struct Wave2D {
    Grid g;
    double c{1.0};
    double dt{1e-3};
    std::vector<float> u, u_prev, u_next;

    explicit Wave2D(std::size_t nx=256, std::size_t ny=256)
        : g(nx,ny), u(g.size(), 0.0f), u_prev(g.size(), 0.0f), u_next(g.size(), 0.0f) {}

    void step() {
        const double invhx2 = 1.0/(g.hx*g.hx);
        const double invhy2 = 1.0/(g.hy*g.hy);
        double c2dt2 = (c*c)*(dt*dt);
        for (std::size_t j=0; j<g.ny; ++j) {
            for (std::size_t i=0; i<g.nx; ++i) {
                const std::size_t k = g.idx(i,j);
                const bool boundary = (i==0 || j==0 || i==g.nx-1 || j==g.ny-1);
                if (boundary) {
                    u_next[k] = 0.0f;
                } else {
                    float uij = u[k];
                    float ux1 = u[g.idx(i+1,j)];
                    float ux0 = u[g.idx(i-1,j)];
                    float uy1 = u[g.idx(i,j+1)];
                    float uy0 = u[g.idx(i,j-1)];
                    double lap = (ux1 - 2.0*uij + ux0)*invhx2 + (uy1 - 2.0*uij + uy0)*invhy2;
                    u_next[k] = static_cast<float>(2.0*uij - u_prev[k] + c2dt2*lap);
                }
            }
        }
        u_prev.swap(u);
        u.swap(u_next);
    }

    void paint(int ix, int iy, int radius, float amp) {
        const int r2 = radius*radius;
        for (int y=std::max(1, iy-radius); y<std::min<int>(g.ny-1, iy+radius); ++y) {
            for (int x=std::max(1, ix-radius); x<std::min<int>(g.nx-1, ix+radius); ++x) {
                int dx = x-ix, dy = y-iy;
                if (dx*dx + dy*dy <= r2) {
                    std::size_t k = g.idx(static_cast<std::size_t>(x), static_cast<std::size_t>(y));
                    u[k] += amp;
                }
            }
        }
    }

    double stable_dt_max() const {
        double h = std::min(g.hx, g.hy);
        return h / (std::max(1e-12, c) * std::sqrt(2.0));
    }

    void reset() { std::fill(u.begin(), u.end(), 0.0f); std::fill(u_prev.begin(), u_prev.end(), 0.0f); }
};

}
