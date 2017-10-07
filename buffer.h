#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>

template<uint16_t Capacity = 1024>
class Buffer {
	uint8_t buffer[Capacity];
	uint16_t tail = 0, head = 0, dim = 0;

	inline void grow(uint16_t n)
	{
		head = (head + n) % Capacity;
		dim += n;
	}

	inline void shrink(uint16_t n)
	{

		tail = (tail + n) % Capacity;
		dim -= n;
	}
public:
	Buffer() = default;
	~Buffer() = default;
	Buffer(const Buffer &) = delete;
	Buffer(Buffer &&) = delete;
	Buffer &operator=(const Buffer &) = delete;
	Buffer &operator=(Buffer &&) = delete;

	uint16_t write(const void *src, uint16_t n)
	{
		n = (dim + n > Capacity) ? Capacity - dim : n;
		const uint8_t *ptr = static_cast<const uint8_t*>(src);

		for (uint16_t i = head; i < n; i++)
			buffer[i % Capacity] = ptr[i];

		grow(n);
		return n;
	};

	uint16_t read(void *dst, uint16_t n)
	{
		n = (dim > n) ? n : dim;
		uint8_t *ptr = static_cast<uint8_t*>(dst);

		for (uint16_t i = tail; i < n; i++)
			ptr[i] = buffer[i % Capacity];

		shrink(n);
		return n;
	};

	bool size() const
	{
		return dim;
	};

	bool empty() const
	{
		return dim == 0;
	};

	bool full() const
	{
		return dim == Capacity;
	};
};

#endif