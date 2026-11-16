// Aplicación 2: catálogo de libros con un Árbol AVL.
// Paradigma: Transformar y vencerás. La estructura está en comun/arbol_avl.h.

#include "../comun/app_individual.h"
#include "../comun/arbol_avl.h"

const char* TEORIA = R"(
ARBOL AVL (arbol binario de busqueda auto-balanceado)

PARADIGMA: TRANSFORMAR Y VENCERAS (transform-and-conquer)
  En lugar de resolver el problema sobre cualquier arbol, primero se TRANSFORMA la
  instancia en otra equivalente mas facil: despues de cada insercion, si un nodo queda
  con una diferencia de alturas mayor que 1, se rota hasta dejar el arbol balanceado.
  Con el arbol ya transformado, buscar siempre cuesta O(log n).
  (El AVL es el ejemplo clasico de este paradigma: simplificacion de la instancia.)

ESTRUCTURAS Y TECNICAS QUE USA
  1. Arbol AVL enlazado (cada nodo guarda tambien su altura).
  2. Recursion: insercion con rebalanceo de abajo hacia arriba, inorden y liberacion.
  3. Rotaciones simples y dobles (4 casos: II, DD, ID, DI).
  4. Busqueda binaria sobre el arbol (iterativa).

COMPLEJIDAD
  Buscar             O(log n) SIEMPRE (altura <= 1.44 log2 n)
  Insertar           O(log n) SIEMPRE
  Insertar N libros  O(n log n)
  Listar ordenado    O(n)
  Memoria            O(n): cada nodo = codigo + libro + 2 punteros + altura
  Pila (recursion)   O(log n)

DESTACA: no tiene peor caso: con 1.000.000 de libros revisa como maximo ~29 nodos,
         lleguen en el orden que lleguen. Conserva el orden para listar.
FALLA:   mas codigo (rotaciones); cada insercion es algo mas lenta que en el BST en el
         caso promedio y cada nodo ocupa un poco mas (la altura). Nunca llega al O(1)
         de la tabla hash al buscar.
)";

int main(int argc, char* argv[]) {
    InfoApp info = {"Arbol AVL", "arbol_avl.h", "Transformar y venceras", NINGUNO, TEORIA};
    return ejecutar_app<ArbolAVL>(info, argc, argv);
}
