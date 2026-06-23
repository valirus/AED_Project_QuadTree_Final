#pragma once
#include "../physics/Particle.h"
#include <vector>
#include <utility>

class BruteForce {
public:
    static void detectCollisions(
        const std::vector<Particle>& particles,
        std::vector<std::pair<int, int>>& collisions,
        int& comparisons
    );
};