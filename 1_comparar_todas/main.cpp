// Comparación de 3 soluciones para el mismo problema:
// "Guardar N libros y buscarlos por código (y poder listarlos en orden)".
//
//   1. Árbol Binario de Búsqueda sin balancear  (comun/arbol_bst.h)   Reducir y vencerás
//   2. Árbol AVL                                (comun/arbol_avl.h)   Transformar y vencerás
//   3. Tabla Hash con encadenamiento            (comun/tabla_hash.h)  Compromiso espacio-tiempo
//
// Cada una tiene además su aplicación individual en 2_bst/, 3_avl/ y 4_hash/.

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "../comun/arbol_avl.h"
#include "../comun/arbol_bst.h"
#include "../comun/consola.h"
#include "../comun/libro.h"
#include "../comun/medicion.h"
#include "../comun/tabla_hash.h"

using namespace std;

enum Tipo { BST, AVL, HASH };
const char* NOMBRE_TIPO[] = {"BST", "AVL", "Tabla hash"};

Resultado medir_tipo(Tipo tipo, Escenario escenario, const Datos& datos, int repeticiones) {
    if (tipo == BST) return medir_si_cabe<ArbolBinarioBusqueda>(datos, escenario, ORDENADO, repeticiones);
    if (tipo == AVL) return medir_si_cabe<ArbolAVL>(datos, escenario, NINGUNO, repeticiones);
    return medir_si_cabe<TablaHash>(datos, escenario, ADVERSARIO, repeticiones);
}

// ---------------------------------------------------------------- opción 1: comparar

void imprimir_resumen(const char* operacion, Resultado res[3], double Resultado::*campo,
                      const char* mejor, const char* peor, const char* unidad) {
    int menor = -1, mayor = -1;
    for (int t = 0; t < 3; t++) {
        if (res[t].omitido) continue;
        if (menor == -1 || res[t].*campo < res[menor].*campo) menor = t;
        if (mayor == -1 || res[t].*campo > res[mayor].*campo) mayor = t;
    }
    double veces = res[menor].*campo > 0 ? res[mayor].*campo / res[menor].*campo : 0;
    printf("  %-22s %s: %-10s  %s: %-10s (%.1f veces %s)\n", operacion, mejor, NOMBRE_TIPO[menor],
           peor, NOMBRE_TIPO[mayor], veces, unidad);
}

void comparar(int n, Escenario escenario, int repeticiones) {
    printf("\n=== N = %d libros | escenario %s\n", n, NOMBRE_ESCENARIO[escenario]);
    printf("    (promedio de %d repeticion(es), %d busquedas de cada tipo)\n\n", repeticiones,
           n < BUSQUEDAS ? n : BUSQUEDAS);

    Datos datos = generar_datos(n, escenario);
    Resultado res[3];
    for (int t = 0; t < 3; t++) res[t] = medir_tipo((Tipo)t, escenario, datos, repeticiones);

    printf("TIEMPO\n");
    printf("%-11s %13s %14s %16s %11s %10s %11s  %s\n", "Estructura", "Insertar N",
           "Buscar existe", "Buscar no existe", "Listar", "Nodos rev.", "Nodos rev.", "Forma");
    printf("%-11s %13s %14s %16s %11s %10s %11s\n", "", "(ms)", "(ns/busq.)", "(ns/busq.)",
           "ordenado ms", "(existe)", "(no existe)");
    printf("--------------------------------------------------------------------------------------------------------\n");
    for (int t = 0; t < 3; t++) {
        if (res[t].omitido) {
            printf("%-11s  omitido: es O(n^2) en este escenario y con N > %d tardaria demasiado\n",
                   NOMBRE_TIPO[t], LIMITE_CUADRATICO);
            continue;
        }
        printf("%-11s %13.3f %14.1f %16.1f %11.3f %10.1f %11.1f  %s%s\n", NOMBRE_TIPO[t],
               res[t].ms_construir, res[t].ns_buscar_ok, res[t].ns_buscar_fallo, res[t].ms_listar,
               res[t].comp_ok, res[t].comp_fallo, res[t].forma.c_str(),
               res[t].error ? "  [ERROR: resultados incorrectos]" : "");
    }

    printf("\nMEMORIA (bytes pedidos al heap; incluye los libros guardados)\n");
    printf("%-11s %16s %16s %18s %18s\n", "Estructura", "Estructura (KB)", "Bytes por libro",
           "Pico insertar (KB)", "Extra listar (KB)");
    printf("--------------------------------------------------------------------------------\n");
    for (int t = 0; t < 3; t++) {
        if (res[t].omitido) {
            printf("%-11s  omitido\n", NOMBRE_TIPO[t]);
            continue;
        }
        printf("%-11s %16.1f %16.1f %18.1f %18.1f\n", NOMBRE_TIPO[t], res[t].kb_estructura,
               res[t].bytes_por_libro, res[t].kb_pico_insertar, res[t].kb_extra_listar);
    }

    printf("Nota: Extra listar excluye el arreglo comun de salida (8N bytes en 64 bits).\n");
    printf("Los KB reflejan asignaciones al heap; no incluyen pila ni RAM total del proceso.\n");

    int medidos = 0;
    for (int t = 0; t < 3; t++) if (!res[t].omitido) medidos++;
    if (medidos >= 2) {
        printf("\nResumen:\n");
        imprimir_resumen("Insertar N libros:", res, &Resultado::ms_construir, "mas rapido", "mas lento", "mas lento");
        imprimir_resumen("Buscar (existe):", res, &Resultado::ns_buscar_ok, "mas rapido", "mas lento", "mas lento");
        imprimir_resumen("Buscar (no existe):", res, &Resultado::ns_buscar_fallo, "mas rapido", "mas lento", "mas lento");
        imprimir_resumen("Listar ordenado:", res, &Resultado::ms_listar, "mas rapido", "mas lento", "mas lento");
        imprimir_resumen("Memoria:", res, &Resultado::kb_estructura, "menos memoria", "mas memoria", "mas memoria");
    }
}

