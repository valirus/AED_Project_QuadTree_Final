// src/main.cpp
// Propietario: Mikael (integra todos los módulos)
// SFML 3.0.2 — event loop completamente reescrito
#include <SFML/Graphics.hpp>
#include "quadtree/QuadTree.h"
#include "quadtree/BruteForce.h"
#include "physics/PhysicsEngine.h"
#include "physics/Distributions.h"
#include "rendering/Renderer.h"
#include "rendering/HUD.h"
#include "benchmark/Benchmark.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>

struct AppConfig {
    int    n               = 1000;
    double worldW          = 1280.0;
    double worldH          = 720.0;
    int    qtCapacity      = 4;
    double minRadius       = 3.0;
    double maxRadius       = 8.0;
    double minSpeed        = 50.0;
    double maxSpeed        = 150.0;
    int    benchmarkFrames = 120;
};

static void printUsage() {
    std::cout
        << "Uso:\n"
        << "  QuadTreeSim [opciones]\n"
        << "  QuadTreeSim benchmark [opciones]\n\n"
        << "Opciones:\n"
        << "  --n <int>             numero inicial de particulas\n"
        << "  --width <double>      ancho del mundo 2D\n"
        << "  --height <double>     alto del mundo 2D\n"
        << "  --capacity <int>      capacidad maxima por nodo QuadTree\n"
        << "  --min-radius <double> radio minimo\n"
        << "  --max-radius <double> radio maximo\n"
        << "  --min-speed <double>  rapidez minima\n"
        << "  --max-speed <double>  rapidez maxima\n"
        << "  --frames <int>        frames por experimento benchmark\n";
}

static bool hasValue(int i, int argc) {
    return i + 1 < argc;
}

static AppConfig parseConfig(int argc, char* argv[], int startIndex) {
    AppConfig cfg;
    for (int i = startIndex; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage();
            std::exit(0);
        } else if (arg == "--n" && hasValue(i, argc)) {
            cfg.n = std::stoi(argv[++i]);
        } else if (arg == "--width" && hasValue(i, argc)) {
            cfg.worldW = std::stod(argv[++i]);
        } else if (arg == "--height" && hasValue(i, argc)) {
            cfg.worldH = std::stod(argv[++i]);
        } else if (arg == "--capacity" && hasValue(i, argc)) {
            cfg.qtCapacity = std::stoi(argv[++i]);
        } else if (arg == "--min-radius" && hasValue(i, argc)) {
            cfg.minRadius = std::stod(argv[++i]);
        } else if (arg == "--max-radius" && hasValue(i, argc)) {
            cfg.maxRadius = std::stod(argv[++i]);
        } else if (arg == "--min-speed" && hasValue(i, argc)) {
            cfg.minSpeed = std::stod(argv[++i]);
        } else if (arg == "--max-speed" && hasValue(i, argc)) {
            cfg.maxSpeed = std::stod(argv[++i]);
        } else if (arg == "--frames" && hasValue(i, argc)) {
            cfg.benchmarkFrames = std::stoi(argv[++i]);
        } else {
            std::cerr << "[WARN] Argumento ignorado o incompleto: " << arg << "\n";
        }
    }

    cfg.n               = std::clamp(cfg.n, 1, 200000);
    cfg.worldW          = std::max(100.0, cfg.worldW);
    cfg.worldH          = std::max(100.0, cfg.worldH);
    cfg.qtCapacity      = std::clamp(cfg.qtCapacity, 1, 256);
    cfg.minRadius       = std::max(0.5, cfg.minRadius);
    cfg.maxRadius       = std::max(cfg.minRadius, cfg.maxRadius);
    cfg.minSpeed        = std::max(0.0, cfg.minSpeed);
    cfg.maxSpeed        = std::max(cfg.minSpeed, cfg.maxSpeed);
    cfg.benchmarkFrames = std::clamp(cfg.benchmarkFrames, 1, 10000);
    return cfg;
}

