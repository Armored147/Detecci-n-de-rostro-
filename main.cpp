#include <iostream>
#include <string>
#include <fstream>
#include "jpeg_manager.h"
#include "procesador_imagen.h"
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <memory>  // Para usar std::shared_ptr

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

    // Crear una instancia del gestor de JPEG usando punteros inteligentes
    std::shared_ptr<jpeg_manager> gestor = std::make_shared<jpeg_manager>();

    // 4. Leer archivo JPEG de entrada
    if (!gestor->leer_archivo(archivo_entrada)) {
        std::cerr << "Error al leer el archivo: " << archivo_entrada << std::endl;
        return 1;
    }

    std::cout << "Archivo " << archivo_entrada << " leído exitosamente." << std::endl;

    auto encabezado = gestor->ver_encabezado();
    std::cout << "Dimensiones de la imagen: " << encabezado.ancho << "x" << encabezado.alto << std::endl;

    auto matriz = gestor->obtener_matriz_pixeles();

    if (matriz.empty() || matriz[0].empty() || matriz[0][0].empty()) {
        std::cerr << "Error: La matriz de píxeles está vacía" << std::endl;
        return 1;
    }

    // Procesamiento según la opción
    try {
        if (opcion == "-p") {
            // 5. Procesar la imagen (invertir colores y detectar rostros)
            std::shared_ptr<cv::Mat> imagen = std::make_shared<cv::Mat>(gestor->convertir_matriz_a_opencv(matriz));
            std::shared_ptr<ProcesadorImagen> procesador = std::make_shared<ProcesadorImagen>();
            procesador->procesar_imagen(imagen, archivo_salida);
            gestor->convertir_opencv_a_matriz(*imagen, matriz);  // Actualizamos la matriz después de procesar la imagen
        } else {
            mostrar_uso(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << std::endl;
        return 1;
    }

}
