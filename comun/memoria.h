#pragma once
#include <cstdlib>
#include <new>

// Medición de memoria: se reemplazan los operadores new/delete globales para llevar la
// cuenta de los bytes pedidos al heap. Así se mide lo que ocupa cada estructura
// (nodos, libros, casillas, arreglos auxiliares) sin depender del sistema operativo.
// No cuenta la pila de la recursión ni la sobrecarga interna de malloc.
//
// IMPORTANTE: incluir este archivo en UN SOLO .cpp por programa (reemplaza funciones
// globales; incluirlo en dos .cpp del mismo ejecutable daría definiciones duplicadas).

// Sin esto GCC inlinea delete y da avisos falsos sobre la aritmética de la cabecera
#if defined(_MSC_VER)
#define MEMORIA_NO_INLINE __declspec(noinline)
#else
#define MEMORIA_NO_INLINE __attribute__((noinline))
#endif

namespace memoria {
long long en_uso = 0;   // Bytes pedidos al heap que aún no se liberan
long long pico = 0;     // Máximo de en_uso desde el último reiniciar_pico()

void reiniciar_pico() {
    pico = en_uso;
}

// Cada bloque guarda su tamaño en una cabecera de 16 bytes (mantiene la alineación)
const size_t CABECERA = 16;

MEMORIA_NO_INLINE void* pedir(size_t bytes) {
    void* bloque = std::malloc(bytes + CABECERA);
    if (bloque == nullptr) return nullptr;
    *(size_t*)bloque = bytes;
    en_uso += bytes;
    if (en_uso > pico) pico = en_uso;
    return (char*)bloque + CABECERA;
}

MEMORIA_NO_INLINE void devolver(void* p) {
    if (p == nullptr) return;
    void* bloque = (char*)p - CABECERA;
    en_uso -= *(size_t*)bloque;
    std::free(bloque);
}
}  // namespace memoria

void* operator new(size_t bytes) {
    void* p = memoria::pedir(bytes);
    if (p == nullptr) throw std::bad_alloc();
    return p;
}
void* operator new[](size_t bytes) {
    void* p = memoria::pedir(bytes);
    if (p == nullptr) throw std::bad_alloc();
    return p;
}
void* operator new(size_t bytes, const std::nothrow_t&) noexcept { return memoria::pedir(bytes); }
void* operator new[](size_t bytes, const std::nothrow_t&) noexcept { return memoria::pedir(bytes); }
void operator delete(void* p) noexcept { memoria::devolver(p); }
void operator delete[](void* p) noexcept { memoria::devolver(p); }
void operator delete(void* p, size_t) noexcept { memoria::devolver(p); }
void operator delete[](void* p, size_t) noexcept { memoria::devolver(p); }
void operator delete(void* p, const std::nothrow_t&) noexcept { memoria::devolver(p); }
void operator delete[](void* p, const std::nothrow_t&) noexcept { memoria::devolver(p); }
