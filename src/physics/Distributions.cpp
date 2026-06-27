// src/physics/Distributions.cpp
// Propietario: Nicolas
#include "Distributions.h"
#include <random>
#include <cmath>
#include <algorithm>

// Semilla fija para reproducibilidad (importante para el reporte)
static std::mt19937 rng(42);

// ─────────────────────────────────────────────────────────────────────
// Distribución 1: UNIFORME
// Cada coordenada es independiente y uniformemente distribuida.
// ─────────────────────────────────────────────────────────────────────
std::vector<Particle> Distributions::uniform(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    std::uniform_real_distribution<double> distX(0.0, worldW);
    std::uniform_real_distribution<double> distY(0.0, worldH);
    std::uniform_real_distribution<double> distV(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> distR(minR, maxR);

    std::vector<Particle> ps(n);
    for (int i = 0; i < n; i++) {
        ps[i].id     = i;
        ps[i].x      = distX(rng);
        ps[i].y      = distY(rng);
        ps[i].vx     = distV(rng);
        ps[i].vy     = distV(rng);
        ps[i].radius = distR(rng);
    }
    return ps;
}

// ─────────────────────────────────────────────────────────────────────
// Distribución 2: CLUSTERS
// Centros aleatorios, partículas con ruido gaussiano alrededor.
// Evidencia el comportamiento del QuadTree con zonas de alta densidad local.
// ─────────────────────────────────────────────────────────────────────
std::vector<Particle> Distributions::clusters(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed,
        int numClusters) {
    std::uniform_real_distribution<double> distCX(50.0, worldW - 50.0);
    std::uniform_real_distribution<double> distCY(50.0, worldH - 50.0);
    std::normal_distribution<double>       spread(0.0, 70.0);
    std::uniform_real_distribution<double> distV(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> distR(minR, maxR);
    std::uniform_int_distribution<int>     distC(0, numClusters - 1);

    // Generar centros de los clusters
    std::vector<std::pair<double,double>> centers(numClusters);
    for (auto& c : centers) c = { distCX(rng), distCY(rng) };

    std::vector<Particle> ps(n);
    for (int i = 0; i < n; i++) {
        auto [ox, oy] = centers[distC(rng)];
        ps[i].id     = i;
        ps[i].x      = std::clamp(ox + spread(rng), 0.0, worldW);
        ps[i].y      = std::clamp(oy + spread(rng), 0.0, worldH);
        ps[i].vx     = distV(rng);
        ps[i].vy     = distV(rng);
        ps[i].radius = distR(rng);
    }
    return ps;
}

// ─────────────────────────────────────────────────────────────────────
// Distribución 3: ZONA DE ALTA DENSIDAD
// 70% en el cuadrante superior-izquierdo, 30% disperso.
// Útil para mostrar el caso degenerado del QuadTree.
// ─────────────────────────────────────────────────────────────────────
std::vector<Particle> Distributions::denseZone(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    int densePart  = (int)(n * 0.70);
    int sparsePart = n - densePart;

    std::uniform_real_distribution<double> denseX(0.0, worldW * 0.30);
    std::uniform_real_distribution<double> denseY(0.0, worldH * 0.30);
    std::uniform_real_distribution<double> sparseX(0.0, worldW);
    std::uniform_real_distribution<double> sparseY(0.0, worldH);
    std::uniform_real_distribution<double> distV(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> distR(minR, maxR);

    std::vector<Particle> ps;
    ps.reserve(n);

    for (int i = 0; i < densePart; i++) {
        Particle p;
        p.id = i; p.x = denseX(rng); p.y = denseY(rng);
        p.vx = distV(rng); p.vy = distV(rng); p.radius = distR(rng);
        ps.push_back(p);
    }
    for (int i = densePart; i < n; i++) {
        Particle p;
        p.id = i; p.x = sparseX(rng); p.y = sparseY(rng);
        p.vx = distV(rng); p.vy = distV(rng); p.radius = distR(rng);
        ps.push_back(p);
    }
    return ps;
}