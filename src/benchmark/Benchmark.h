// src/benchmark/Benchmark.h
// Propietario: Mikael
// Sin dependencias de SFML — mide tiempos con std::chrono
#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

// ─── Timer de alta resolución ───────────────────────────────────────
class Timer {
    using Clock = std::chrono::high_resolution_clock;
    std::chrono::time_point<Clock> t0;
public:
    void  start()     { t0 = Clock::now(); }
    float elapsedMs() const {
        return std::chrono::duration<float, std::milli>(Clock::now() - t0).count();
    }
};

// ─── Resultado de un experimento completo ───────────────────────────
struct BenchResult {
    int         n;
    std::string distribution;   // "Uniforme", "Clusters", "Zona Densa"
    float       buildMs;        // tiempo de reconstrucción del QT
    float       qtQueryMs;      // tiempo promedio de consulta con QT
    float       bfMs;           // tiempo de BruteForce
    long long   qtComparisons;  // comparaciones promedio QT
    long long   bfComparisons;  // comparaciones BF = n*(n-1)/2
    float       candidatesPerParticle;  // MÉTRICA RÚBRICA: candidatos/partícula
    int         collisions;
};

// ─── Recolector de resultados ────────────────────────────────────────
class Benchmark {
public:
    static inline std::vector<BenchResult> results;

    static void record(BenchResult r) { results.push_back(r); }

    static void printTable() {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "\n╔══ BENCHMARK RESULTS ═══════════════════════════════════════╗\n";
        std::cout << std::left
                  << std::setw(8)  << "N"
                  << std::setw(12) << "Distrib."
                  << std::setw(10) << "BldMs"
                  << std::setw(10) << "QT_ms"
                  << std::setw(10) << "BF_ms"
                  << std::setw(10) << "Speedup"
                  << std::setw(12) << "QT_comp"
                  << std::setw(12) << "BF_comp"
                  << std::setw(10) << "Cand/P"
                  << "\n";
        for (auto& r : results) {
            float speedup = (r.bfMs > 0 && r.qtQueryMs > 0.001f) ? r.bfMs / r.qtQueryMs : 0.f;
            std::cout << std::setw(8)  << r.n
                      << std::setw(12) << r.distribution
                      << std::setw(10) << r.buildMs
                      << std::setw(10) << r.qtQueryMs
                      << std::setw(10) << r.bfMs
                      << std::setw(10) << speedup
                      << std::setw(12) << r.qtComparisons
                      << std::setw(12) << r.bfComparisons
                      << std::setw(10) << r.candidatesPerParticle
                      << "\n";
        }
        std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    }

    static void exportCSV(const std::string& path = "benchmark_results.csv") {
        std::ofstream f(path);
        f << "n,distribution,buildMs,qtQueryMs,bfMs,speedup,qtComparisons,"
             "bfComparisons,candidatesPerParticle,collisions\n";
        for (auto& r : results) {
            float sp = (r.bfMs > 0 && r.qtQueryMs > 0.001f) ? r.bfMs / r.qtQueryMs : 0.f;
            f << r.n << "," << r.distribution << ","
              << r.buildMs << "," << r.qtQueryMs << "," << r.bfMs << ","
              << sp << "," << r.qtComparisons << "," << r.bfComparisons << ","
              << r.candidatesPerParticle << "," << r.collisions << "\n";
        }
        std::cout << "[Benchmark] Exportado a " << path << "\n";
    }
};