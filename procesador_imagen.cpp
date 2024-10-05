#include "procesador_imagen.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>  // Para manejar la creación de carpetas

void ProcesadorImagen::procesar_imagen(cv::Mat& imagen, const std::string& ruta_imagen) {

    // Paso 1: Convertir la imagen escala de grises para la detección de rostros
    cv::Mat imagen_gris(imagen.size(), CV_8UC1);
    convertir_a_grises(imagen, imagen_gris);

    // Paso 2: Detectar los rostros en la imagen invertida
    std::vector<cv::Rect> rostros = detectar_rostros(imagen_gris);

    if (rostros.empty()) {
        std::cout << "No se detectaron rostros." << std::endl;
        return;
    }

    // Crear una carpeta con el nombre de la imagen para almacenar los rostros
    std::string carpeta = "resultados" + ruta_imagen;
    std::filesystem::create_directory(carpeta);

    // Paso 3: Dibujar los rectángulos en la imagen invertida y guardar la imagen procesada
    dibujar_rectangulos(imagen, rostros);

    // Paso 4: Extraer los rostros de la imagen invertida y guardarlos en la carpeta
    guardar_rostros_individuales(imagen, rostros, carpeta);
}

//IMPLEMENTAR PARALELISMO
void ProcesadorImagen::convertir_a_grises(cv::Mat& imagen_color, cv::Mat& imagen_gris) {
    // Convertimos cada pixel a escala de grises.
    for (int i = 0; i < imagen_color.rows; i++) {
        for (int j = 0; j < imagen_color.cols; j++) {
            cv::Vec3b color = imagen_color.at<cv::Vec3b>(i, j);
            uchar gris = 0.299 * color[2] + 0.587 * color[1] + 0.114 * color[0];
            imagen_gris.at<uchar>(i, j) = gris;
        }
    }
}

std::vector<cv::Rect> ProcesadorImagen::detectar_rostros(cv::Mat& imagen_gris) {
    cv::CascadeClassifier detector_rostros;
    if (!detector_rostros.load("data/haarcascade_frontalface_default.xml")) {
        std::cerr << "Error cargando el archivo haarcascade_frontalface_default.xml" << std::endl;
        return {};
    }

    std::vector<cv::Rect> rostros;
    detector_rostros.detectMultiScale(imagen_gris, rostros, 1.1, 3, 0, cv::Size(30, 30));

    return rostros;
}

//IMPLEMENTAR PARALELISMO
void ProcesadorImagen::dibujar_rectangulos(cv::Mat& imagen, const std::vector<cv::Rect>& rostros) {
    for (size_t i = 0; i < rostros.size(); i++) {
        cv::rectangle(imagen, rostros[i], cv::Scalar(0, 255, 0), 2);  // Dibuja un rectángulo verde alrededor del rostro
    }

    // Guarda la imagen con los rostros marcados
    std::string imagen_marcada = "imagen_marcada.jpg";
    cv::imwrite(imagen_marcada, imagen);  // Asegurarnos de que se guarde en BGR
}

//IMPLEMENTAR PARALELISMO
void ProcesadorImagen::guardar_rostros_individuales(cv::Mat& imagen, const std::vector<cv::Rect>& rostros, const std::string& carpeta) {
    for (size_t i = 0; i < rostros.size(); i++) {
        // Extrae cada rostro detectado de la imagen invertida
        cv::Mat rostro = imagen(rostros[i]);

        // Guarda cada rostro en la carpeta
        std::string nombre_archivo = carpeta + "/rostro_" + std::to_string(i) + ".jpg";
        cv::imwrite(nombre_archivo, rostro);  // Guardar el rostro individual
    }
}