void opcion_comparar() {
    int n = leer_entero("\nCantidad de libros N (100 - 1000000): ", 100, 1000000);
    int escenario = pedir_escenario();
    int repeticiones = leer_entero("Repeticiones para promediar (1 - 10): ", 1, 10);
    if (escenario == 3) {
        for (int e = 0; e < 3; e++) comparar(n, (Escenario)e, repeticiones);
    } else {
        comparar(n, (Escenario)escenario, repeticiones);
    }
    pausa();
}

// ---------------------------------------------------------------- opción 2: catálogo real

// "antes" se toma ANTES de crear la estructura: la hash ya pide sus casillas al nacer
template <class Estructura>
double insertar_y_medir(Estructura& estructura, const vector<Libro>& libros, long long antes) {
    for (const Libro& l : libros) estructura.insertar(l.codigo, l);
    return (memoria::en_uso - antes) / 1024.0;
}

void opcion_catalogo() {
    string ruta = ruta_catalogo();
    vector<Libro> libros = cargar_catalogo(ruta);
    if (libros.empty()) {
        cout << "\nNo se pudo leer " << ruta << "\n";
        pausa();
        return;
    }

    long long antes = memoria::en_uso;
    ArbolBinarioBusqueda bst;
    double kb_bst = insertar_y_medir(bst, libros, antes);
    antes = memoria::en_uso;
    ArbolAVL avl;
    double kb_avl = insertar_y_medir(avl, libros, antes);
    antes = memoria::en_uso;
    TablaHash hash;
    double kb_hash = insertar_y_medir(hash, libros, antes);

    printf("\nSe cargaron %d libros de %s en el mismo orden del archivo.\n", (int)libros.size(),
           ruta.c_str());
    printf("Forma y memoria de cada estructura:\n");
    printf("  BST:        %-28s %6.1f KB   (el archivo viene ordenado: el arbol quedo como una lista)\n",
           bst.forma().c_str(), kb_bst);
    printf("  AVL:        %-28s %6.1f KB\n", avl.forma().c_str(), kb_avl);
    printf("  Tabla hash: %-28s %6.1f KB\n", hash.forma().c_str(), kb_hash);
    printf("Codigos del %lld al %lld.\n", libros.front().codigo, libros.back().codigo);

    while (true) {
        int codigo = leer_entero("\nCodigo a buscar (0 para volver): ", 0, 2000000000);
        if (codigo == 0) return;
        probar_busqueda("BST", bst, codigo);
        probar_busqueda("AVL", avl, codigo);
        probar_busqueda("Tabla hash", hash, codigo);
    }
}

// ---------------------------------------------------------------- opción 3: ver código

void opcion_codigo() {
    cout << "\n  1) arbol_bst.h   (Arbol Binario de Busqueda)\n"
            "  2) arbol_avl.h   (Arbol AVL)\n"
            "  3) tabla_hash.h  (Tabla Hash)\n"
            "  4) memoria.h     (como se mide la memoria)\n";
    int opcion = leer_entero("Opcion: ", 1, 4);
    const char* archivos[] = {"arbol_bst.h", "arbol_avl.h", "tabla_hash.h", "memoria.h"};
    mostrar_archivo(archivos[opcion - 1]);
    pausa();
}

// ---------------------------------------------------------------- opción 4: teoría

