#pragma once
#include "../physics/Particle.h"
#include <vector>
#include <utility>

class BruteForce {
public:
    // Compara todos contra todos: O(n²)
    static void detectCollisions(
        const std::vector<Particle>& particles,
        std::vector<std::pair<int,int>>& collisions,
        int& comparisons);

    // Vecinos en radio: O(n) por partícula consultada
    static void queryRadius(
        const std::vector<Particle>& particles,
        float cx, float cy, float r,
        std::vector<int>& found,
        int& comparisons);
};