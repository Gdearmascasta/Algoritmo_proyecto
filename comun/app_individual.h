#pragma once
// Menú común de las tres aplicaciones individuales (2_bst, 3_avl, 4_hash).
// Cada aplicación pasa su estructura y la descripción de su paradigma; el menú,
// las mediciones y el catálogo son los mismos para que los números sean comparables.

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "consola.h"
#include "libro.h"
#include "medicion.h"

using namespace std;

struct InfoApp {
    const char* nombre;      // "Arbol Binario de Busqueda (BST)"
    const char* archivo;     // Archivo de comun/ con la estructura
    const char* paradigma;   // Una línea: nombre del paradigma
    Escenario peor;          // Escenario que la vuelve O(n^2), o NINGUNO
    const char* teoria;      // Texto de la opción "Teoria"
};

void imprimir_resultado(const InfoApp& info, int n, Escenario escenario, const Resultado& r,
                        int repeticiones) {
    printf("\n=== %s | N = %d | escenario %s\n", info.nombre, n, NOMBRE_ESCENARIO[escenario]);
    printf("    (promedio de %d repeticion(es), %d busquedas de cada tipo)\n\n", repeticiones,
           n < BUSQUEDAS ? n : BUSQUEDAS);
    if (r.omitido) {
        printf("  Omitido: es O(n^2) en este escenario y con N > %d tardaria demasiado.\n",
               LIMITE_CUADRATICO);
        return;
    }
    printf("  TIEMPO\n");
    printf("    Insertar los N libros:     %12.3f ms\n", r.ms_construir);
    printf("    Buscar (existe):           %12.1f ns por busqueda  (%.1f nodos revisados)\n",
           r.ns_buscar_ok, r.comp_ok);
    printf("    Buscar (no existe):        %12.1f ns por busqueda  (%.1f nodos revisados)\n",
           r.ns_buscar_fallo, r.comp_fallo);
    printf("    Listar ordenado:           %12.3f ms\n", r.ms_listar);
    printf("    Forma:                     %s\n", r.forma.c_str());
    printf("  MEMORIA (bytes pedidos al heap; incluye los libros guardados)\n");
    printf("    Estructura construida:     %12.1f KB  (%.1f bytes por libro)\n", r.kb_estructura,
           r.bytes_por_libro);
    printf("    Pico mientras insertaba:   %12.1f KB\n", r.kb_pico_insertar);
    printf("    Extra para listar:         %12.1f KB\n", r.kb_extra_listar);
    printf("    Nota: se excluye el arreglo de salida comun (N punteros: 8N bytes en 64 bits).\n");
    printf("    No incluye pila recursiva ni memoria total del proceso.\n");
    if (r.error) printf("\n  [ERROR: la estructura devolvio resultados incorrectos]\n");
}

template <class Estructura>
void medir_y_mostrar(const InfoApp& info, int n, Escenario escenario, int repeticiones) {
    Datos datos = generar_datos(n, escenario);
    Resultado r = medir_si_cabe<Estructura>(datos, escenario, info.peor, repeticiones);
    imprimir_resultado(info, n, escenario, r, repeticiones);
}

template <class Estructura>
void opcion_medir(const InfoApp& info) {
    int n = leer_entero("\nCantidad de libros N (100 - 1000000): ", 100, 1000000);
    int escenario = pedir_escenario();
    int repeticiones = leer_entero("Repeticiones para promediar (1 - 10): ", 1, 10);
    if (escenario == 3) {
        for (int e = 0; e < 3; e++) medir_y_mostrar<Estructura>(info, n, (Escenario)e, repeticiones);
    } else {
        medir_y_mostrar<Estructura>(info, n, (Escenario)escenario, repeticiones);
    }
    pausa();
}

// Inserta los libros en la estructura recién creada y devuelve los KB que ocupa.
// "antes" se toma ANTES de crear la estructura: la hash ya pide sus casillas al nacer.
template <class Estructura>
double insertar_y_medir(Estructura& estructura, const vector<Libro>& libros, long long antes) {
    for (const Libro& l : libros) estructura.insertar(l.codigo, l);
    return (memoria::en_uso - antes) / 1024.0;
}

