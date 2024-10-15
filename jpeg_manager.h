#ifndef JPEG_MANAGER_H
#define JPEG_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>  // OpenCV para manejar imágenes

class jpeg_manager {
private:
    struct Encabezado {
        int ancho;
        int alto;
        int canales;
    };

    // Puntero inteligente para almacenar el encabezado
    std::shared_ptr<Encabezado> encabezado;
    
    // Puntero inteligente para la matriz de píxeles
    std::shared_ptr<std::vector<std::vector<std::vector<unsigned char>>>> matriz_pixeles;

public:
    jpeg_manager();
    ~jpeg_manager();

    // Lee un archivo JPEG y almacena su contenido
    bool leer_archivo(const std::string& nombre_archivo);

    // Obtiene la información del encabezado
    Encabezado ver_encabezado() const;

    // Obtiene la matriz de píxeles de la imagen
    std::vector<std::vector<std::vector<unsigned char>>> obtener_matriz_pixeles() const;

    // Crea un nuevo archivo JPEG a partir de una matriz de píxeles
    bool crear_archivo(const std::string& nombre_archivo, const std::vector<std::vector<std::vector<unsigned char>>>& nueva_matriz);

    // Convierte la matriz de píxeles a formato OpenCV (cv::Mat)
    cv::Mat convertir_matriz_a_opencv(const std::vector<std::vector<std::vector<unsigned char>>>& matriz) const;

    // Convierte una imagen OpenCV a matriz de píxeles
    void convertir_opencv_a_matriz(const cv::Mat& imagen, std::vector<std::vector<std::vector<unsigned char>>>& matriz);
};

#endif  // JPEG_MANAGER_H
