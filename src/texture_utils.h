#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H

#include "raylib.h"
#include <cstdlib>   // abs()

// Compara dos colores con una tolerancia dada (0..255 por canal)
static inline bool ColorsNear(Color a, Color b, int tol) {
    return (abs((int)a.r - (int)b.r) <= tol &&
            abs((int)a.g - (int)b.g) <= tol &&
            abs((int)a.b - (int)b.b) <= tol);
}

// Carga una textura haciendo transparente el color de fondo.
// Detecta el color del pixel (0,0) como color de fondo y lo vuelve BLANK.
// tolerance: cuántos niveles de diferencia RGB se consideran "el mismo" color (0 = exacto).
// Usar tolerance=10 para manejar artefactos de compresión JPEG cerca del borde.
static inline Texture2D LoadTextureTransparent(const char* fileName, int tolerance = 15) {
    Image img = LoadImage(fileName);
    if (img.data == NULL) {
        Texture2D empty = {0};
        return empty;
    }

    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Color* pixels = (Color*)img.data;
    int total = img.width * img.height;
    
    for (int i = 0; i < total; i++) {
        Color c = pixels[i];
        if (c.a > 0) {
            // The fake checkerboard uses white (255) and light grays (215, 237, etc).
            // We remove any pixel that is bright (> 200) and mostly grayscale (R, G, B are similar).
            if (c.r > 200 && c.g > 200 && c.b > 200) {
                if (abs((int)c.r - (int)c.g) < 15 && abs((int)c.g - (int)c.b) < 15) {
                    pixels[i] = BLANK;
                }
            }
        }
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

#endif // TEXTURE_UTILS_H
