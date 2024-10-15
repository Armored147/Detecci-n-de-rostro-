#ifndef PROCESADOR_IMAGEN_H
#define PROCESADOR_IMAGEN_H

#include <vector>
#include <memory>  // Para usar std::shared_ptr
#include <opencv2/opencv.hpp>

class ProcesadorImagen {
public:
    // Procesa la imagen: invierte los colores y detecta los rostros
    void procesar_imagen(const std::shared_ptr<cv::Mat>& imagen, const std::string& ruta_imagen);

private:
    // Convierte la imagen a escala de grises en paralelo
    void convertir_a_grises(const std::shared_ptr<cv::Mat>& imagen_color, std::shared_ptr<cv::Mat>& imagen_gris);

    // Detecta los rostros en la imagen
    std::vector<cv::Rect> detectar_rostros(const std::shared_ptr<cv::Mat>& imagen_gris);

    // Marca y guarda los rostros detectados
    void dibujar_rectangulos(const std::shared_ptr<cv::Mat>& imagen, const std::vector<cv::Rect>& rostros);

    // Guarda cada rostro individual en una carpeta
    void guardar_rostros_individuales(const std::shared_ptr<cv::Mat>& imagen, const std::vector<cv::Rect>& rostros, const std::string& carpeta);
};

#endif // PROCESADOR_IMAGEN_H