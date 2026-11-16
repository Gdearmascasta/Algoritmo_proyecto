// Aplicación 3: catálogo de libros con una Tabla Hash con encadenamiento.
// Paradigma: Compromiso espacio-tiempo. La estructura está en comun/tabla_hash.h.

#include "../comun/app_individual.h"
#include "../comun/tabla_hash.h"

const char* TEORIA = R"(
TABLA HASH CON ENCADENAMIENTO (casilla = codigo % tamano)

PARADIGMA: COMPROMISO ESPACIO-TIEMPO (space-time trade-off)
  Se gasta memoria extra (un arreglo de casillas, con al menos 30% vacias porque la
  tabla se duplica al pasar 0.7 de carga) para ganar tiempo: el codigo dice
  directamente en que casilla esta el libro, sin recorrer ni comparar con los demas.
  Para listar en orden usa merge sort, que es DIVIDE Y VENCERAS: parte el arreglo en
  dos mitades, ordena cada una recursivamente y las mezcla.

ESTRUCTURAS Y TECNICAS QUE USA
  1. Arreglo dinamico de casillas (se duplica al llenarse: redimensionar).
  2. Listas enlazadas para las colisiones (encadenamiento).
  3. Funcion hash (residuo de la division).
  4. Ordenamiento por mezcla (merge sort, recursivo) para listar.

COMPLEJIDAD
  Buscar             O(1) promedio          O(n) peor caso (todo en una casilla)
  Insertar           O(1) amortizado        O(n) peor caso
  Insertar N libros  O(n)                   O(n^2) peor caso
  Listar ordenado    O(n log n) (hay que ordenar)
  Memoria            O(n + casillas): cada nodo = codigo + libro + 1 puntero,
                     mas 1 puntero por casilla (incluidas las vacias)
  Extra al listar    O(n): arreglo auxiliar del merge sort

DESTACA: la mas rapida para buscar por codigo exacto, sin importar si los datos llegan
         ordenados.
FALLA:   no guarda orden (listar exige ordenar); no sirve para rangos ("libros del 200
         al 300"); si muchos codigos dan el mismo residuo caen en la misma casilla y
         todo pasa a O(n). Pruebalo con el escenario ADVERSARIO.
)";

int main(int argc, char* argv[]) {
    InfoApp info = {"Tabla Hash", "tabla_hash.h", "Compromiso espacio-tiempo", ADVERSARIO, TEORIA};
    return ejecutar_app<TablaHash>(info, argc, argv);
}
