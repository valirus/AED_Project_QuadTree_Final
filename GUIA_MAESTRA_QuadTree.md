# GUÍA MAESTRA — QuadTree Particle Simulator
### CS2023 · Algoritmos y Estructuras de Datos
**Stack:** C++17 · SFML 3.0.2 · CMake 3.20+ · vcpkg · Git  
**Repositorio:** `https://github.com/valirus/AED_Project_QuadTree_Final.git`

---

## Índice

1. [Roles e ingeniería de propiedad](#1-roles-e-ingeniería-de-propiedad)
2. [Setup e instalación — Windows (Nicolas y Mikael)](#2-setup-e-instalación--windows-nicolas-y-mikael)
3. [Flujo de Git y comandos](#3-flujo-de-git-y-comandos)
4. [Audit de cumplimiento vs rúbrica](#4-audit-de-cumplimiento-vs-rúbrica)
5. [Código — Valentino: Arquitecto de Estructuras Espaciales](#5-código--valentino-arquitecto-de-estructuras-espaciales)
6. [Código — Nicolas: Ingeniero de Simulación Física](#6-código--nicolas-ingeniero-de-simulación-física)
7. [Código — Mikael: Lead de Sistemas de Renderizado](#7-código--mikael-lead-de-sistemas-de-renderizado)
8. [Archivos compartidos: CMakeLists.txt y vcpkg.json](#8-archivos-compartidos-cmakeliststxt-y-vcpkgjson)

---

## 1. Roles e ingeniería de propiedad

> **Regla de oro de Git:** nadie toca un archivo que no sea de su dominio sin antes
> abrir un issue en el repo. Los conflictos de merge en proyectos académicos son la
> causa número uno de pérdida de tiempo. Esta sección es ley.

### 1.1 Valentino — Arquitecto de Estructuras Espaciales

**Dominio conceptual:** implementación desde cero del QuadTree y su contraparte ingenua.
Este rol es el núcleo académico del proyecto; el código aquí es 100% algorítmico, sin dependencias
externas y debe ser explicable en la defensa línea a línea.

| Archivo | Estado | Descripción |
|---|---|---|
| `src/quadtree/Rectangle.h` | **Propietario exclusivo** | AABB: región, contains, intersects |
| `src/quadtree/QuadTree.h` | **Propietario exclusivo** | Interfaz pública del árbol |
| `src/quadtree/QuadTree.cpp` | **Propietario exclusivo** | insert, subdivide, query, clear |
| `src/quadtree/BruteForce.h` | **Propietario exclusivo** | Interfaz de la solución ingenua |
| `src/quadtree/BruteForce.cpp` | **Propietario exclusivo** | O(n²) de detección de colisiones |

**Rama de trabajo:** `feat/quadtree`  
**Interfaz que expone hacia el equipo** (no cambiar sin avisar):
```
QuadTree(Rectangle, int capacity)
bool insert(Particle*)
void query(Rectangle, vector<Particle*>&, int& comparisons)
void queryCircle(float cx, cy, r, vector<Particle*>&, int& comparisons)
void clear()
void getBoundaries(vector<Rectangle>&)
```

---

### 1.2 Nicolas — Ingeniero de Simulación Física

**Dominio conceptual:** el motor de física determina que las partículas se comporten
de forma creíble y que los tres escenarios de distribución sean reproducibles y distinguibles.
Este módulo es completamente independiente de SFML; si compila sin él, mejor.

| Archivo | Estado | Descripción |
|---|---|---|
| `src/physics/Particle.h` | **Propietario exclusivo** | Struct de datos puro, sin SFML |
| `src/physics/PhysicsEngine.h` | **Propietario exclusivo** | Interfaz del motor |
| `src/physics/PhysicsEngine.cpp` | **Propietario exclusivo** | Euler, bounce, resolveCollisions |
| `src/physics/Distributions.h` | **Propietario exclusivo** | Interfaz de generadores |
| `src/physics/Distributions.cpp` | **Propietario exclusivo** | Uniforme, clusters, zona densa |

**Rama de trabajo:** `feat/physics`  
**Interfaz que expone hacia el equipo:**
```
vector<Particle> Distributions::uniform(n, W, H, minR, maxR, minSpd, maxSpd)
vector<Particle> Distributions::clusters(n, W, H, minR, maxR, minSpd, maxSpd, k)
vector<Particle> Distributions::denseZone(n, W, H, minR, maxR, minSpd, maxSpd)
void PhysicsEngine::update(vector<Particle>&, double dt, double W, double H)
void PhysicsEngine::resolveCollisions(vector<Particle>&, vector<pair<int,int>>&)
```

---

### 1.3 Mikael — Lead de Sistemas de Renderizado

**Dominio conceptual:** todo lo que el usuario ve y las métricas que se reportan.
Este rol integra los módulos de Valentino y Nicolas en la pantalla y produce los datos
empíricos que justifican el proyecto. Es el único módulo con dependencia directa de SFML.

| Archivo | Estado | Descripción |
|---|---|---|
| `src/rendering/Renderer.h` | **Propietario exclusivo** | Interfaz de dibujado SFML |
| `src/rendering/Renderer.cpp` | **Propietario exclusivo** | Partículas, QuadTree grid, query |
| `src/rendering/HUD.h` | **Propietario exclusivo** | Interfaz del panel de estadísticas |
| `src/rendering/HUD.cpp` | **Propietario exclusivo** | FPS, comparaciones, speedup |
| `src/benchmark/Benchmark.h` | **Propietario exclusivo** | Timer y recolección de métricas |
| `src/benchmark/Benchmark.cpp` | **Propietario exclusivo** | Modo headless, exportar CSV |
| `src/main.cpp` | **Propietario exclusivo** | Game loop, eventos SFML 3 |

**Rama de trabajo:** `feat/renderer`

---

## 2. Setup e Instalación — Windows (Nicolas y Mikael)

> Valentino ya tiene el repo clonado. Estas instrucciones son para Nicolas y Mikael
> que empiezan desde cero en Windows.

### Paso 0 — Prerrequisitos

Instalar en este orden exacto si no los tienen ya:

1. **Git for Windows** → https://git-scm.com/download/win
2. **Visual Studio Build Tools 2022** (solo las herramientas de build, no el IDE completo)  
   Descargar de: https://visualstudio.microsoft.com/visual-cpp-build-tools/  
   Durante la instalación, marcar: `C++ build tools` + `Windows SDK`
3. **CMake 3.20+** → https://cmake.org/download/ (marcar "Add to PATH" durante instalación)
4. **VS Code** → https://code.visualstudio.com/
5. **VS Code Extensions** — instalar desde el panel de extensiones:
   - `ms-vscode.cpptools` (C/C++)
   - `ms-vscode.cmake-tools` (CMake Tools)
   - `ms-vscode.cpptools-extension-pack`

### Paso 1 — Instalar vcpkg

Abrir **PowerShell como Administrador** y ejecutar:

```powershell
# 1. Clonar vcpkg en C:\vcpkg (ubicación recomendada)
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# 2. Bootstrapear (compilar vcpkg)
.\bootstrap-vcpkg.bat

# 3. Integrar con MSBuild y CMake a nivel de sistema
.\vcpkg integrate install

# 4. Agregar VCPKG_ROOT a las variables de entorno del sistema
# (hacerlo también en Settings > System > Advanced > Environment Variables)
[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "Machine")
```

> **Verificación:** cerrar y reabrir PowerShell, luego ejecutar `vcpkg version`. Debe mostrar la versión.

### Paso 2 — Instalar SFML 3.0.2 via vcpkg

```powershell
# Instalar SFML 3 para 64 bits
C:\vcpkg\vcpkg install sfml:x64-windows

# Verificar que instaló correctamente
C:\vcpkg\vcpkg list
# Debe aparecer: sfml:x64-windows   3.x.x   ...
```

> La instalación puede tardar 5–10 minutos la primera vez porque compila desde fuente.

### Paso 3 — Clonar el repositorio y crear rama de trabajo

Abrir una terminal normal (no admin) en la carpeta donde quieran trabajar:

```bash
# Clonar el repositorio del equipo
git clone https://github.com/valirus/AED_Project_QuadTree_Final.git

# Entrar a la carpeta
cd AED_Project_QuadTree_Final

# ---- NICOLAS hace esto ----
git checkout -b feat/physics

# ---- MIKAEL hace esto ----
git checkout -b feat/renderer

# Verificar que estás en tu rama correcta
git branch
# El asterisco (*) indica la rama activa
```

### Paso 4 — Configurar VS Code con CMake Tools

1. Abrir VS Code en la carpeta del proyecto: `code .`
2. Presionar `Ctrl+Shift+P` → buscar **"CMake: Configure"**
3. VS Code pedirá el kit de compilación → seleccionar **"Visual Studio Build Tools 2022 - amd64"**
4. Si CMake Tools no encuentra SFML automáticamente, crear o editar `.vscode/settings.json`:

```json
{
    "cmake.configureArgs": [
        "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
    ],
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja"
}
```

5. Volver a hacer `Ctrl+Shift+P` → **"CMake: Configure"** → debe completarse sin errores
6. Para compilar: `Ctrl+Shift+P` → **"CMake: Build"** o presionar `F7`
7. Para ejecutar: `Ctrl+Shift+P` → **"CMake: Run Without Debugging"** o `Shift+F5`

> **Si CMake falla con "Could not find SFML":** verificar que `VCPKG_ROOT` esté en las
> variables de entorno del sistema (no solo del usuario) y reiniciar VS Code completamente.

---

## 3. Flujo de Git y comandos

### 3.1 Reglas del repositorio

| Rama | Propietario | Política |
|---|---|---|
| `main` | Todo el equipo | **Protegida.** Solo se mergea vía PR revisado |
| `feat/quadtree` | Valentino | Push libre; nadie más hace commit aquí |
| `feat/physics` | Nicolas | Push libre; nadie más hace commit aquí |
| `feat/renderer` | Mikael | Push libre; nadie más hace commit aquí |

### 3.2 Flujo al final de cada sesión de trabajo

```bash
# 1. Ver qué archivos modificaste
git status

# 2. Agregar solo TUS archivos (NUNCA usar "git add ." sin revisar antes)
# Ejemplo para Nicolas:
git add src/physics/PhysicsEngine.cpp
git add src/physics/Distributions.cpp

# 3. Commit con mensaje descriptivo en formato: [módulo] descripción
git commit -m "[physics] Implementar update() con Euler y resolveCollisions()"

# 4. Subir al repositorio remoto
git push origin feat/physics   # cada uno usa su rama
```

### 3.3 Convención de mensajes de commit

```
[quadtree]  insert() y subdivide() implementados con redistribución de partículas
[physics]   Distribuciones: uniforme y clusters listos, denseZone en progreso
[renderer]  HUD con FPS y comparaciones QT vs BF en tiempo real
[main]      Game loop con reconstrucción QT cada frame
[fix]       Corregir condición de intersects() que fallaba en bordes del mundo
[benchmark] Agregar candidatos promedio por objeto al reporte CSV
```

### 3.4 Integración a main (al final de cada sesión grande o cuando una feature está completa)

**Valentino** coordina los merges al final de cada sesión:

```bash
# Desde la rama de cada persona, antes de mergear:
git fetch origin
git rebase origin/main   # traer los cambios más recientes de main

# Si hay conflictos, resolverlos en VS Code (botón "Accept Current/Incoming")
# Luego:
git add <archivo-resuelto>
git rebase --continue

# Finalmente, desde main:
git checkout main
git merge feat/quadtree --no-ff -m "merge: QuadTree core - Sesión 1"
git merge feat/physics  --no-ff -m "merge: Physics engine - Sesión 1"
git merge feat/renderer --no-ff -m "merge: Renderer base - Sesión 1"
git push origin main
```

### 3.5 Sincronizar tu rama con los cambios de tus compañeros

```bash
# Al inicio de cada sesión, desde tu rama:
git fetch origin
git merge origin/main   # traer lo que mergearon tus compañeros

# Si no hay conflictos, continuar trabajando normalmente
```

---

## 4. Audit de cumplimiento vs rúbrica

### 4.1 Problemas críticos en el plan original (ya corregidos en este documento)

| # | Problema | Impacto | Estado |
|---|---|---|---|
| 1 | `find_package(SFML 2.6 ...)` + targets `sfml-graphics` | **Build falla con SFML 3** | ✅ Corregido |
| 2 | `window.pollEvent(event)` como parámetro | **API eliminada en SFML 3** | ✅ Corregido |
| 3 | `event.type == sf::Event::Closed` | **API de variante en SFML 3** | ✅ Corregido |
| 4 | `sf::Keyboard::Q` (enum no scoped) | **Error de compilación en SFML 3** | ✅ Corregido |
| 5 | `sf::Mouse::Left` | **Error de compilación en SFML 3** | ✅ Corregido |
| 6 | `font.loadFromFile()` | **Renombrado a openFromFile() en SFML 3** | ✅ Corregido |
| 7 | `sf::Text text; text.setFont(font)` | **Constructor requiere font en SFML 3** | ✅ Corregido |
| 8 | `Particle.h` incluye `<SFML/Graphics.hpp>` | **Viola separación de módulos** | ✅ Corregido |
| 9 | Benchmark no mide candidatos por objeto | **Métrica explícita de la rúbrica** | ✅ Agregado |

### 4.2 Checklist de requisitos de la rúbrica

| Requisito del PDF | Plan original | Estado |
|---|---|---|
| QuadTree implementado desde cero | ✅ | ✅ Completo |
| Subdividir al superar capacidad | ✅ | ✅ Completo |
| Reconstruir QuadTree cada frame | ✅ | ✅ Completo |
| Consulta por región rectangular | ✅ | ✅ `query()` |
| Consulta de vecinos por punto/radio | ✅ | ✅ `queryCircle()` |
| Detección de colisiones | ✅ | ✅ Completo |
| Distribución uniforme | ✅ | ✅ Completo |
| Distribución con clusters | ✅ | ✅ Completo |
| Zona de alta densidad | ✅ | ✅ Completo |
| Comparación fuerza bruta | ✅ | ✅ `BruteForce::detectCollisions()` |
| Visualizar subdivisiones del QT | ✅ | ✅ `getBoundaries()` + Renderer |
| Visualizar región consultada | ✅ | ✅ `drawQueryRegion()` |
| Visualizar candidatos retornados | ✅ | ✅ flag `highlighted` |
| Comparaciones QT vs BF en pantalla | ✅ | ✅ HUD |
| Tiempo por frame o consulta | ✅ | ✅ HUD |
| Experimento 1k partículas | ✅ | ✅ benchmark mode |
| Experimento 5k partículas | ✅ | ✅ benchmark mode |
| Experimento 10k+ partículas | ✅ | ✅ benchmark mode |
| **Candidatos revisados por objeto** | ❌ faltaba | ✅ Agregado en Benchmark |
| **Efecto de distribución en rendimiento** | parcial | ✅ Modo benchmark por distribución |

---

## 5. Código — Valentino: Arquitecto de Estructuras Espaciales

> Estos archivos no tienen dependencias de SFML. Compilan solos con solo `g++ -std=c++17`.
> El plan original era correcto en lógica; se mantiene sin cambios.

### `src/quadtree/Rectangle.h`

```cpp
// src/quadtree/Rectangle.h
// Propietario: Valentino
// Sin dependencias externas — AABB puro
#pragma once

struct Rectangle {
    float x, y;    // centro del rectángulo
    float hw, hh;  // half-width y half-height

    Rectangle() : x(0), y(0), hw(0), hh(0) {}
    Rectangle(float x, float y, float hw, float hh)
        : x(x), y(y), hw(hw), hh(hh) {}

    // ¿El punto (px, py) cae dentro de esta región?
    bool contains(float px, float py) const {
        return (px >= x - hw && px < x + hw &&
                py >= y - hh && py < y + hh);
    }

    // ¿Esta región se solapa con otra? (descartar hijos irrelevantes)
    bool intersects(const Rectangle& o) const {
        return !(o.x - o.hw >= x + hw ||
                 o.x + o.hw <= x - hw ||
                 o.y - o.hh >= y + hh ||
                 o.y + o.hh <= y - hh);
    }
};
```

### `src/quadtree/QuadTree.h`

```cpp
// src/quadtree/QuadTree.h
// Propietario: Valentino
#pragma once
#include "Rectangle.h"
#include "../physics/Particle.h"
#include <vector>

class QuadTree {
public:
    Rectangle               boundary;
    int                     capacity;
    std::vector<Particle*>  particles;
    bool                    divided = false;

    QuadTree* ne = nullptr;
    QuadTree* nw = nullptr;
    QuadTree* se = nullptr;
    QuadTree* sw = nullptr;

    QuadTree(Rectangle boundary, int capacity);
    ~QuadTree();

    // Operaciones principales — explicables en la defensa
    bool insert(Particle* p);
    void query(const Rectangle& range,
               std::vector<Particle*>& found,
               int& comparisons) const;
    void queryCircle(float cx, float cy, float r,
                     std::vector<Particle*>& found,
                     int& comparisons) const;
    void clear();

    // Para Mikael: devuelve todos los AABB del árbol actual
    void getBoundaries(std::vector<Rectangle>& out) const;

private:
    void subdivide();
};
```

### `src/quadtree/QuadTree.cpp`

```cpp
// src/quadtree/QuadTree.cpp
// Propietario: Valentino
// IMPLEMENTACIÓN DESDE CERO — requerido por la rúbrica
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
```

### `src/quadtree/BruteForce.h`

```cpp
// src/quadtree/BruteForce.h
// Propietario: Valentino
#pragma once
#include "../physics/Particle.h"
#include <vector>
#include <utility>

class BruteForce {
public:
    // Compara todos contra todos: O(n²)
    static void detectCollisions(
        const std::vector<Particle>& particles,
        std::vector<std::pair<int,int>>& collisions,
        int& comparisons);

    // Vecinos en radio: O(n) por partícula consultada
    static void queryRadius(
        const std::vector<Particle>& particles,
        float cx, float cy, float r,
        std::vector<int>& found,
        int& comparisons);
};
```

### `src/quadtree/BruteForce.cpp`

```cpp
// src/quadtree/BruteForce.cpp
// Propietario: Valentino
#include "BruteForce.h"
#include <cmath>

// Todos contra todos — referencia O(n²) que el QuadTree debe superar
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
            float dx = (float)(particles[i].x - particles[j].x);
            float dy = (float)(particles[i].y - particles[j].y);
            float radSum = (float)(particles[i].radius + particles[j].radius);
            if (dx*dx + dy*dy < radSum*radSum)
                collisions.push_back({i, j});
        }
    }
}

void BruteForce::queryRadius(
        const std::vector<Particle>& particles,
        float cx, float cy, float r,
        std::vector<int>& found,
        int& comparisons) {
    comparisons = 0;
    found.clear();
    for (int i = 0; i < (int)particles.size(); i++) {
        comparisons++;
        float dx = (float)particles[i].x - cx;
        float dy = (float)particles[i].y - cy;
        if (dx*dx + dy*dy <= r*r)
            found.push_back(i);
    }
}
```

---

## 6. Código — Nicolas: Ingeniero de Simulación Física

> **Cambio crítico respecto al plan original:** `Particle.h` ya NO incluye
> `<SFML/Graphics.hpp>`. El color y los flags de visualización son responsabilidad
> del Renderer, no del struct de datos. Esto es separación de responsabilidades.

### `src/physics/Particle.h`

```cpp
// src/physics/Particle.h
// Propietario: Nicolas
// Struct puro — CERO dependencias de SFML ni de renderizado
#pragma once

struct Particle {
    int    id     = 0;
    double x      = 0.0, y  = 0.0;   // posición
    double vx     = 0.0, vy = 0.0;   // velocidad
    double radius = 5.0;

    // Flags de estado — los lee el Renderer para decidir el color
    bool highlighted = false;  // candidata de una query del QuadTree
    bool colliding   = false;  // detectada en colisión este frame
};
```

### `src/physics/PhysicsEngine.h`

```cpp
// src/physics/PhysicsEngine.h
// Propietario: Nicolas
#pragma once
#include "Particle.h"
#include <vector>
#include <utility>

class PhysicsEngine {
public:
    // Integración de Euler + rebote en bordes del mundo
    static void update(std::vector<Particle>& particles,
                       double dt,
                       double worldW, double worldH);

    // Resolver colisiones detectadas (intercambio elástico de velocidades)
    static void resolveCollisions(std::vector<Particle>& particles,
                                  const std::vector<std::pair<int,int>>& pairs);
};
```

### `src/physics/PhysicsEngine.cpp`

```cpp
// src/physics/PhysicsEngine.cpp
// Propietario: Nicolas
#include "PhysicsEngine.h"
#include <cmath>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────
// update(): integración de Euler hacia adelante.
// Concepto: pos_nueva = pos_vieja + vel * dt
// dt viene del reloj de SFML (sf::Clock::restart().asSeconds())
// ─────────────────────────────────────────────────────────────────────
void PhysicsEngine::update(std::vector<Particle>& particles,
                           double dt,
                           double worldW, double worldH) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        // Rebote en borde izquierdo / derecho
        if (p.x - p.radius < 0.0) {
            p.x  = p.radius;
            p.vx = std::abs(p.vx);
        } else if (p.x + p.radius > worldW) {
            p.x  = worldW - p.radius;
            p.vx = -std::abs(p.vx);
        }

        // Rebote en borde superior / inferior
        if (p.y - p.radius < 0.0) {
            p.y  = p.radius;
            p.vy = std::abs(p.vy);
        } else if (p.y + p.radius > worldH) {
            p.y  = worldH - p.radius;
            p.vy = -std::abs(p.vy);
        }

        // Limpiar flags de visualización cada frame
        p.highlighted = false;
        p.colliding   = false;
    }
}

// ─────────────────────────────────────────────────────────────────────
// resolveCollisions(): colisión elástica simplificada (masas iguales).
// Separa las partículas solapadas y refleja velocidades en la normal.
// ─────────────────────────────────────────────────────────────────────
void PhysicsEngine::resolveCollisions(
        std::vector<Particle>& particles,
        const std::vector<std::pair<int,int>>& pairs) {
    for (auto& [i, j] : pairs) {
        Particle& a = particles[i];
        Particle& b = particles[j];

        double dx   = b.x - a.x, dy = b.y - a.y;
        double dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 1e-9) continue;  // evitar división por cero

        // Separar partículas para eliminar solapamiento
        double overlap = (a.radius + b.radius) - dist;
        double nx = dx / dist, ny = dy / dist;
        a.x -= nx * overlap * 0.5;
        a.y -= ny * overlap * 0.5;
        b.x += nx * overlap * 0.5;
        b.y += ny * overlap * 0.5;

        // Intercambiar componente de velocidad en la dirección normal
        double dvx = b.vx - a.vx, dvy = b.vy - a.vy;
        double dot = dvx * nx + dvy * ny;
        if (dot < 0.0) {  // solo resolver si se aproximan
            a.vx += dot * nx;  a.vy += dot * ny;
            b.vx -= dot * nx;  b.vy -= dot * ny;
        }

        a.colliding = true;
        b.colliding = true;
    }
}
```

### `src/physics/Distributions.h`

```cpp
// src/physics/Distributions.h
// Propietario: Nicolas
// Los tres escenarios requeridos por la rúbrica, más firma descriptiva
#pragma once
#include "Particle.h"
#include <vector>
#include <string>

class Distributions {
public:
    // Distribución 1: partículas repartidas uniformemente por todo el mundo
    static std::vector<Particle> uniform(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed);

    // Distribución 2: partículas agrupadas alrededor de k centros aleatorios
    static std::vector<Particle> clusters(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed,
        int numClusters = 5);

    // Distribución 3: 70% en zona superior-izquierda, 30% disperso
    static std::vector<Particle> denseZone(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed);

    // Utilidad: nombre legible para el HUD
    static std::string name(int index) {
        switch(index) {
            case 0: return "Uniforme";
            case 1: return "Clusters";
            case 2: return "Zona Densa";
            default: return "?";
        }
    }
};
```

### `src/physics/Distributions.cpp`

```cpp
// src/physics/Distributions.cpp
// Propietario: Nicolas
#include "Distributions.h"
#include <random>
#include <cmath>
#include <algorithm>

// Semilla fija para reproducibilidad (importante para el reporte)
static std::mt19937 rng(42);

// ─────────────────────────────────────────────────────────────────────
// Distribución 1: UNIFORME
// Cada coordenada es independiente y uniformemente distribuida.
// ─────────────────────────────────────────────────────────────────────
std::vector<Particle> Distributions::uniform(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    std::uniform_real_distribution<double> distX(0.0, worldW);
    std::uniform_real_distribution<double> distY(0.0, worldH);
    std::uniform_real_distribution<double> distV(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> distR(minR, maxR);

    std::vector<Particle> ps(n);
    for (int i = 0; i < n; i++) {
        ps[i].id     = i;
        ps[i].x      = distX(rng);
        ps[i].y      = distY(rng);
        ps[i].vx     = distV(rng);
        ps[i].vy     = distV(rng);
        ps[i].radius = distR(rng);
    }
    return ps;
}

// ─────────────────────────────────────────────────────────────────────
// Distribución 2: CLUSTERS
// Centros aleatorios, partículas con ruido gaussiano alrededor.
// Evidencia el comportamiento del QuadTree con zonas de alta densidad local.
// ─────────────────────────────────────────────────────────────────────
std::vector<Particle> Distributions::clusters(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed,
        int numClusters) {
    std::uniform_real_distribution<double> distCX(50.0, worldW - 50.0);
    std::uniform_real_distribution<double> distCY(50.0, worldH - 50.0);
    std::normal_distribution<double>       spread(0.0, 70.0);
    std::uniform_real_distribution<double> distV(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> distR(minR, maxR);
    std::uniform_int_distribution<int>     distC(0, numClusters - 1);

    // Generar centros de los clusters
    std::vector<std::pair<double,double>> centers(numClusters);
    for (auto& c : centers) c = { distCX(rng), distCY(rng) };

    std::vector<Particle> ps(n);
    for (int i = 0; i < n; i++) {
        auto [ox, oy] = centers[distC(rng)];
        ps[i].id     = i;
        ps[i].x      = std::clamp(ox + spread(rng), 0.0, worldW);
        ps[i].y      = std::clamp(oy + spread(rng), 0.0, worldH);
        ps[i].vx     = distV(rng);
        ps[i].vy     = distV(rng);
        ps[i].radius = distR(rng);
    }
    return ps;
}

// ─────────────────────────────────────────────────────────────────────
// Distribución 3: ZONA DE ALTA DENSIDAD
// 70% en el cuadrante superior-izquierdo, 30% disperso.
// Útil para mostrar el caso degenerado del QuadTree.
// ─────────────────────────────────────────────────────────────────────
std::vector<Particle> Distributions::denseZone(
        int n, double worldW, double worldH,
        double minR, double maxR,
        double minSpeed, double maxSpeed) {
    int densePart  = (int)(n * 0.70);
    int sparsePart = n - densePart;

    std::uniform_real_distribution<double> denseX(0.0, worldW * 0.30);
    std::uniform_real_distribution<double> denseY(0.0, worldH * 0.30);
    std::uniform_real_distribution<double> sparseX(0.0, worldW);
    std::uniform_real_distribution<double> sparseY(0.0, worldH);
    std::uniform_real_distribution<double> distV(-maxSpeed, maxSpeed);
    std::uniform_real_distribution<double> distR(minR, maxR);

    std::vector<Particle> ps;
    ps.reserve(n);

    for (int i = 0; i < densePart; i++) {
        Particle p;
        p.id = i; p.x = denseX(rng); p.y = denseY(rng);
        p.vx = distV(rng); p.vy = distV(rng); p.radius = distR(rng);
        ps.push_back(p);
    }
    for (int i = densePart; i < n; i++) {
        Particle p;
        p.id = i; p.x = sparseX(rng); p.y = sparseY(rng);
        p.vx = distV(rng); p.vy = distV(rng); p.radius = distR(rng);
        ps.push_back(p);
    }
    return ps;
}
```

---

## 7. Código — Mikael: Lead de Sistemas de Renderizado

> Este es el módulo con todos los cambios de SFML 3.0.2. Leer con atención.
> Las diferencias respecto a SFML 2.6 están marcadas con comentarios `// SFML3`.

### `src/benchmark/Benchmark.h`

```cpp
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
    int         qtComparisons;  // comparaciones promedio QT
    int         bfComparisons;  // comparaciones BF = n*(n-1)/2
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
            float speedup = r.bfMs > 0 ? r.bfMs / r.qtQueryMs : 0.f;
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
            float sp = r.bfMs > 0 ? r.bfMs / r.qtQueryMs : 0.f;
            f << r.n << "," << r.distribution << ","
              << r.buildMs << "," << r.qtQueryMs << "," << r.bfMs << ","
              << sp << "," << r.qtComparisons << "," << r.bfComparisons << ","
              << r.candidatesPerParticle << "," << r.collisions << "\n";
        }
        std::cout << "[Benchmark] Exportado a " << path << "\n";
    }
};
```

### `src/rendering/Renderer.h`

```cpp
// src/rendering/Renderer.h
// Propietario: Mikael
#pragma once
#include <SFML/Graphics.hpp>
#include "../quadtree/Rectangle.h"
#include "../physics/Particle.h"
#include <vector>

class Renderer {
public:
    static void drawParticles   (sf::RenderTarget& target,
                                 const std::vector<Particle>& particles);
    static void drawQuadTree    (sf::RenderTarget& target,
                                 const std::vector<Rectangle>& bounds);
    static void drawQueryRect   (sf::RenderTarget& target,
                                 const Rectangle& range);
    static void drawQueryCircle (sf::RenderTarget& target,
                                 float cx, float cy, float r);
};
```

### `src/rendering/Renderer.cpp`

```cpp
// src/rendering/Renderer.cpp
// Propietario: Mikael
// SFML 3.0.2 — usar sf::Vector2f{x, y} para posición/tamaño/origen
#include "Renderer.h"

void Renderer::drawParticles(sf::RenderTarget& target,
                             const std::vector<Particle>& particles) {
    sf::CircleShape shape;
    for (const auto& p : particles) {
        float r = (float)p.radius;
        shape.setRadius(r);
        shape.setOrigin({r, r});                       // SFML3: Vector2f
        shape.setPosition({(float)p.x, (float)p.y});  // SFML3: Vector2f

        if (p.colliding)
            shape.setFillColor(sf::Color(255, 80, 80));    // rojo = colisión
        else if (p.highlighted)
            shape.setFillColor(sf::Color(80, 220, 80));    // verde = candidata
        else
            shape.setFillColor(sf::Color(140, 180, 255));  // azul = normal

        target.draw(shape);
    }
}

void Renderer::drawQuadTree(sf::RenderTarget& target,
                            const std::vector<Rectangle>& bounds) {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color(60, 80, 160, 130));
    rect.setOutlineThickness(1.0f);

    for (const auto& b : bounds) {
        rect.setSize({b.hw * 2.0f, b.hh * 2.0f});     // SFML3: Vector2f
        rect.setPosition({b.x - b.hw, b.y - b.hh});   // SFML3: Vector2f
        target.draw(rect);
    }
}

void Renderer::drawQueryRect(sf::RenderTarget& target,
                             const Rectangle& range) {
    sf::RectangleShape rect;
    rect.setSize({range.hw * 2.0f, range.hh * 2.0f});
    rect.setPosition({range.x - range.hw, range.y - range.hh});
    rect.setFillColor(sf::Color(255, 255, 0, 35));
    rect.setOutlineColor(sf::Color::Yellow);
    rect.setOutlineThickness(2.0f);
    target.draw(rect);
}

void Renderer::drawQueryCircle(sf::RenderTarget& target,
                               float cx, float cy, float r) {
    sf::CircleShape circle(r);
    circle.setOrigin({r, r});
    circle.setPosition({cx, cy});
    circle.setFillColor(sf::Color(100, 255, 100, 30));
    circle.setOutlineColor(sf::Color(100, 255, 100, 200));
    circle.setOutlineThickness(2.0f);
    target.draw(circle);
}
```

### `src/rendering/HUD.h`

```cpp
// src/rendering/HUD.h
// Propietario: Mikael
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    static void draw(sf::RenderTarget& target,
                     const sf::Font& font,
                     int fps, int n,
                     int qtComp, int bfComp,
                     float qtMs, float bfMs,
                     float candidatesPerParticle,
                     const std::string& distribution,
                     bool showGrid, bool runBF);
};
```

### `src/rendering/HUD.cpp`

```cpp
// src/rendering/HUD.cpp
// Propietario: Mikael
// SFML 3.0.2 — sf::Text requiere font en el constructor
#include "HUD.h"
#include <sstream>
#include <iomanip>

void HUD::draw(sf::RenderTarget& target,
               const sf::Font& font,
               int fps, int n,
               int qtComp, int bfComp,
               float qtMs, float bfMs,
               float candidatesPerParticle,
               const std::string& distribution,
               bool showGrid, bool runBF) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "FPS: " << fps << "\n"
       << "Particulas: " << n << "\n"
       << "Dist: " << distribution << "\n"
       << "Grid: " << (showGrid ? "ON" : "OFF")
       << "  BF: " << (runBF ? "ON" : "OFF") << "\n"
       << "\n"
       << "--- QuadTree ---\n"
       << "Comp.: " << qtComp << "\n"
       << "Cand/P: " << candidatesPerParticle << "\n"
       << "Tiempo: " << qtMs << " ms\n"
       << "\n"
       << "--- Fuerza Bruta ---\n"
       << "Comp.: " << bfComp << "\n"
       << "Tiempo: " << bfMs << " ms\n"
       << "\n"
       << "Speedup: ";
    if (runBF && qtMs > 0.001f)
        ss << (bfMs / qtMs) << "x";
    else
        ss << "N/A";
    ss << "\n\n"
       << "[1/2/3] Distribucion\n"
       << "[Q] Grid ON/OFF\n"
       << "[B] BruteForce ON/OFF\n"
       << "[+/-] Particulas\n"
       << "[C] Click query circulo\n"
       << "[R] Click query rect";

    // Fondo semitransparente
    sf::RectangleShape bg({270.f, 380.f});
    bg.setPosition({8.f, 8.f});
    bg.setFillColor(sf::Color(10, 10, 20, 185));
    target.draw(bg);

    // SFML 3.0.2: sf::Text(font, string, characterSize)
    sf::Text text(font, ss.str(), 13);
    text.setFillColor(sf::Color(220, 230, 255));
    text.setPosition({16.f, 16.f});
    target.draw(text);
}
```

### `src/main.cpp`

```cpp
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

// ────────────────────────────────────────────────────────────────────
// Modo benchmark sin ventana (para los experimentos del reporte)
// Uso: pasar "benchmark" como primer argumento de línea de comandos
// ────────────────────────────────────────────────────────────────────
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

            QuadTree qt(Rectangle(W/2, H/2, W/2, H/2), 4);
            float totalBuildMs = 0, totalQTms = 0, totalBFms = 0;
            int totalQTcomp = 0, totalBFcomp = 0;
            float totalCandidates = 0;
            int totalCollisions = 0;
            Timer t;

            for (int f = 0; f < FRAMES; f++) {
                PhysicsEngine::update(particles, 1.0/60.0, W, H);

                // ── Medir reconstrucción del QuadTree ──
                t.start();
                qt.clear();
                for (auto& p : particles) qt.insert(&p);
                totalBuildMs += t.elapsedMs();

                // ── Medir consultas con QuadTree ──
                int qtComp = 0;
                int candidates = 0;
                std::vector<std::pair<int,int>> qtCollisions;
                t.start();
                for (auto& p : particles) {
                    std::vector<Particle*> found;
                    float r = (float)(p.radius * 2.0);
                    Rectangle range((float)p.x, (float)p.y, r, r);
                    qt.query(range, found, qtComp);
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
                totalQTms   += t.elapsedMs();
                totalQTcomp += qtComp;
                totalCandidates += (float)candidates / (float)n;
                totalCollisions += (int)qtCollisions.size();
                PhysicsEngine::resolveCollisions(particles, qtCollisions);

                // ── Medir BruteForce ──
                std::vector<std::pair<int,int>> bfColls;
                int bfComp = 0;
                t.start();
                BruteForce::detectCollisions(particles, bfColls, bfComp);
                totalBFms   += t.elapsedMs();
                totalBFcomp += bfComp;
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

// ────────────────────────────────────────────────────────────────────
// Modo interactivo con ventana SFML
// ────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "benchmark") {
        runBenchmark();
        return 0;
    }

    const float WORLD_W = 1280.f, WORLD_H = 720.f;

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
    int  N          = 1000;
    int  distIndex  = 0;        // 0=uniforme, 1=clusters, 2=densa
    int  qtCapacity = 4;
    bool showGrid   = true;
    bool runBF      = true;
    bool queryMode  = false;    // false=rect, true=círculo

    auto buildParticles = [&]() -> std::vector<Particle> {
        if (distIndex == 0)
            return Distributions::uniform(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
        else if (distIndex == 1)
            return Distributions::clusters(N, WORLD_W, WORLD_H, 3, 8, 50, 150, 5);
        else
            return Distributions::denseZone(N, WORLD_W, WORLD_H, 3, 8, 50, 150);
    };

    std::vector<Particle> particles = buildParticles();
    QuadTree qt(Rectangle(WORLD_W/2, WORLD_H/2, WORLD_W/2, WORLD_H/2), qtCapacity);

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

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // ════════════════════════════════════════════════════════════
        // EVENTOS — SFML 3.0.2: pollEvent() retorna std::optional
        // ════════════════════════════════════════════════════════════
        while (const auto event = window.pollEvent()) {

            // Cerrar ventana
            if (event->is<sf::Event::Closed>())
                window.close();

            // ── Teclado ──────────────────────────────────────────────
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                switch (key->code) {
                    // Distribuciones
                    case sf::Keyboard::Key::Num1:
                        distIndex = 0; particles = buildParticles(); break;
                    case sf::Keyboard::Key::Num2:
                        distIndex = 1; particles = buildParticles(); break;
                    case sf::Keyboard::Key::Num3:
                        distIndex = 2; particles = buildParticles(); break;

                    // Toggles
                    case sf::Keyboard::Key::Q: showGrid  = !showGrid;  break;
                    case sf::Keyboard::Key::B: runBF     = !runBF;     break;
                    case sf::Keyboard::Key::C: queryMode = !queryMode; break;

                    // Tamaño de simulación
                    case sf::Keyboard::Key::Equal:
                        N = std::min(N + 500, 15000);
                        particles = buildParticles(); break;
                    case sf::Keyboard::Key::Hyphen:
                        N = std::max(N - 500, 100);
                        particles = buildParticles(); break;

                    // Limpiar query
                    case sf::Keyboard::Key::Escape:
                        hasQuery = false; break;

                    default: break;
                }
            }

            // ── Ratón: inicio del arrastre ───────────────────────────
            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    dragging  = true;
                    dragStart = {(float)mb->position.x, (float)mb->position.y};
                    // Limpiar highlights anteriores
                    for (auto& p : particles) p.highlighted = false;
                    hasQuery = false;
                }
            }

            // ── Ratón: fin del arrastre (realizar consulta) ──────────
            if (const auto* mb = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mb->button == sf::Mouse::Button::Left && dragging) {
                    dragging = false;
                    sf::Vector2f dragEnd = {(float)mb->position.x,
                                            (float)mb->position.y};

                    std::vector<Particle*> found;
                    int queryComp = 0;

                    if (!queryMode) {
                        // Modo rectangular
                        float cx = (dragStart.x + dragEnd.x) / 2.f;
                        float cy = (dragStart.y + dragEnd.y) / 2.f;
                        float hw = std::abs(dragEnd.x - dragStart.x) / 2.f;
                        float hh = std::abs(dragEnd.y - dragStart.y) / 2.f;
                        queryRect = Rectangle(cx, cy, std::max(hw, 1.f),
                                                       std::max(hh, 1.f));
                        qt.query(queryRect, found, queryComp);
                    } else {
                        // Modo circular (radio = distancia del drag)
                        float dx = dragEnd.x - dragStart.x;
                        float dy = dragEnd.y - dragStart.y;
                        queryCircleR = std::sqrt(dx*dx + dy*dy);
                        queryRect = Rectangle(dragStart.x, dragStart.y,
                                              queryCircleR, queryCircleR);
                        qt.queryCircle(dragStart.x, dragStart.y,
                                       queryCircleR, found, queryComp);
                    }

                    for (Particle* p : found) p->highlighted = true;
                    hasQuery = true;
                }
            }
        }

        // ════════════════════════════════════════════════════════════
        // FÍSICA
        // ════════════════════════════════════════════════════════════
        PhysicsEngine::update(particles, (double)dt, WORLD_W, WORLD_H);

        // ════════════════════════════════════════════════════════════
        // QUADTREE: reconstruir + detectar colisiones
        // ════════════════════════════════════════════════════════════
        qtTimer.start();
        qt.clear();
        for (auto& p : particles) qt.insert(&p);

        qtComp = 0;
        int totalCandidates = 0;
        std::vector<std::pair<int,int>> qtCollisions;

        for (auto& p : particles) {
            std::vector<Particle*> found;
            float r = (float)(p.radius * 2.0);
            Rectangle range((float)p.x, (float)p.y, r, r);
            qt.query(range, found, qtComp);
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

        // ════════════════════════════════════════════════════════════
        // FUERZA BRUTA (opcional — toggle [B])
        // ════════════════════════════════════════════════════════════
        if (runBF) {
            bfTimer.start();
            std::vector<std::pair<int,int>> bfColls;
            BruteForce::detectCollisions(particles, bfColls, bfComp);
            bfMs = bfTimer.elapsedMs();
        } else {
            bfComp = 0; bfMs = 0;
        }

        // ════════════════════════════════════════════════════════════
        // RENDER
        // ════════════════════════════════════════════════════════════
        window.clear(sf::Color(12, 12, 24));

        // 1. Grilla del QuadTree
        if (showGrid) {
            std::vector<Rectangle> bounds;
            qt.getBoundaries(bounds);
            Renderer::drawQuadTree(window, bounds);
        }

        // 2. Región de consulta activa
        if (hasQuery) {
            if (!queryMode)
                Renderer::drawQueryRect(window, queryRect);
            else
                Renderer::drawQueryCircle(window,
                    queryRect.x, queryRect.y, queryCircleR);
        }

        // 3. Partículas
        Renderer::drawParticles(window, particles);

        // 4. HUD
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
```

---

## 8. Archivos compartidos: CMakeLists.txt y vcpkg.json

> Estos archivos son responsabilidad de **Valentino** (como owner del repo).
> Los demás no los tocan. Si hay cambios de dependencias, abrir un issue.

### `vcpkg.json` — manifest de dependencias

```json
{
  "name": "quadtreesim",
  "version-string": "1.0.0",
  "dependencies": [
    {
      "name": "sfml",
      "version>=": "3.0.0"
    }
  ]
}
```

Colocar este archivo en la **raíz del repositorio** (junto a `CMakeLists.txt`).
vcpkg lo detecta automáticamente y resuelve las dependencias al hacer `cmake ..`.

### `CMakeLists.txt` — corregido para SFML 3.0.2

```cmake
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
```

### Comandos de build y ejecución

```bash
# Primera vez (o al cambiar CMakeLists.txt):
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build:
cmake --build . --config Release

# Ejecutar simulador:
.\Release\QuadTreeSim.exe

# Ejecutar benchmarks (genera benchmark_results.csv):
.\Release\QuadTreeSim.exe benchmark
```

---

*Guía maestra generada para CS2023 — Proyecto 2, Opción A*  
*Stack: C++17 · SFML 3.0.2 · CMake 3.20+ · vcpkg*  
*Roles: Valentino (Arquitecto de Estructuras Espaciales) · Nicolas (Ingeniero de Simulación Física) · Mikael (Lead de Sistemas de Renderizado)*
