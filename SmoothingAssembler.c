#include <stdio.h>  // Para printf
#include <stdlib.h> // Para malloc y free
#include <time.h>   // Para time
#include <iostream>

#define WIDTH 1280
#define HEIGHT 720
#define NUM_IMAGES 20

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

void x86smoothImageA(int i, int j, int width, int height, int* sumR, int* sumG, int* sumB, int* count, Pixel** img)
{
    __asm
    {
        mov eax, -1 //eax tiene k
        jmp ln4

        ln2:
            add eax, 1

        ln4:
            cmp eax, 1
            jg ln1
            mov ebx, -1 //ebx tiene l
            jmp ln7

        ln5:
            add ebx, 1

        ln7:
            cmp ebx, 1
            jg ln2

            mov esi, i // i
            add esi, eax // ni
        
            mov edi, j // j
            add edi, ebx // nj

            cmp esi, 0 // ni == 0?
            jl ln5

            cmp esi, height // ni and height
            jge ln5

            cmp edi, 0 // nj == 0?
            jl ln5

            cmp edi, width // nj and width
            jge ln5

            push eax

            imul eax, edi, 12
            mov edx, img
            mov ecx, DWORD PTR[edx + esi * 4]
            mov edx, sumR
            mov edx, [edx]
            add edx, [ecx+eax]
            mov eax, sumR
            mov DWORD PTR[eax], edx
        
            imul ecx, edi, 12
            mov eax, img
            mov edx, [eax+esi*4]
            mov eax, sumG
            mov eax, [eax]
            add eax, [edx+ecx+4]
            mov ecx, sumG
            mov DWORD PTR[ecx], eax

            imul edx, edi, 12
            mov ecx, img
            mov eax, [ecx + esi * 4]
            mov ecx, sumB
            mov ecx, [ecx]
            add ecx, [eax + edx + 8]
            mov edx, sumB
            mov DWORD PTR[edx], ecx

            mov eax, count
            mov ecx, [eax]
            add ecx, 1
            mov edx, count
            mov [edx], ecx

            pop eax

            jmp ln5

        ln1:
    }
}

void CsmoothImageA(int i, int j, int width, int height, int* sumR, int* sumG, int* sumB, int* count, Pixel** img)
{
    for (int k = -1; k <= 1; k++)
    {
        for (int l = -1; l <= 1; l++)
        {
            int ni = i + k;
            int nj = j + l;

            if (ni >= 0 && ni < height && nj >= 0 && nj < width)
            {
                *sumR += img[ni][nj].r;
                *sumG += img[ni][nj].g;
                *sumB += img[ni][nj].b;
                (*count)++;
            }
        }
    }
}

void SSEsmoothImageA(int i, int j, int width, int height, int* sumR, int* sumG, int* sumB, int* count, Pixel** img)
{
    int color[3];
    int result[4] = { *sumR, *sumG, *sumB}; // one extra for memory issues
    __asm
    {
        mov eax, -1 //eax tiene k
        jmp ln4

        ln2 :
        add eax, 1

            ln4 :
            cmp eax, 1
            jg ln1
            mov ebx, -1 //ebx tiene l
            jmp ln7

            ln5 :
        add ebx, 1

            ln7 :
            cmp ebx, 1
            jg ln2

            mov esi, i // i
            add esi, eax // ni

            mov edi, j // j
            add edi, ebx // nj

            cmp esi, 0 // ni == 0?
            jl ln5

            cmp esi, height // ni and height
            jge ln5

            cmp edi, 0 // nj == 0?
            jl ln5

            cmp edi, width // nj and width
            jge ln5

            push eax

            imul    eax, edi, 12
            mov edx, img
            mov ecx, DWORD PTR[edx + esi * 4]
            mov ecx, [ecx + eax]
            mov dword ptr[color], ecx

            imul ecx, edi, 12
            mov eax, img
            mov edx, [eax + esi * 4]
            mov edx, [edx + ecx + 4]
            mov dword ptr[color+4], edx

            imul edx, edi, 12
            mov ecx, img
            mov eax, [ecx + esi * 4]
            mov eax, [eax + edx + 8]
            mov dword ptr[color + 8], eax

            movdqu xmm0, result
            movdqu xmm1, color
            paddq xmm0, xmm1
            movdqu result, xmm0

            mov eax, count
            mov ecx, [eax]
            add ecx, 1
            mov edx, count
            mov[edx], ecx

            pop eax

            jmp ln5

            ln1:
            mov eax, dword ptr[result]
            mov ebx, sumR
            mov[ebx], eax

            mov eax, dword ptr[result+4]
            mov ebx, sumG
            mov[ebx], eax

            mov eax, dword ptr[result+8]
            mov ebx, sumB
            mov[ebx], eax
    }
}

Image smoothImage(Image img, int width, int height, char indicator)
{
    Image newImg = createImage(width, height);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;
            if (indicator == 'x') x86smoothImageA(i, j, width, height, &sumR, &sumG, &sumB, &count, img);
            else if (indicator == 'c') CsmoothImageA(i, j, width, height, &sumR, &sumG, &sumB, &count, img);
            else if (indicator == 's') SSEsmoothImageA(i, j, width, height, &sumR, &sumG, &sumB, &count, img);

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

    printf("-----------------------------------------\n");
    printf("IMAGE SMOOTHING ALGORITHM\n");
    srand(time(NULL)); // Initialize the seed for the random number generator


    Image originalImages[NUM_IMAGES];
    Image smoothedImages[NUM_IMAGES];

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        originalImages[i] = createImage(WIDTH, HEIGHT);
        fillImage(originalImages[i], WIDTH, HEIGHT);
        //displayImage(originalImages[i], WIDTH, HEIGHT);
    }

    clock_t timeC_start = clock();
    for (int i = 0; i < NUM_IMAGES; i++)
    {
        smoothedImages[i] = smoothImage(originalImages[i], WIDTH, HEIGHT, 'c');
    }
    clock_t timeC_end = clock();
    double time_spentC = ((double)(timeC_end - timeC_start)) / CLOCKS_PER_SEC;
    printf("C: %f\n", time_spentC);
    for (int i = 0;i < NUM_IMAGES;i++)
    {
        //displayImage(smoothedImages[i], WIDTH, HEIGHT);
        freeImage(smoothedImages[i], HEIGHT);
    }

    clock_t timeX_start = clock();
    for (int i = 0; i < NUM_IMAGES; i++)
    {
        smoothedImages[i] = smoothImage(originalImages[i], WIDTH, HEIGHT, 'x');
    }
    clock_t timeX_end = clock();
    double time_spentX = ((double)(timeX_end - timeX_start)) / CLOCKS_PER_SEC;
    printf("x86: %f\n", time_spentX);
    for (int i = 0;i < NUM_IMAGES;i++)
    {
        //displayImage(smoothedImages[i], WIDTH, HEIGHT);
        freeImage(smoothedImages[i], HEIGHT);
    }

    size_t timeS_start = clock();
    for (int i = 0; i < NUM_IMAGES; i++)
    {
        smoothedImages[i] = smoothImage(originalImages[i], WIDTH, HEIGHT, 's');
    }
    clock_t timeS_end = clock();
    double time_spentS = ((double)(timeS_end - timeS_start)) / CLOCKS_PER_SEC;
    printf("SSE: %f\n", time_spentS);
    for (int i = 0;i < NUM_IMAGES;i++)
    {
        //displayImage(smoothedImages[i], WIDTH, HEIGHT);
        freeImage(smoothedImages[i], HEIGHT);
    }

    printf("-----------------------------------------\n");

    return 0;
}