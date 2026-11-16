// Aplicación 1: catálogo de libros con un Árbol Binario de Búsqueda sin balancear.
// Paradigma: Reducir y vencerás. La estructura está en comun/arbol_bst.h.

#include "../comun/app_individual.h"
#include "../comun/arbol_bst.h"

const char* TEORIA = R"(
ARBOL BINARIO DE BUSQUEDA (BST) SIN BALANCEAR

PARADIGMA: REDUCIR Y VENCERAS (decrease-and-conquer, de tamano variable)
  Para buscar un codigo se compara con la raiz: si es menor, el problema se reduce al
  subarbol izquierdo; si es mayor, al derecho. Cada paso descarta una parte del arbol y
  el problema sigue con UNA sola parte (por eso es "reducir" y no "dividir").
  Cuanto se reduce depende de la forma del arbol: la mitad si esta balanceado, un solo
  nodo si esta degenerado.

ESTRUCTURAS Y TECNICAS QUE USA
  1. Arbol binario enlazado (nodos con puntero izquierdo y derecho).
  2. Recursion: recorrido inorden, calculo de altura y liberacion de memoria.
  3. Busqueda binaria sobre el arbol (iterativa) para insertar y buscar.
  4. Ordenamiento implicito: el recorrido inorden entrega los libros ya ordenados.

COMPLEJIDAD
  Buscar             O(log n) promedio      O(n) peor caso
  Insertar           O(log n) promedio      O(n) peor caso
  Insertar N libros  O(n log n) promedio    O(n^2) peor caso
  Listar ordenado    O(n)
  Memoria            O(n): cada nodo = codigo + libro + 2 punteros
  Pila (recursion)   O(h), h = altura: O(log n) promedio, O(n) si se degenera

DESTACA: codigo corto y facil; lista en orden sin ordenar; es la que menos memoria
         gasta por libro.
FALLA:   si los codigos llegan ORDENADOS (como el Excel) cada libro se cuelga a la
         derecha del anterior: el arbol se vuelve una lista y todo pasa a O(n).
         Pruebalo con el escenario ORDENADO o con el catalogo real (altura = 50).
)";

int main(int argc, char* argv[]) {
    InfoApp info = {"Arbol Binario de Busqueda (BST)", "arbol_bst.h", "Reducir y venceras",
                    ORDENADO, TEORIA};
    return ejecutar_app<ArbolBinarioBusqueda>(info, argc, argv);
}
