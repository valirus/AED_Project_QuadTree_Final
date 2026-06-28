# QuadTree Particle Simulator - CS2023 AED

Proyecto final elegido: **Opción A - QuadTree, simulador de partículas 2D**.

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
- Benchmark para 1,000, 5,000 y 10,000 partículas en tres distribuciones.

## Stack

- C++17
- CMake 3.20+
- SFML 3.x
- vcpkg

## Instalación de dependencias en Windows

```powershell
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install sfml:x64-windows
```

## Compilación

Desde la raíz del proyecto:

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Con Ninja single-config, el ejecutable queda usualmente en:

```powershell
.\build\QuadTreeSim.exe
```

Con Visual Studio generator/multi-config, puede quedar en:

```powershell
.\build\Release\QuadTreeSim.exe
```

## Ejecución interactiva

```powershell
.\build\QuadTreeSim.exe
```

Opciones configurables por CLI:

```powershell
.\build\QuadTreeSim.exe --n 5000 --width 1280 --height 720 --capacity 4 --min-radius 3 --max-radius 8 --min-speed 50 --max-speed 150
```

Controles:

- `1`, `2`, `3`: cambiar distribución: uniforme, clusters, zona densa.
- `Q`: mostrar/ocultar subdivisiones del QuadTree.
- `B`: activar/desactivar fuerza bruta.
- `C`: modo de consulta circular.
- `R`: modo de consulta rectangular.
- `+` / `-`: aumentar/disminuir cantidad de partículas.
- `Esc`: limpiar consulta activa.
- Mouse izquierdo + arrastre: definir región o radio de consulta.

## Benchmark

```powershell
.\build\QuadTreeSim.exe benchmark
```

Benchmark con configuración explícita:

```powershell
.\build\QuadTreeSim.exe benchmark --capacity 4 --min-radius 3 --max-radius 8 --min-speed 50 --max-speed 150 --frames 120
```

Genera:

```text
benchmark_results.csv
```

El benchmark ejecuta tres distribuciones espaciales y tres tamaños: 1,000, 5,000 y 10,000 partículas.
