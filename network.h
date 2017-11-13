#ifndef NETWORK_H
#define NETWORK_H

uint16_t static inline ntohs(uint16_t s)
{
	return (((0xFF) & s) << 8) | (((0xFF00) & s) >> 8);
}

uint32_t static inline ntohl(uint32_t l)
{
	return ntohs((l & 0xFFFF0000) >> 16) | (ntohs(l & 0xFFFF) << 16);
}

uint16_t static inline htons(uint16_t s)
{
	return ntohs(s);
}

uint32_t static inline htonl(uint32_t l)
{
	return ntohl(l);
}

#endif