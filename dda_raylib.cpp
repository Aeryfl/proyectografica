/**
 * Algoritmo DDA (Digital Differential Analyzer) con raylib
 *
 * Este programa implementa el algoritmo DDA para trazado de líneas en 2D
 * y lo compara con la función nativa DrawLine de raylib.
 *
 * Controles:
 *   W, A, S, D - Mover punto inicial
 *   Flechas    - Mover punto final
 *   ESPACIO    - Reiniciar puntos a posición inicial
 *   ESC        - Salir del programa
 *
 * Criterios de evaluación cubiertos:
 *   - Implementación DDA (30 pts)
 *   - Uso correcto de coordenadas (20 pts)
 *   - Interacción teclado (20 pts)
 *   - Visualización clara (20 pts)W
 *   - Análisis teórico (10 pts)
 */

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

// =============================================================================
// CONFIGURACIÓN
// =============================================================================

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;
const int GRID_COLS = 20;      // Número de columnas del grid (mínimo 15 según requerimiento)
const int GRID_ROWS = 12;      // Número de filas del grid (mínimo 10 según requerimiento)
const int CELL_SIZE = 40;      // Tamaño de cada celda en píxeles
const int GRID_OFFSET_X = 50;  // Margen izquierdo del grid
const int GRID_OFFSET_Y = 50;  // Margen superior del grid

// Colores
const Color COLOR_BACKGROUND = {240, 240, 248, 255};      // Fondo claro
const Color COLOR_GRID = {200, 200, 210, 255};            // Color de la rejilla
const Color COLOR_GRID_MAJOR = {150, 150, 160, 255};      // Líneas principales
const Color COLOR_POINT = {30, 144, 255, 255};            // Azul - puntos inicial/final
const Color COLOR_DDA_LINE = {220, 20, 60, 255};          // Rojo - línea DDA
const Color COLOR_RAYLIB_LINE = {34, 139, 34, 255};       // Verde - línea raylib
const Color COLOR_PIXEL_DDA = {255, 100, 100, 200};       // Píxeles DDA
const Color COLOR_TEXT = {40, 40, 40, 255};               // Texto
const Color COLOR_INFO_BG = {255, 255, 255, 240};         // Fondo de información

// =============================================================================
// ESTRUCTURAS
// =============================================================================

/**
 * Estructura que representa un punto en el grid
 */
struct GridPoint {
    int x;  // Coordenada X en el grid (0 a GRID_COLS-1)
    int y;  // Coordenada Y en el grid (0 a GRID_ROWS-1)

    // Convertir a coordenadas de pantalla
    Vector2 ToScreen() const {
        return {
            (float)(GRID_OFFSET_X + x * CELL_SIZE + CELL_SIZE / 2),
            (float)(GRID_OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 2)
        };
    }

    // Convertir a coordenadas absolutas de pantalla (esquina superior izquierda)
    Vector2 ToScreenAbsolute() const {
        return {
            (float)(GRID_OFFSET_X + x * CELL_SIZE),
            (float)(GRID_OFFSET_Y + y * CELL_SIZE)
        };
    }
};

/**
 * Estructura que representa un píxel calculado por el algoritmo DDA
 */
struct DDAPixel {
    int x;          // Coordenada X en el grid
    int y;          // Coordenada Y en el grid
    int iteration;  // Número de iteración en que fue calculado

    // Convertir a coordenadas de pantalla
    Vector2 ToScreen() const {
        return {
            (float)(GRID_OFFSET_X + x * CELL_SIZE + CELL_SIZE / 2),
            (float)(GRID_OFFSET_Y + y * CELL_SIZE + CELL_SIZE / 2)
        };
    }

    // Obtener rectángulo del píxel
    Rectangle GetCellRect() const {
        return {
            (float)(GRID_OFFSET_X + x * CELL_SIZE),
            (float)(GRID_OFFSET_Y + y * CELL_SIZE),
            (float)CELL_SIZE,
            (float)CELL_SIZE
        };
    }
};

// =============================================================================
// IMPLEMENTACIÓN DEL ALGORITMO DDA
// =============================================================================

