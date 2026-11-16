// Comparación de 3 estructuras para el mismo problema:
// "Guardar N libros y buscarlos por código (y poder listarlos en orden)".
//
//   1. Árbol Binario de Búsqueda sin balancear  (arbol_bst.h)
//   2. Árbol AVL                                (arbol_avl.h)
//   3. Tabla Hash con encadenamiento            (tabla_hash.h)

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "arbol_avl.h"
#include "arbol_bst.h"
#include "libro.h"
#include "tabla_hash.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;
typedef chrono::steady_clock Reloj;

// 100 casillas iniciales de la tabla * 2^20. Todos sus múltiplos caen en la casilla 0
// mientras la tabla tenga menos de 2^20 duplicaciones: el peor caso de la hash.
const long long SALTO_ADVERSARIO = 104857600LL;

// Por encima de este N no se corren los casos O(n^2) (BST ordenado, hash adversario):
// tardarían minutos y la recursión del árbol degenerado podría desbordar la pila.
const int LIMITE_CUADRATICO = 20000;

const int BUSQUEDAS = 10000;
volatile long long sumidero = 0;   // Evita que el compilador elimine las búsquedas

enum Escenario { ALEATORIO, ORDENADO, ADVERSARIO };
const char* NOMBRE_ESCENARIO[] = {
    "ALEATORIO  - codigos en desorden (caso promedio)",
    "ORDENADO   - codigos de menor a mayor, como el Excel (peor caso del BST)",
    "ADVERSARIO - codigos que colisionan en la hash (peor caso de la hash)"};

enum Tipo { BST, AVL, HASH };
const char* NOMBRE_TIPO[] = {"BST", "AVL", "Tabla hash"};

string carpeta_base;   // Carpeta donde está el ejecutable (para encontrar src/ y datos/)

// ---------------------------------------------------------------- datos de prueba

struct Datos {
    vector<Libro> libros;              // En el orden en que se insertan
    vector<long long> existentes;      // Códigos a buscar que SÍ están
    vector<long long> inexistentes;    // Códigos a buscar que NO están
};

Libro libro_sintetico(long long codigo) {
    Libro l;
    l.codigo = codigo;
    l.titulo = "Libro " + to_string(codigo);
    l.editorial = "Editorial " + to_string(codigo % 40);
    l.autores = "Autor " + to_string(codigo % 300);
    return l;
}

Datos generar_datos(int n, Escenario escenario) {
    mt19937_64 aleatorio(2026 + n * 3 + escenario);
    vector<long long> codigos(n), faltantes(n);
    for (int i = 0; i < n; i++) {
        if (escenario == ADVERSARIO) {
            codigos[i] = (long long)(i + 1) * SALTO_ADVERSARIO;
            faltantes[i] = (long long)(n + i + 1) * SALTO_ADVERSARIO;
        } else {
            codigos[i] = 101 + 3LL * i;     // 101, 104, 107...
            faltantes[i] = 102 + 3LL * i;   // 102, 105, 108... (huecos entre los existentes)
        }
    }
    if (escenario != ORDENADO) {
        for (int i = n - 1; i > 0; i--) {   // Barajar (Fisher-Yates)
            int j = aleatorio() % (i + 1);
            long long temporal = codigos[i];
            codigos[i] = codigos[j];
            codigos[j] = temporal;
        }
    }

    Datos datos;
    for (int i = 0; i < n; i++) datos.libros.push_back(libro_sintetico(codigos[i]));
    int m = n < BUSQUEDAS ? n : BUSQUEDAS;
    for (int i = 0; i < m; i++) {
        datos.existentes.push_back(codigos[aleatorio() % n]);
        datos.inexistentes.push_back(faltantes[aleatorio() % n]);
    }
    return datos;
}

// ---------------------------------------------------------------- medición

struct Resultado {
    bool omitido = false;
    bool error = false;
    double ms_construir = 0;     // Insertar los N libros
    double ns_buscar_ok = 0;     // Promedio por búsqueda de un código que existe
    double ns_buscar_fallo = 0;  // Promedio por búsqueda de un código que no existe
    double ms_listar = 0;        // Sacar todos los libros ordenados por código
    double comp_ok = 0;          // Nodos revisados en promedio por búsqueda exitosa
    double comp_fallo = 0;
    string forma;
};

