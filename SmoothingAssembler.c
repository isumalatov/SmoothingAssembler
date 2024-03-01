#include <stdlib.h> // Para malloc, free y rand
#include <time.h>   // Para time

typedef struct {
    int r;
    int g;
    int b;
} Pixel;

typedef Pixel** Image;

Image createImage(int width, int height) {
    Image img = (Pixel**)malloc(height * sizeof(Pixel*));
    for(int i = 0; i < height; i++) {
        img[i] = (Pixel*)malloc(width * sizeof(Pixel));
    }
    return img;
}

void fillImage(Image img, int width, int height) {
    srand(time(NULL)); // Inicializar la semilla del generador de números aleatorios
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            img[i][j].r = rand() % 256;
            img[i][j].g = rand() % 256;
            img[i][j].b = rand() % 256;
        }
    }
}

Image smoothImage(Image img, int width, int height) {
    Image newImg = createImage(width, height);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;

            for(int k = -1; k <= 1; k++) {
                for(int l = -1; l <= 1; l++) {
                    int ni = i + k;
                    int nj = j + l;

                    if(ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        sumR += img[ni][nj].r;
                        sumG += img[ni][nj].g;
                        sumB += img[ni][nj].b;
                        count++;
                    }
                }
            }

            newImg[i][j].r = sumR / count;
            newImg[i][j].g = sumG / count;
            newImg[i][j].b = sumB / count;
        }
    }

    return newImg;
}

