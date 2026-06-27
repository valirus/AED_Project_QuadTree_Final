#include "Renderer.h"

void Renderer::drawParticles(sf::RenderTarget& target,
                             const std::vector<Particle>& particles) {
    sf::CircleShape shape;
    for (const auto& p : particles) {
        float r = (float)p.radius;
        shape.setRadius(r);
        shape.setOrigin({r, r});                       // SFML3: Vector2f
        shape.setPosition({(float)p.x, (float)p.y});  // SFML3: Vector2f

        if (p.colliding)
            shape.setFillColor(sf::Color(255, 80, 80));    // rojo = colisión
        else if (p.highlighted)
            shape.setFillColor(sf::Color(80, 220, 80));    // verde = candidata
        else
            shape.setFillColor(sf::Color(140, 180, 255));  // azul = normal

        target.draw(shape);
    }
}

void Renderer::drawQuadTree(sf::RenderTarget& target,
                            const std::vector<Rectangle>& bounds) {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color(60, 80, 160, 130));
    rect.setOutlineThickness(1.0f);

    for (const auto& b : bounds) {
        rect.setSize({b.hw * 2.0f, b.hh * 2.0f});     // SFML3: Vector2f
        rect.setPosition({b.x - b.hw, b.y - b.hh});   // SFML3: Vector2f
        target.draw(rect);
    }
}

void Renderer::drawQueryRect(sf::RenderTarget& target,
                             const Rectangle& range) {
    sf::RectangleShape rect;
    rect.setSize({range.hw * 2.0f, range.hh * 2.0f});
    rect.setPosition({range.x - range.hw, range.y - range.hh});
    rect.setFillColor(sf::Color(255, 255, 0, 35));
    rect.setOutlineColor(sf::Color::Yellow);
    rect.setOutlineThickness(2.0f);
    target.draw(rect);
}

void Renderer::drawQueryCircle(sf::RenderTarget& target,
                               float cx, float cy, float r) {
    sf::CircleShape circle(r);
    circle.setOrigin({r, r});
    circle.setPosition({cx, cy});
    circle.setFillColor(sf::Color(100, 255, 100, 30));
    circle.setOutlineColor(sf::Color(100, 255, 100, 200));
    circle.setOutlineThickness(2.0f);
    target.draw(circle);
}