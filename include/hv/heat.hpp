// Heat equation simulation (explicit scheme)
#pragma once
#include <vector>
#include <cstddef>
#include <algorithm>

namespace hv {

struct Grid {
    std::size_t nx{}, ny{};
    double hx{}, hy{};
    
    explicit Grid(std::size_t nx_=256, std::size_t ny_=256) 
        : nx(nx_), ny(ny_) { hx = 1.0 / double(nx-1); hy = 1.0 / double(ny-1); }

    std::size_t size() const { return nx*ny; }
    std::size_t idx(std::size_t i, std::size_t j) const { return j*nx + i; }
};

struct Heat2D {
    Grid g;
    double alpha{0.2};
    double dt{1e-4};
    std::vector<float> u, u_next;

    explicit Heat2D(std::size_t nx=256, std::size_t ny=256)
        : g(nx,ny), u(g.size(), 0.0f), u_next(g.size(), 0.0f) {}

    void step() {
        const double invhx2 = 1.0/(g.hx*g.hx);
        const double invhy2 = 1.0/(g.hy*g.hy);
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
                    u_next[k] = static_cast<float>(uij + dt*alpha*lap);
                }
            }
        }
        u.swap(u_next);
    }

    void paint(int ix, int iy, int radius, float amp) {
        const int r2 = radius*radius;
        for (int y=std::max(1, iy-radius); y<std::min<int>(g.ny-1, iy+radius); ++y) {
            for (int x=std::max(1, ix-radius); x<std::min<int>(g.nx-1, ix+radius); ++x) {
                int dx = x-ix, dy = y-iy;
                if (dx*dx + dy*dy <= r2) {
                    u[g.idx(static_cast<std::size_t>(x), static_cast<std::size_t>(y))] += amp;
                }
            }
        }
    }

    double stable_dt_max() const {
        const double h2 = std::min(g.hx*g.hx, g.hy*g.hy);
        return 0.5 * h2 / std::max(1e-12, alpha);
    }

    void reset() { std::fill(u.begin(), u.end(), 0.0f); }
};

}
