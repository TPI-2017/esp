#pragma once

template<typename T>
class ModuloNumber {
public:
	ModuloNumber<T>(T initial, T max, T min = 0)
	: mNumber(initial),
	  mMax(max),
	  mMin(min)
	{
	}

	ModuloNumber &operator++(int dummy)
	{
		if (mNumber == mMax)
			mNumber = mMin;
		else
			mNumber++;
		return *this;
	}

	ModuloNumber &operator--(int dummy)
	{
		if (mNumber == mMin)
			mNumber = mMax;
		else
			mNumber--;
		return *this;
	}

	T value()
	{
		return mNumber;
	}
private:
	T mNumber;
	T mMax;
	T mMin;
};