double milisegundos(Reloj::time_point desde, Reloj::time_point hasta) {
    return chrono::duration<double, milli>(hasta - desde).count();
}

template <class Estructura>
Resultado medir(const Datos& datos, int repeticiones) {
    Resultado r;
    int n = datos.libros.size();
    double busquedas = datos.existentes.size();
    Libro** lista = new Libro*[n];

    for (int rep = 0; rep < repeticiones; rep++) {
        Estructura estructura;

        Reloj::time_point t0 = Reloj::now();
        for (int i = 0; i < n; i++) estructura.insertar(datos.libros[i].codigo, datos.libros[i]);
        Reloj::time_point t1 = Reloj::now();

        estructura.comparaciones = 0;
        long long encontrados = 0;
        for (long long codigo : datos.existentes)
            if (estructura.buscar(codigo) != nullptr) encontrados++;
        Reloj::time_point t2 = Reloj::now();
        long long comp_ok = estructura.comparaciones;

        estructura.comparaciones = 0;
        long long falsos = 0;
        for (long long codigo : datos.inexistentes)
            if (estructura.buscar(codigo) != nullptr) falsos++;
        Reloj::time_point t3 = Reloj::now();
        long long comp_fallo = estructura.comparaciones;

        int pos = 0;
        estructura.recorrido_inorden(lista, pos);
        Reloj::time_point t4 = Reloj::now();

        // Comprobación de que la estructura respondió bien
        if (encontrados != (long long)busquedas || falsos != 0 || pos != n) r.error = true;
        for (int i = 1; i < pos; i++)
            if (lista[i - 1]->codigo >= lista[i]->codigo) r.error = true;
        sumidero = sumidero + encontrados + pos;

        r.ms_construir += milisegundos(t0, t1);
        r.ns_buscar_ok += milisegundos(t1, t2) * 1e6 / busquedas;
        r.ns_buscar_fallo += milisegundos(t2, t3) * 1e6 / busquedas;
        r.ms_listar += milisegundos(t3, t4);
        r.comp_ok += comp_ok / busquedas;
        r.comp_fallo += comp_fallo / busquedas;
        r.forma = estructura.forma();
    }
    delete[] lista;

    r.ms_construir /= repeticiones;
    r.ns_buscar_ok /= repeticiones;
    r.ns_buscar_fallo /= repeticiones;
    r.ms_listar /= repeticiones;
    r.comp_ok /= repeticiones;
    r.comp_fallo /= repeticiones;
    return r;
}

bool es_caso_cuadratico(Tipo tipo, Escenario escenario) {
    return (tipo == BST && escenario == ORDENADO) || (tipo == HASH && escenario == ADVERSARIO);
}

Resultado medir_tipo(Tipo tipo, Escenario escenario, const Datos& datos, int repeticiones) {
    if (es_caso_cuadratico(tipo, escenario) && (int)datos.libros.size() > LIMITE_CUADRATICO) {
        Resultado r;
        r.omitido = true;
        return r;
    }
    if (tipo == BST) return medir<ArbolBinarioBusqueda>(datos, repeticiones);
    if (tipo == AVL) return medir<ArbolAVL>(datos, repeticiones);
    return medir<TablaHash>(datos, repeticiones);
}

// ---------------------------------------------------------------- utilidades de consola

int leer_entero(const string& mensaje, int minimo, int maximo) {
    while (true) {
        cout << mensaje;
        string linea;
        if (!getline(cin, linea)) exit(0);
        try {
            size_t usados = 0;
            long long valor = stoll(linea, &usados);
            if (usados == linea.size() && valor >= minimo && valor <= maximo) return (int)valor;
        } catch (...) {
        }
        cout << "  Valor invalido: escribe un numero entre " << minimo << " y " << maximo << ".\n";
    }
}

void pausa() {
    cout << "\nPresiona Enter para volver al menu...";
    string linea;
    getline(cin, linea);
}

// ---------------------------------------------------------------- opción 1: comparar

void imprimir_resumen(const char* operacion, Resultado res[3], double Resultado::*campo) {
    int rapido = -1, lento = -1;
    for (int t = 0; t < 3; t++) {
        if (res[t].omitido) continue;
        if (rapido == -1 || res[t].*campo < res[rapido].*campo) rapido = t;
        if (lento == -1 || res[t].*campo > res[lento].*campo) lento = t;
    }
    double veces = res[rapido].*campo > 0 ? res[lento].*campo / res[rapido].*campo : 0;
    printf("  %-22s mas rapido: %-10s  mas lento: %-10s (%.1f veces mas lento)\n",
           operacion, NOMBRE_TIPO[rapido], NOMBRE_TIPO[lento], veces);
}

