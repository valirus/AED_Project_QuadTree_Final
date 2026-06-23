#include "BruteForce.h"
#include <cmath>

// Todos contra todos — referencia O(n²) que el QuadTree debe superar
void BruteForce::detectCollisions(
        const std::vector<Particle>& particles,
        std::vector<std::pair<int,int>>& collisions,
        int& comparisons) {
    comparisons = 0;
    collisions.clear();
    int n = (int)particles.size();

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            comparisons++;
            float dx = (float)(particles[i].x - particles[j].x);
            float dy = (float)(particles[i].y - particles[j].y);
            float radSum = (float)(particles[i].radius + particles[j].radius);
            if (dx*dx + dy*dy < radSum*radSum)
                collisions.push_back({i, j});
        }
    }
}

void BruteForce::queryRadius(
        const std::vector<Particle>& particles,
        float cx, float cy, float r,
        std::vector<int>& found,
        int& comparisons) {
    comparisons = 0;
    found.clear();
    for (int i = 0; i < (int)particles.size(); i++) {
        comparisons++;
        float dx = (float)particles[i].x - cx;
        float dy = (float)particles[i].y - cy;
        if (dx*dx + dy*dy <= r*r)
            found.push_back(i);
    }
}