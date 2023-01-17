#include "FFT.h"

FFT::FFT(int N, bool idft) : m_N(N), m_idft(idft)
{
	m_reversed = new int[N];
	for (int i = 0; i < N; ++i)
	{
		m_reversed[i] = reverse(i);
	}
	int log2N = log(N) / log(2);
	m_t = new Complex* [log2N];
	int pow2 = 1;
	for (int i = 0; i < log2N; ++i)
	{
		m_t[i] = new Complex[pow2];
		for (int j = 0; j < pow2; ++j)
		{
			if (idft)
			{
				m_t[i][j] = t(j, pow2 * 2).conjugate();
			}
			else
			{
				m_t[i][j] = t(j, pow2 * 2);
			}
			
		}
		pow2 *= 2;
		
		
	}
	m_c = new Complex[N];
}

FFT::~FFT()
{
	delete m_c;
	int log2N = log2(m_N);
	for (int i = 0; i < log2N; ++i)
	{
		delete m_t[i];
	}
	delete m_t;
}

int FFT::reverse(int i)
{
	int res = 0;
	int log2N = log2(m_N);
	for (int j = 0; j < log2N; ++j)
	{
		if ((i >> j) & 1)
		{
			res |= (1 << (log2N - j - 1));
		}	
	}
	return res;
}
#define PI 3.1415926535f
Complex FFT::t(unsigned int x, unsigned int N)
{
	return Complex(cosf(PI * 2.f * x / N), sinf(PI * 2.f * x / N));
}
#include <iostream>
void FFT::execute(Complex* in, Complex* out, int stride, int offset)
{
	for (int i = 0; i < m_N; ++i)
	{
		int index = m_reversed[i] * stride + offset;
		m_c[i] = in[index];
	}
	int w = 0;
	for (int i = 2; i <= m_N; i *= 2)
	{
		int m = i / 2;
		for (Complex* p = m_c; p != m_c + m_N; p += i)
		{
			for (int j = 0; j < m; ++j)
			{
				auto real = m_c[j + m].real;
				
				Complex tw = m_t[w][j] * p[j + m];
				p[j + m] = p[j] - tw;
				p[j] += tw;
			}
		}
		++w;
	}
	for (int i = 0; i < m_N; ++i)
	{
		out[i * stride + offset] = m_c[i];
	}
}