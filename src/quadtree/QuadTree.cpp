#include "QuadTree.h"
#include <cmath>

QuadTree::QuadTree(Rectangle boundary, int capacity)
    : boundary(boundary), capacity(capacity) {}

QuadTree::~QuadTree() {
    // Liberar hijos recursivamente (RAII manual)
    delete ne; delete nw; delete se; delete sw;
}

// ─────────────────────────────────────────────────────────────────────
// subdivide(): divide este nodo en cuatro cuadrantes hijos.
// Invariante post-condición: particles de este nodo queda vacío;
// cada partícula se redistribuye a algún hijo.
// ─────────────────────────────────────────────────────────────────────
void QuadTree::subdivide() {
    float x  = boundary.x,     y  = boundary.y;
    float hw = boundary.hw / 2.0f, hh = boundary.hh / 2.0f;

    ne = new QuadTree(Rectangle(x + hw, y - hh, hw, hh), capacity);
    nw = new QuadTree(Rectangle(x - hw, y - hh, hw, hh), capacity);
    se = new QuadTree(Rectangle(x + hw, y + hh, hw, hh), capacity);
    sw = new QuadTree(Rectangle(x - hw, y + hh, hw, hh), capacity);
    divided = true;

    // Redistribuir las partículas existentes a los hijos correspondientes
    for (Particle* p : particles) {
        if (!ne->insert(p))
        if (!nw->insert(p))
        if (!se->insert(p))
            sw->insert(p);
    }
    particles.clear();  // este nodo ya no guarda partículas directamente
}

// ─────────────────────────────────────────────────────────────────────
// insert(): inserta una partícula en el árbol.
// Retorna false si el punto está fuera de la región de este nodo.
// ─────────────────────────────────────────────────────────────────────
bool QuadTree::insert(Particle* p) {
    // 1. Rechazar si el punto no pertenece a nuestra región
    if (!boundary.contains((float)p->x, (float)p->y))
        return false;

    // 2. Hay espacio y no estamos divididos: guardar aquí
    if (!divided && (int)particles.size() < capacity) {
        particles.push_back(p);
        return true;
    }

    // 3. Llegamos al límite de capacidad: subdividir (solo la primera vez)
    if (!divided) subdivide();

    // 4. Delegar al hijo correspondiente
    if (ne->insert(p)) return true;
    if (nw->insert(p)) return true;
    if (se->insert(p)) return true;
    if (sw->insert(p)) return true;

    // No debería ocurrir si boundary y contains son correctos
    return false;
}

// ─────────────────────────────────────────────────────────────────────
// query(): devuelve todas las partículas dentro de 'range'.
// comparisons cuenta cada partícula evaluada (métrica de la rúbrica).
// ─────────────────────────────────────────────────────────────────────
void QuadTree::query(const Rectangle& range,
                     std::vector<Particle*>& found,
                     int& comparisons) const {
    // Poda: si este nodo no se intersecta con el rango, ignorar todo el subárbol
    if (!boundary.intersects(range)) return;

    for (Particle* p : particles) {
        comparisons++;
        if (range.contains((float)p->x, (float)p->y))
            found.push_back(p);
    }

    if (divided) {
        ne->query(range, found, comparisons);
        nw->query(range, found, comparisons);
        se->query(range, found, comparisons);
        sw->query(range, found, comparisons);
    }
}

// ─────────────────────────────────────────────────────────────────────
// queryCircle(): variante para "vecinos cercanos a un punto" (rúbrica).
// Usa el AABB del círculo para la poda, luego verifica distancia real.
// ─────────────────────────────────────────────────────────────────────
void QuadTree::queryCircle(float cx, float cy, float r,
                           std::vector<Particle*>& found,
                           int& comparisons) const {
    Rectangle circleBounds(cx, cy, r, r);
    if (!boundary.intersects(circleBounds)) return;

    for (Particle* p : particles) {
        comparisons++;
        float dx = (float)p->x - cx, dy = (float)p->y - cy;
        if (dx * dx + dy * dy <= r * r)
            found.push_back(p);
    }

    if (divided) {
        ne->queryCircle(cx, cy, r, found, comparisons);
        nw->queryCircle(cx, cy, r, found, comparisons);
        se->queryCircle(cx, cy, r, found, comparisons);
        sw->queryCircle(cx, cy, r, found, comparisons);
    }
}

// ─────────────────────────────────────────────────────────────────────
// clear(): destruir hijos y vaciar el nodo para reutilizarlo.
// Se llama al inicio de cada frame antes de reinsertar las partículas.
// ─────────────────────────────────────────────────────────────────────
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

void QuadTree::getBoundaries(std::vector<Rectangle>& out) const {
    out.push_back(boundary);
    if (divided) {
        ne->getBoundaries(out);
        nw->getBoundaries(out);
        se->getBoundaries(out);
        sw->getBoundaries(out);
    }
}