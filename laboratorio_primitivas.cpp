/**
 * Laboratorio: Primitivas
 * Docente: MSc. Ing. Richard Henry Sivila Rios
 *
 * Objetivo:
 *   Aplicar los algoritmos de dibujo de lineas, en el dibujo de figuras.
 *
 * Ejercicio Propuesto:
 *   Haciendo uso de los Algoritmos DDA y Bresenham, se implementan dos metodos
 *   respectivamente (dda y bres) y se dibujan las siguientes figuras:
 *     1. Pentagono  - color RGB (255, 255, 0)   - Algoritmo DDA
 *     2. Cruz/Plus  - color RGB (204, 0,  153)  - Algoritmo Bresenham
 *     3. Estrella   - color RGB (0,  51,  204)  - Algoritmo DDA
 *
 *   Todas las figuras estan dentro de la misma pantalla.
 *   glOrtho(-700, 700, -540, 540, -1, 1);
 *
 * Compilar (con freeglut instalado):
 *   g++ laboratorio_primitivas.cpp -o primitivas -lfreeglut -lopengl32 -lglu32
 */

#ifdef _WIN32
  #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>

// ============================================================
// COLORES (normalizados a 0.0 - 1.0 para OpenGL)
// ============================================================

// RGB (255, 255, 0)  -> Amarillo  -> Pentagono
#define R1 (255.0f/255.0f)
#define G1 (255.0f/255.0f)
#define B1 (0.0f  /255.0f)

// RGB (204, 0, 153)  -> Magenta   -> Cruz
#define R2 (204.0f/255.0f)
#define G2 (0.0f  /255.0f)
#define B2 (153.0f/255.0f)

// RGB (0, 51, 204)   -> Azul      -> Estrella
#define R3 (0.0f  /255.0f)
#define G3 (51.0f /255.0f)
#define B3 (204.0f/255.0f)

// ============================================================
// ALGORITMO DDA
// ============================================================
/**
 * dda - Traza una linea desde (x1,y1) hasta (x2,y2)
 *       usando el algoritmo Digital Differential Analyzer.
 *
 * El algoritmo calcula los incrementos (incX, incY) dividiendo
 * la diferencia entre el numero de pasos = max(|dx|, |dy|).
 * En cada paso suma los incrementos y coloca un punto (glVertex2i).
 */
void dda(int x1, int y1, int x2, int y2) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);

    // Numero de pasos = eje con mayor variacion
    float steps = (fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy);

    if (steps == 0.0f) {
        // Punto degenerado
        glBegin(GL_POINTS);
            glVertex2i(x1, y1);
        glEnd();
        return;
    }

    float incX = dx / steps;
    float incY = dy / steps;

    float x = (float)x1;
    float y = (float)y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= (int)steps; i++) {
        glVertex2i((int)roundf(x), (int)roundf(y));
        x += incX;
        y += incY;
    }
    glEnd();
}

// ============================================================
// ALGORITMO BRESENHAM
// ============================================================
/**
 * bres - Traza una linea desde (x1,y1) hasta (x2,y2)
 *        usando el algoritmo de Bresenham.
 *
 * Trabaja unicamente con enteros (sumas y restas).
 * Maneja los 8 octantes reflejando/intercambiando segun sea necesario.
 */
void bres(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;   // Signo del incremento en X
    int sy = (y1 < y2) ? 1 : -1;   // Signo del incremento en Y

    int err = dx - dy;              // Error inicial

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);

        // Llegamos al punto final
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1  += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1  += sy;
        }
    }
    glEnd();
}

// ============================================================
// FIGURA 1: PENTAGONO - Algoritmo DDA - Color Amarillo
// ============================================================
/**
 * Pentagono regular centrado en (-350, 100).
 * Radio = 180 unidades.
 * Se trazan los 5 lados conectando los 5 vertices.
 * Cada lado se dibuja con DDA.
 */
