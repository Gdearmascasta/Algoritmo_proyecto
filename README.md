# Catálogo de libros: BST vs AVL vs Tabla Hash

Mismo problema, tres soluciones: **guardar N libros y buscarlos por código** (y poder listarlos en orden).

| Archivo | Estructura |
|---|---|
| `src/arbol_bst.h` | Árbol Binario de Búsqueda sin balancear (el mismo de la app en Python) |
| `src/arbol_avl.h` | Árbol AVL (BST auto-balanceado con rotaciones) |
| `src/tabla_hash.h` | Tabla Hash con encadenamiento (`codigo % tamano`, se duplica al pasar 0.7 de carga) |
| `src/main.cpp` | Menú y mediciones de tiempo |
| `datos/libros.csv` | Los 50 libros reales de `BD_libros.xlsx` |

## Compilar y ejecutar

```bash
./ejecutar.sh                   # compila si hace falta y abre el menú
./ejecutar.sh --reporte 10000   # los 3 escenarios sin menú
```

O a mano: `make` (o `g++ -std=c++17 -O2 -o comparar src/main.cpp`) y luego `./comparar`.

En Windows: doble clic en `ejecutar.bat` (o `ejecutar.bat --reporte 10000` desde la consola). Compila con `g++` (MSYS2/MinGW-w64 en el PATH) o, si no lo encuentra, con `cl` de Visual Studio cuando se abre desde el "Developer Command Prompt". A mano:

```bat
g++ -std=c++17 -O2 -Wl,--stack,16777216 -o comparar.exe src\main.cpp
```

La pila de 16 MB evita que la recursión del BST degenerado la desborde.

Compilar siempre con `-O2`: sin optimización los tiempos no son representativos.

## Menú

1. **Comparar las 3 estructuras**: eliges N, el escenario y las repeticiones. Muestra el tiempo de insertar los N libros, el tiempo promedio de buscar un código que existe y uno que no, el tiempo de listar en orden, los nodos revisados por búsqueda y la forma de la estructura (altura del árbol, o carga y cadena más larga de la hash).
2. **Catálogo real**: carga `datos/libros.csv` en las tres estructuras y busca el código que escribas.
3. **Ver el código** de cualquiera de las tres estructuras, con números de línea.
4. **Teoría**: tabla de Big O, en qué destaca y en qué falla cada una.

Las gráficas de resultados están en `resultados_graficas.xlsx` y el análisis en el documento del proyecto.

Sin menú: `./comparar --reporte 10000` corre los tres escenarios con N = 10.000.

## Escenarios

| Escenario | Qué prueba |
|---|---|
| Aleatorio | Códigos en desorden: caso promedio de las tres |
| Ordenado | Códigos de menor a mayor, como viene el Excel: **peor caso del BST**, que se vuelve una lista |
| Adversario | Códigos múltiplos de 100·2²⁰: todos caen en la misma casilla, **peor caso de la hash** |

Los casos O(n²) (BST ordenado y hash adversaria) se omiten con N > 20.000 porque tardarían minutos. El límite se cambia en `LIMITE_CUADRATICO` dentro de `main.cpp`.

## Complejidad

| Operación | BST | AVL | Tabla hash |
|---|---|---|---|
| Buscar | O(log n) prom. / **O(n)** peor | **O(log n)** siempre | **O(1)** prom. / O(n) peor |
| Insertar | O(log n) / O(n) | O(log n) | O(1) amortizado / O(n) |
| Insertar N libros | O(n log n) / O(n²) | O(n log n) | O(n) / O(n²) |
| Listar ordenado | O(n) | O(n) | **O(n log n)** (hay que ordenar con merge sort) |

"Nodos rev." cuenta las comparaciones por búsqueda: no depende de la computadora y es la columna que debe seguir la Big O. Los tiempos en nanosegundos varían un poco entre corridas por la caché del procesador.
