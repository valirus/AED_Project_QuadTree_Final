cmake_minimum_required(VERSION 3.20)
project(QuadTreeSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── SFML 3.0.2 via vcpkg ────────────────────────────────────────────
# DIFERENCIA CRÍTICA vs SFML 2.x:
#   Antes:  find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
#           target_link_libraries(... sfml-graphics sfml-window sfml-system)
#
#   Ahora:  find_package(SFML 3 COMPONENTS Graphics REQUIRED)
#           target_link_libraries(... PRIVATE SFML::Graphics)
#   SFML::Graphics ya incluye Window y System como dependencias transitivas.
find_package(SFML 3 COMPONENTS Graphics REQUIRED)

# ── Ejecutable ───────────────────────────────────────────────────────
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

target_link_libraries(QuadTreeSim PRIVATE SFML::Graphics)
target_include_directories(QuadTreeSim PRIVATE src)

# ── Copiar assets al directorio de build ────────────────────────────
# Asegura que font.ttf esté disponible al ejecutar desde build/
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})