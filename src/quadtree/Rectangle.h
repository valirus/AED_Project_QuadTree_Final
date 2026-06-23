#pragma once

struct Rectangle {
    float x, y;   // Centro del rectángulo
    float hw, hh; // Mitad del ancho (half-width) y mitad del alto (half-height)

    Rectangle(float x, float y, float hw, float hh)
        : x(x), y(y), hw(hw), hh(hh) {}

    // Verifica si un punto (px, py) está dentro de este rectángulo
    bool contains(float px, float py) const {
        return (px >= x - hw && px <= x + hw &&
                py >= y - hh && py <= y + hh);
    }

    // Verifica si este rectángulo se intersecta (solapa) con otro
    bool intersects(const Rectangle& other) const {
        return !(other.x - other.hw > x + hw ||
                 other.x + other.hw < x - hw ||
                 other.y - other.hh > y + hh ||
                 other.y + other.hh < y - hh);
    }
};