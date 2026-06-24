#pragma once

// Temporarl para que pueda correr, borrar despues

struct Particle {
    int    id     = 0;
    double x      = 0.0, y  = 0.0;
    double vx     = 0.0, vy = 0.0;
    double radius = 5.0;

    bool highlighted = false;
    bool colliding   = false;
};