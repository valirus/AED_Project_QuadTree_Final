# Plan de Proyecto — Opción A: QuadTree Simulador de Partículas 2D
**CS2023 – Algoritmos y Estructuras de Datos**  
Grupo de 3 personas · Stack: C++17 + SFML 2.6 + CMake

---

## Tabla de contenidos

1. [Por qué esta opción](#1-por-qué-esta-opción)
2. [Stack tecnológico y configuración inicial](#2-stack-tecnológico-y-configuración-inicial)
3. [Estructura de carpetas del proyecto](#3-estructura-de-carpetas-del-proyecto)
4. [División del trabajo (3 personas)](#4-división-del-trabajo-3-personas)
5. [Sesión 1 — Setup y QuadTree core](#5-sesión-1--setup-y-quadtree-core)
6. [Sesión 2 — Física de partículas e integración](#6-sesión-2--física-de-partículas-e-integración)
7. [Sesión 3 — Visualización con SFML](#7-sesión-3--visualización-con-sfml)
8. [Sesión 4 — Benchmarks, experimentos y reporte](#8-sesión-4--benchmarks-experimentos-y-reporte)
9. [Conceptos clave para explicar en la defensa](#9-conceptos-clave-para-explicar-en-la-defensa)
10. [Checklist final del proyecto](#10-checklist-final-del-proyecto)


---

## 1. Por qué esta opción

El QuadTree es la estructura visualmente más intuitiva de las cuatro opciones: se puede
dibujar en tiempo real conforme el árbol se subdivide, las colisiones se ven en pantalla, y
la comparación contra fuerza bruta es inmediatamente perceptible. Para 2.º año es ideal
porque el concepto central (dividir el espacio en cuatro cuadrantes recursivamente) se
entiende con un dibujo en la pizarra antes de escribir una sola línea de código.

**Ventajas adicionales:**
- No requiere dataset externo — los datos son sintéticos y configurables.
- El resultado visual (partículas rebotando) es impresionante para la demo.
- La comparación QuadTree vs fuerza bruta es gráfica: se ven en tiempo real cuántos
  nodos se revisan versus cuántos revisa el algoritmo ingenuo.

---

## 2. Stack tecnológico y configuración inicial

### Tecnologías
| Componente | Herramienta | Por qué |
|---|---|---|
| Lenguaje | C++17 | Requerido por el curso; control total de memoria |
| Visualización | SFML 2.6 | Librería 2D simple, bien documentada, multi-plataforma |
| Build | CMake 3.20+ | Estándar de industria, funciona en Windows/macOS/Linux |
| IDE sugerido | VS Code + CMake Tools | Gratis, funciona en todos los sistemas |

### Instalación de SFML

**Ubuntu/Debian:**
```bash
sudo apt-get install libsfml-dev
```

**macOS (Homebrew):**
```bash
brew install sfml
```

**Windows (vcpkg):**
```bash
vcpkg install sfml:x64-windows
```

### CMakeLists.txt base
```cmake
cmake_minimum_required(VERSION 3.20)
project(QuadTreeSim)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(QuadTreeSim
    src/main.cpp
    src/quadtree/QuadTree.cpp
    src/quadtree/BruteForce.cpp
    src/physics/PhysicsEngine.cpp
    src/physics/Distributions.cpp
    src/rendering/Renderer.cpp
    src/rendering/HUD.cpp
    src/benchmark/Benchmark.cpp
)

target_link_libraries(QuadTreeSim sfml-graphics sfml-window sfml-system)
target_include_directories(QuadTreeSim PRIVATE src)
```

### Compilar y ejecutar
```bash
mkdir build && cd build
cmake ..
make -j4          # Linux/macOS
cmake --build .   # Windows
./QuadTreeSim
```

---

## 3. Estructura de carpetas del proyecto

```
proyecto2/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp                  ← Game loop principal
│   ├── quadtree/
│   │   ├── Rectangle.h           ← AABB: región rectangular
│   │   ├── QuadTree.h
│   │   ├── QuadTree.cpp          ← IMPLEMENTACIÓN DESDE CERO (requerido)
│   │   ├── BruteForce.h
│   │   └── BruteForce.cpp        ← Solución ingenua para comparar
│   ├── physics/
│   │   ├── Particle.h            ← Struct del enunciado
│   │   ├── PhysicsEngine.h
│   │   ├── PhysicsEngine.cpp     ← Update de posición, colisiones
│   │   ├── Distributions.h
│   │   └── Distributions.cpp     ← Uniforme, clusters, zona densa
│   ├── rendering/
│   │   ├── Renderer.h
│   │   ├── Renderer.cpp          ← Dibuja partículas y QuadTree
│   │   ├── HUD.h
│   │   └── HUD.cpp               ← FPS, comparaciones, stats
│   └── benchmark/
│       ├── Benchmark.h
│       └── Benchmark.cpp         ← Timer, reporte de métricas
└── assets/
    └── font.ttf                  ← (ej. OpenSans, libre de uso)
```

---

## 4. División del trabajo (3 personas)

### Persona A — "El Algorítmico": QuadTree + BruteForce

**Responsabilidad:** Implementar la estructura de datos central y su competidor.

**Archivos propios:**
- `quadtree/Rectangle.h`
- `quadtree/QuadTree.h` y `QuadTree.cpp`
- `quadtree/BruteForce.h` y `BruteForce.cpp`

**Lo que debe implementar:**

#### Rectangle.h — el AABB (Axis-Aligned Bounding Box)
```cpp
// quadtree/Rectangle.h
#pragma once

struct Rectangle {
    float x, y;   // centro
    float hw, hh; // half-width, half-height

    Rectangle(float x, float y, float hw, float hh)
        : x(x), y(y), hw(hw), hh(hh) {}

    // ¿El punto (px, py) está dentro de este rectángulo?
    bool contains(float px, float py) const {
        return (px >= x - hw && px < x + hw &&
                py >= y - hh && py < y + hh);
    }

    // ¿Este rectángulo se intersecta con otro?
    bool intersects(const Rectangle& other) const {
        return !(other.x - other.hw > x + hw ||
                 other.x + other.hw < x - hw ||
                 other.y - other.hh > y + hh ||
                 other.y + other.hh < y - hh);
    }
};
```

> **Concepto clave (para explicar):** Un AABB define una región rectangular con centro
> (x,y) y semiejes hw (ancho/2) y hh (alto/2). `contains` verifica si un punto cae dentro.
> `intersects` detecta si dos regiones se solapan — esto es lo que usamos para saber
> si un nodo del QuadTree puede contener objetos relevantes para una consulta.

#### QuadTree.h — interfaz
```cpp
// quadtree/QuadTree.h
#pragma once
#include "Rectangle.h"
#include "../physics/Particle.h"
#include <vector>

class QuadTree {
public:
    Rectangle boundary;   // región que cubre este nodo
    int capacity;         // máx. partículas antes de subdividir

    std::vector<Particle*> particles;  // partículas en este nodo
    bool divided = false;

    // Los cuatro hijos (nullptr si no hay subdivisión)
    QuadTree* ne = nullptr;
    QuadTree* nw = nullptr;
    QuadTree* se = nullptr;
    QuadTree* sw = nullptr;

    QuadTree(Rectangle boundary, int capacity);
    ~QuadTree();

    bool insert(Particle* p);
    void subdivide();
    void query(const Rectangle& range,
               std::vector<Particle*>& found,
               int& comparisons) const;
    void queryCircle(float cx, float cy, float r,
                     std::vector<Particle*>& found,
                     int& comparisons) const;
    void clear();

    // Para visualización: devuelve todos los rectángulos del árbol
    void getBoundaries(std::vector<Rectangle>& rects) const;
};
```

#### QuadTree.cpp — implementación explicada línea a línea
```cpp
// quadtree/QuadTree.cpp
#include "QuadTree.h"
#include <cmath>

QuadTree::QuadTree(Rectangle boundary, int capacity)
    : boundary(boundary), capacity(capacity) {}

QuadTree::~QuadTree() {
    // Liberar memoria de los hijos recursivamente
    delete ne; delete nw; delete se; delete sw;
}

// subdivide(): crea los 4 hijos con las 4 subregiones
void QuadTree::subdivide() {
    float x  = boundary.x,  y  = boundary.y;
    float hw = boundary.hw / 2.0f;
    float hh = boundary.hh / 2.0f;

    ne = new QuadTree(Rectangle(x + hw, y - hh, hw, hh), capacity);
    nw = new QuadTree(Rectangle(x - hw, y - hh, hw, hh), capacity);
    se = new QuadTree(Rectangle(x + hw, y + hh, hw, hh), capacity);
    sw = new QuadTree(Rectangle(x - hw, y + hh, hw, hh), capacity);

    divided = true;

    // Redistribuir las partículas actuales a los hijos
    for (Particle* p : particles) {
        int dummy = 0;
        if (!ne->insert(p)) if (!nw->insert(p))
            if (!se->insert(p)) sw->insert(p);
    }
    particles.clear();
}

// insert(): intenta insertar una partícula
bool QuadTree::insert(Particle* p) {
    // 1. Si el punto no cae en nuestra región, rechazar
    if (!boundary.contains(p->x, p->y))
        return false;

    // 2. Si hay espacio y no estamos divididos, guardar aquí
    if ((int)particles.size() < capacity && !divided) {
        particles.push_back(p);
        return true;
    }

    // 3. Si llegamos al límite, subdividir (la primera vez)
    if (!divided) subdivide();

    // 4. Intentar insertar en algún hijo
    if (ne->insert(p)) return true;
    if (nw->insert(p)) return true;
    if (se->insert(p)) return true;
    if (sw->insert(p)) return true;

    return false; // no debería ocurrir
}

// query(): encuentra todas las partículas en un rectángulo
void QuadTree::query(const Rectangle& range,
                     std::vector<Particle*>& found,
                     int& comparisons) const {
    // Si esta región no se intersecta con la consulta, ignorar
    if (!boundary.intersects(range)) return;

    // Revisar partículas de este nodo
    for (Particle* p : particles) {
        comparisons++;
        if (range.contains(p->x, p->y))
            found.push_back(p);
    }

    // Si estamos divididos, recursar en los hijos
    if (divided) {
        ne->query(range, found, comparisons);
        nw->query(range, found, comparisons);
        se->query(range, found, comparisons);
        sw->query(range, found, comparisons);
    }
}

// queryCircle(): variante para radio de vecindad
void QuadTree::queryCircle(float cx, float cy, float r,
                           std::vector<Particle*>& found,
                           int& comparisons) const {
    // Crear el AABB del círculo para prueba rápida
    Rectangle circleBounds(cx, cy, r, r);
    if (!boundary.intersects(circleBounds)) return;

    for (Particle* p : particles) {
        comparisons++;
        float dx = p->x - cx, dy = p->y - cy;
        if (dx*dx + dy*dy <= r*r)
            found.push_back(p);
    }
    if (divided) {
        ne->queryCircle(cx, cy, r, found, comparisons);
        nw->queryCircle(cx, cy, r, found, comparisons);
        se->queryCircle(cx, cy, r, found, comparisons);
        sw->queryCircle(cx, cy, r, found, comparisons);
    }
}

// clear(): limpiar todo el árbol (se llama cada frame)
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
```

#### BruteForce.cpp — solución ingenua O(n²)
```cpp
// quadtree/BruteForce.cpp
#include "BruteForce.h"
#include <cmath>

// Para cada partícula, revisa TODAS las demás: O(n²)
void BruteForce::detectCollisions(
        const std::vector<Particle>& particles,
        std::vector<std::pair<int,int>>& collisions,
        int& comparisons) {
    comparisons = 0;
    collisions.clear();
    int n = (int)particles.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            comparisons++;
            float dx = particles[i].x - particles[j].x;
            float dy = particles[i].y - particles[j].y;
            float dist2 = dx*dx + dy*dy;
            float radSum = particles[i].radius + particles[j].radius;
            if (dist2 < radSum * radSum)
                collisions.push_back({i, j});
        }
    }
}
```

---

### Persona B — "El Físico": Motor de partículas + distribuciones

**Responsabilidad:** Simular el movimiento, las colisiones y generar los tres escenarios.

**Archivos propios:**
- `physics/Particle.h`
- `physics/PhysicsEngine.h` y `PhysicsEngine.cpp`
- `physics/Distributions.h` y `Distributions.cpp`

#### Particle.h — struct del enunciado
```cpp
// physics/Particle.h
#pragma once
#include <SFML/Graphics.hpp>

struct Particle {
    int id;
    double x, y;    // posición
    double vx, vy;  // velocidad
    double radius;

    // Para visualización (no afecta el QuadTree)
    sf::Color color    = sf::Color::White;
    bool highlighted   = false;  // ¿candidata de una query?
    bool colliding     = false;  // ¿está colisionando?
};
```

#### PhysicsEngine.cpp — explicado
```cpp
// physics/PhysicsEngine.cpp
#include "PhysicsEngine.h"
#include <cmath>

// Integración de Euler: la forma más simple de física
// pos_nueva = pos_vieja + velocidad * delta_tiempo
void PhysicsEngine::update(std::vector<Particle>& particles,
                           double dt,
                           double worldW, double worldH) {
    for (auto& p : particles) {
        // Actualizar posición (Euler forward)
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        // Rebotar en los bordes (boundary bounce)
        if (p.x - p.radius < 0) {
            p.x = p.radius;
            p.vx = std::abs(p.vx);   // invertir velocidad horizontal
        }
        if (p.x + p.radius > worldW) {
            p.x = worldW - p.radius;
            p.vx = -std::abs(p.vx);
        }
        if (p.y - p.radius < 0) {
            p.y = p.radius;
            p.vy = std::abs(p.vy);   // invertir velocidad vertical
        }
        if (p.y + p.radius > worldH) {
            p.y = worldH - p.radius;
            p.vy = -std::abs(p.vy);
        }

        // Limpiar flags de visualización
        p.highlighted = false;
        p.colliding   = false;
    }
}

// Resolver colisiones detectadas: intercambiar velocidades
// (colisión elástica simplificada, masas iguales)
void PhysicsEngine::resolveCollisions(
        std::vector<Particle>& particles,
        const std::vector<std::pair<int,int>>& collisions) {
    for (auto& [i, j] : collisions) {
        // Separar las partículas para que no se solapen
        double dx = particles[j].x - particles[i].x;
        double dy = particles[j].y - particles[i].y;
        double dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 1e-9) continue;  // evitar división por cero

        double overlap = (particles[i].radius + particles[j].radius) - dist;
        double nx = dx / dist, ny = dy / dist;  // normal unitaria

        // Empujar cada partícula la mitad del solapamiento
        particles[i].x -= nx * overlap / 2.0;
        particles[i].y -= ny * overlap / 2.0;
        particles[j].x += nx * overlap / 2.0;
        particles[j].y += ny * overlap / 2.0;

        // Intercambiar velocidades (colisión elástica 1D en la normal)
        double dvx = particles[j].vx - particles[i].vx;
        double dvy = particles[j].vy - particles[i].vy;
        double dot = dvx * nx + dvy * ny;
        if (dot < 0) {  // solo resolver si se acercan
            particles[i].vx += dot * nx;
            particles[i].vy += dot * ny;
            particles[j].vx -= dot * nx;
            particles[j].vy -= dot * ny;
        }

        particles[i].colliding = true;
        particles[j].colliding = true;
    }
}
```

#### Distributions.cpp — los tres escenarios requeridos
```cpp
// physics/Distributions.cpp
#include "Distributions.h"
#include <random>
#include <cmath>

static std::mt19937 rng(42);  // semilla fija para reproducibilidad

// 1. Distribución UNIFORME: partículas repartidas aleatoriamente
std::vector<Particle> Distributions::uniform(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    std::uniform_real_distribution<double> rx(0, worldW);
    std::uniform_real_distribution<double> ry(0, worldH);
    std::uniform_real_distribution<double> rv(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> rr(minR, maxR);

    std::vector<Particle> ps;
    for (int i = 0; i < n; i++) {
        Particle p;
        p.id = i;
        p.x = rx(rng);  p.y = ry(rng);
        p.vx = rv(rng); p.vy = rv(rng);
        p.radius = rr(rng);
        ps.push_back(p);
    }
    return ps;
}

// 2. Distribución con CLUSTERS: grupos de partículas
std::vector<Particle> Distributions::clusters(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed,
        int numClusters) {
    std::uniform_real_distribution<double> cx(100, worldW - 100);
    std::uniform_real_distribution<double> cy(100, worldH - 100);
    std::normal_distribution<double> spread(0.0, 60.0);
    std::uniform_real_distribution<double> rv(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> rr(minR, maxR);

    // Elegir centros de clusters
    std::vector<std::pair<double,double>> centers;
    for (int c = 0; c < numClusters; c++)
        centers.push_back({cx(rng), cy(rng)});

    std::uniform_int_distribution<int> rc(0, numClusters - 1);
    std::vector<Particle> ps;
    for (int i = 0; i < n; i++) {
        auto [ox, oy] = centers[rc(rng)];
        Particle p;
        p.id = i;
        p.x = std::clamp(ox + spread(rng), 0.0, worldW);
        p.y = std::clamp(oy + spread(rng), 0.0, worldH);
        p.vx = rv(rng); p.vy = rv(rng);
        p.radius = rr(rng);
        ps.push_back(p);
    }
    return ps;
}

// 3. ZONA DE ALTA DENSIDAD: una esquina concentrada, resto disperso
std::vector<Particle> Distributions::denseZone(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    std::uniform_real_distribution<double> rv(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> rr(minR, maxR);

    // 70% en la zona densa (cuadrante superior izquierdo)
    int densePart  = (int)(n * 0.7);
    int sparsePart = n - densePart;

    std::uniform_real_distribution<double> rdx(0, worldW * 0.3);
    std::uniform_real_distribution<double> rdy(0, worldH * 0.3);
    std::uniform_real_distribution<double> rsx(0, worldW);
    std::uniform_real_distribution<double> rsy(0, worldH);

    std::vector<Particle> ps;
    for (int i = 0; i < densePart; i++) {
        Particle p; p.id = i;
        p.x = rdx(rng); p.y = rdy(rng);
        p.vx = rv(rng); p.vy = rv(rng);
        p.radius = rr(rng);
        ps.push_back(p);
    }
    for (int i = densePart; i < n; i++) {
        Particle p; p.id = i;
        p.x = rsx(rng); p.y = rsy(rng);
        p.vx = rv(rng); p.vy = rv(rng);
        p.radius = rr(rng);
        ps.push_back(p);
    }
    return ps;
}
```

---

### Persona C — "El Visual": Renderer + HUD + Benchmark

**Responsabilidad:** Todo lo que se ve en pantalla y las métricas.

**Archivos propios:**
- `rendering/Renderer.h` y `Renderer.cpp`
- `rendering/HUD.h` y `HUD.cpp`
- `benchmark/Benchmark.h` y `Benchmark.cpp`

#### Renderer.cpp — dibujar con SFML
```cpp
// rendering/Renderer.cpp
#include "Renderer.h"
#include <SFML/Graphics.hpp>

void Renderer::drawParticles(sf::RenderWindow& window,
                             const std::vector<Particle>& particles) {
    sf::CircleShape shape;
    for (const auto& p : particles) {
        shape.setRadius((float)p.radius);
        shape.setOrigin((float)p.radius, (float)p.radius);
        shape.setPosition((float)p.x, (float)p.y);

        // Color según estado:
        if (p.colliding)
            shape.setFillColor(sf::Color(255, 80, 80));    // rojo = colisión
        else if (p.highlighted)
            shape.setFillColor(sf::Color(80, 220, 80));    // verde = candidata
        else
            shape.setFillColor(sf::Color(180, 200, 255));  // azul = normal

        window.draw(shape);
    }
}

void Renderer::drawQuadTree(sf::RenderWindow& window,
                            const std::vector<Rectangle>& bounds) {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color(60, 60, 120, 140));  // azul semitransparente
    rect.setOutlineThickness(1.0f);

    for (const auto& b : bounds) {
        rect.setSize({b.hw * 2, b.hh * 2});
        rect.setPosition(b.x - b.hw, b.y - b.hh);
        window.draw(rect);
    }
}

void Renderer::drawQueryRegion(sf::RenderWindow& window,
                               const Rectangle& range) {
    sf::RectangleShape rect;
    rect.setSize({range.hw * 2, range.hh * 2});
    rect.setPosition(range.x - range.hw, range.y - range.hh);
    rect.setFillColor(sf::Color(255, 255, 0, 40));      // amarillo translúcido
    rect.setOutlineColor(sf::Color::Yellow);
    rect.setOutlineThickness(2.0f);
    window.draw(rect);
}
```

#### HUD.cpp — panel de estadísticas en pantalla
```cpp
// rendering/HUD.cpp
#include "HUD.h"
#include <sstream>
#include <iomanip>

void HUD::draw(sf::RenderWindow& window, sf::Font& font,
               int fps, int n,
               int qtComparisons, int bfComparisons,
               float qtMs, float bfMs,
               const std::string& distribution) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "FPS: " << fps << "\n"
       << "Partículas: " << n << "\n"
       << "Distribución: " << distribution << "\n\n"
       << "--- QuadTree ---\n"
       << "Comparaciones: " << qtComparisons << "\n"
       << "Tiempo (ms):   " << qtMs << "\n\n"
       << "--- Fuerza Bruta ---\n"
       << "Comparaciones: " << bfComparisons << "\n"
       << "Tiempo (ms):   " << bfMs << "\n\n"
       << "Speedup: " << (bfMs > 0 ? bfMs/qtMs : 0) << "x\n\n"
       << "[1/2/3] Distribución\n"
       << "[+/-] Partículas\n"
       << "[Q] Grid ON/OFF\n"
       << "[B] BruteForce ON/OFF";

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);
    text.setString(ss.str());

    // Fondo semi-transparente para el HUD
    sf::RectangleShape bg;
    bg.setSize({260, 340});
    bg.setFillColor(sf::Color(0, 0, 0, 160));
    bg.setPosition(10, 10);
    window.draw(bg);

    text.setPosition(18, 18);
    window.draw(text);
}
```

#### Benchmark.h — medir tiempos
```cpp
// benchmark/Benchmark.h
#pragma once
#include <chrono>
#include <vector>
#include <string>

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    std::chrono::time_point<Clock> start_;
public:
    void start() { start_ = Clock::now(); }
    float elapsedMs() const {
        auto end = Clock::now();
        return std::chrono::duration<float, std::milli>(end - start_).count();
    }
};

struct BenchmarkResult {
    int n;
    std::string distribution;
    float buildTimeMs;
    float qtQueryMs;
    float bfQueryMs;
    int qtComparisons;
    int bfComparisons;
    int colisiones;
};

class Benchmark {
public:
    static std::vector<BenchmarkResult> results;
    static void record(BenchmarkResult r) { results.push_back(r); }
    static void printCSV();  // exportar para reporte
};
```

---

## 5. Sesión 1 — Setup y QuadTree core

**Duración:** 3–4 horas · **Asistencia:** los 3 juntos

### Objetivos de la sesión
- [ ] Todos tienen el proyecto compilando (SFML + CMake)
- [ ] Persona A tiene `Rectangle.h` y `QuadTree.cpp` con `insert` y `subdivide`
- [ ] Existe un test unitario básico que verifica el insert

### Agenda
| Hora | Actividad |
|---|---|
| 0:00–0:30 | Instalar SFML y CMake, verificar que compila `main.cpp` vacío |
| 0:30–1:30 | Persona A: implementar `Rectangle.h` y `QuadTree::subdivide()` |
| 1:30–2:30 | Persona A: `QuadTree::insert()` y `QuadTree::query()` |
| 2:30–3:00 | Test conjunto: insertar 10 puntos a mano, verificar que `query` los encuentra |
| 3:00–3:30 | Persona B: implementar `Particle.h` y `PhysicsEngine::update()` simple |
| 3:30–4:00 | Persona C: abrir ventana SFML vacía, dibujar un círculo de prueba |

### Test unitario sin frameworks (en main.cpp para probar)
```cpp
// Test rápido en main.cpp - borrarlo después
void testQuadTree() {
    QuadTree qt(Rectangle(400, 300, 400, 300), 4);
    Particle ps[5];
    ps[0] = {0, 100, 100, 0, 0, 5};
    ps[1] = {1, 200, 150, 0, 0, 5};
    ps[2] = {2, 600, 400, 0, 0, 5};
    ps[3] = {3, 700, 500, 0, 0, 5};
    ps[4] = {4, 150, 120, 0, 0, 5};  // quinta: fuerza subdivisión

    for (auto& p : ps) qt.insert(&p);

    std::vector<Particle*> found;
    int comp = 0;
    Rectangle queryRegion(175, 150, 120, 120);
    qt.query(queryRegion, found, comp);

    // Debe encontrar ps[0], ps[1], ps[4]
    std::cout << "Encontradas: " << found.size() << " (esperado: 3)\n";
    std::cout << "Comparaciones: " << comp << "\n";
}
```

---

## 6. Sesión 2 — Física de partículas e integración

**Duración:** 3–4 horas · **Persona B lidera, A y C integran**

### Objetivos de la sesión
- [ ] Las partículas se mueven y rebotan en los bordes
- [ ] El QuadTree se reconstruye cada frame
- [ ] Las colisiones se detectan usando el QuadTree
- [ ] Las 3 distribuciones generan partículas correctamente

### Game loop básico (main.cpp)
```cpp
// src/main.cpp
#include <SFML/Graphics.hpp>
#include "quadtree/QuadTree.h"
#include "physics/PhysicsEngine.h"
#include "physics/Distributions.h"
#include "rendering/Renderer.h"
#include "rendering/HUD.h"
#include "benchmark/Benchmark.h"

int main() {
    const float WORLD_W = 1280, WORLD_H = 720;
    sf::RenderWindow window(
        sf::VideoMode((int)WORLD_W, (int)WORLD_H),
        "QuadTree Simulator - CS2023"
    );
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("assets/font.ttf");

    // Configuración inicial
    int N = 1000;
    int qtCapacity = 4;
    bool showGrid  = true;
    bool runBF     = true;

    auto particles = Distributions::uniform(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
    QuadTree qt(Rectangle(WORLD_W/2, WORLD_H/2, WORLD_W/2, WORLD_H/2), qtCapacity);

    sf::Clock clock;
    Timer frameTimer, qtTimer, bfTimer;

    int qtComp = 0, bfComp = 0;
    float qtMs = 0, bfMs = 0;
    std::vector<std::pair<int,int>> collisions;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // --- EVENTOS ---
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1)
                    particles = Distributions::uniform(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
                if (event.key.code == sf::Keyboard::Num2)
                    particles = Distributions::clusters(N, WORLD_W, WORLD_H, 3, 8, 50, 150, 5);
                if (event.key.code == sf::Keyboard::Num3)
                    particles = Distributions::denseZone(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
                if (event.key.code == sf::Keyboard::Q) showGrid = !showGrid;
                if (event.key.code == sf::Keyboard::B) runBF = !runBF;
                if (event.key.code == sf::Keyboard::Equal) {
                    N += 500;
                    particles = Distributions::uniform(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
                }
            }
        }

        // --- FÍSICA ---
        PhysicsEngine::update(particles, dt, WORLD_W, WORLD_H);

        // --- QUADTREE: reconstruir cada frame ---
        qtTimer.start();
        qt.clear();
        for (auto& p : particles) qt.insert(&p);

        // Detectar colisiones con QuadTree
        qtComp = 0;
        collisions.clear();
        for (auto& p : particles) {
            std::vector<Particle*> candidates;
            // Buscar en un rectángulo del tamaño del radio de colisión
            float r = (float)(p.radius * 2);
            Rectangle range((float)p.x, (float)p.y, r, r);
            qt.query(range, candidates, qtComp);

            for (Particle* other : candidates) {
                if (other->id <= p.id) continue;
                float dx = (float)(p.x - other->x), dy = (float)(p.y - other->y);
                float dist2 = dx*dx + dy*dy;
                float radSum = (float)(p.radius + other->radius);
                if (dist2 < radSum * radSum)
                    collisions.push_back({p.id, other->id});
            }
        }
        qtMs = qtTimer.elapsedMs();
        PhysicsEngine::resolveCollisions(particles, collisions);

        // --- FUERZA BRUTA (comparación) ---
        if (runBF) {
            bfTimer.start();
            std::vector<std::pair<int,int>> bfCollisions;
            BruteForce::detectCollisions(particles, bfCollisions, bfComp);
            bfMs = bfTimer.elapsedMs();
        }

        // --- DIBUJAR ---
        window.clear(sf::Color(15, 15, 30));

        if (showGrid) {
            std::vector<Rectangle> bounds;
            qt.getBoundaries(bounds);
            Renderer::drawQuadTree(window, bounds);
        }
        Renderer::drawParticles(window, particles);

        int fps = (dt > 0) ? (int)(1.0f / dt) : 0;
        HUD::draw(window, font, fps, N, qtComp, bfComp, qtMs, bfMs, "Uniforme");

        window.display();
    }
    return 0;
}
```

---

## 7. Sesión 3 — Visualización con SFML

**Duración:** 3–4 horas · **Persona C lidera**

### Objetivos de la sesión
- [ ] El HUD muestra FPS, comparaciones de QT y BF, y speedup en tiempo real
- [ ] La grilla del QuadTree se dibuja y actualiza cada frame
- [ ] Con clic izquierdo se dibuja una región de consulta
- [ ] Las partículas candidatas se resaltan en verde

### Consulta interactiva con el ratón
```cpp
// En el game loop, dentro del bloque de eventos:
static bool dragging = false;
static sf::Vector2f dragStart;
static Rectangle queryRegion(0, 0, 0, 0);
static bool hasQuery = false;

if (event.type == sf::Event::MouseButtonPressed &&
    event.mouseButton.button == sf::Mouse::Left) {
    dragging = true;
    dragStart = {(float)event.mouseButton.x,
                 (float)event.mouseButton.y};
}
if (event.type == sf::Event::MouseButtonReleased &&
    event.mouseButton.button == sf::Mouse::Left) {
    dragging = false;
    sf::Vector2f dragEnd = {(float)event.mouseButton.x,
                            (float)event.mouseButton.y};
    float cx = (dragStart.x + dragEnd.x) / 2.0f;
    float cy = (dragStart.y + dragEnd.y) / 2.0f;
    float hw = std::abs(dragEnd.x - dragStart.x) / 2.0f;
    float hh = std::abs(dragEnd.y - dragStart.y) / 2.0f;
    queryRegion = Rectangle(cx, cy, hw, hh);
    hasQuery = true;

    // Marcar candidatas
    std::vector<Particle*> found;
    int qtQueryComp = 0;
    qt.query(queryRegion, found, qtQueryComp);
    for (Particle* p : found) p->highlighted = true;
}

// En la sección de dibujo:
if (hasQuery)
    Renderer::drawQueryRegion(window, queryRegion);
```

---

## 8. Sesión 4 — Benchmarks, experimentos y reporte

**Duración:** 2–3 horas · **Todo el grupo**

### Experimentos requeridos

Ejecutar el simulador en modo benchmark (sin ventana gráfica) con:

| Tamaño | N | Descripción |
|---|---|---|
| Pequeño | 1,000 | Visible en tiempo real, muy rápido |
| Mediano | 5,000 | Diferencia notable entre QT y BF |
| Grande | 10,000+ | BF comienza a ser inaceptable |

Para cada tamaño, reportar:
- Tiempo promedio por frame (ms)
- Comparaciones promedio de QuadTree vs Fuerza Bruta
- Speedup = t_BF / t_QT
- Efecto de la distribución (uniforme vs clusters vs densa)

### Función de benchmark automático (sin ventana)
```cpp
void runBenchmarkMode(int n, const std::string& distName) {
    const float W = 1280, H = 720;
    std::vector<Particle> particles;
    if (distName == "uniform")
        particles = Distributions::uniform(n, W, H, 3, 8, 50, 150);
    else if (distName == "clusters")
        particles = Distributions::clusters(n, W, H, 3, 8, 50, 150, 5);
    else
        particles = Distributions::denseZone(n, W, H, 3, 8, 50, 150);

    QuadTree qt(Rectangle(W/2, H/2, W/2, H/2), 4);
    const int FRAMES = 120;
    float totalQTms = 0, totalBFms = 0;
    int totalQTcomp = 0, totalBFcomp = 0;

    for (int f = 0; f < FRAMES; f++) {
        PhysicsEngine::update(particles, 1.0f/60.0f, W, H);

        Timer t;
        t.start();
        qt.clear();
        for (auto& p : particles) qt.insert(&p);
        int qtComp = 0;
        std::vector<Particle*> found;
        for (auto& p : particles) {
            Rectangle r((float)p.x, (float)p.y,
                        (float)p.radius*2, (float)p.radius*2);
            qt.query(r, found, qtComp);
            found.clear();
        }
        totalQTms   += t.elapsedMs();
        totalQTcomp += qtComp;

        t.start();
        std::vector<std::pair<int,int>> bfColls;
        int bfComp = 0;
        BruteForce::detectCollisions(particles, bfColls, bfComp);
        totalBFms   += t.elapsedMs();
        totalBFcomp += bfComp;
    }

    std::cout << "N=" << n << " dist=" << distName << "\n";
    std::cout << "QT avg ms: " << totalQTms/FRAMES
              << "  comp: "  << totalQTcomp/FRAMES << "\n";
    std::cout << "BF avg ms: " << totalBFms/FRAMES
              << "  comp: "  << totalBFcomp/FRAMES << "\n";
    std::cout << "Speedup: "   << totalBFms/totalQTms << "x\n\n";
}
```

---

## 9. Conceptos clave para explicar en la defensa

### ¿Qué es un QuadTree?
Un QuadTree es un árbol donde cada nodo representa una región rectangular del espacio 2D.
Cuando se insertan más objetos de los que cabe en un nodo (capacidad `k`), el nodo se
subdivide en 4 cuadrantes: noreste, noroeste, sureste y suroeste. Esto permite que las
consultas espaciales eviten revisar objetos que están lejos de la región de interés.

### ¿Por qué es más rápido que fuerza bruta?
- **Fuerza bruta:** para `n` partículas, detectar todas las colisiones requiere comparar
  cada par → O(n²) operaciones. Con 5000 partículas: ~12.5 millones de comparaciones.
- **QuadTree:** para cada partícula, solo se revisan las partículas en la misma región
  del árbol. Con una distribución uniforme y capacidad 4, el número de comparaciones
  escala aproximadamente como O(n log n).

### ¿Cuándo el QuadTree NO ayuda?
Cuando todas las partículas están en la misma región (alta densidad), el QuadTree
degenera y la ventaja se reduce. Esto se evidencia con la distribución "densa" de tu
experimento — es un resultado honesto y el profesor lo valorará positivamente.

### Invariante del QuadTree
Si un nodo tiene `divided == true`, sus listas `particles` deben estar vacías.
Las partículas solo viven en los nodos hoja. Esto garantiza que cada partícula
aparece exactamente una vez en el árbol.

### ¿Por qué reconstruimos el árbol cada frame?
En un simulador dinámico (partículas en movimiento), las posiciones cambian
constantemente. La opción más simple y correcta es limpiar y reconstruir el árbol
en cada frame. Para 10k partículas, esto cuesta O(n log n) que en la práctica
es rápido comparado con el ahorro en las consultas.

---

## 10. Checklist final del proyecto

### Implementación (requerido)
- [ ] QuadTree implementado desde cero (sin STL de trees)
- [ ] `insert`, `subdivide`, `query` rect, `queryCircle`, `clear`
- [ ] BruteForce implementado para comparación
- [ ] Distribución uniforme, clusters y zona densa
- [ ] Motor de física: Euler + rebote en bordes + colisiones elásticas
- [ ] Reconstrucción del QuadTree cada frame

### Visualización (requerido)
- [ ] Partículas visibles en pantalla
- [ ] Grilla del QuadTree visible (toggle on/off)
- [ ] Región de consulta dibujada
- [ ] Candidatas resaltadas en un color diferente
- [ ] HUD con comparaciones QT vs BF en tiempo real

### Experimentos (requerido)
- [ ] 3 tamaños: 1k, 5k, 10k+
- [ ] 3 distribuciones para cada tamaño
- [ ] Tabla de resultados: tiempo, comparaciones, speedup

### Reporte y defensa
- [ ] Explicar el invariante del QuadTree
- [ ] Explicar `subdivide` con diagrama
- [ ] Análisis de complejidad: O(n log n) vs O(n²)
- [ ] Interpretar cuándo el QuadTree es mejor/peor

---

## Referencias para aprender más

- **Coding Train (YouTube):** "Coding Challenge #98: QuadTree" — visualización
  idéntica a tu proyecto, en 30 minutos, muy didáctico.
- **Libro:** *Introduction to Algorithms* (CLRS), Cap. 12 — árboles de búsqueda
  (la lógica de inserción recursiva es la misma).
- **SFML Docs:** https://www.sfml-dev.org/learn.php — referencia completa.
- **CMake Tutorial:** https://cmake.org/cmake/help/latest/guide/tutorial/

---
*Generado para CS2023 — Proyecto 2, Opción A*
