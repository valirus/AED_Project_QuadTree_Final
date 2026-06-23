#pragma once

struct Rectangle {
    float x, y;    // centro del rectángulo
    float hw, hh;  // half-width y half-height

    Rectangle() : x(0), y(0), hw(0), hh(0) {}
    Rectangle(float x, float y, float hw, float hh)
        : x(x), y(y), hw(hw), hh(hh) {}

    // ¿El punto (px, py) cae dentro de esta región?
    bool contains(float px, float py) const {
        return (px >= x - hw && px < x + hw &&
                py >= y - hh && py < y + hh);
    }

    // ¿Esta región se solapa con otra? (descartar hijos irrelevantes)
    bool intersects(const Rectangle& o) const {
        return !(o.x - o.hw >= x + hw ||
                 o.x + o.hw <= x - hw ||
                 o.y - o.hh >= y + hh ||
                 o.y + o.hh <= y - hh);
    }
};