static std::vector<Particle> buildParticlesForDist(int distIndex, int n, const AppConfig& cfg) {
    if (distIndex == 0)
        return Distributions::uniform(n, cfg.worldW, cfg.worldH,
                                      cfg.minRadius, cfg.maxRadius,
                                      cfg.minSpeed, cfg.maxSpeed);
    if (distIndex == 1)
        return Distributions::clusters(n, cfg.worldW, cfg.worldH,
                                       cfg.minRadius, cfg.maxRadius,
                                       cfg.minSpeed, cfg.maxSpeed, 5);
    return Distributions::denseZone(n, cfg.worldW, cfg.worldH,
                                    cfg.minRadius, cfg.maxRadius,
                                    cfg.minSpeed, cfg.maxSpeed);
}

static std::vector<std::pair<int,int>> detectCollisionsQT(
        QuadTree& qt,
        std::vector<Particle>& particles,
        double maxRadius,
        int& qtComp,
        int& totalCandidates) {
    qtComp = 0;
    totalCandidates = 0;
    std::vector<std::pair<int,int>> qtCollisions;

    for (auto& p : particles) {
        std::vector<Particle*> found;
        // Radio de broad-phase correcto para radios variables:
        // toda partícula que pueda colisionar con p cumple d <= p.radius + maxRadius.
        float searchR = (float)(p.radius + maxRadius);
        qt.queryCircle((float)p.x, (float)p.y, searchR, found, qtComp);

        for (Particle* other : found) {
            if (other->id == p.id) continue;
            totalCandidates++;
            if (other->id <= p.id) continue;

            float dx = (float)(p.x - other->x);
            float dy = (float)(p.y - other->y);
            float rs = (float)(p.radius + other->radius);
            if (dx*dx + dy*dy < rs*rs)
                qtCollisions.push_back({p.id, other->id});
        }
    }
    return qtCollisions;
}

// Modo benchmark sin ventana (para los experimentos del reporte)
// Uso: pasar "benchmark" como primer argumento de línea de comandos
void runBenchmark(const AppConfig& cfg) {
    const float W = (float)cfg.worldW;
    const float H = (float)cfg.worldH;
    const int   FRAMES = cfg.benchmarkFrames;

    for (int distIdx = 0; distIdx < 3; distIdx++) {
        for (int n : {1000, 5000, 10000}) {
            std::vector<Particle> particles = buildParticlesForDist(distIdx, n, cfg);
            std::string distName = Distributions::name(distIdx);

            QuadTree qt(Rectangle(W/2, H/2, W/2, H/2), cfg.qtCapacity);
            float totalBuildMs = 0, totalQTms = 0, totalBFms = 0;
            long long totalQTcomp = 0, totalBFcomp = 0;
            float totalCandidates = 0;
            int totalCollisions = 0;
            Timer t;

            for (int f = 0; f < FRAMES; f++) {
                PhysicsEngine::update(particles, 1.0/60.0, W, H);

                // Medir reconstrucción del QuadTree
                t.start();
                qt.clear();
                for (auto& p : particles) qt.insert(&p);
                totalBuildMs += t.elapsedMs();

                // Medir consultas con QuadTree
                int qtCompFrame = 0;
                int candidatesFrame = 0;
                t.start();
                auto qtCollisions = detectCollisionsQT(qt, particles, cfg.maxRadius,
                                                        qtCompFrame, candidatesFrame);
                totalQTms += t.elapsedMs();
                totalQTcomp += qtCompFrame;
                totalCandidates += (float)candidatesFrame / (float)n;
                totalCollisions += (int)qtCollisions.size();

                // Medir BruteForce sobre el mismo snapshot antes de resolver colisiones.
                std::vector<std::pair<int,int>> bfColls;
                int bfCompFrame = 0;
                t.start();
                BruteForce::detectCollisions(particles, bfColls, bfCompFrame);
                totalBFms += t.elapsedMs();
                totalBFcomp += bfCompFrame;

                // Avanzar la simulación para el siguiente frame usando el resultado del QuadTree.
                PhysicsEngine::resolveCollisions(particles, qtCollisions);
            }

            Benchmark::record({
                n, distName,
                totalBuildMs / FRAMES,
                totalQTms    / FRAMES,
                totalBFms    / FRAMES,
                totalQTcomp  / FRAMES,
                totalBFcomp  / FRAMES,
                totalCandidates / FRAMES,
                totalCollisions / FRAMES
            });
        }
    }
    Benchmark::printTable();
    Benchmark::exportCSV();
}

