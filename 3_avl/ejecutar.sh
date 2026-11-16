#!/usr/bin/env bash
# Compila (si hace falta) y abre la aplicacion del AVL.
# Uso: ./ejecutar.sh              -> menú interactivo
#      ./ejecutar.sh --reporte N  -> los 3 escenarios con N libros, sin menú
set -e
cd "$(dirname "$0")"

if ! command -v g++ >/dev/null 2>&1; then
    echo "No se encontró g++. Instálalo (Fedora: sudo dnf install gcc-c++ | Ubuntu: sudo apt install g++)."
    exit 1
fi

if [ ! -x app_avl ] || [ -n "$(find main.cpp ../comun -newer app_avl -print -quit)" ]; then
    echo "Compilando..."
    g++ -std=c++17 -O2 -Wall -o app_avl main.cpp
fi

./app_avl "$@"
