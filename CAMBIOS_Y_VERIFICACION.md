# Actualización para entrega, 17 de noviembre de 2026

**Documento definitivo:** `Informe_comparacion_estructuras.pdf` (9 páginas, última versión enviada por el equipo) y `Informe_comparacion_estructuras.docx` (versión editable, con bibliografía sincronizada).

## Cambios
1. Portada: cuatro autores y sus códigos completos.
2. Especificación del modelo Intel Core i5-13600K: 14 núcleos (6 P + 8 E) y 20 hilos, comprobados en la ficha técnica oficial. La identificación física del procesador de origen no está verificada, porque no se adjuntó captura `lscpu` del equipo donde se obtuvieron los datos.
3. Memoria: los bytes mostrados representan las solicitudes al heap atribuibles a cada estructura; no equivalen a la RAM total del proceso. La memoria adicional al listar excluye el arreglo de salida común de N punteros (8N bytes en 64 bits), reservado para las tres; la tabla hash solicita además otro arreglo de N punteros para merge sort. La pila de recursión y la sobrecarga interna del asignador no se incluyen.
4. Paradigmas: se especifica que son **estrategias de diseño de algoritmos**, no paradigmas de lenguaje. BST: reducir y vencerás durante la búsqueda; AVL: transformar y vencerás durante el rebalanceo; hash: compromiso espacio-tiempo, con divide y vencerás adicional en merge sort. El BST y AVL comparten la lógica de búsqueda; el AVL es diferente en su política de mantenimiento.
5. Se añadió la fuente oficial de Intel y se revisó el alcance de las conclusiones y de las limitaciones de las mediciones.
6. Las notas del Excel y la explicación en pantalla de las aplicaciones coinciden ahora con la metodología del informe.
7. Se retiró de **este paquete** el borrador secundario `BST, AVL y tabla hash...docx` que contenía una portada anterior de dos autores. El resto de la estructura del repositorio se conserva.

## Comprobaciones
- Compilación: los cuatro ejecutables compilaron con `make` y g++ C++17 -O2 -Wall en Linux, sin errores.
- Humo de ejecución: comparador, BST, AVL y hash ejecutaron `--reporte 100` en los tres escenarios sin indicadores de error de resultados.
- Diagramas: se conservaron las 8 gráficas del informe y las 6 gráficas editables de Excel.
- No se sustituyeron las mediciones originales ni se cambió la implementación algorítmica. Solo se añadieron aclaraciones en la salida de consola y en comentarios.

**Fuentes externas comprobadas:**
- Intel: https://www.intel.com/content/www/us/en/products/sku/230493/intel-core-i513600k-processor-24m-cache-up-to-5-10-ghz/specifications.html
- Pearson (Levitin): https://www.pearson.com/en-us/subject-catalog/p/Levitin-Introduction-to-the-Design-and-Analysis-of-Algorithms-3rd-Edition/P200000003403

**Advertencia académica:** La interpretación de «paradigmas de programación» de la consigna podría ser más estricta que «estrategias de diseño algorítmico» y depende del criterio docente. La propuesta incluye una justificación teórica verificable, pero no permite garantizar la aceptación de esa equivalencia.

**Última actualización del equipo:** se sustituyó el PDF del informe por la versión más reciente recibida y se sincronizó la bibliografía del Word. No se modificaron programas, mediciones ni gráficas en esta actualización.
