#include <stdio.h>  // Para printf
#include <stdlib.h> // Para malloc y free
#include <time.h>   // Para time

#define WIDTH 10
#define HEIGHT 10
#define NUM_IMAGES 20

typedef struct
{
    int r;
    int g;
    int b;
} Pixel;

typedef Pixel **Image;

Image createImage(int width, int height)
{
    Image img = (Pixel **)malloc(height * sizeof(Pixel *));
    for (int i = 0; i < height; i++)
    {
        img[i] = (Pixel *)malloc(width * sizeof(Pixel));
    }
    return img;
}

void fillImage(Image img, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            img[i][j].r = rand() % 256;
            img[i][j].g = rand() % 256;
            img[i][j].b = rand() % 256;
        }
    }
}

void smoothImageA(int i, int j, int width, int height, int *sumR, int *sumG, int *sumB, int *count, Pixel **img)
{
    asm(
        "push bp\n"
        "mov bp, sp\n"
        "push si\n"
        "push di\n"

        "mov dword ptr [bp + 20], 0\n"
        "mov dword ptr [bp + 24], 0\n"
        "mov dword ptr [bp + 28], 0\n"
        "mov dword ptr [bp + 32], 0\n"

        "mov cx, 3\n"
        "label1:\n"
            "mov bx, 3\n"
            "label2:\n"
                "lea si, [bp + 8 + cx - 2]\n"
                "lea di, [bp + 12 + bx - 2]\n"

                "cmp si, 0\n"
                "jl endif\n"
                "cmp si, [bp + 16]\n"
                "jge endif\n"
                "cmp di, 0\n"
                "jl endif\n"
                "cmp di, [bp + 20]\n"
                "jge endif\n"

                "add [bp + 24], [bp + 36 + si*PIXEL_SIZE + di*PIXEL_SIZE + R_OFFSET]\n"
                "add [bp + 28], [bp + 36 + si*PIXEL_SIZE + di*PIXEL_SIZE + G_OFFSET]\n"
                "add [bp + 32], [bp + 36 + si*PIXEL_SIZE + di*PIXEL_SIZE + B_OFFSET]\n"
                "inc [bp + 32]\n"

                "endif:\n"
                "dec bx\n"
                "jnz label2\n"
            "dec cx\n"
            "jnz label1\n"

        "pop di\n"
        "pop si\n"
        "pop bp\n"
    );
}

Image smoothImage(Image img, int width, int height)
{
    Image newImg = createImage(width, height);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;

            smoothImageA(i, j, width, height, &sumR, &sumG, &sumB, &count, img);

            newImg[i][j].r = sumR / count;
            newImg[i][j].g = sumG / count;
            newImg[i][j].b = sumB / count;
        }
    }

    return newImg;
}

void displayImage(Image img, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("(%d, %d, %d) ", img[i][j].r, img[i][j].g, img[i][j].b);
        }
        printf("\n");
    }
}

void freeImage(Image img, int height)
{
    for (int i = 0; i < height; i++)
    {
        free(img[i]);
    }
    free(img);
}

int main()
{
    srand(time(NULL)); // Initialize the seed for the random number generator

    Image originalImages[NUM_IMAGES];
    Image smoothedImages[NUM_IMAGES];

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        originalImages[i] = createImage(WIDTH, HEIGHT);
        fillImage(originalImages[i], WIDTH, HEIGHT);
    }

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        printf("\nOriginal image %d:\n", i + 1);
        displayImage(originalImages[i], WIDTH, HEIGHT);
    }

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        smoothedImages[i] = smoothImage(originalImages[i], WIDTH, HEIGHT);
    }

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        printf("\nSmoothed image %d:\n", i + 1);
        displayImage(smoothedImages[i], WIDTH, HEIGHT);
    }

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        freeImage(originalImages[i], HEIGHT);
        freeImage(smoothedImages[i], HEIGHT);
    }

    return 0;
}