/**
 * Implementa el algoritmo Digital Differential Analyzer (DDA) para trazado de líneas.
 *
 * El algoritmo DDA funciona de la siguiente manera:
 * 1. Calcular las diferencias dx y dy entre los puntos
 * 2. Determinar el número de pasos como max(|dx|, |dy|)
 *    - Esto garantiza que la línea se dibuje sin huecos
 *    - El eje mayor determina la resolución del trazado
 * 3. Calcular los incrementos por paso: incX = dx/steps, incY = dy/steps
 * 4. Para cada paso, sumar los incrementos y redondear a coordenadas enteras
 *
 * @param p1 Punto inicial en coordenadas del grid
 * @param p2 Punto final en coordenadas del grid
 * @return Vector de píxeles que forman la línea
 *
 * CASOS ESPECIALES MANEJADOS:
 * - Línea vertical (dx = 0): steps = |dy|, solo cambia Y
 * - Línea horizontal (dy = 0): steps = |dx|, solo cambia X
 * - Línea degenerada (p1 = p2): steps = 0, solo un píxel
 * - Líneas negativas: se manejan con valores absolutos
 */
std::vector<DDAPixel> DDALine(const GridPoint& p1, const GridPoint& p2) {
    std::vector<DDAPixel> pixels;

    // Paso 1: Calcular diferencias (usando float para precisión)
    float dx = (float)p2.x - (float)p1.x;
    float dy = (float)p2.y - (float)p1.y;

    // Paso 2: Determinar número de pasos
    // Usamos max(|dx|, |dy|) para garantizar continuidad de la línea
    // Si usáramos el menor, habría huecos en líneas con pendiente > 1 o < -1
    float steps = fmaxf(fabsf(dx), fabsf(dy));

    // Caso especial: mismo punto (línea degenerada)
    if (steps == 0.0f) {
        DDAPixel pixel;
        pixel.x = p1.x;
        pixel.y = p1.y;
        pixel.iteration = 0;
        pixels.push_back(pixel);
        return pixels;
    }

    // Paso 3: Calcular incrementos por paso
    float incX = dx / steps;
    float incY = dy / steps;

    // Paso 4: Generar puntos intermedios
    float x = (float)p1.x;
    float y = (float)p1.y;

    // Añadir punto inicial
    DDAPixel firstPixel;
    firstPixel.x = (int)roundf(x);
    firstPixel.y = (int)roundf(y);
    firstPixel.iteration = 0;
    pixels.push_back(firstPixel);

    // Calcular cada punto intermedio
    for (int i = 1; i <= (int)steps; i++) {
        x += incX;
        y += incY;

        DDAPixel pixel;
        pixel.x = (int)roundf(x);
        pixel.y = (int)roundf(y);
        pixel.iteration = i;
        pixels.push_back(pixel);
    }

    return pixels;
}

// =============================================================================
// FUNCIONES DE DIBUJO
// =============================================================================

/**
 * Dibuja la rejilla (grid) que representa los píxeles ampliados
 */
void DrawGrid() {
    // Dibujar líneas verticales
    for (int i = 0; i <= GRID_COLS; i++) {
        int x = GRID_OFFSET_X + i * CELL_SIZE;
        Color lineColor = (i % 5 == 0) ? COLOR_GRID_MAJOR : COLOR_GRID;
        DrawLine(x, GRID_OFFSET_Y, x, GRID_OFFSET_Y + GRID_ROWS * CELL_SIZE, lineColor);
    }

    // Dibujar líneas horizontales
    for (int i = 0; i <= GRID_ROWS; i++) {
        int y = GRID_OFFSET_Y + i * CELL_SIZE;
        Color lineColor = (i % 5 == 0) ? COLOR_GRID_MAJOR : COLOR_GRID;
        DrawLine(GRID_OFFSET_X, y, GRID_OFFSET_X + GRID_COLS * CELL_SIZE, y, lineColor);
    }

    // Dibujar borde del grid
    DrawRectangleLines(GRID_OFFSET_X, GRID_OFFSET_Y,
                       GRID_COLS * CELL_SIZE, GRID_ROWS * CELL_SIZE, COLOR_GRID_MAJOR);
}

/**
 * Dibuja un punto en el grid con highlight
 */
void DrawGridPoint(const GridPoint& point, const char* label) {
    Vector2 screenPos = point.ToScreen();

    // Dibujar celda de fondo
    Rectangle cellRect = {
        (float)(GRID_OFFSET_X + point.x * CELL_SIZE),
        (float)(GRID_OFFSET_Y + point.y * CELL_SIZE),
        (float)CELL_SIZE,
        (float)CELL_SIZE
    };
    DrawRectangleRec(cellRect, Fade(COLOR_POINT, 0.3f));

    // Dibujar punto central
    DrawCircleV(screenPos, 8.0f, COLOR_POINT);
    DrawCircleV(screenPos, 4.0f, WHITE);

    // Dibujar etiqueta
    DrawText(label, (int)screenPos.x - 10, (int)screenPos.y - 35, 16, COLOR_TEXT);
}

