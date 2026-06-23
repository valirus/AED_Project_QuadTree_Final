#include "BruteForce.h"

void BruteForce::detectCollisions(
        const std::vector<Particle>& particles,
        std::vector<std::pair<int, int>>& collisions,
        int& comparisons) {
    
    comparisons = 0;
    collisions.clear();
    int n = (int)particles.size();
    
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            comparisons++;
            float dx = (float)(particles[i].x - particles[j].x);
            float dy = (float)(particles[i].y - particles[j].y);
            float dist2 = dx * dx + dy * dy;
            
            float radSum = (float)(particles[i].radius + particles[j].radius);
            
            if (dist2 < radSum * radSum) {
                collisions.push_back({i, j});
            }
        }
    }
}