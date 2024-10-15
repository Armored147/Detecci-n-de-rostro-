CXX = g++
CXXFLAGS = -std=c++17 -Wall -fopenmp `pkg-config --cflags opencv4`
LDFLAGS = -ljpeg `pkg-config --libs opencv4`

SRCS = main.cpp jpeg_manager.cpp procesador_imagen.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = programa_jpeg

# Compilación de todo el proyecto
all: $(EXEC)

# Cómo se enlazan los objetos y se crea el binario final
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Comando para limpiar archivos compilados
clean:
	rm -f $(OBJS) $(EXEC)

# Comandos para ejecutar el programa
run-invertir:
	@echo "Ejecutando el programa con la opción de invertir colores..."
	./$(EXEC) imagen_ejemplo.jpg imagen_procesada.jpg -i  # Si en el main usas -i para invertir

run-detectar:
	@echo "Ejecutando el programa para detectar rostros..."
	./$(EXEC) imagen_ejemplo.jpg imagen_procesada.jpg -p  # Aquí corres el procesamiento de rostros con -p

# Comando para instalar dependencias
install-deps:
	@echo "Instalando dependencias del proyecto..."
	@chmod +x install_dependencies.sh
	@./install_dependencies.sh

.PHONY: all clean run-invertir run-detectar install-deps
