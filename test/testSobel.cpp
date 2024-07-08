#include <iostream>
#include <opencv2/opencv.hpp>

// Funzione per applicare il filtro di Sobel manualmente
void sobelFilter(int* input, int* output, int width, int height) {
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
            int magnitude = std::abs(gradient_x) + std::abs(gradient_y);

            // Assegnazione del valore risultante all'output
            output[i * width + j] = magnitude;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_image>" << std::endl;
        return -1;
    }

    // Carica l'immagine in input
    cv::Mat inputImage = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    if (inputImage.empty()) {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }

    int width = inputImage.cols;
    int height = inputImage.rows;

    // Allocazione di memoria per l'input e l'output
    int* input = new int[width * height];
    int* output = new int[width * height];

    // Copia dei pixel dell'immagine in input
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            input[i * width + j] = inputImage.at<uchar>(i, j);
        }
    }

    // Applica il filtro di Sobel manualmente
    sobelFilter(input, output, width, height);

    // Creazione dell'immagine di output
    cv::Mat outputImage(height, width, CV_8UC1);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            outputImage.at<uchar>(i, j) = output[i * width + j];
        }
    }

    // Visualizza l'immagine originale e l'immagine filtrata
    cv::imshow("Original Image", inputImage);
    cv::imshow("Sobel Filtered Image", outputImage);
    cv::waitKey(0);

    // Liberazione della memoria allocata
    delete[] input;
    delete[] output;

    return 0;
}
