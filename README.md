# Catálogo de libros: BST vs AVL vs Tabla Hash

Mismo problema, tres soluciones: **guardar N libros y buscarlos por código** (y poder listarlos en orden).

Hay **cuatro ejecutables**: uno que compara las tres soluciones y tres aplicaciones individuales, una por solución. Las tres aplicaciones se distinguen por su **estrategia de diseño algorítmico**, no por un paradigma del lenguaje C++ (que es el mismo en las tres).

```
comparacion_libros/
├── 1_comparar_todas/   Las 3 estructuras lado a lado (tiempo y memoria)
├── 2_bst/              Aplicación 1: Árbol Binario de Búsqueda  → Reducir y vencerás
├── 3_avl/              Aplicación 2: Árbol AVL                   → Transformar y vencerás
├── 4_hash/             Aplicación 3: Tabla Hash                  → Compromiso espacio-tiempo
├── comun/              Código compartido (las estructuras, la medición y el menú)
└── datos/libros.csv    Los 50 libros reales de BD_libros.xlsx
```

Cada carpeta de programa tiene su `main.cpp`, su `ejecutar.sh` (Linux/macOS) y su `ejecutar.bat` (Windows).

| Archivo de `comun/` | Contenido |
|---|---|
| `arbol_bst.h` | Árbol Binario de Búsqueda sin balancear (el mismo de la app en Python) |
| `arbol_avl.h` | Árbol AVL (BST auto-balanceado con rotaciones) |
| `tabla_hash.h` | Tabla Hash con encadenamiento (`codigo % tamano`, se duplica al pasar 0.7 de carga) |
| `medicion.h` | Datos de prueba y medición de tiempo, nodos revisados y memoria |
| `memoria.h` | Cuenta los bytes que cada estructura pide al heap |
| `consola.h`, `app_individual.h` | Menús y utilidades de consola |

## Paradigma y estructuras de cada aplicación

| Aplicación | Paradigma | Estructuras y técnicas que usa |
|---|---|---|
| BST | **Reducir y vencerás**: cada comparación descarta un subárbol y el problema sigue en el otro | Árbol binario enlazado, recursión (inorden, altura, liberar), búsqueda binaria sobre el árbol |
| AVL | **Transformar y vencerás**: tras cada inserción las rotaciones transforman el árbol en uno balanceado | Árbol AVL, recursión (inserción con rebalanceo, inorden), rotaciones, búsqueda binaria |
| Tabla hash | **Compromiso espacio-tiempo**: gasta memoria en casillas para buscar en O(1); para listar usa merge sort (**divide y vencerás**) | Arreglo dinámico de casillas, listas enlazadas, función hash, merge sort recursivo |

Esta clasificación de estrategias se sustenta en Levitin, *Introduction to the Design and Analysis of Algorithms* (reducir y vencerás para la búsqueda del BST, transformar y vencerás para el balanceo del AVL y compromiso espacio-tiempo para la hash). **Precisiones:** BST y AVL comparten el procedimiento de búsqueda; el AVL se diferencia por el rebalanceo con rotaciones tras insertar. La hash utiliza, además, divide y vencerás al listar mediante merge sort. La clasificación no implica tres paradigmas de lenguaje distintos ni elimina la posibilidad de una interpretación más estricta del requisito docente.

## Integrantes y entrega

- Germán Eduardo De Armas Castaño, T00068765
- Ángel Luis Vega Rodríguez, T00068186
- Sara Bastidas Triana, T00081674
- Daniela Carolina Ortiz Las Calles, T00070793

**Informe oficial:** `Informe_comparacion_estructuras.pdf` (9 páginas, PDF final actualizado por el equipo) y versión editable `Informe_comparacion_estructuras.docx` con la misma bibliografía y contenido académico. La paginación puede diferir entre PDF y Word. El archivo original de consigna `Trabajo Final 2026-2.pdf` se incluye como referencia.


## Compilar y ejecutar

Entra a la carpeta del programa que quieras y ejecuta su script:

```bash
./1_comparar_todas/ejecutar.sh                 # menú del comparador
./1_comparar_todas/ejecutar.sh --reporte 10000 # los 3 escenarios sin menú
./2_bst/ejecutar.sh                            # aplicación del BST
./3_avl/ejecutar.sh                            # aplicación del AVL
./4_hash/ejecutar.sh --reporte 10000           # reporte de la tabla hash
```

El script compila solo si hace falta. También se pueden compilar los cuatro con `make`.

En Windows: doble clic en el `ejecutar.bat` de la carpeta (o `ejecutar.bat --reporte 10000` desde la consola). Compila con `g++` (MSYS2/MinGW-w64 en el PATH) o, si no lo encuentra, con `cl` de Visual Studio cuando se abre desde el "Developer Command Prompt". A mano, desde la carpeta del programa:

```bat
g++ -std=c++17 -O2 -Wl,--stack,16777216 -o app_bst.exe main.cpp
```

La pila de 16 MB evita que la recursión del BST degenerado la desborde.

Compilar siempre con `-O2`: sin optimización los tiempos no son representativos.

## Menús

**Comparador (`1_comparar_todas`)**

