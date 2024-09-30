#!/bin/bash

# Función para instalar en sistemas basados en Debian
install_debian() {
    echo "Detectado sistema basado en Debian. Instalando dependencias..."
    sudo apt-get update
    sudo apt-get install -y g++ make libjpeg-dev
}

# Función para instalar en sistemas basados en Red Hat
install_redhat() {
    echo "Detectado sistema basado en Red Hat. Instalando dependencias..."
    sudo dnf update
    sudo dnf install -y gcc-c++ make libjpeg-devel
}

# Función para instalar en macOS
install_macos() {
    echo "Detectado sistema macOS. Instalando dependencias con Homebrew..."
    brew update
    brew install libjpeg
    brew install gcc  # Para OpenMP
}

# Detectar el sistema operativo
if [ "$(uname)" == "Darwin" ]; then
    # macOS
    install_macos
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Linux
    if [ -f /etc/debian_version ]; then
        install_debian
    elif [ -f /etc/redhat-release ]; then
        install_redhat
    else
        echo "Distribución de Linux no reconocida. Por favor, instala las dependencias manualmente."
        exit 1
    fi
else
    echo "Sistema operativo no soportado para instalación automática de dependencias."
    echo "Por favor, instala las dependencias manualmente según las instrucciones en el README."
    exit 1
fi

echo "Dependencias instaladas correctamente."