void comparar(int n, Escenario escenario, int repeticiones) {
    printf("\n=== N = %d libros | escenario %s\n", n, NOMBRE_ESCENARIO[escenario]);
    printf("    (promedio de %d repeticion(es), %d busquedas de cada tipo)\n\n", repeticiones,
           n < BUSQUEDAS ? n : BUSQUEDAS);

    Datos datos = generar_datos(n, escenario);
    Resultado res[3];
    for (int t = 0; t < 3; t++) res[t] = medir_tipo((Tipo)t, escenario, datos, repeticiones);

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

    int medidos = 0;
    for (int t = 0; t < 3; t++) if (!res[t].omitido) medidos++;
    if (medidos >= 2) {
        printf("\nResumen:\n");
        imprimir_resumen("Insertar N libros:", res, &Resultado::ms_construir);
        imprimir_resumen("Buscar (existe):", res, &Resultado::ns_buscar_ok);
        imprimir_resumen("Buscar (no existe):", res, &Resultado::ns_buscar_fallo);
        imprimir_resumen("Listar ordenado:", res, &Resultado::ms_listar);
    }
}

int pedir_escenario() {
    cout << "\nEscenario:\n";
    for (int e = 0; e < 3; e++) cout << "  " << e + 1 << ") " << NOMBRE_ESCENARIO[e] << "\n";
    cout << "  4) Los tres escenarios\n";
    return leer_entero("Opcion: ", 1, 4) - 1;
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

vector<Libro> cargar_catalogo(const string& ruta) {
    vector<Libro> libros;
    ifstream archivo(ruta);
    string linea;
    getline(archivo, linea);   // Encabezado
    while (getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;
        string campos[4];
        int c = 0;
        for (char letra : linea) {
            if (letra == ';' && c < 3) c++;
            else campos[c] += letra;
        }
        try {
            Libro l;
            l.codigo = stoll(campos[0]);
            l.titulo = campos[1];
            l.editorial = campos[2];
            l.autores = campos[3];
            libros.push_back(l);
        } catch (...) {
        }
    }
    return libros;
}

template <class Estructura>
void probar_busqueda(const char* nombre, Estructura& estructura, long long codigo) {
    estructura.comparaciones = 0;
    Libro* libro = estructura.buscar(codigo);
    long long nodos = estructura.comparaciones;

    // Una sola búsqueda dura nanosegundos: se repite muchas veces y se promedia
    const int VECES = 200000;
    Reloj::time_point t0 = Reloj::now();
    for (int i = 0; i < VECES; i++) sumidero = sumidero + (estructura.buscar(codigo) != nullptr);
    Reloj::time_point t1 = Reloj::now();
    double ns = milisegundos(t0, t1) * 1e6 / VECES;

    printf("  %-11s %7.1f ns  %3lld nodos revisados  -> %s\n", nombre, ns, nodos,
           libro ? (libro->titulo + " (" + libro->autores + ")").c_str() : "no esta en el catalogo");
}

void opcion_catalogo() {
    string ruta = carpeta_base + "datos/libros.csv";
    vector<Libro> libros = cargar_catalogo(ruta);
    if (libros.empty()) {
        cout << "\nNo se pudo leer " << ruta << "\n";
        pausa();
        return;
    }

    ArbolBinarioBusqueda bst;
    ArbolAVL avl;
    TablaHash hash;
    for (const Libro& l : libros) {
        bst.insertar(l.codigo, l);
        avl.insertar(l.codigo, l);
        hash.insertar(l.codigo, l);
    }

    printf("\nSe cargaron %d libros de %s en el mismo orden del archivo.\n", (int)libros.size(),
           ruta.c_str());
    printf("Forma de cada estructura:\n");
    printf("  BST:        %s   (el archivo viene ordenado: el arbol quedo como una lista)\n",
           bst.forma().c_str());
    printf("  AVL:        %s\n", avl.forma().c_str());
    printf("  Tabla hash: %s\n", hash.forma().c_str());
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
            "  3) tabla_hash.h  (Tabla Hash)\n";
    int opcion = leer_entero("Opcion: ", 1, 3);
    const char* archivos[] = {"arbol_bst.h", "arbol_avl.h", "tabla_hash.h"};
    string ruta = carpeta_base + "src/" + archivos[opcion - 1];
    ifstream archivo(ruta);
    if (!archivo) {
        cout << "No se encontro " << ruta << "\n";
        pausa();
        return;
    }
    cout << "\n";
    string linea;
    int numero = 1;
    while (getline(archivo, linea)) printf("%4d | %s\n", numero++, linea.c_str());
    pausa();
}

// ---------------------------------------------------------------- opción 4: teoría

void opcion_teoria() {
    cout << R"(
PROBLEMA: guardar N libros y encontrarlos por su codigo (y listarlos en orden).

COMPLEJIDAD (Big O)
                     BST sin balancear      AVL                  Tabla hash
  Buscar             O(log n) promedio      O(log n) SIEMPRE     O(1) promedio
                     O(n) peor caso                              O(n) peor caso
  Insertar           O(log n) / O(n)        O(log n) SIEMPRE     O(1) amortizado / O(n)
  Insertar N libros  O(n log n) / O(n^2)    O(n log n)           O(n) / O(n^2)
  Listar ordenado    O(n)                   O(n)                 O(n log n) (hay que ordenar)
  Memoria            O(n)                   O(n) + altura/nodo   O(n + casillas)

1) ARBOL BINARIO DE BUSQUEDA (BST)
   Destaca: codigo corto y facil; lista en orden "gratis" con el recorrido inorden;
            si los codigos llegan en desorden queda bajo (altura ~ 2-3 log2 n).
   Falla:   si los codigos llegan ORDENADOS (como el Excel) cada libro se cuelga a la
            derecha del anterior: el arbol se vuelve una lista y todo pasa a O(n).
            Con N grande la recursion (inorden, liberar) puede desbordar la pila.

2) ARBOL AVL
   Destaca: garantiza O(log n) en cualquier orden de llegada (altura <= 1.44 log2 n):
            con 1.000.000 de libros revisa como maximo ~29 nodos. Conserva el orden.
   Falla:   mas codigo (rotaciones) y cada insercion es algo mas lenta que en el BST
            en el caso promedio, por recalcular alturas y rotar. Sigue siendo
            logaritmico: nunca llega al O(1) de la hash.

