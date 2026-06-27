// src/physics/PhysicsEngine.h
// Propietario: Nicolas
#pragma once
#include "Particle.h"
#include <vector>
#include <utility>

class PhysicsEngine {
public:
    // Integración de Euler + rebote en bordes del mundo
    static void update(std::vector<Particle>& particles,
                       double dt,
                       double worldW, double worldH);

    // Resolver colisiones detectadas (intercambio elástico de velocidades)
    static void resolveCollisions(std::vector<Particle>& particles,
                                  const std::vector<std::pair<int,int>>& pairs);
};