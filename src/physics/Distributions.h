// src/physics/Distributions.h
// Propietario: Nicolas
#pragma once
#include "Particle.h"
#include <vector>
#include <string>

class Distributions {
public:
    // Distribución 1: partículas repartidas uniformemente por todo el mundo
    static std::vector<Particle> uniform(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed);

    // Distribución 2: partículas agrupadas alrededor de k centros aleatorios
    static std::vector<Particle> clusters(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed,
        int numClusters = 5);

    // Distribución 3: 70% en zona superior-izquierda, 30% disperso
    static std::vector<Particle> denseZone(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed);

    // Utilidad: nombre legible para el HUD
    static std::string name(int index) {
        switch(index) {
            case 0: return "Uniforme";
            case 1: return "Clusters";
            case 2: return "Zona Densa";
            default: return "?";
        }
    }
};