3) TABLA HASH (encadenamiento, casilla = codigo % tamano)
   Destaca: busqueda e insercion en O(1) promedio: la mas rapida para buscar por codigo
            exacto, sin importar si los datos vienen ordenados.
   Falla:   no guarda orden (listar exige ordenar: O(n log n)); no sirve para rangos
            ("libros del 200 al 300"); si muchos codigos dan el mismo residuo (ej. todos
            multiplos del tamano de la tabla) caen en la misma casilla, la cadena crece
            y todo pasa a O(n). Ademas, al duplicarse copia todos los elementos.

ESCENARIOS DEL PROGRAMA
   ALEATORIO:  caso promedio de las tres.
   ORDENADO:   peor caso del BST (el AVL y la hash no se afectan).
   ADVERSARIO: codigos multiplos de 100*2^20 -> todos a la casilla 0: peor caso de la hash
               (los arboles no se afectan porque los codigos llegan barajados).

"Nodos rev." en las tablas cuenta cuantos nodos se compararon por busqueda: es la
medida que no depende de la computadora y deberia seguir la Big O.
)";
    pausa();
}

// ---------------------------------------------------------------- main

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    string ejecutable = argv[0];
    size_t barra = ejecutable.find_last_of("/\\");
    carpeta_base = barra == string::npos ? "" : ejecutable.substr(0, barra + 1);

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
                "  1) Comparar las 3 estructuras (un N y un escenario)\n"
                "  2) Catalogo real (datos/libros.csv): buscar un libro\n"
                "  3) Ver el codigo de una estructura\n"
                "  4) Teoria: Big O, donde destaca y donde falla cada una\n"
                "  0) Salir\n";
        int opcion = leer_entero("Opcion: ", 0, 4);
        if (opcion == 0) return 0;
        if (opcion == 1) opcion_comparar();
        if (opcion == 2) opcion_catalogo();
        if (opcion == 3) opcion_codigo();
        if (opcion == 4) opcion_teoria();
    }
}
