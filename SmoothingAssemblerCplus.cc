#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

struct Pixel
{
    int r, g, b;
};

using Image = std::vector<std::vector<Pixel>>;

Image createRandomImage(int width, int height)
{
    std::srand(std::time(nullptr));

    Image img(width, std::vector<Pixel>(height));
    for (auto &row : img)
    {
        for (auto &pixel : row)
        {
            pixel.r = std::rand() % 256;
            pixel.g = std::rand() % 256;
            pixel.b = std::rand() % 256;
        }
    }
    return img;
}

Image convertToGrayscale(const Image &img)
{
    Image grayscaleImg = img;
    for (auto &row : grayscaleImg)
    {
        for (auto &pixel : row)
        {
            int sum = pixel.r + pixel.g + pixel.b;
            if (sum != 0)
            {
                pixel.r = pixel.r * 100 / sum;
                pixel.g = pixel.g * 100 / sum;
                pixel.b = pixel.b * 100 / sum;
            }
            else
            {
                pixel.r = 0;
                pixel.g = 0;
                pixel.b = 0;
            }
        }
    }
    return grayscaleImg;
}

Image smoothImage(const Image &img)
{
    Image smoothedImg = img;
    int dx[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    for (int i = 1; i < img.size() - 1; ++i)
    {
        for (int j = 1; j < img[i].size() - 1; ++j)
        {
            int sumR = 0, sumG = 0, sumB = 0;
            for (int k = 0; k < 9; ++k)
            {
                int ni = i + dx[k];
                int nj = j + dy[k];
                if (ni >= 0 && ni < img.size() && nj >= 0 && nj < img[i].size())
                {
                    sumR += img[ni][nj].r;
                    sumG += img[ni][nj].g;
                    sumB += img[ni][nj].b;
                }
            }
            smoothedImg[i][j].r = sumR / 9;
            smoothedImg[i][j].g = sumG / 9;
            smoothedImg[i][j].b = sumB / 9;
        }
    }
    return smoothedImg;
}

void printImage(const Image &img)
{
    for (const auto &row : img)
    {
        for (const auto &pixel : row)
        {
            std::cout << "(" << pixel.r << ", " << pixel.g << ", " << pixel.b << ") ";
        }
        std::cout << '\n';
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Image> originalImages;  // Vector para almacenar las imágenes originales
    std::vector<Image> smoothedImages;  // Vector para almacenar las imágenes suavizadas
    std::vector<Image> grayscaleImages; // Vector para almacenar las imágenes en escala de grises

    // Crear imágenes
    for (int i = 0; i < 10; i++)
    {
        Image img = createRandomImage(20, 20);
        originalImages.push_back(img);
    }

    // Suavizar imágenes
    for (int i = 0; i < 10; i++)
    {
        Image smoothedImg = smoothImage(originalImages[i]);
        smoothedImages.push_back(smoothedImg);
    }

    // Convertir a escala de grises
    for (int i = 0; i < 10; i++)
    {
        Image grayscaleImg = convertToGrayscale(smoothedImages[i]);
        grayscaleImages.push_back(grayscaleImg);
    }

    // Imprimir imágenes
    for (int i = 0; i < 10; i++)
    {
        std::cout << "Imagen original:\n";
        printImage(originalImages[i]);

        std::cout << "\nImagen suavizada:\n";
        printImage(smoothedImages[i]);

        std::cout << "\nImagen en escala de grises:\n";
        printImage(grayscaleImages[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\nTiempo de ejecución: " << duration << " milisegundos.\n";

    return 0;
}