// Modo interactivo con ventana SFML
int main(int argc, char* argv[]) {
    bool benchmarkMode = (argc > 1 && std::string(argv[1]) == "benchmark");
    AppConfig cfg = parseConfig(argc, argv, benchmarkMode ? 2 : 1);

    if (benchmarkMode) {
        runBenchmark(cfg);
        return 0;
    }

    const float WORLD_W = (float)cfg.worldW;
    const float WORLD_H = (float)cfg.worldH;

    sf::RenderWindow window(
        sf::VideoMode({(unsigned)WORLD_W, (unsigned)WORLD_H}),
        "QuadTree Particle Simulator — CS2023"
    );
    window.setFramerateLimit(60);

    // SFML 3.0.2: openFromFile() en lugar de loadFromFile()
    sf::Font font;
    if (!font.openFromFile("assets/font.ttf")) {
        // Fallback: fuente del sistema en Windows
        if (!font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
            std::cerr << "[ERROR] No se pudo cargar ninguna fuente.\n"
                      << "Colocar un .ttf en assets/font.ttf\n";
            return 1;
        }
    }

    // Estado de la simulación
    int  N          = cfg.n;
    int  distIndex  = 0;        // 0=uniforme, 1=clusters, 2=densa
    bool showGrid   = true;
    bool runBF      = true;
    bool queryMode  = false;    // false=rect, true=círculo

    auto buildParticles = [&]() -> std::vector<Particle> {
        return buildParticlesForDist(distIndex, N, cfg);
    };

    std::vector<Particle> particles = buildParticles();
    QuadTree qt(Rectangle(WORLD_W/2, WORLD_H/2, WORLD_W/2, WORLD_H/2), cfg.qtCapacity);

    // Estado de consulta interactiva con el ratón
    bool      dragging  = false;
    sf::Vector2f dragStart{0, 0};
    bool      hasQuery  = false;
    Rectangle queryRect {0, 0, 0, 0};
    float     queryCircleR = 80.f;

    // Métricas
    int   qtComp = 0, bfComp = 0;
    float qtMs   = 0, bfMs   = 0;
    float candPerParticle = 0;

    sf::Clock clock;
    Timer qtTimer, bfTimer;

    auto applyStoredQueryHighlight = [&]() {
        if (!hasQuery) return;

        std::vector<Particle*> found;
        int queryComp = 0;
        if (!queryMode) {
            qt.query(queryRect, found, queryComp);
        } else {
            qt.queryCircle(queryRect.x, queryRect.y, queryCircleR, found, queryComp);
        }
        for (Particle* p : found) p->highlighted = true;
    };

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // EVENTOS — SFML 3.0.2: pollEvent() retorna std::optional
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                    // Distribuciones
                    case sf::Keyboard::Key::Num1:
                        distIndex = 0; particles = buildParticles(); hasQuery = false; break;
                    case sf::Keyboard::Key::Num2:
                        distIndex = 1; particles = buildParticles(); hasQuery = false; break;
                    case sf::Keyboard::Key::Num3:
                        distIndex = 2; particles = buildParticles(); hasQuery = false; break;

                    // Toggles
                    case sf::Keyboard::Key::Q: showGrid  = !showGrid;  break;
                    case sf::Keyboard::Key::B: runBF     = !runBF;     break;
                    case sf::Keyboard::Key::C: queryMode = true;       break;
                    case sf::Keyboard::Key::R: queryMode = false;      break;

                    // Tamaño de simulación
                    case sf::Keyboard::Key::Equal:
                        N = std::min(N + 500, 15000);
                        particles = buildParticles(); hasQuery = false; break;
                    case sf::Keyboard::Key::Hyphen:
                        N = std::max(N - 500, 100);
                        particles = buildParticles(); hasQuery = false; break;

                    // Limpiar query
                    case sf::Keyboard::Key::Escape:
                        hasQuery = false; break;

                    default: break;
                }
            }

            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    dragging  = true;
                    dragStart = {(float)mb->position.x, (float)mb->position.y};
                    hasQuery = false;
                }
            }

            if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mb->button == sf::Mouse::Button::Left && dragging) {
                    dragging = false;
                    sf::Vector2f dragEnd = {(float)mb->position.x,
                                            (float)mb->position.y};

                    if (!queryMode) {
                        // Modo rectangular
                        float cx = (dragStart.x + dragEnd.x) / 2.f;
                        float cy = (dragStart.y + dragEnd.y) / 2.f;
                        float hw = std::abs(dragEnd.x - dragStart.x) / 2.f;
                        float hh = std::abs(dragEnd.y - dragStart.y) / 2.f;
                        queryRect = Rectangle(cx, cy, std::max(hw, 1.f),
                                                       std::max(hh, 1.f));
                    } else {
                        // Modo circular (radio = distancia del drag)
                        float dx = dragEnd.x - dragStart.x;
                        float dy = dragEnd.y - dragStart.y;
                        queryCircleR = std::sqrt(dx*dx + dy*dy);
                        queryRect = Rectangle(dragStart.x, dragStart.y,
                                              queryCircleR, queryCircleR);
                    }
                    hasQuery = true;
                }
            }
        }

        // FÍSICA
        PhysicsEngine::update(particles, (double)dt, WORLD_W, WORLD_H);

        // QUADTREE: reconstruir + detectar colisiones
        qtTimer.start();
        qt.clear();
        for (auto& p : particles) qt.insert(&p);

        int totalCandidates = 0;
        std::vector<std::pair<int,int>> qtCollisions =
            detectCollisionsQT(qt, particles, cfg.maxRadius, qtComp, totalCandidates);

        qtMs = qtTimer.elapsedMs();
        candPerParticle = (float)totalCandidates / (float)std::max(N, 1);

        // FUERZA BRUTA (opcional — toggle [B]) sobre el mismo snapshot
        if (runBF) {
            bfTimer.start();
            std::vector<std::pair<int,int>> bfColls;
            BruteForce::detectCollisions(particles, bfColls, bfComp);
            bfMs = bfTimer.elapsedMs();
        } else {
            bfComp = 0; bfMs = 0;
        }

        PhysicsEngine::resolveCollisions(particles, qtCollisions);
        applyStoredQueryHighlight();

        // RENDER
        window.clear(sf::Color(12, 12, 24));

        if (showGrid) {
            std::vector<Rectangle> bounds;
            qt.getBoundaries(bounds);
            Renderer::drawQuadTree(window, bounds);
        }

        if (hasQuery) {
            if (!queryMode)
                Renderer::drawQueryRect(window, queryRect);
            else
                Renderer::drawQueryCircle(window, queryRect.x, queryRect.y, queryCircleR);
        }

        Renderer::drawParticles(window, particles);

        int fps = (dt > 0.0f) ? (int)(1.0f / dt) : 0;
        HUD::draw(window, font, fps, N,
                  qtComp, bfComp, qtMs, bfMs,
                  candPerParticle,
                  Distributions::name(distIndex),
                  showGrid, runBF);

        window.display();
    }

    return 0;
}
