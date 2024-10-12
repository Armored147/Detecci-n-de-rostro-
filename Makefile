CXX = g++
CXXFLAGS = -std=c++17 -Wall -fopenmp `pkg-config --cflags opencv4`
LDFLAGS = -ljpeg `pkg-config --libs opencv4`

SRCS = main.cpp jpeg_manager.cpp procesador_imagen.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = programa_jpeg

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run-invertir:
	./$(EXEC) imagen_ejemplo.jpg imagen_procesada.jpg -p

run-detectar:
	./$(EXEC) imagen_ejemplo.jpg imagen_procesada.jpg -p

install-deps:
	@echo "Instalando dependencias..."
	@chmod +x install_dependencies.sh
	@./install_dependencies.sh

.PHONY: all clean run-invertir run-detectar install-deps
