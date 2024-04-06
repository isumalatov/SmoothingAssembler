#include <stdio.h>  // Para printf
#include <stdlib.h> // Para malloc y free
#include <time.h>   // Para time

#define WIDTH 10
#define HEIGHT 10
#define NUM_IMAGES 10

typedef struct
{
    int r;
    int g;
    int b;
} Pixel;

typedef Pixel** Image;

Image createImage(int width, int height)
{
    Image img = (Pixel**)malloc(height * sizeof(Pixel*));
    for (int i = 0; i < height; i++)
    {
        img[i] = (Pixel*)malloc(width * sizeof(Pixel));
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

void smoothImageA(int i, int j, int width, int height, int* sumR, int* sumG, int* sumB, int* count, Pixel** img)
{
    __asm
    {
        mov ecx, i //ecx tiene i
        mov edx, j //edx tiene j

        mov eax, -1 //eax tiene k
        loop_start:

        mov ebx, -1 //ebx tiene l
            loop_start2 :

            mov esi, ecx
            add esi, eax
            mov edi, edx
            add edi, ebx

            cmp esi, 0
            jl end_if

            cmp esi, height
            jge end_if

            cmp edi, 0
            jl end_if

            cmp edi, width
            jge end_if  //esi y edi tienen ni y nj

            //aqui va lo que hay dentro del if

            //aqui va lo de sumar sumr, sumg, sumb
            push eax
            mov eax, img
            mov eax, [eax + esi * 4] // obtener img[ni]
            mov eax, [eax + edi * 4] // obtener img[ni][nj]

            mov edi, sumR
            add[edi], eax // sumar img[ni][nj].r a sumR
            add eax, 4 // desplazarse 4 bytes para acceder a g
            mov edi, sumG
            add[edi], eax // sumar img[ni][nj].g a sumG
            add eax, 4 // desplazarse 4 bytes para acceder a b
            mov edi, sumB
            add[edi], eax // sumar img[ni][nj].b a sumB
            pop eax
            //aqui sumamos sumr, sumg, sumb

            //aqui sumamos count
            mov esi, count
            add dword ptr[esi], 1 // incrementa count en 1
            //aqui acaba el sumar count

            //aqui acaba el if
        end_if:
        inc ebx
            cmp ebx, 1
            jle loop_start2
            inc eax
            cmp eax, 1
            jle loop_start
    }
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