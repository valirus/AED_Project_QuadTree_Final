#pragma once
#include "Particle.h"
#include <vector>
#include <string>

class Distributions {
public:
    static std::vector<Particle> uniform(int n, double W, double H,
        double minR, double maxR, double minSpd, double maxSpd);
    static std::vector<Particle> clusters(int n, double W, double H,
        double minR, double maxR, double minSpd, double maxSpd, int k = 5);
    static std::vector<Particle> denseZone(int n, double W, double H,
        double minR, double maxR, double minSpd, double maxSpd);
    static std::string name(int index) {
        switch(index) {
            case 0: return "Uniforme";
            case 1: return "Clusters";
            case 2: return "Zona Densa";
            default: return "?";
        }
    }
};