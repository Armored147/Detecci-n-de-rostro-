# Procesador de Imágenes JPEG

## Descripción
Este proyecto implementa un procesador de imágenes JPEG en C++ utilizando la biblioteca libjpeg y OpenMP para paralelización. Permite leer archivos JPEG, procesar la imagen (actualmente invirtiendo los colores) y crear nuevos archivos JPEG con los cambios aplicados.

## Estructura del Proyecto
- `main.cpp`: Punto de entrada del programa.
- `jpeg_manager.h` y `jpeg_manager.cpp`: Manejo de archivos JPEG.
- `procesador_imagen.h` y `procesador_imagen.cpp`: Procesamiento de imágenes.
- `Makefile`: Reglas de compilación.
- `install_dependencies.sh`: Script de instalación de dependencias.

## Dependencias
- Compilador de C++ con soporte para C++17
- libjpeg
- OpenMP

## Instalación

### Dependencias
En sistemas basados en Debian/Ubuntu:
```bash
./install_dependencies.sh
```

Para otros sistemas, consulte la documentación de su distribución para instalar libjpeg y OpenMP.

### Compilación
```bash
make
```

## Uso
```bash
./programa_jpeg <archivo_entrada> [archivo_salida]
```
Si no se especifica un archivo de salida, se usará "imagen_procesada.jpg".

## Pseudocódigos

### main.cpp
```
Función principal:
1. Verificar argumentos de línea de comandos
2. Obtener nombres de archivos de entrada y salida
3. Verificar existencia del archivo de entrada
4. Crear instancia de jpeg_manager
5. Leer archivo JPEG de entrada
6. Mostrar información del encabezado de la imagen
7. Obtener matriz de píxeles
8. Procesar la imagen (invertir colores) usando ProcesadorImagen
9. Crear nuevo archivo JPEG con la imagen procesada
10. Mostrar mensaje de éxito
```

### jpeg_manager.cpp
```
Clase jpeg_manager:

Método leer_archivo:
1. Abrir el archivo JPEG
2. Inicializar el decodificador JPEG
3. Leer el encabezado del archivo
4. Iniciar la descompresión
5. Leer los datos de la imagen línea por línea
6. Almacenar los datos en la matriz_pixeles
7. Finalizar la descompresión y cerrar el archivo

Método crear_archivo:
1. Verificar la validez de la matriz de entrada
2. Abrir un nuevo archivo para escritura
3. Inicializar el codificador JPEG
4. Configurar los parámetros de compresión
5. Iniciar la compresión
6. Escribir los datos de la imagen línea por línea
7. Finalizar la compresión y cerrar el archivo

Método ver_encabezado:
1. Retornar la estructura de encabezado

Método obtener_matriz_pixeles:
1. Retornar la matriz de píxeles
```

### procesador_imagen.cpp
```
Clase ProcesadorImagen:

Método estático invertir_colores:
1. Paralelizar el procesamiento de la imagen:
   Para cada píxel en la matriz:
     Para cada canal de color:
       Invertir el valor del canal (255 - valor)
```

## Flujo de Ejecución
1. El programa inicia en `main.cpp`, gestionando la entrada del usuario.
2. `jpeg_manager` lee el archivo JPEG de entrada.
3. `ProcesadorImagen` procesa la matriz de píxeles.
4. `jpeg_manager` escribe la imagen procesada en un nuevo archivo JPEG.

## Paralelización
- Se utiliza OpenMP para paralelizar la lectura, escritura y procesamiento de la imagen.
- Los bucles en `jpeg_manager.cpp` y `procesador_imagen.cpp` utilizan directivas `#pragma omp parallel for`.

## Manejo de Errores
- Se implementan verificaciones de argumentos y existencia de archivos.
- Se utilizan estructuras try-catch para manejar excepciones durante la lectura/escritura de archivos.
- Los errores se reportan a través de la salida de error estándar (cerr).

## Limitaciones Actuales
- Solo soporta imágenes RGB (3 canales).
- La calidad de compresión para las imágenes de salida está fija en 90%.

## Extensibilidad
Para añadir nuevas funciones de procesamiento de imágenes:
1. Agregue un nuevo método en `ProcesadorImagen` en `procesador_imagen.h`.
2. Implemente el método en `procesador_imagen.cpp`.
3. Actualice `main.cpp` para utilizar la nueva función.
