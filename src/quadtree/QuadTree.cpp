#include "QuadTree.h"

QuadTree::QuadTree(Rectangle boundary, int capacity)
    : boundary(boundary), capacity(capacity) {}

QuadTree::~QuadTree() {
    delete ne;
    delete nw;
    delete se;
    delete sw;
}

void QuadTree::subdivide() {
    float x  = boundary.x;
    float y  = boundary.y;
    float hw = boundary.hw / 2.0f;
    float hh = boundary.hh / 2.0f;

    // Crear los 4 sub-cuadrantes
    ne = new QuadTree(Rectangle(x + hw, y - hh, hw, hh), capacity);
    nw = new QuadTree(Rectangle(x - hw, y - hh, hw, hh), capacity);
    se = new QuadTree(Rectangle(x + hw, y + hh, hw, hh), capacity);
    sw = new QuadTree(Rectangle(x - hw, y + hh, hw, hh), capacity);

    divided = true;

    // Redistribuir las partículas que ya estaban en este nodo a los nuevos hijos
    for (Particle* p : particles) {
        if (!ne->insert(p)) {
            if (!nw->insert(p)) {
                if (!se->insert(p)) {
                    sw->insert(p);
                }
            }
        }
    }
    // Vaciar el nodo padre, ya que las partículas ahora viven en las hojas
    particles.clear();
}

bool QuadTree::insert(Particle* p) {
    // Rechazar si la partícula no está dentro del límite
    if (!boundary.contains((float)p->x, (float)p->y)) {
        return false;
    }

    // Si hay espacio y no está dividido, se inserta aquí
    if ((int)particles.size() < capacity && !divided) {
        particles.push_back(p);
        return true;
    }

    // Si llegamos a la capacidad máxima y aún no hay hijos, subdividir
    if (!divided) {
        subdivide();
    }

    // Intentar insertar en los hijos
    if (ne->insert(p)) return true;
    if (nw->insert(p)) return true;
    if (se->insert(p)) return true;
    if (sw->insert(p)) return true;

    return false;
}

void QuadTree::query(const Rectangle& range, std::vector<Particle*>& found, int& comparisons) const {
    // Si la región de consulta no cruza este nodo, abortar
    if (!boundary.intersects(range)) {
        return;
    }

    // Comprobar partículas en este nodo
    for (Particle* p : particles) {
        comparisons++;
        if (range.contains((float)p->x, (float)p->y)) {
            found.push_back(p);
        }
    }

    // Si tiene hijos, buscar en ellos recursivamente
    if (divided) {
        ne->query(range, found, comparisons);
        nw->query(range, found, comparisons);
        se->query(range, found, comparisons);
        sw->query(range, found, comparisons);
    }
}

void QuadTree::queryCircle(float cx, float cy, float r, std::vector<Particle*>& found, int& comparisons) const {
    // AABB rápido para descartar el cuadrante entero
    Rectangle circleBounds(cx, cy, r, r);
    if (!boundary.intersects(circleBounds)) {
        return;
    }

    // Comprobación circular exacta
    for (Particle* p : particles) {
        comparisons++;
        float dx = (float)p->x - cx;
        float dy = (float)p->y - cy;
        if (dx * dx + dy * dy <= r * r) {
            found.push_back(p);
        }
    }

    if (divided) {
        ne->queryCircle(cx, cy, r, found, comparisons);
        nw->queryCircle(cx, cy, r, found, comparisons);
        se->queryCircle(cx, cy, r, found, comparisons);
        sw->queryCircle(cx, cy, r, found, comparisons);
    }
}

void QuadTree::clear() {
    particles.clear();
    if (divided) {
        delete ne; ne = nullptr;
        delete nw; nw = nullptr;
        delete se; se = nullptr;
        delete sw; sw = nullptr;
        divided = false;
    }
}

void QuadTree::getBoundaries(std::vector<Rectangle>& rects) const {
    rects.push_back(boundary);
    if (divided) {
        ne->getBoundaries(rects);
        nw->getBoundaries(rects);
        se->getBoundaries(rects);
        sw->getBoundaries(rects);
    }
}