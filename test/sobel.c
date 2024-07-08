#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Funzione per applicare il filtro di Sobel manualmente
void sobelFilter(unsigned char* input, unsigned char* output, int width, int height) {
    // Kernel di convoluzione per il filtro di Sobel in direzione x e y
    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Applicazione dei filtri di Sobel
    for (int i = 1; i < height - 1; ++i) {
        for (int j = 1; j < width - 1; ++j) {
            // Calcolo del gradiente in direzione x
            int gradient_x = 0;
            for (int k = -1; k <= 1; ++k) {
                for (int l = -1; l <= 1; ++l) {
                    gradient_x += input[(i + k) * width + (j + l)] * sobel_x[k + 1][l + 1];
                }
            }

            // Calcolo del gradiente in direzione y
            int gradient_y = 0;
            for (int k = -1; k <= 1; ++k) {
                for (int l = -1; l <= 1; ++l) {
                    gradient_y += input[(i + k) * width + (j + l)] * sobel_y[k + 1][l + 1];
                }
            }

            // Calcolo della magnitudo del gradiente
            int magnitude = abs(gradient_x) + abs(gradient_y);

            // Assegnazione del valore risultante all'output
            output[i * width + j] = (unsigned char)(magnitude > 255 ? 255 : magnitude);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path_to_image>\n", argv[0]);
        return -1;
    }

    // Apertura del file immagine
    FILE* file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open or find the image!\n");
        return -1;
    }

    // Lettura dell'header dell'immagine BMP (ignorando i primi 54 byte)
    fseek(file, 54, SEEK_SET);

    // Lettura della larghezza e altezza dell'immagine
    int width, height;
    fread(&width, sizeof(int), 1, file);
    fread(&height, sizeof(int), 1, file);

    // Allocazione di memoria per l'input e l'output
    unsigned char* input = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    unsigned char* output = (unsigned char*)malloc(width * height * sizeof(unsigned char));

    // Lettura dei pixel dell'immagine in input
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fread(&input[i * width + j], sizeof(unsigned char), 1, file);
        }
    }

    // Applicazione del filtro di Sobel manualmente
    sobelFilter(input, output, width, height);

    // Scrittura dell'immagine in output
    FILE* outFile = fopen("output.bmp", "wb");
    if (outFile == NULL) {
        fprintf(stderr, "Could not create output image file!\n");
        free(input);
        free(output);
        fclose(file);
        return -1;
    }

    // Scrittura dell'header dell'immagine BMP
    fseek(file, 0, SEEK_SET);
    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, file);
    fwrite(header, sizeof(unsigned char), 54, outFile);

    // Scrittura dei pixel dell'immagine in output
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fwrite(&output[i * width + j], sizeof(unsigned char), 1, outFile);
        }
    }

    // Chiusura dei file e liberazione della memoria allocata
    fclose(outFile);
    free(input);
    free(output);
    fclose(file);

    printf("Sobel filter applied successfully. Output saved as 'output.bmp'.\n");

    return 0;
}
