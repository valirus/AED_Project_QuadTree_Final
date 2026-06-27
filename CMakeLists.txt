cmake_minimum_required(VERSION 3.20)
project(QuadTreeSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SFML_DIR "C:/SFML-3.0.2/lib/cmake/SFML")
find_package(SFML 3 COMPONENTS Graphics REQUIRED)

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

add_definitions(-DSFML_STATIC)
target_link_options(QuadTreeSim PRIVATE "-Wl,--entry,mainCRTStartup")

target_link_libraries(QuadTreeSim PRIVATE SFML::Graphics)
target_include_directories(QuadTreeSim PRIVATE src)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})