// src/rendering/HUD.cpp
// Propietario: Mikael
// SFML 3.0.2 — sf::Text requiere font en el constructor
#include "HUD.h"
#include <sstream>
#include <iomanip>

void HUD::draw(sf::RenderTarget& target,
               const sf::Font& font,
               int fps, int n,
               int qtComp, int bfComp,
               float qtMs, float bfMs,
               float candidatesPerParticle,
               const std::string& distribution,
               bool showGrid, bool runBF) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "FPS: " << fps << "\n"
       << "Particulas: " << n << "\n"
       << "Dist: " << distribution << "\n"
       << "Grid: " << (showGrid ? "ON" : "OFF")
       << "  BF: " << (runBF ? "ON" : "OFF") << "\n"
       << "\n"
       << "--- QuadTree ---\n"
       << "Comp.: " << qtComp << "\n"
       << "Cand/P: " << candidatesPerParticle << "\n"
       << "Tiempo: " << qtMs << " ms\n"
       << "\n"
       << "--- Fuerza Bruta ---\n"
       << "Comp.: " << bfComp << "\n"
       << "Tiempo: " << bfMs << " ms\n"
       << "\n"
       << "Speedup: ";
    if (runBF && qtMs > 0.001f)
        ss << (bfMs / qtMs) << "x";
    else
        ss << "N/A";
    ss << "\n\n"
       << "[1/2/3] Distribucion\n"
       << "[Q] Grid ON/OFF\n"
       << "[B] BruteForce ON/OFF\n"
       << "[+/-] Particulas\n"
       << "[C] Modo circulo\n"
       << "[R] Modo rectangulo";

    // Fondo semitransparente
    sf::RectangleShape bg({270.f, 380.f});
    bg.setPosition({8.f, 8.f});
    bg.setFillColor(sf::Color(10, 10, 20, 185));
    target.draw(bg);

    // SFML 3.0.2: sf::Text(font, string, characterSize)
    sf::Text text(font, ss.str(), 13);
    text.setFillColor(sf::Color(220, 230, 255));
    text.setPosition({16.f, 16.f});
    target.draw(text);
}