# Guardianes de Tarija 🌿

Un mini-juego 2D desarrollado en **C++ con Raylib** que combina mecánicas de **plataformero** con segmentos de **novela gráfica con toma de decisiones**.

## 🎮 Sobre el juego

El juego gira en torno a la polémica real de la **Reserva Natural de Tariquía** en Bolivia, donde el gobierno firmó contratos con empresas petroleras para perforar pozos de gas dentro de la reserva. El jugador debe decidir entre proteger la naturaleza o apoyar el desarrollo económico — y esa decisión cambia completamente el juego.

### Personajes
- **Don Ciriaco** – Guardián local de los valles tarijeños. Defiende la reserva y la naturaleza.
- **Rodrigo Paz** – Gobernador de Tarija. Argumenta que Bolivia necesita el gas para progresar.

### Estructura
| Segmento | Descripción |
|---|---|
| Menú Principal | Vista panorámica de la ciudad de Tarija |
| Novela Gráfica (Intro) | Debate entre Don Ciriaco y Rodrigo Paz |
| **Camino ECO** 🌿 | Bosque de Tariquía → enemigos: blobs de contaminación (pisotazo los elimina) |
| **Camino ECONÓMICO** 💰 | Zona de Perforación → enemigos: Drill-Golems (criaturas de maquinaria/petróleo) |
| Novela Gráfica (Medio) | Consecuencias de tu primera decisión |
| Nivel 2 | Más difícil, ambientación más dramática |
| Final | Buen final o Mal final según tus decisiones |

## 🕹️ Controles

| Acción | Tecla |
|---|---|
| Moverse | `←→` / `A D` |
| Saltar | `↑` / `W` / `Espacio` |
| **Disparar** *(camino económico)* | `Z` / `Ctrl Izquierdo` |
| Avanzar diálogo | `Enter` / `Espacio` |
| Elegir opción | `↑↓` + `Enter` |

> **Nota:** En el camino ECO, los blobs se eliminan **pisándolos** (1 salto encima).
> En el camino ECONÓMICO, los Drill-Golems necesitan **2 pisotones** para morir — o un solo **disparo** (`Z`).
> ¡Si pisas un Golem a medias, queda aturdido brevemente!

## 🛠️ Compilación

### Requisitos
- [Raylib](https://www.raylib.com/) instalado y accesible por el compilador
- `g++` (MinGW o similar con soporte C++17)

### Compilar
```bash
g++ src/*.cpp -o mi_juego.exe -O2 -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Con CMake
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

## 📁 Estructura del proyecto

```
eleoele/
├── src/
│   ├── main.cpp           # Punto de entrada
│   ├── game.h / game.cpp  # Máquina de estados principal
│   ├── player.h / player.cpp  # Jugador + sistema de balas
│   ├── level.h / level.cpp    # Niveles, enemigos, colisiones
│   ├── enemy.h                # Tipos de enemigos
│   └── visual_novel.h / visual_novel.cpp  # Motor de novela gráfica
├── assets/
│   ├── bg_*.png           # Fondos (menú, diálogos, niveles)
│   ├── player.png         # Sprite del jugador
│   ├── enemy_blob.png       # Enemigo: blob de contaminación (camino eco)
│   ├── enemy_drillgolem.png # Enemigo: Drill-Golem de petróleo (camino económico)
│   ├── npc_guide.png      # Retrato de Don Ciriaco
│   ├── rodrigo_paz.png    # Retrato de Rodrigo Paz
│   ├── platform.png       # Tile de plataformas
│   ├── collectibles.png   # Sprite sheet de coleccionables
│   ├── obstacles.png      # Sprite sheet de obstáculos
│   └── bullet.png         # Sprite de bala
└── CMakeLists.txt
```

## 🎨 Assets

Todos los fondos y sprites fueron generados con IA y editados para el proyecto. Los retratos de los personajes son pixel art estilizado.

## 📚 Contexto

Proyecto final desarrollado en el contexto de la materia de **Computación Gráfica**. Implementa primitivas gráficas, manejo de texturas, máquina de estados y renderizado 2D mediante Raylib.
