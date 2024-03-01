#include <stdio.h> // Para printf
#include <stdlib.h> // Para malloc y free
#include <time.h>   // Para time

#define WIDTH 10
#define HEIGHT 10
#define NUM_IMAGES 20

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

void printImage(Image img, int width, int height) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            printf("(%d, %d, %d) ", img[i][j].r, img[i][j].g, img[i][j].b);
        }
        printf("\n");
    }
}

void freeImage(Image img, int height) {
    for(int i = 0; i < height; i++) {
        free(img[i]);
    }
    free(img);
}

int main() {
    srand(time(NULL)); // Initialize the seed for the random number generator

    Image originalImages[NUM_IMAGES];
    Image smoothedImages[NUM_IMAGES];

    for(int i = 0; i < NUM_IMAGES; i++) {
        originalImages[i] = createImage(WIDTH, HEIGHT);
        fillImage(originalImages[i], WIDTH, HEIGHT);
    }

    for(int i = 0; i < NUM_IMAGES; i++) {
        printf("\nOriginal image %d:\n", i + 1);
        printImage(originalImages[i], WIDTH, HEIGHT);
    }

    for(int i = 0; i < NUM_IMAGES; i++) {
        smoothedImages[i] = smoothImage(originalImages[i], WIDTH, HEIGHT);
    }

    for(int i = 0; i < NUM_IMAGES; i++) {
        printf("\nSmoothed image %d:\n", i + 1);
        printImage(smoothedImages[i], WIDTH, HEIGHT);
    }

    for(int i = 0; i < NUM_IMAGES; i++) {
        freeImage(originalImages[i], HEIGHT);
        freeImage(smoothedImages[i], HEIGHT);
    }

    return 0;
}