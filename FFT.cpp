#include "FFT.h"

FFT::FFT(int N, bool idft) : m_N(N), m_idft(true)
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

Complex FFT::t(unsigned int x, unsigned int N)
{
	return Complex{ cosf(-1 * PI * 2 * x / m_N), sinf(-1 * PI * 2 * x) / m_N };
}

void FFT::execute(Complex* in, int stride, int offset)
{
	for (int i = 0; i < m_N; ++i)
	{
		m_c[i] = in[m_reversed[i] * stride + offset];
	}
	int w = 0;
	for (int i = 2; i <= m_N; i *= 2)
	{
		int m = i / 2;
		for (Complex* p = m_c; p != m_c + m_N; p += i)
		{
			for (int j = 0; j < m; ++j)
			{
				Complex tw = m_t[w][j] * p[j + m];
				p[j + m] = p[j] - tw;
				p[j] += tw;
			}
		}
		++w;
	}
	for (int i = 0; i < m_N; ++i)
	{
		in[i * stride + offset] = m_c[i];
	}
}
