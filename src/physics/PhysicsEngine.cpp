// src/physics/PhysicsEngine.cpp
// Propietario: Nicolas
#include "PhysicsEngine.h"
#include <cmath>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────
// update(): integración de Euler hacia adelante.
// Concepto: pos_nueva = pos_vieja + vel * dt
// dt viene del reloj de SFML (sf::Clock::restart().asSeconds())
// ─────────────────────────────────────────────────────────────────────
void PhysicsEngine::update(std::vector<Particle>& particles,
                           double dt,
                           double worldW, double worldH) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        // Rebote en borde izquierdo / derecho
        if (p.x - p.radius < 0.0) {
            p.x  = p.radius;
            p.vx = std::abs(p.vx);
        } else if (p.x + p.radius > worldW) {
            p.x  = worldW - p.radius;
            p.vx = -std::abs(p.vx);
        }

        // Rebote en borde superior / inferior
        if (p.y - p.radius < 0.0) {
            p.y  = p.radius;
            p.vy = std::abs(p.vy);
        } else if (p.y + p.radius > worldH) {
            p.y  = worldH - p.radius;
            p.vy = -std::abs(p.vy);
        }

        // Limpiar flags de visualización cada frame
        p.highlighted = false;
        p.colliding   = false;
    }
}

// ─────────────────────────────────────────────────────────────────────
// resolveCollisions(): colisión elástica simplificada (masas iguales).
// Separa las partículas solapadas y refleja velocidades en la normal.
// ─────────────────────────────────────────────────────────────────────
void PhysicsEngine::resolveCollisions(
        std::vector<Particle>& particles,
        const std::vector<std::pair<int,int>>& pairs) {
    for (auto& [i, j] : pairs) {
        Particle& a = particles[i];
        Particle& b = particles[j];

        double dx   = b.x - a.x, dy = b.y - a.y;
        double dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 1e-9) continue;  // evitar división por cero

        // Separar partículas para eliminar solapamiento
        double overlap = (a.radius + b.radius) - dist;
        double nx = dx / dist, ny = dy / dist;
        a.x -= nx * overlap * 0.5;
        a.y -= ny * overlap * 0.5;
        b.x += nx * overlap * 0.5;
        b.y += ny * overlap * 0.5;

        // Intercambiar componente de velocidad en la dirección normal
        double dvx = b.vx - a.vx, dvy = b.vy - a.vy;
        double dot = dvx * nx + dvy * ny;
        if (dot < 0.0) {  // solo resolver si se aproximan
            a.vx += dot * nx;  a.vy += dot * ny;
            b.vx -= dot * nx;  b.vy -= dot * ny;
        }

        a.colliding = true;
        b.colliding = true;
    }
}
