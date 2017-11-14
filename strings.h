#ifndef STRINGS_H
#define STRINGS_H

#include <stdint.h>

// Realiza la copia desde el puntero src, hasta dst, indicando cuantos bytes se
// quieren copiar. Mientras se realiza la copia, se busca en src un terminador 
// 0. En caso de que no se encuentre se coloca en dst un terminador 0 al inicio.
void static inline strcpy_s(void *dst, const void *src, uint8_t dstSize)
{
	const char *csrc = reinterpret_cast<const char*>(src);
	char *cdst = reinterpret_cast<char*>(dst);

	if (dstSize)
		dstSize--;

	while (*csrc && dstSize) {
		*(cdst++) = *(csrc++);
		dstSize--;
	}

	if (*csrc)
		reinterpret_cast<char*>(dst)[0] = '\0';
}

// Realiza la comparación entre los strings. En caso de que alguno de los dos
// no tenga terminador 0, el comportamiento es indefinido y se intentará leer 
// hasta que el otro llegue a 0. En caso de que ninguno de los dos tenga el
// terminador, el comportamiento también resulta indefinido y se intentará leer
// hasta alcanzar dstSize.
bool static inline strcmp_s(void *dst, const void *src, uint8_t dstSize)
{
	#warning Implementar la comparacion de strings.
	return false;
}

#endif