# QuadTree Particle Simulator - CS2023 AED

Repositorio: https://github.com/valirus/AED_Project_QuadTree_Final

Proyecto final elegido: **Opción A - QuadTree, simulador de partículas 2D**.

## Descripción

Este proyecto implementa desde cero un **QuadTree** para acelerar consultas espaciales en una simulación 2D de partículas circulares en movimiento. En cada frame se actualizan las posiciones, se reconstruye el QuadTree y se usa la estructura para detectar vecinos cercanos y posibles colisiones. La aplicación compara el rendimiento del QuadTree contra una solución ingenua de fuerza bruta que revisa todos los pares de partículas.

## Requisitos cubiertos

- QuadTree implementado desde cero.
- Inserción de partículas.
- Subdivisión automática al superar la capacidad máxima por nodo.
- Actualización de posiciones frame por frame.
- Reconstrucción del QuadTree después del movimiento.
- Consulta rectangular.
- Consulta circular por punto/radio.
- Detección de posibles colisiones.
- Comparación contra fuerza bruta.
- Visualización de partículas, subdivisiones, consultas, candidatos, colisiones y métricas.
- Benchmark para 1,000, 5,000 y 10,000 partículas en tres distribuciones espaciales.

## Stack

- C++17
- CMake 3.20+
- SFML 3.x
- vcpkg

## Estructura del proyecto

```text
.
├── assets/
├── src/
│   ├── benchmark/
│   ├── physics/
│   ├── quadtree/
│   ├── rendering/
│   └── main.cpp
├── CMakeLists.txt
├── vcpkg.json
├── README.md
├── benchmark_results.csv
└── JUSTIFICACION_COMPLEJIDAD.pdf
```

## Instalación de dependencias en Windows

Instalar previamente:

- Git
- CMake 3.20 o superior
- Visual Studio Build Tools con soporte para C++
- vcpkg

Instalación básica de vcpkg:

```powershell
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

El proyecto incluye `vcpkg.json`, por lo que SFML se resuelve automáticamente al configurar CMake con el toolchain de vcpkg.

## Compilación con Visual Studio generator

Desde la raíz del proyecto:

```powershell
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build build --config Release
```

El ejecutable queda en:

```powershell
.\build\Release\QuadTreeSim.exe
```

## Compilación con Ninja

Si se usa Ninja:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

El ejecutable queda en:

```powershell
.\build\QuadTreeSim.exe
```

## Ejecución interactiva

Con Visual Studio generator:

```powershell
.\build\Release\QuadTreeSim.exe
```

Con Ninja:

```powershell
.\build\QuadTreeSim.exe
```

Ejemplo con parámetros configurables:

```powershell
.\build\Release\QuadTreeSim.exe --n 5000 --width 1280 --height 720 --capacity 4 --min-radius 3 --max-radius 8 --min-speed 50 --max-speed 150
```

Parámetros disponibles:

- `--n`: número inicial de partículas.
- `--width`: ancho del espacio 2D.
- `--height`: alto del espacio 2D.
- `--capacity`: capacidad máxima por nodo del QuadTree.
- `--min-radius`: radio mínimo de las partículas.
- `--max-radius`: radio máximo de las partículas.
- `--min-speed`: velocidad mínima inicial.
- `--max-speed`: velocidad máxima inicial.

## Controles de la interfaz

- `1`: distribución uniforme.
- `2`: distribución con clusters.
- `3`: distribución con zona de alta densidad.
- `Q`: mostrar u ocultar subdivisiones del QuadTree.
- `B`: activar o desactivar fuerza bruta.
- `C`: modo de consulta circular.
- `R`: modo de consulta rectangular.
- `+`: aumentar cantidad de partículas.
- `-`: disminuir cantidad de partículas.
- `Esc`: limpiar consulta activa.
- Mouse izquierdo + arrastre: definir región rectangular o radio de consulta, según el modo activo.

## Benchmark

Con Visual Studio generator:

```powershell
.\build\Release\QuadTreeSim.exe benchmark
```

Con Ninja:

```powershell
.\build\QuadTreeSim.exe benchmark
```

Benchmark con configuración explícita:

```powershell
.\build\Release\QuadTreeSim.exe benchmark --capacity 4 --min-radius 3 --max-radius 8 --min-speed 50 --max-speed 150 --frames 120
```

El benchmark genera:

```text
benchmark_results.csv
```

El benchmark evalúa tres tamaños de entrada:

- 1,000 partículas.
- 5,000 partículas.
- 10,000 partículas.

Y tres distribuciones espaciales:

- Uniforme.
- Clusters.
- Zona de alta densidad.

Para cada caso se reportan tiempos promedio, comparaciones, speedup y candidatos revisados por partícula.

## Archivos principales

- `src/quadtree/QuadTree.h` y `src/quadtree/QuadTree.cpp`: implementación del QuadTree.
- `src/quadtree/Rectangle.h`: región rectangular usada por cada nodo.
- `src/quadtree/BruteForce.h` y `src/quadtree/BruteForce.cpp`: solución ingenua para comparación.
- `src/physics/Particle.h`: modelo de partícula.
- `src/physics/PhysicsEngine.h` y `src/physics/PhysicsEngine.cpp`: actualización física y resolución básica de colisiones.
- `src/physics/Distributions.h` y `src/physics/Distributions.cpp`: generación de distribuciones uniforme, clusters y zona densa.
- `src/rendering/Renderer.h` y `src/rendering/Renderer.cpp`: visualización de partículas, QuadTree y consultas.
- `src/rendering/HUD.h` y `src/rendering/HUD.cpp`: panel de métricas.
- `src/benchmark/Benchmark.h` y `src/benchmark/Benchmark.cpp`: registro y exportación de resultados.
- `src/main.cpp`: integración del modo interactivo y benchmark.

## Complejidad esperada

La fuerza bruta compara todos los pares de partículas, por lo que realiza `n(n - 1) / 2` comparaciones y tiene complejidad `O(n^2)`.

El QuadTree reduce el número de comparaciones mediante poda espacial. En el caso esperado:

- Inserción de una partícula: `O(log n)`.
- Construcción o reconstrucción del árbol: `O(n log n)`.
- Consulta rectangular o circular: `O(log n + k)`, donde `k` es el número de candidatos retornados.
- Detección total de colisiones: aproximadamente `O(n log n + n · k)`.

En el peor caso, si muchas partículas caen en una misma región, el QuadTree puede degradarse hacia comportamiento cuadrático. Por eso se evalúan tres distribuciones espaciales distintas.

## Limpieza del proyecto

No se deben subir ni entregar carpetas generadas por compilación:

```text
build/
vcpkg_installed/
.vs/
Debug/
Release/
x64/
out/
.cache/
```

Estas carpetas se regeneran localmente al compilar.
