#pragma once
#include "Rectangle.h"
#include "../physics/Particle.h"
#include <vector>

class QuadTree {
public:
    Rectangle               boundary;
    int                     capacity;
    std::vector<Particle*>  particles;
    bool                    divided = false;

    QuadTree* ne = nullptr;
    QuadTree* nw = nullptr;
    QuadTree* se = nullptr;
    QuadTree* sw = nullptr;

    QuadTree(Rectangle boundary, int capacity);
    ~QuadTree();

    // Operaciones principales — explicables en la defensa
    bool insert(Particle* p);
    void query(const Rectangle& range,
               std::vector<Particle*>& found,
               int& comparisons) const;
    void queryCircle(float cx, float cy, float r,
                     std::vector<Particle*>& found,
                     int& comparisons) const;
    void clear();

    // Para Mikael: devuelve todos los AABB del árbol actual
    void getBoundaries(std::vector<Rectangle>& out) const;

private:
    void subdivide();
};