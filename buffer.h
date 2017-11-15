#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>

template<uint16_t Capacity = 1024>
class Buffer {
	uint8_t mBuffer[Capacity];
	uint16_t mDim = 0;

public:
	Buffer() = default;
	~Buffer() = default;
	Buffer(const Buffer &) = delete;
	Buffer(Buffer &&) = delete;
	Buffer &operator=(const Buffer &) = delete;
	Buffer &operator=(Buffer &&) = delete;

	bool write(const void *src, uint16_t n)
	{
		const uint8_t *ptr = static_cast<const uint8_t*>(src);
		if (mDim + n > Capacity)
			return false;

		for (uint16_t i = 0; i < n; i++)
			mBuffer[mDim++] = ptr[i];

		return true;
	};

	const void *data() const
	{
		return mBuffer;
	}

	uint16_t size() const
	{
		return mDim;
	};

	bool empty() const
	{
		return mDim == 0;
	};

	bool full() const
	{
		return mDim == Capacity;
	};

	void clear()
	{
		mDim = 0;
	}

	uint16_t capacity() const
	{
		return Capacity;
	}
};

#endif