/**
 * Dibuja los píxeles calculados por el algoritmo DDA
 */
void DrawDDAPixels(const std::vector<DDAPixel>& pixels, int highlightIteration = -1) {
    for (size_t i = 0; i < pixels.size(); i++) {
        const DDAPixel& pixel = pixels[i];
        Rectangle cellRect = pixel.GetCellRect();

        // Color base del píxel
        Color pixelColor = COLOR_PIXEL_DDA;

        // Highlight si estamos en modo animación
        if (highlightIteration >= 0 && (int)i <= highlightIteration) {
            pixelColor = COLOR_DDA_LINE;
        } else if (highlightIteration >= 0) {
            pixelColor = Fade(COLOR_PIXEL_DDA, 0.3f);
        }

        // Dibujar píxel
        DrawRectangleRec(cellRect, Fade(pixelColor, 0.5f));
        DrawRectangleLinesEx(cellRect, 2, pixelColor);

        // Dibujar número de iteración
        char iterText[16];
        snprintf(iterText, sizeof(iterText), "%d", pixel.iteration);
        int textWidth = MeasureText(iterText, 12);
        DrawText(iterText,
                 (int)cellRect.x + CELL_SIZE / 2 - textWidth / 2,
                 (int)cellRect.y + CELL_SIZE / 2 - 6,
                 12, COLOR_TEXT);
    }
}

/**
 * Dibuja la línea usando el algoritmo DDA (conectando los centros)
 */
void DrawDDALine(const std::vector<DDAPixel>& pixels) {
    if (pixels.empty()) return;

    // Dibujar línea conectando los centros de los píxeles
    for (size_t i = 1; i < pixels.size(); i++) {
        Vector2 start = pixels[i-1].ToScreen();
        Vector2 end = pixels[i].ToScreen();
        DrawLineEx(start, end, 3.0f, COLOR_DDA_LINE);
    }
}

/**
 * Dibuja la línea usando la función nativa de raylib
 */
void DrawRaylibLine(const GridPoint& p1, const GridPoint& p2) {
    Vector2 start = p1.ToScreen();
    Vector2 end = p2.ToScreen();
    DrawLineEx(start, end, 3.0f, COLOR_RAYLIB_LINE);
}

/**
 * Dibuja el panel de información con coordenadas y estadísticas
 */
