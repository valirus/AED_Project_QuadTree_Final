#pragma once
#include <SFML/Graphics.hpp>
#include "../quadtree/Rectangle.h"
#include "../physics/Particle.h"
#include <vector>

class Renderer {
public:
    static void drawParticles   (sf::RenderTarget& target,
                                 const std::vector<Particle>& particles);
    static void drawQuadTree    (sf::RenderTarget& target,
                                 const std::vector<Rectangle>& bounds);
    static void drawQueryRect   (sf::RenderTarget& target,
                                 const Rectangle& range);
    static void drawQueryCircle (sf::RenderTarget& target,
                                 float cx, float cy, float r);
};