void opcion_teoria() {
    cout << R"(
PROBLEMA: guardar N libros y encontrarlos por su codigo (y listarlos en orden).

PARADIGMA Y ESTRUCTURAS DE CADA SOLUCION
  BST          Reducir y venceras (decrease-and-conquer): cada comparacion descarta
               un subarbol y el problema sigue en el otro.
               Usa: arbol binario enlazado, recursion (inorden, altura, liberar),
               busqueda binaria sobre el arbol.
  AVL          Transformar y venceras (transform-and-conquer): tras cada insercion
               transforma el arbol en uno equivalente balanceado (rotaciones).
               Usa: arbol AVL, recursion (insercion, inorden), rotaciones,
               busqueda binaria sobre el arbol.
  Tabla hash   Compromiso espacio-tiempo (space-time trade-off): gasta memoria en
               casillas para buscar en O(1). Para listar ordena con merge sort,
               que es divide y venceras.
               Usa: arreglo de casillas, listas enlazadas (encadenamiento),
               funcion hash, ordenamiento por mezcla (recursivo).

COMPLEJIDAD (Big O)
                     BST sin balancear      AVL                  Tabla hash
  Buscar             O(log n) promedio      O(log n) SIEMPRE     O(1) promedio
                     O(n) peor caso                              O(n) peor caso
  Insertar           O(log n) / O(n)        O(log n) SIEMPRE     O(1) amortizado / O(n)
  Insertar N libros  O(n log n) / O(n^2)    O(n log n)           O(n) / O(n^2)
  Listar ordenado    O(n)                   O(n)                 O(n log n) (hay que ordenar)
  Memoria            O(n)                   O(n) + altura/nodo   O(n + casillas)
  Extra al listar    O(h) de pila           O(log n) de pila     O(n) arreglo auxiliar

1) ARBOL BINARIO DE BUSQUEDA (BST)
   Destaca: codigo corto y facil; lista en orden "gratis" con el recorrido inorden;
            si los codigos llegan en desorden queda bajo (altura ~ 2-3 log2 n).
            Es la que menos memoria usa por libro (solo 2 punteros por nodo).
   Falla:   si los codigos llegan ORDENADOS (como el Excel) cada libro se cuelga a la
            derecha del anterior: el arbol se vuelve una lista y todo pasa a O(n).
            Con N grande la recursion (inorden, liberar) puede desbordar la pila.

2) ARBOL AVL
   Destaca: garantiza O(log n) en cualquier orden de llegada (altura <= 1.44 log2 n):
            con 1.000.000 de libros revisa como maximo ~29 nodos. Conserva el orden.
   Falla:   mas codigo (rotaciones) y cada insercion es algo mas lenta que en el BST
            en el caso promedio, por recalcular alturas y rotar. Guarda la altura en
            cada nodo (algo mas de memoria). Nunca llega al O(1) de la hash.

3) TABLA HASH (encadenamiento, casilla = codigo % tamano)
   Destaca: busqueda e insercion en O(1) promedio: la mas rapida para buscar por codigo
            exacto, sin importar si los datos vienen ordenados.
   Falla:   no guarda orden (listar exige ordenar: O(n log n) y un arreglo auxiliar
            de n punteros); no sirve para rangos ("libros del 200 al 300"); las casillas
            vacias gastan memoria; si muchos codigos dan el mismo residuo caen en la misma
            casilla, la cadena crece y todo pasa a O(n). Al duplicarse copia todo.

ESCENARIOS DEL PROGRAMA
   ALEATORIO:  caso promedio de las tres.
   ORDENADO:   peor caso del BST (el AVL y la hash no se afectan).
   ADVERSARIO: codigos multiplos de 100*2^20 -> todos a la casilla 0: peor caso de la hash
               (los arboles no se afectan porque los codigos llegan barajados).

"Nodos rev." cuenta cuantos nodos se compararon por busqueda: no depende de la
computadora y deberia seguir la Big O. La memoria se mide contando los bytes que cada
estructura pide al heap (ver comun/memoria.h).
)";
    pausa();
}

// ---------------------------------------------------------------- main

int main(int argc, char* argv[]) {
    preparar_consola(argv[0]);

    // Modo sin menú: ./comparar --reporte [N]  -> los 3 escenarios con ese N
    if (argc >= 2 && string(argv[1]) == "--reporte") {
        int n = argc >= 3 ? atoi(argv[2]) : 10000;
        if (n < 100) n = 100;
        for (int e = 0; e < 3; e++) comparar(n, (Escenario)e, 3);
        return 0;
    }

    while (true) {
        cout << "\n==============================================================\n"
                " Catalogo de libros: BST vs AVL vs Tabla Hash\n"
                " Problema: guardar N libros y buscarlos por codigo\n"
                "==============================================================\n"
                "  1) Comparar las 3 estructuras (tiempo y memoria)\n"
                "  2) Catalogo real (datos/libros.csv): buscar un libro\n"
                "  3) Ver el codigo de una estructura\n"
                "  4) Teoria: paradigmas, Big O, donde destaca y donde falla cada una\n"
                "  0) Salir\n";
        int opcion = leer_entero("Opcion: ", 0, 4);
        if (opcion == 0) return 0;
        if (opcion == 1) opcion_comparar();
        if (opcion == 2) opcion_catalogo();
        if (opcion == 3) opcion_codigo();
        if (opcion == 4) opcion_teoria();
    }
}