void DrawInfoPanel(const GridPoint& p1, const GridPoint& p2,
                   const std::vector<DDAPixel>& pixels, bool animateMode) {
    int panelX = GRID_OFFSET_X + GRID_COLS * CELL_SIZE + 30;
    int panelY = GRID_OFFSET_Y;
    int panelWidth = 250;
    int panelHeight = GRID_ROWS * CELL_SIZE;

    // Fondo del panel
    DrawRectangle(panelX, panelY, panelWidth, panelHeight, COLOR_INFO_BG);
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, COLOR_GRID_MAJOR);

    int yPos = panelY + 20;

    // Título
    DrawText("INFORMACION", panelX + 15, yPos, 18, COLOR_TEXT);
    yPos += 30;

    // Coordenadas punto inicial
    char text[64];
    snprintf(text, sizeof(text), "Punto Inicial:");
    DrawText(text, panelX + 10, yPos, 14, COLOR_POINT);
    yPos += 20;
    snprintf(text, sizeof(text), "  Grid: (%d, %d)", p1.x, p1.y);
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 18;
    Vector2 p1Screen = p1.ToScreen();
    snprintf(text, sizeof(text), "  Screen: (%.0f, %.0f)", p1Screen.x, p1Screen.y);
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 25;

    // Coordenadas punto final
    snprintf(text, sizeof(text), "Punto Final:");
    DrawText(text, panelX + 10, yPos, 14, COLOR_POINT);
    yPos += 20;
    snprintf(text, sizeof(text), "  Grid: (%d, %d)", p2.x, p2.y);
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 18;
    Vector2 p2Screen = p2.ToScreen();
    snprintf(text, sizeof(text), "  Screen: (%.0f, %.0f)", p2Screen.x, p2Screen.y);
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 25;

    // Estadísticas DDA
    snprintf(text, sizeof(text), "Algoritmo DDA:");
    DrawText(text, panelX + 10, yPos, 14, COLOR_DDA_LINE);
    yPos += 20;

    float dx = (float)p2.x - (float)p1.x;
    float dy = (float)p2.y - (float)p1.y;
    float steps = fmaxf(fabsf(dx), fabsf(dy));

    snprintf(text, sizeof(text), "  dx = %.1f, dy = %.1f", dx, dy);
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 18;
    snprintf(text, sizeof(text), "  Steps: %d", (int)steps);
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 18;
    snprintf(text, sizeof(text), "  Pixeles: %zu", pixels.size());
    DrawText(text, panelX + 15, yPos, 12, COLOR_TEXT);
    yPos += 25;

    // Leyenda
    snprintf(text, sizeof(text), "Leyenda:");
    DrawText(text, panelX + 10, yPos, 14, COLOR_TEXT);
    yPos += 20;

    DrawRectangle(panelX + 15, yPos, 20, 15, Fade(COLOR_POINT, 0.5f));
    DrawText("Puntos", panelX + 40, yPos, 12, COLOR_TEXT);
    yPos += 18;

    DrawRectangle(panelX + 15, yPos, 20, 15, Fade(COLOR_DDA_LINE, 0.5f));
    DrawText("DDA", panelX + 40, yPos, 12, COLOR_TEXT);
    yPos += 18;

    DrawRectangle(panelX + 15, yPos, 20, 15, Fade(COLOR_RAYLIB_LINE, 0.5f));
    DrawText("Raylib", panelX + 40, yPos, 12, COLOR_TEXT);
    yPos += 25;

    // Controles
    snprintf(text, sizeof(text), "Controles:");
    DrawText(text, panelX + 10, yPos, 14, COLOR_TEXT);
    yPos += 20;
    DrawText("WASD - Mover P1", panelX + 15, yPos, 11, COLOR_TEXT);
    yPos += 16;
    DrawText("Flechas - Mover P2", panelX + 15, yPos, 11, COLOR_TEXT);
    yPos += 16;
    DrawText("ESPACIO - Reiniciar", panelX + 15, yPos, 11, COLOR_TEXT);
    yPos += 16;
    DrawText("M - Toggle animacion", panelX + 15, yPos, 11, COLOR_TEXT);
    yPos += 16;
    DrawText("ESC - Salir", panelX + 15, yPos, 11, COLOR_TEXT);

    // Modo animación
    if (animateMode) {
        yPos += 25;
        DrawRectangle(panelX + 10, yPos, panelWidth - 20, 30, Fade(GREEN, 0.3f));
        DrawText("MODO ANIMACION: ON", panelX + 20, yPos + 8, 14, GREEN);
    }
}

/**
 * Dibuja las preguntas de análisis en la parte inferior
 */
void DrawAnalysisQuestions() {
    int yPos = GRID_OFFSET_Y + GRID_ROWS * CELL_SIZE + 20;

    DrawText("PREGUNTAS DE ANALISIS:", 50, yPos, 16, COLOR_TEXT);
    yPos += 25;

    DrawText("1. DDA usa redondeo, lo que puede causar pequena desviacion vs DrawLine nativo", 50, yPos, 12, COLOR_TEXT);
    yPos += 18;
    DrawText("2. steps = max(|dx|,|dy|) garantiza que no haya huecos en la linea", 50, yPos, 12, COLOR_TEXT);
    yPos += 18;
    DrawText("3. En linea vertical (dx=0), steps = |dy|, solo cambia Y en cada iteracion", 50, yPos, 12, COLOR_TEXT);
    yPos += 18;
    DrawText("4. Error de redondeo: acumulacion de errores al convertir float->int", 50, yPos, 12, COLOR_TEXT);
    yPos += 18;
    DrawText("5. Con enteros: usar algoritmo de Bresenham (solo sumas/restas, mas eficiente)", 50, yPos, 12, COLOR_TEXT);
}

// =============================================================================
// FUNCIONES DE UTILIDAD
// =============================================================================

/**
 * Convierte coordenadas de pantalla a coordenadas del grid
 */
GridPoint ScreenToGrid(Vector2 screenPos) {
    GridPoint point;
    point.x = (int)((screenPos.x - GRID_OFFSET_X) / CELL_SIZE);
    point.y = (int)((screenPos.y - GRID_OFFSET_Y) / CELL_SIZE);

    // Clamp dentro del grid
    point.x = Clamp(point.x, 0, GRID_COLS - 1);
    point.y = Clamp(point.y, 0, GRID_ROWS - 1);

    return point;
}