1. **Comparar las 3 estructuras**: eliges N, el escenario y las repeticiones. Muestra una tabla de tiempo (insertar los N libros, buscar un código que existe y uno que no, listar en orden, nodos revisados por búsqueda, forma de la estructura) y una de memoria. Al final, un resumen con la más rápida, la más lenta y la que menos memoria usa.
2. **Catálogo real**: carga `datos/libros.csv` en las tres estructuras, muestra su forma y su memoria, y busca el código que escribas.
3. **Ver el código** de cualquiera de las tres estructuras (o de `memoria.h`), con números de línea.
4. **Teoría**: paradigma de cada solución, tabla de Big O, en qué destaca y en qué falla cada una.

**Aplicaciones individuales (`2_bst`, `3_avl`, `4_hash`)**

1. **Medir tiempo y memoria** de esa estructura con el N y el escenario que elijas.
2. **Catálogo real**: buscar un libro por código.
3. **Catálogo real**: listar los 50 libros ordenados por código.
4. **Ver el código** de la estructura.
5. **Teoría**: paradigma, estructuras que usa, Big O, en qué destaca y en qué falla.

Las gráficas de resultados están en `resultados_graficas.xlsx` y el análisis en el documento del proyecto.

## Memoria

Se mide reemplazando `new`/`delete` para contar los bytes que cada estructura solicita al heap (`comun/memoria.h`). Incluye los libros almacenados; **no es la RAM total del proceso**. No cuenta la pila recursiva, la sobrecarga de `malloc`, los datos de entrada ni otros gastos ajenos a la estructura. Los resultados se refieren a la compilación y representación de tipos de 64 bits usada, y pueden variar entre compiladores/plataformas.

**Búfer de salida común:** `comun/medicion.h` reserva `Libro** lista` de N punteros *antes* de contabilizar la memoria de cada estructura. En 64 bits representa **8N bytes** (156,25 KiB con N = 20 000) para cada alternativa, y no forma parte de la columna «Extra listar». La tabla hash requiere además un segundo búfer de **8N bytes** durante el merge sort; BST y AVL no solicitan ese segundo arreglo. La pila necesaria para el recorrido recursivo de los árboles tampoco se incluye en la columna.

| Columna | Qué es |
|---|---|
| Estructura (KB) / Bytes por libro | Lo que ocupa la estructura ya construida |
| Pico insertar | El máximo mientras se insertaba (en la hash incluye el momento de duplicar la tabla) |
| Extra listar | Lo que hace falta además para listar en orden: 0 en los árboles, n punteros en la hash (arreglo auxiliar del merge sort) |

Con N = 10.000 (escenario aleatorio): BST 128 B/libro, AVL 136 B/libro (guarda la altura en cada nodo), hash ≈ 140 B/libro (las casillas vacías también ocupan).

## Hardware declarado y límites de replicación

El informe original identifica un **Intel Core i5-13600K de 13.ª generación**, con Fedora Linux, g++ 16.2 y optimización `-O2`. La ficha técnica oficial del modelo especifica **14 núcleos (6 P + 8 E) y 20 hilos**, no 8 núcleos y 12 hilos. Se ha corregido ese dato en el informe y en las notas del Excel. **No se adjuntó una captura `lscpu` del equipo utilizado**, por lo cual la identidad física del procesador de origen no se ha podido comprobar de forma independiente. Fuente oficial: https://www.intel.com/content/www/us/en/products/sku/230493/intel-core-i513600k-processor-24m-cache-up-to-5-10-ghz/specifications.html

El Excel conserva **las mediciones originales**; no se han sustituido por mediciones de otro equipo. Los tiempos de nanosegundos dependen del hardware y deben interpretarse junto con la complejidad y los nodos revisados.

## Escenarios

| Escenario | Qué prueba |
|---|---|
| Aleatorio | Códigos en desorden: caso promedio de las tres |
| Ordenado | Códigos de menor a mayor, como viene el Excel: **peor caso del BST**, que se vuelve una lista |
| Adversario | Códigos múltiplos de 100·2²⁰: todos caen en la misma casilla, **peor caso de la hash** |

Los casos O(n²) (BST ordenado y hash adversaria) se omiten con N > 20.000 porque tardarían minutos. El límite se cambia en `LIMITE_CUADRATICO` dentro de `comun/medicion.h`.

## Complejidad

| Operación | BST | AVL | Tabla hash |
|---|---|---|---|
| Buscar | O(log n) prom. / **O(n)** peor | **O(log n)** siempre | **O(1)** prom. / O(n) peor |
| Insertar | O(log n) / O(n) | O(log n) | O(1) amortizado / O(n) |
| Insertar N libros | O(n log n) / O(n²) | O(n log n) | O(n) / O(n²) |
| Listar ordenado | O(n) | O(n) | **O(n log n)** (hay que ordenar con merge sort) |
| Memoria | O(n) | O(n) | O(n + casillas) |
| Recorrido: pila o búfer adicional (además de salida común) | O(h) de pila, no medida | O(log n) de pila, no medida | O(n) arreglo auxiliar en heap |

"Nodos rev." cuenta las comparaciones por búsqueda: no depende de la computadora y es la columna que debe seguir la Big O. Los tiempos en nanosegundos varían un poco entre corridas por la caché del procesador.
