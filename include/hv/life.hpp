// Conway's Game of Life implementation
#pragma once
#include <vector>
#include <cstddef>
#include <algorithm>
#include <random>

namespace hv {

struct Life2D {
    std::size_t nx{}, ny{};
    std::vector<uint8_t> a, b; // 0 or 1

    explicit Life2D(std::size_t nx_=256, std::size_t ny_=256) 
        : nx(nx_), ny(ny_), a(nx_*ny_, 0), b(nx_*ny_, 0) {}

    inline std::size_t idx(std::size_t i, std::size_t j) const { return j*nx + i; }

    void randomize(double p=0.15) {
        std::mt19937 rng(12345);
        std::bernoulli_distribution bern(p);
        for (auto &v : a) v = bern(rng) ? 1 : 0;
    }

    void step() {
        auto alive = [&](int x,int y)->int {
            if (x<0 || y<0 || x>= (int)nx || y>= (int)ny) return 0;
            return a[idx((std::size_t)x,(std::size_t)y)] ? 1 : 0;
        };
        for (std::size_t j=0;j<ny;++j){
            for (std::size_t i=0;i<nx;++i){
                int n = alive((int)i-1,(int)j-1)+alive((int)i,(int)j-1)+alive((int)i+1,(int)j-1)
                      + alive((int)i-1,(int)j)+                         alive((int)i+1,(int)j)
                      + alive((int)i-1,(int)j+1)+alive((int)i,(int)j+1)+alive((int)i+1,(int)j+1);
                uint8_t cur = a[idx(i,j)];
                uint8_t nxt = cur;
                if (cur) nxt = (n==2 || n==3) ? 1 : 0;
                else     nxt = (n==3) ? 1 : 0;
                b[idx(i,j)] = nxt;
            }
        }
        a.swap(b);
    }

    void toggle(int ix, int iy, int radius=0) {
        if (ix<0 || iy<0 || ix>=(int)nx || iy>=(int)ny) return;
        if (radius<=0) {
            a[idx((std::size_t)ix,(std::size_t)iy)] ^= 1;
        } else {
            int r2 = radius*radius;
            for (int y=std::max(0,iy-radius); y<std::min<int>(ny,iy+radius+1); ++y)
                for (int x=std::max(0,ix-radius); x<std::min<int>(nx,ix+radius+1); ++x) {
                    int dx=x-ix, dy=y-iy; if (dx*dx+dy*dy<=r2) a[idx((std::size_t)x,(std::size_t)y)]=1;
                }
        }
    }

    void clear() { std::fill(a.begin(), a.end(), 0); }
};

}
