#pragma once
#include "libro.h"

// ALGORITMO 2: Árbol AVL (árbol binario de búsqueda AUTO-BALANCEADO).
// Igual que el BST, pero después de cada inserción revisa que la diferencia de
// alturas entre el lado izquierdo y el derecho no pase de 1. Si pasa, rota.

class NodoAVL {
public:
    long long clave;
    Libro valor;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;           // Altura del subárbol que empieza en este nodo

    NodoAVL(long long c, const Libro& v) {
        clave = c;
        valor = v;
        izquierdo = nullptr;
        derecho = nullptr;
        altura = 1;
    }
};

class ArbolAVL {
public:
    NodoAVL* raiz;
    long long comparaciones;

    ArbolAVL() {
        raiz = nullptr;
        comparaciones = 0;
    }

    ~ArbolAVL() {
        liberar(raiz);
    }

    void insertar(long long clave, const Libro& valor) {
        raiz = insertar_recursivo(raiz, clave, valor);
    }

    // La búsqueda es idéntica a la del BST: lo que cambia es que el árbol es bajo
    Libro* buscar(long long clave) {
        NodoAVL* actual = raiz;
        while (actual != nullptr) {
            comparaciones++;
            if (clave == actual->clave) {
                return &actual->valor;
            }
            if (clave < actual->clave) {
                actual = actual->izquierdo;
            } else {
                actual = actual->derecho;
            }
        }
        return nullptr;
    }

    void recorrido_inorden(Libro** resultado, int& pos) {
        inorden_recursivo(raiz, resultado, pos);
    }

    string forma() {
        return "altura=" + to_string(altura(raiz));
    }

private:
    int altura(NodoAVL* nodo) {
        if (nodo == nullptr) return 0;
        return nodo->altura;
    }

    void actualizar_altura(NodoAVL* nodo) {
        int izq = altura(nodo->izquierdo);
        int der = altura(nodo->derecho);
        if (izq > der) nodo->altura = izq + 1;
        else nodo->altura = der + 1;
    }

    // El hijo izquierdo (x) sube y el nodo (y) baja a su derecha.
    // El subárbol derecho de x pasa a ser el izquierdo de y.
    NodoAVL* rotar_derecha(NodoAVL* y) {
        NodoAVL* x = y->izquierdo;
        y->izquierdo = x->derecho;
        x->derecho = y;
        actualizar_altura(y);
        actualizar_altura(x);
        return x;
    }

    // El hijo derecho (y) sube y el nodo (x) baja a su izquierda.
    // El subárbol izquierdo de y pasa a ser el derecho de x.
    NodoAVL* rotar_izquierda(NodoAVL* x) {
        NodoAVL* y = x->derecho;
        x->derecho = y->izquierdo;
        y->izquierdo = x;
        actualizar_altura(x);
        actualizar_altura(y);
        return y;
    }

    NodoAVL* insertar_recursivo(NodoAVL* nodo, long long clave, const Libro& valor) {
        // 1. Inserción normal de BST
        if (nodo == nullptr) {
            return new NodoAVL(clave, valor);
        }
        comparaciones++;
        if (clave < nodo->clave) {
            nodo->izquierdo = insertar_recursivo(nodo->izquierdo, clave, valor);
        } else if (clave > nodo->clave) {
            nodo->derecho = insertar_recursivo(nodo->derecho, clave, valor);
        } else {
            nodo->valor = valor;
            return nodo;
        }

        // 2. Recalcular altura y factor de balance
        actualizar_altura(nodo);
        int balance = altura(nodo->izquierdo) - altura(nodo->derecho);

        // 3. Si quedó desbalanceado, rotar (4 casos)
        if (balance > 1) {                                  // Cargado a la izquierda
            if (clave > nodo->izquierdo->clave) {           // Caso izquierda-derecha
                nodo->izquierdo = rotar_izquierda(nodo->izquierdo);
            }
            return rotar_derecha(nodo);                     // Caso izquierda-izquierda
        }
        if (balance < -1) {                                 // Cargado a la derecha
            if (clave < nodo->derecho->clave) {             // Caso derecha-izquierda
                nodo->derecho = rotar_derecha(nodo->derecho);
            }
            return rotar_izquierda(nodo);                   // Caso derecha-derecha
        }
        return nodo;
    }

    void inorden_recursivo(NodoAVL* nodo, Libro** resultado, int& pos) {
        if (nodo == nullptr) return;
        inorden_recursivo(nodo->izquierdo, resultado, pos);
        resultado[pos] = &nodo->valor;
        pos++;
        inorden_recursivo(nodo->derecho, resultado, pos);
    }

    void liberar(NodoAVL* nodo) {
        if (nodo == nullptr) return;
        liberar(nodo->izquierdo);
        liberar(nodo->derecho);
        delete nodo;
    }
};
