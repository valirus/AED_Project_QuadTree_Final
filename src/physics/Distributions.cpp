// src/physics/Distributions.cpp
// Propietario: Nicolas
#include "Distributions.h"
#include <random>
#include <cmath>
#include <algorithm>
#include <utility>

// Semilla fija para reproducibilidad (importante para el reporte)
static std::mt19937 rng(42);
static constexpr double PI = 3.14159265358979323846;

static double sampleCoord(double lo, double hi) {
    if (hi < lo) return 0.5 * (lo + hi);
    std::uniform_real_distribution<double> d(lo, hi);
    return d(rng);
}

static void assignVelocity(Particle& p, double minSpeed, double maxSpeed) {
    minSpeed = std::max(0.0, minSpeed);
    maxSpeed = std::max(minSpeed, maxSpeed);

    std::uniform_real_distribution<double> distAngle(0.0, 2.0 * PI);
    std::uniform_real_distribution<double> distSpeed(minSpeed, maxSpeed);

    double angle = distAngle(rng);
    double speed = distSpeed(rng);
    p.vx = std::cos(angle) * speed;
    p.vy = std::sin(angle) * speed;
}

// Distribución 1: UNIFORME
// Cada coordenada es independiente y uniformemente distribuida.
std::vector<Particle> Distributions::uniform(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    if (maxR < minR) std::swap(maxR, minR);
    std::uniform_real_distribution<double> distR(minR, maxR);

    std::vector<Particle> ps(n);
    for (int i = 0; i < n; i++) {
        ps[i].id     = i;
        ps[i].radius = distR(rng);
        ps[i].x      = sampleCoord(ps[i].radius, worldW - ps[i].radius);
        ps[i].y      = sampleCoord(ps[i].radius, worldH - ps[i].radius);
        assignVelocity(ps[i], minSpeed, maxSpeed);
    }
    return ps;
}

// Distribución 2: CLUSTERS
// Centros aleatorios, partículas con ruido gaussiano alrededor.
std::vector<Particle> Distributions::clusters(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed,
        int numClusters) {
    if (maxR < minR) std::swap(maxR, minR);
    numClusters = std::max(1, numClusters);

    std::normal_distribution<double>       spread(0.0, 70.0);
    std::uniform_real_distribution<double> distR(minR, maxR);
    std::uniform_int_distribution<int>     distC(0, numClusters - 1);

    // Generar centros dentro del mundo, evitando bordes extremos.
    std::vector<std::pair<double,double>> centers(numClusters);
    for (auto& c : centers) {
        c = { sampleCoord(worldW * 0.10, worldW * 0.90),
              sampleCoord(worldH * 0.10, worldH * 0.90) };
    }

    std::vector<Particle> ps(n);
    for (int i = 0; i < n; i++) {
        auto [ox, oy] = centers[distC(rng)];
        ps[i].id     = i;
        ps[i].radius = distR(rng);
        ps[i].x      = std::clamp(ox + spread(rng), ps[i].radius, worldW - ps[i].radius);
        ps[i].y      = std::clamp(oy + spread(rng), ps[i].radius, worldH - ps[i].radius);
        assignVelocity(ps[i], minSpeed, maxSpeed);
    }
    return ps;
}

// Distribución 3: ZONA DE ALTA DENSIDAD
// 70% en el cuadrante superior-izquierdo, 30% disperso.
std::vector<Particle> Distributions::denseZone(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    if (maxR < minR) std::swap(maxR, minR);
    int densePart = (int)(n * 0.70);

    std::uniform_real_distribution<double> distR(minR, maxR);

    std::vector<Particle> ps;
    ps.reserve(n);

    for (int i = 0; i < densePart; i++) {
        Particle p;
        p.id = i;
        p.radius = distR(rng);
        p.x = sampleCoord(p.radius, worldW * 0.30);
        p.y = sampleCoord(p.radius, worldH * 0.30);
        assignVelocity(p, minSpeed, maxSpeed);
        ps.push_back(p);
    }
    for (int i = densePart; i < n; i++) {
        Particle p;
        p.id = i;
        p.radius = distR(rng);
        p.x = sampleCoord(p.radius, worldW - p.radius);
        p.y = sampleCoord(p.radius, worldH - p.radius);
        assignVelocity(p, minSpeed, maxSpeed);
        ps.push_back(p);
    }
    return ps;
}
