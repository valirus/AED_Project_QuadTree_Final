// src/physics/Particle.h
// Propietario: Nicolas
// Struct puro — CERO dependencias de SFML ni de renderizado
#pragma once

struct Particle {
    int    id     = 0;
    double x      = 0.0, y  = 0.0;   // posición
    double vx     = 0.0, vy = 0.0;   // velocidad
    double radius = 5.0;

    // Flags de estado — los lee el Renderer para decidir el color
    bool highlighted = false;  // candidata de una query del QuadTree
    bool colliding   = false;  // detectada en colisión este frame
};