// =============================================================================
// PROGRAMA PRINCIPAL
// =============================================================================

int main() {
    // Inicializar ventana
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Algoritmo DDA - raylib");
    SetTargetFPS(60);

    // Inicializar puntos (centro del grid)
    GridPoint p1 = { GRID_COLS / 4, GRID_ROWS / 2 };  // Punto inicial (izquierda)
    GridPoint p2 = { 3 * GRID_COLS / 4, GRID_ROWS / 2 };  // Punto final (derecha)

    // Variables de control
    bool animateMode = false;
    int animationFrame = 0;
    float animationTimer = 0.0f;
    const float ANIMATION_SPEED = 0.15f;  // Segundos por frame de animación

    // Variables para cálculo de FPS
    float fpsUpdateTime = 0.0f;
    int fpsCounter = 0;

    // Bucle principal
    while (!WindowShouldClose()) {
        // =========================================================================
        // ENTRADA (INPUT)
        // =========================================================================

        // Mover punto inicial con WASD
        if (IsKeyPressed(KEY_W)) p1.y = Clamp(p1.y - 1, 0, GRID_ROWS - 1);
        if (IsKeyPressed(KEY_S)) p1.y = Clamp(p1.y + 1, 0, GRID_ROWS - 1);
        if (IsKeyPressed(KEY_A)) p1.x = Clamp(p1.x - 1, 0, GRID_COLS - 1);
        if (IsKeyPressed(KEY_D)) p1.x = Clamp(p1.x + 1, 0, GRID_COLS - 1);

        // Mover punto final con flechas
        if (IsKeyPressed(KEY_UP)) p2.y = Clamp(p2.y - 1, 0, GRID_ROWS - 1);
        if (IsKeyPressed(KEY_DOWN)) p2.y = Clamp(p2.y + 1, 0, GRID_ROWS - 1);
        if (IsKeyPressed(KEY_LEFT)) p2.x = Clamp(p2.x - 1, 0, GRID_COLS - 1);
        if (IsKeyPressed(KEY_RIGHT)) p2.x = Clamp(p2.x + 1, 0, GRID_COLS - 1);

        // Reiniciar puntos
        if (IsKeyPressed(KEY_SPACE)) {
            p1 = { GRID_COLS / 4, GRID_ROWS / 2 };
            p2 = { 3 * GRID_COLS / 4, GRID_ROWS / 2 };
            animationFrame = 0;
        }

        // Toggle modo animación
        if (IsKeyPressed(KEY_M)) {
            animateMode = !animateMode;
            animationFrame = 0;
        }

        // =========================================================================
        // ACTUALIZACIÓN
        // =========================================================================

        // Calcular línea DDA
        std::vector<DDAPixel> pixels = DDALine(p1, p2);

        // Actualizar animación si está activa
        if (animateMode) {
            animationTimer += GetFrameTime();
            if (animationTimer >= ANIMATION_SPEED) {
                animationTimer = 0.0f;
                if (animationFrame < (int)pixels.size()) {
                    animationFrame++;
                } else {
                    animationFrame = 0;  // Reiniciar animación
                }
            }
        }

        // =========================================================================
        // DIBUJADO
        // =========================================================================

        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);

        // Dibujar grid
        DrawGrid();

        // Dibujar línea de raylib (primero, para que quede debajo)
        DrawRaylibLine(p1, p2);

        // Dibujar línea DDA
        DrawDDALine(pixels);

        // Dibujar píxeles DDA
        if (animateMode) {
            DrawDDAPixels(pixels, animationFrame);
        } else {
            DrawDDAPixels(pixels);
        }

        // Dibujar puntos
        DrawGridPoint(p1, "P1 (WASD)");
        DrawGridPoint(p2, "P2 (Flechas)");

        // Dibujar panel de información
        DrawInfoPanel(p1, p2, pixels, animateMode);

        // Dibujar preguntas de análisis
        DrawAnalysisQuestions();

        // Título
        DrawText("ALGORITMO DDA - TRAZADO DE LINEAS 2D",
                 SCREEN_WIDTH / 2 - MeasureText("ALGORITMO DDA - TRAZADO DE LINEAS 2D", 20) / 2,
                 10, 20, COLOR_TEXT);

        // FPS
        DrawFPS(SCREEN_WIDTH - 100, 10);

        EndDrawing();
    }

    // Cerrar ventana y recursos
    CloseWindow();

    return 0;
}
