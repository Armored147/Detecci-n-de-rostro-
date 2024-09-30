#include <iostream>
#include <string>
#include <fstream>
#include "jpeg_manager.h"
#include "procesador_imagen.h"
#include <omp.h>
#include <opencv2/opencv.hpp>

// Muestra las instrucciones de uso del programa
void mostrar_uso(const char* nombre_programa) {
    std::cout << "Uso: " << nombre_programa << " <archivo_entrada> [archivo_salida] [opcion]" << std::endl;
    std::cout << "opcion:" << std::endl;
    std::cout << "  -p : Procesar imagen (invertir colores y detectar rostros)" << std::endl;
}

// Verifica si un archivo existe
bool archivo_existe(const std::string& nombre_archivo) {
    std::ifstream f(nombre_archivo.c_str());
    return f.good();
}

// Conversión de matriz a formato OpenCV (cv::Mat)
cv::Mat convertir_matriz_a_opencv(const std::vector<std::vector<std::vector<unsigned char>>>& matriz) {
    int alto = matriz.size();
    int ancho = matriz[0].size();
    cv::Mat imagen_opencv(alto, ancho, CV_8UC3);  // CV_8UC3 para una imagen de 3 canales (RGB)
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            imagen_opencv.at<cv::Vec3b>(y, x) = cv::Vec3b(matriz[y][x][2], matriz[y][x][1], matriz[y][x][0]);
        }
    }
    return imagen_opencv;
}

// Conversión de formato OpenCV (cv::Mat) a matriz de píxeles
void convertir_opencv_a_matriz(const cv::Mat& imagen_opencv, std::vector<std::vector<std::vector<unsigned char>>>& matriz) {
    int alto = imagen_opencv.rows;
    int ancho = imagen_opencv.cols;
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            cv::Vec3b color = imagen_opencv.at<cv::Vec3b>(y, x);
            matriz[y][x][0] = color[2];  // Rojo
            matriz[y][x][1] = color[1];  // Verde
            matriz[y][x][2] = color[0];  // Azul
        }
    }
}

int main(int argc, char* argv[]) {
    // 1. Verificar argumentos de línea de comandos
    if (argc < 3 || argc > 4) {
        mostrar_uso(argv[0]);
        return 1;
    }

    // 2. Obtener nombres de archivos de entrada, salida y opción
    std::string archivo_entrada = argv[1];
    std::string archivo_salida = argv[2];
    std::string opcion = (argc == 4) ? argv[3] : "-p";  // Por defecto, procesa imagen (invierte colores y detecta rostros)

    // 3. Verificar existencia del archivo de entrada
    if (!archivo_existe(archivo_entrada)) {
        std::cerr << "Error: El archivo de entrada '" << archivo_entrada << "' no existe." << std::endl;
        return 1;
    }

    jpeg_manager gestor;

    // 4. Leer archivo JPEG de entrada
    if (!gestor.leer_archivo(archivo_entrada)) {
        std::cerr << "Error al leer el archivo: " << archivo_entrada << std::endl;
        return 1;
    }

    std::cout << "Archivo " << archivo_entrada << " leído exitosamente." << std::endl;

    auto encabezado = gestor.ver_encabezado();
    std::cout << "Dimensiones de la imagen: " << encabezado.ancho << "x" << encabezado.alto << std::endl;

    auto matriz = gestor.obtener_matriz_pixeles();

    if (matriz.empty() || matriz[0].empty() || matriz[0][0].empty()) {
        std::cerr << "Error: La matriz de píxeles está vacía" << std::endl;
        return 1;
    }

    // Procesamiento según la opción
    try {
        if (opcion == "-p") {
            // 5. Procesar la imagen (invertir colores y detectar rostros)
            cv::Mat imagen = convertir_matriz_a_opencv(matriz);
            ProcesadorImagen procesador;
            procesador.procesar_imagen(imagen, archivo_salida);
            gestor.convertir_opencv_a_matriz(imagen, matriz);  // Actualizamos la matriz después de procesar la imagen
        } else {
            mostrar_uso(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << std::endl;
        return 1;
    }

    // 6. Crear nuevo archivo JPEG con la imagen procesada
    if (!gestor.crear_archivo(archivo_salida, matriz)) {
        std::cerr << "Error al crear el nuevo archivo: " << archivo_salida << std::endl;
        return 1;
    }

    std::cout << "Nuevo archivo creado exitosamente: " << archivo_salida << std::endl;
    return 0;
}
