#pragma once
#include "Rectangle.h"
#include "../physics/Particle.h"
#include <vector>

class QuadTree {
public:
    Rectangle boundary;   // La región espacial que cubre este nodo
    int capacity;         // Máximo de partículas antes de subdividirse

    std::vector<Particle*> particles;  // Partículas contenidas en este nodo
    bool divided = false;              // Estado de subdivisión

    // Punteros a los 4 cuadrantes hijos
    QuadTree* ne = nullptr;
    QuadTree* nw = nullptr;
    QuadTree* se = nullptr;
    QuadTree* sw = nullptr;

    QuadTree(Rectangle boundary, int capacity);
    ~QuadTree();

    // Métodos principales
    bool insert(Particle* p);
    void subdivide();
    void query(const Rectangle& range, std::vector<Particle*>& found, int& comparisons) const;
    void queryCircle(float cx, float cy, float r, std::vector<Particle*>& found, int& comparisons) const;
    
    // Utilidades
    void clear();
    void getBoundaries(std::vector<Rectangle>& rects) const;
};