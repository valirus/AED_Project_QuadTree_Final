cmake_minimum_required(VERSION 3.20)
project(QuadTreeSim)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS Graphics Window System REQUIRED)

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

# Enlace con los nuevos targets de SFML 3
target_link_libraries(QuadTreeSim PRIVATE SFML::Graphics SFML::Window SFML::System)
target_include_directories(QuadTreeSim PRIVATE src)