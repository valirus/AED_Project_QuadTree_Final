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

void runBenchmark() {
    const float W = 1280.f, H = 720.f;
    const int   FRAMES = 120;

    for (int distIdx = 0; distIdx < 3; distIdx++) {
        for (int n : {1000, 5000, 10000}) {
            std::vector<Particle> particles;
            std::string distName = Distributions::name(distIdx);

            if (distIdx == 0)
                particles = Distributions::uniform(n, W, H, 3, 8, 50, 150);
            else if (distIdx == 1)
                particles = Distributions::clusters(n, W, H, 3, 8, 50, 150, 5);
            else
                particles = Distributions::denseZone(n, W, H, 3, 8, 50, 150);

            QuadTree qt(Rectangle(W/2.f, H/2.f, W/2.f, H/2.f), 4);
            float totalBuildMs = 0.f, totalQTms = 0.f, totalBFms = 0.f;
            int   totalQTcomp  = 0,   totalBFcomp = 0;
            float totalCandidates = 0.f;
            int   totalCollisions = 0;
            Timer t;

            for (int f = 0; f < FRAMES; f++) {
                PhysicsEngine::update(particles, 1.0/60.0, (double)W, (double)H);

                t.start();
                qt.clear();
                for (auto& p : particles) qt.insert(&p);
                totalBuildMs += t.elapsedMs();

                int qtComp     = 0;
                int candidates = 0;
                std::vector<std::pair<int,int>> qtCollisions;

                t.start();
                for (auto& p : particles) {
                    std::vector<Particle*> found;
                    float r = (float)(p.radius * 2.0);
                    Rectangle rng((float)p.x, (float)p.y, r, r);
                    qt.query(rng, found, qtComp);
                    candidates += (int)found.size();
                    for (Particle* other : found) {
                        if (other->id <= p.id) continue;
                        float dx = (float)(p.x - other->x);
                        float dy = (float)(p.y - other->y);
                        float rs = (float)(p.radius + other->radius);
                        if (dx*dx + dy*dy < rs*rs)
                            qtCollisions.push_back({p.id, other->id});
                    }
                }
                totalQTms       += t.elapsedMs();
                totalQTcomp     += qtComp;
                totalCandidates += (float)candidates / (float)n;
                totalCollisions += (int)qtCollisions.size();
                PhysicsEngine::resolveCollisions(particles, qtCollisions);

                std::vector<std::pair<int,int>> bfColls;
                int bfComp = 0;
                t.start();
                BruteForce::detectCollisions(particles, bfColls, bfComp);
                totalBFms   += t.elapsedMs();
                totalBFcomp += bfComp;
            }

            Benchmark::record({
                n, distName,
                totalBuildMs    / FRAMES,
                totalQTms       / FRAMES,
                totalBFms       / FRAMES,
                totalQTcomp     / FRAMES,
                totalBFcomp     / FRAMES,
                totalCandidates / FRAMES,
                totalCollisions / FRAMES
            });
        }
    }
    Benchmark::printTable();
    Benchmark::exportCSV();
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "benchmark") {
        runBenchmark();
        return 0;
    }

    const float WORLD_W = 1280.f, WORLD_H = 720.f;

    sf::RenderWindow window(
        sf::VideoMode({(unsigned int)WORLD_W, (unsigned int)WORLD_H}),
        "QuadTree Particle Simulator — CS2023"
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/font.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
            std::cerr << "[ERROR] No se pudo cargar fuente.\n"
                      << "Coloca un .ttf en assets/font.ttf\n";
            return 1;
        }
    }

    int  N          = 1000;
    int  distIndex  = 0;
    int  qtCapacity = 4;
    bool showGrid   = true;
    bool runBF      = true;
    bool queryMode  = false;

    auto buildParticles = [&]() -> std::vector<Particle> {
        if (distIndex == 0)
            return Distributions::uniform(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
        else if (distIndex == 1)
            return Distributions::clusters(N, WORLD_W, WORLD_H, 3, 8, 50, 150, 5);
        else
            return Distributions::denseZone(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
    };

    std::vector<Particle> particles = buildParticles();
    QuadTree qt(Rectangle(WORLD_W/2.f, WORLD_H/2.f, WORLD_W/2.f, WORLD_H/2.f), qtCapacity);

    bool         dragging     = false;
    sf::Vector2f dragStart    = {0.f, 0.f};
    bool         hasQuery     = false;
    Rectangle    queryRect    = {0.f, 0.f, 0.f, 0.f};
    float        queryCircleR = 80.f;

    int   qtComp = 0, bfComp = 0;
    float qtMs   = 0.f, bfMs = 0.f;
    float candPerParticle = 0.f;

    sf::Clock clock;
    Timer qtTimer, bfTimer;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                    case sf::Keyboard::Key::Num1:
                        distIndex = 0; particles = buildParticles(); break;
                    case sf::Keyboard::Key::Num2:
                        distIndex = 1; particles = buildParticles(); break;
                    case sf::Keyboard::Key::Num3:
                        distIndex = 2; particles = buildParticles(); break;
                    case sf::Keyboard::Key::Q:
                        showGrid  = !showGrid;  break;
                    case sf::Keyboard::Key::B:
                        runBF     = !runBF;     break;
                    case sf::Keyboard::Key::C:
                        queryMode = !queryMode; break;
                    case sf::Keyboard::Key::Equal:
                        N = std::min(N + 500, 15000);
                        particles = buildParticles(); break;
                    case sf::Keyboard::Key::Hyphen:
                        N = std::max(N - 500, 100);
                        particles = buildParticles(); break;
                    case sf::Keyboard::Key::Escape:
                        hasQuery = false; break;
                    default: break;
                }
            }

            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    dragging  = true;
                    dragStart = {(float)mb->position.x, (float)mb->position.y};
                    for (auto& p : particles) p.highlighted = false;
                    hasQuery  = false;
                }
            }

            if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mb->button == sf::Mouse::Button::Left && dragging) {
                    dragging = false;
                    sf::Vector2f dragEnd = {(float)mb->position.x,
                                            (float)mb->position.y};
                    std::vector<Particle*> found;
                    int queryComp = 0;

                    if (!queryMode) {
                        float cx = (dragStart.x + dragEnd.x) / 2.f;
                        float cy = (dragStart.y + dragEnd.y) / 2.f;
                        float hw = std::abs(dragEnd.x - dragStart.x) / 2.f;
                        float hh = std::abs(dragEnd.y - dragStart.y) / 2.f;
                        queryRect = Rectangle(cx, cy,
                                              std::max(hw, 1.f),
                                              std::max(hh, 1.f));
                        qt.query(queryRect, found, queryComp);
                    } else {
                        float dx = dragEnd.x - dragStart.x;
                        float dy = dragEnd.y - dragStart.y;
                        queryCircleR = std::sqrt(dx*dx + dy*dy);
                        queryRect    = Rectangle(dragStart.x, dragStart.y,
                                                 queryCircleR, queryCircleR);
                        qt.queryCircle(dragStart.x, dragStart.y,
                                       queryCircleR, found, queryComp);
                    }
                    for (Particle* p : found) p->highlighted = true;
                    hasQuery = true;
                }
            }
        }

        // ── Física ───────────────────────────────────────────────────
        PhysicsEngine::update(particles, (double)dt, WORLD_W, WORLD_H);

        // ── QuadTree ─────────────────────────────────────────────────
        qtTimer.start();
        qt.clear();
        for (auto& p : particles) qt.insert(&p);

        qtComp = 0;
        int totalCandidates = 0;
        std::vector<std::pair<int,int>> qtCollisions;

        for (auto& p : particles) {
            std::vector<Particle*> found;
            float r = (float)(p.radius * 2.0);
            Rectangle rng((float)p.x, (float)p.y, r, r);
            qt.query(rng, found, qtComp);
            totalCandidates += (int)found.size();

            for (Particle* other : found) {
                if (other->id <= p.id) continue;
                float dx = (float)(p.x - other->x);
                float dy = (float)(p.y - other->y);
                float rs = (float)(p.radius + other->radius);
                if (dx*dx + dy*dy < rs*rs)
                    qtCollisions.push_back({p.id, other->id});
            }
        }
        qtMs = qtTimer.elapsedMs();
        candPerParticle = (float)totalCandidates / (float)std::max(N, 1);
        PhysicsEngine::resolveCollisions(particles, qtCollisions);

        // ── BruteForce ───────────────────────────────────────────────
        if (runBF) {
            bfTimer.start();
            std::vector<std::pair<int,int>> bfColls;
            BruteForce::detectCollisions(particles, bfColls, bfComp);
            bfMs = bfTimer.elapsedMs();
        } else {
            bfComp = 0; bfMs = 0.f;
        }

        // ── Render ───────────────────────────────────────────────────
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
                Renderer::drawQueryCircle(window,
                    queryRect.x, queryRect.y, queryCircleR);
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
