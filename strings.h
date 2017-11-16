#ifndef STRINGS_H
#define STRINGS_H

#include <stdint.h>

// Realiza la copia desde el puntero src, hasta dst, indicando cuantos bytes se
// quieren copiar. Mientras se realiza la copia, se busca en src un terminador 
// 0. En caso de que no se encuentre se coloca en dst un terminador 0 al inicio.
int static inline strcpy_s(char *dest, unsigned int dmax, const char *src)
{
	char *dst = dest;

	if (dmax)
		dmax--;
	else
		return 1;

	while (*src && dmax) {
		*(dest++) = *(src++);
		dmax--;
	}

	*dest = '\0';

	if (*src) {
		reinterpret_cast<char*>(dst)[0] = '\0';
		return 1;
	}

	return 0;
}

// Realiza la comparación entre los strings. En caso de que alguno de los dos
// no tenga terminador 0, el comportamiento es indefinido y se intentará leer 
// hasta que el otro llegue a 0. En caso de que ninguno de los dos tenga el
// terminador, el comportamiento también resulta indefinido y se intentará leer
// hasta alcanzar dstSize.
int static inline strcmp_s(const char *dest, int dmax, const char *src, int *indicator)
{
	if (!dmax)
		return 1;
	
	while (*src && *dest && dmax) {
		if (*dest != *src)
			break;
		dest++;
		src++;
		dmax--;
	}

	if (indicator)
		*indicator = *dest - *src;
	else
		return 1;

	return 0;
}

#endif