void dibujarPentagono() {
    glColor3f(R1, G1, B1);  // Amarillo

    // Centro y radio del pentagono
    float cx = -350.0f;
    float cy =  100.0f;
    float r  =  180.0f;

    // Calcular los 5 vertices
    // Angulo inicial -90 grados para que la punta quede arriba
    int vx[5], vy[5];
    for (int i = 0; i < 5; i++) {
        float angulo = -90.0f + (360.0f / 5.0f) * i;
        float rad    = angulo * 3.14159265f / 180.0f;
        vx[i] = (int)roundf(cx + r * cosf(rad));
        vy[i] = (int)roundf(cy + r * sinf(rad));
    }

    // Dibujar los 5 lados con DDA
    glPointSize(2.0f);
    for (int i = 0; i < 5; i++) {
        int j = (i + 1) % 5;
        dda(vx[i], vy[i], vx[j], vy[j]);
    }
}

// ============================================================
// FIGURA 2: CRUZ - Algoritmo Bresenham - Color Magenta
// ============================================================
/**
 * Cruz (simbolo +) centrada en (50, 100).
 * Se compone de 12 vertices que forman un perfil de cruz.
 * Cada segmento se dibuja con Bresenham.
 *
 *   Dimensiones:
 *     Ancho total del brazo: 100 unidades
 *     Largo total: 320 unidades
 */
void dibujarCruz() {
    glColor3f(R2, G2, B2);  // Magenta

    // Centro
    int cx = 50;
    int cy = 100;

    // Mitad del brazo ancho  (a)
    // Mitad del brazo largo  (b)
    int a =  80;  // mitad ancho del brazo
    int b = 160;  // mitad largo del brazo

    // Los 12 vertices de la cruz, en sentido horario desde arriba-izquierda
    int vx[12] = { cx-a, cx+a, cx+a, cx+b, cx+b, cx+a,
                   cx+a, cx-a, cx-a, cx-b, cx-b, cx-a };
    int vy[12] = { cy+b, cy+b, cy+a, cy+a, cy-a, cy-a,
                   cy-b, cy-b, cy-a, cy-a, cy+a, cy+a };

    glPointSize(2.0f);
    for (int i = 0; i < 12; i++) {
        int j = (i + 1) % 12;
        bres(vx[i], vy[i], vx[j], vy[j]);
    }
}

// ============================================================
// FIGURA 3: ESTRELLA DE 5 PUNTAS - Algoritmo DDA - Color Azul
// ============================================================
/**
 * Estrella de 5 puntas centrada en (400, 100).
 * Se genera con radio exterior (puntas) y radio interior (valles).
 * Los 10 vertices alternan entre la punta y el valle.
 * Se trazan los 10 lados de la estrella con DDA.
 */
void dibujarEstrella() {
    glColor3f(R3, G3, B3);  // Azul

    float cx = 400.0f;
    float cy = 100.0f;
    float rOuter = 175.0f;  // Radio exterior (puntas)
    float rInner =  75.0f;  // Radio interior (valles)

    // 10 vertices: 5 puntas + 5 valles alternados
    int vx[10], vy[10];
    for (int i = 0; i < 10; i++) {
        // Para el pentagono exterior, comenzamos con -90 grados (punta arriba)
        // Los vertices de punta estan a 0, 72, 144, 216, 288 grados
        // Los vertices interiores estan a 36, 108, 180, 252, 324 grados
        float angulo = -90.0f + (360.0f / 10.0f) * i;
        float rad    = angulo * 3.14159265f / 180.0f;
        float r      = (i % 2 == 0) ? rOuter : rInner;
        vx[i] = (int)roundf(cx + r * cosf(rad));
        vy[i] = (int)roundf(cy + r * sinf(rad));
    }

    glPointSize(2.0f);
    for (int i = 0; i < 10; i++) {
        int j = (i + 1) % 10;
        dda(vx[i], vy[i], vx[j], vy[j]);
    }
}

// ============================================================
// FUNCION DE DISPLAY
// ============================================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Fondo oscuro

    // --- Figura 1: Pentagono (DDA, amarillo) ---
    dibujarPentagono();

    // --- Figura 2: Cruz (Bresenham, magenta) ---
    dibujarCruz();

    // --- Figura 3: Estrella (DDA, azul) ---
    dibujarEstrella();

    glFlush();
}

// ============================================================
// FUNCION DE RESHAPE
// ============================================================

void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Coordenadas ortograficas segun el enunciado
    glOrtho(-700, 700, -540, 540, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ============================================================
// FUNCION PRINCIPAL
// ============================================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Ventana 1400x1080 para aprovechar el espacio de glOrtho
    glutInitWindowSize(1400, 1080);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("Laboratorio: Primitivas - DDA y Bresenham");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}
