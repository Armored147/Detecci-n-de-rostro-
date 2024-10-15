#include "jpeg_manager.h"
#include <iostream>
#include <fstream>
#include <jpeglib.h>
#include <cstring>
#include <cerrno>
#include <memory>
#include <omp.h>
#include <opencv2/opencv.hpp>

// Constructor: Inicializamos el encabezado y la matriz de píxeles como punteros inteligentes
jpeg_manager::jpeg_manager() {
    encabezado = std::make_shared<Encabezado>();
    matriz_pixeles = std::make_shared<std::vector<std::vector<std::vector<unsigned char>>>>();
}

// Destructor: No es necesario liberar memoria manualmente porque std::shared_ptr lo maneja automáticamente
jpeg_manager::~jpeg_manager() {}

// Lee un archivo JPEG y almacena su contenido
bool jpeg_manager::leer_archivo(const std::string& nombre_archivo) {
    FILE* infile = fopen(nombre_archivo.c_str(), "rb");
    if (!infile) {
        std::cerr << "Error al abrir el archivo " << nombre_archivo << ": " << strerror(errno) << std::endl;
        return false;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);

    try {
        if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
            throw std::runtime_error("Error al leer el encabezado JPEG");
        }

        jpeg_start_decompress(&cinfo);

        // Almacenamos los datos del encabezado
        encabezado->ancho = cinfo.output_width;
        encabezado->alto = cinfo.output_height;
        encabezado->canales = cinfo.output_components;

        if (encabezado->canales != 3) {
            throw std::runtime_error("Solo se soportan imágenes RGB (3 canales)");
        }

        int row_stride = cinfo.output_width * cinfo.output_components;
        auto buffer = std::make_unique<JSAMPLE[]>(row_stride);  // Buffer temporal para cada fila de la imagen
        JSAMPROW row_pointer[1];
        row_pointer[0] = buffer.get();

        // Inicializamos la matriz de píxeles con el tamaño adecuado
        matriz_pixeles = std::make_shared<std::vector<std::vector<std::vector<unsigned char>>>>(
            encabezado->alto, 
            std::vector<std::vector<unsigned char>>(encabezado->ancho, std::vector<unsigned char>(encabezado->canales))
        );

        // Leemos las líneas de la imagen y las almacenamos en la matriz de píxeles
        while (cinfo.output_scanline < cinfo.output_height) {
            jpeg_read_scanlines(&cinfo, row_pointer, 1);
            int y = cinfo.output_scanline - 1;

            #pragma omp parallel for
            for (JDIMENSION x = 0; x < cinfo.output_width; ++x) {
                for (int c = 0; c < encabezado->canales; ++c) {
                    (*matriz_pixeles)[y][x][c] = buffer[x * encabezado->canales + c];
                }
            }
        }

        jpeg_finish_decompress(&cinfo);
    } catch (const std::exception& e) {
        std::cerr << "Error al procesar el archivo JPEG: " << e.what() << std::endl;
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return false;
    }

    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return true;
}

// Obtiene la información del encabezado de la imagen actual
jpeg_manager::Encabezado jpeg_manager::ver_encabezado() const {
    return *encabezado;  // Retornamos el valor del encabezado, desreferenciando el puntero inteligente
}

// Obtiene la matriz de píxeles de la imagen actual
std::vector<std::vector<std::vector<unsigned char>>> jpeg_manager::obtener_matriz_pixeles() const {
    return *matriz_pixeles;  // Retornamos la matriz de píxeles desreferenciada
}

// Crea un nuevo archivo JPEG a partir de una matriz de píxeles
bool jpeg_manager::crear_archivo(const std::string& nombre_archivo, const std::vector<std::vector<std::vector<unsigned char>>>& nueva_matriz) {
    if (nueva_matriz.empty() || nueva_matriz[0].empty() || nueva_matriz[0][0].empty()) {
        std::cerr << "Error: La matriz de píxeles está vacía" << std::endl;
        return false;
    }

    FILE* outfile = fopen(nombre_archivo.c_str(), "wb");
    if (!outfile) {
        std::cerr << "Error al crear el archivo " << nombre_archivo << ": " << strerror(errno) << std::endl;
        return false;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = nueva_matriz[0].size();
    cinfo.image_height = nueva_matriz.size();
    cinfo.input_components = nueva_matriz[0][0].size();
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    std::vector<unsigned char> buffer(cinfo.image_width * cinfo.input_components);
    JSAMPROW row_pointer[1];
    row_pointer[0] = buffer.data();

    while (cinfo.next_scanline < cinfo.image_height) {
        JDIMENSION y = cinfo.next_scanline;
        #pragma omp parallel for
        for (JDIMENSION x = 0; x < cinfo.image_width; ++x) {
            for (int c = 0; c < cinfo.input_components; ++c) {
                buffer[x * cinfo.input_components + c] = nueva_matriz[y][x][c];
            }
        }
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    return true;
}

// Convierte la matriz de píxeles a formato OpenCV (cv::Mat)
cv::Mat jpeg_manager::convertir_matriz_a_opencv(const std::vector<std::vector<std::vector<unsigned char>>>& matriz) const {
    int filas = matriz.size();
    int columnas = matriz[0].size();
    cv::Mat imagen(filas, columnas, CV_8UC3);  // CV_8UC3: 8-bit unsigned, 3 channels (RGB)

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            cv::Vec3b& color = imagen.at<cv::Vec3b>(i, j);
            color[0] = matriz[i][j][0];  // Canal azul
            color[1] = matriz[i][j][1];  // Canal verde
            color[2] = matriz[i][j][2];  // Canal rojo
        }
    }

    return imagen;
}

// Convierte una imagen OpenCV a matriz de píxeles
void jpeg_manager::convertir_opencv_a_matriz(const cv::Mat& imagen, std::vector<std::vector<std::vector<unsigned char>>>& matriz) {
    int filas = imagen.rows;
    int columnas = imagen.cols;
    matriz.resize(filas, std::vector<std::vector<unsigned char>>(columnas, std::vector<unsigned char>(3)));

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            const cv::Vec3b& color = imagen.at<cv::Vec3b>(i, j);
            matriz[i][j][0] = color[0];  // Canal azul
            matriz[i][j][1] = color[1];  // Canal verde
            matriz[i][j][2] = color[2];  // Canal rojo
        }
    }
}
