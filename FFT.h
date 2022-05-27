#pragma once
#include <memory>
class FFT
{
};


struct Complex
{
public:
	inline Complex operator+(const Complex& other)
	{
		Complex res;
		res.a = a + other.a;
		res.b = b + other.b;
		return res;
	}
	inline Complex operator-(const Complex& other)
	{
		Complex res;
		res.a = a - other.a;
		res.b = b - other.b;
		return res;
	}
	inline Complex operator*(const Complex& other)
	{
		Complex res;
		res.a = a * other.a - b * other.b;
		res.b = b * other.a + a * other.b;
		return res;
	}
	inline Complex operator/(const Complex& other)
	{
		Complex res;
		float d = (other.a * other.a + other.b * other.b);
		res.a = (a * other.a + b * other.b) / d;
		res.b = (b * other.a - a * other.b) / d;
		return res;
	}
	inline Complex operator+=(const Complex& other)
	{
		a += other.a;
		b += other.b;
	}
	inline Complex operator-=(const Complex& other)
	{
		a -= other.a;
		b -= other.b;
	}
	inline Complex operator*=(const Complex& other)
	{
		a = a * other.a - b * other.b;
		b = b * other.a + a * other.b;
	}
	inline Complex operator/=(const Complex& other)
	{
		float d = (other.a * other.a + other.b * other.b);
		a = (a * other.a + b * other.b) / d;
		b = (b * other.a - a * other.b) / d;
	}
	inline Complex conjugate()
	{
		return Complex{ a, -b };
	}


public:
	float a;
	float b;
};
#define PI 3.1415926535
#include <math.h>
Complex omega(int n, int k, bool inverse = false)
{
	Complex res;
	res.a = cosf(2 * PI / n * k);
	res.b = sinf(2 * PI / n * k);
	if (inverse)
	{
		return res;
	}
	else
	{
		return res.conjugate();
	}
}

#define N 10
void fft(Complex complex[], int n)
{
	if (n == 1)
	{
		return;
	}

	static Complex buff[N];

	int m = n >> 1;

	for (int i = 0; i < m; ++i)
	{
		buff[i] = complex[i << 1];
		buff[i + m] = complex[i << 1 | 1];
	}

	memcpy(complex, buff, sizeof(Complex) * (n + 1));

	Complex* a1 = complex; 
	Complex* a2 = complex + m;
	fft(a1, m);
	fft(a2, m);

	for (int i = 0; i< m; ++i)
	{
		Complex t = omega(n, i);
		buff[i] = a1[i] + t * a2[i];
		buff[i + m] = a1[i] - t * a2[i];
	}

	memcpy(complex, buff, sizeof(Complex) * (n + 1));

}


