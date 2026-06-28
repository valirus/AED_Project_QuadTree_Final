// src/rendering/HUD.h
// Propietario: Mikael
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    static void draw(sf::RenderTarget& target,
                     const sf::Font& font,
                     int fps, int n,
                     int qtComp, int bfComp,
                     float qtMs, float bfMs,
                     float candidatesPerParticle,
                     const std::string& distribution,
                     bool showGrid, bool runBF);
};