// printf("%-Ns") cuenta bytes y las tildes ocupan 2 en UTF-8: se rellena por caracteres
void imprimir_columna(const string& texto, int ancho) {
    int caracteres = 0;
    for (unsigned char c : texto)
        if ((c & 0xC0) != 0x80) caracteres++;
    printf("%s", texto.c_str());
    for (int i = caracteres; i < ancho; i++) putchar(' ');
}

template <class Estructura>
void opcion_buscar(const InfoApp& info) {
    vector<Libro> libros = cargar_catalogo(ruta_catalogo());
    if (libros.empty()) {
        cout << "\nNo se pudo leer " << ruta_catalogo() << "\n";
        pausa();
        return;
    }
    long long antes = memoria::en_uso;
    Estructura estructura;
    double kb = insertar_y_medir(estructura, libros, antes);
    printf("\nSe cargaron %d libros de %s en el mismo orden del archivo.\n", (int)libros.size(),
           ruta_catalogo().c_str());
    printf("Forma: %s   Memoria: %.1f KB\n", estructura.forma().c_str(), kb);
    printf("Codigos del %lld al %lld.\n", libros.front().codigo, libros.back().codigo);

    while (true) {
        int codigo = leer_entero("\nCodigo a buscar (0 para volver): ", 0, 2000000000);
        if (codigo == 0) return;
        probar_busqueda("Resultado:", estructura, codigo);
    }
}

template <class Estructura>
void opcion_listar() {
    vector<Libro> libros = cargar_catalogo(ruta_catalogo());
    if (libros.empty()) {
        cout << "\nNo se pudo leer " << ruta_catalogo() << "\n";
        pausa();
        return;
    }
    Estructura estructura;
    for (const Libro& l : libros) estructura.insertar(l.codigo, l);
    vector<Libro*> lista(libros.size());
    int pos = 0;
    Reloj::time_point t0 = Reloj::now();
    estructura.recorrido_inorden(lista.data(), pos);
    Reloj::time_point t1 = Reloj::now();

    printf("\n%-7s %-45s %-27s %s\n", "Codigo", "Titulo", "Editorial", "Autores");
    printf("--------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < pos; i++) {
        printf("%-7lld ", lista[i]->codigo);
        imprimir_columna(lista[i]->titulo, 46);
        imprimir_columna(lista[i]->editorial, 28);
        printf("%s\n", lista[i]->autores.c_str());
    }
    printf("\n%d libros listados en orden en %.4f ms.\n", pos, milisegundos(t0, t1));
    pausa();
}

template <class Estructura>
int ejecutar_app(const InfoApp& info, int argc, char* argv[]) {
    preparar_consola(argv[0]);

    // Modo sin menú: ./app --reporte [N]  -> los 3 escenarios con ese N
    if (argc >= 2 && string(argv[1]) == "--reporte") {
        int n = argc >= 3 ? atoi(argv[2]) : 10000;
        if (n < 100) n = 100;
        for (int e = 0; e < 3; e++) medir_y_mostrar<Estructura>(info, n, (Escenario)e, 3);
        return 0;
    }

    while (true) {
        printf("\n==============================================================\n"
               " Catalogo de libros con %s\n"
               " Paradigma: %s\n"
               "==============================================================\n"
               "  1) Medir tiempo y memoria (un N y un escenario)\n"
               "  2) Catalogo real (datos/libros.csv): buscar un libro\n"
               "  3) Catalogo real: listar todos los libros ordenados por codigo\n"
               "  4) Ver el codigo de la estructura (%s)\n"
               "  5) Teoria: paradigma, estructuras que usa, Big O\n"
               "  0) Salir\n",
               info.nombre, info.paradigma, info.archivo);
        int opcion = leer_entero("Opcion: ", 0, 5);
        if (opcion == 0) return 0;
        if (opcion == 1) opcion_medir<Estructura>(info);
        if (opcion == 2) opcion_buscar<Estructura>(info);
        if (opcion == 3) opcion_listar<Estructura>();
        if (opcion == 4) {
            mostrar_archivo(info.archivo);
            pausa();
        }
        if (opcion == 5) {
            cout << info.teoria;
            pausa();
        }
    }
}
