#pragma once
#include "Particle.h"
#include <vector>
#include <utility>

class PhysicsEngine {
public:
    static void update(std::vector<Particle>& particles,
                       double dt, double worldW, double worldH);
    static void resolveCollisions(std::vector<Particle>& particles,
                                  const std::vector<std::pair<int,int>>& pairs);
};