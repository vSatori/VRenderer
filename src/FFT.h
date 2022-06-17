#pragma once
#include <memory>


struct Complex
{
public:
	Complex() {}
	Complex(float a, float b) : real(a), imag(b) {}
public:
	inline Complex operator*(float t)
	{
		Complex res;
		res.real = real *= t;
		res.imag = real *= t;
		return res;
	}
	inline Complex operator+(const Complex& other)
	{
		Complex res;
		res.real = real + other.real;
		res.imag = imag + other.imag;
		return res;
	}
	inline Complex operator-(const Complex& other)
	{
		Complex res;
		res.real = real - other.real;
		res.imag = imag - other.imag;
		return res;
	}
	inline Complex operator*(const Complex& other)
	{
		Complex res;
		res.real = real * other.real - imag * other.imag;
		res.imag = imag * other.real + real * other.imag;
		return res;
	}
	inline Complex operator/(const Complex& other)
	{
		Complex res;
		float d = (other.real * other.real + other.imag * other.imag);
		res.real = (real * other.real + imag * other.imag) / d;
		res.imag = (imag * other.real - real * other.imag) / d;
		return res;
	}
	inline Complex operator+=(const Complex& other)
	{
		real += other.real;
		imag += other.imag;
		return *this;
	}
	inline Complex operator-=(const Complex& other)
	{
		real -= other.real;
		imag -= other.imag;
		return *this;
	}
	inline Complex operator*=(const Complex& other)
	{
		real = real * other.real - imag * other.imag;
		imag = imag * other.real + real * other.imag;
		return *this;
	}
	inline Complex operator/=(const Complex& other)
	{
		float d = (other.real * other.real + other.imag * other.imag);
		real = (real * other.real + imag * other.imag) / d;
		imag = (imag * other.real - real * other.imag) / d;
		return *this;
	}
	inline Complex conjugate()
	{
		return Complex(real, -imag );
	}


public:
	float real = 0.f;
	float imag = 0.f;
};


class FFT
{
public:
	FFT(int N, bool idft);
	~FFT();
public:
	int reverse(int i);
	Complex t(unsigned int x, unsigned int N);
	void execute(Complex* in, Complex* out, int stride, int offset);
private:
	int m_N;
	bool m_idft;
	int* m_reversed;
	Complex* m_c;
	Complex** m_t;
};