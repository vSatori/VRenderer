#include "OceanWave.h"
#include "../FFT/fftw/fftw3.h"
#include <QtCore/qdebug.h>
OceanWave::OceanWave(int N, int M, float A, const Vector2f& W, float length):
	m_N(N), m_M(M), m_A(A), m_W(W), m_WLength(m_W.length()), m_length(length)
{
	int size = M * N;
	wave.vertices.resize(size);
	int indexSize = (N - 1) * (M - 1) * 6;
	wave.indices.resize(indexSize);
	m_tilde0     = new std::complex<float>[M * N];
	m_conjTilde0 = new std::complex<float>[M * N];

	m_generator.seed();
	for (int j = 0; j < N; ++j)
	{
		for (int i = 0; i < M; ++i)
		{
			int index = j * M + i;
			m_tilde0[index] = computeTilde(i, j);
			m_conjTilde0[index] = std::conj(computeTilde(-i, -j));
			//qDebug() << m_tilde0[index].real() << m_tilde0[index].imag();
			
		}
	}
	int index = -1;
	for (int j = 0; j < N - 1; ++j)
	{
		for (int i = 0; i < M - 1; ++i)
		{
			wave.indices[++index] = { i + j * N, i + (j + 1) * N, (i + 1) + j * N };
			wave.indices[++index] = { (i + 1) + j * N , i + (j + 1) * N, (i + 1) + (j + 1) * N };
		}
	}

}
#include <iostream>
void OceanWave::update(float time)
{
	int size = m_M * m_N;
	using Complex = std::complex<float>;
	Complex* inTilde = new Complex[size];
	Complex* outTilde = new Complex[size];
	Complex* inSlopeX = new Complex[size];
	Complex* outSlopeX = new Complex[size];
	Complex* inSlopeZ = new Complex[size];
	Complex* outSlopeZ = new Complex[size];
	Complex* inDX = new Complex[size];
	Complex* outDX = new Complex[size];
	Complex* inDZ = new Complex[size];
	Complex* outDZ = new Complex[size];
	for (int j = 0; j < m_N; ++j)
	{
		float kz = PI * (2 * j - m_N) / m_length;
		for (int i = 0; i < m_M; ++i)
		{
			float kx = PI * (2 * i - m_M) / m_length;
			

			float len = sqrtf(kx * kx + kz * kz);
			
			int index = j * m_M + i;
			qDebug() << kx << kz << index;
			inTilde[index] = computeTilde(i, j, time);
			inSlopeX[index] = inTilde[index] * Complex(0.f, kx);
			inSlopeZ[index] = inTilde[index] * Complex(0.f, kz);
			inDX[index] = inTilde[index] * Complex(0.f, -kx / len);
			inDZ[index] = inTilde[index] * Complex(0.f, -kz / len);
			//qDebug() << inDX[index].real() << inDX[index].imag();
		}
	}
	
	fftwf_plan pTlide = fftwf_plan_dft_2d(m_M, m_N, (fftwf_complex*)inTilde, (fftwf_complex*)outTilde, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_plan pSlopeX = fftwf_plan_dft_2d(m_M, m_N, (fftwf_complex*)inSlopeX, (fftwf_complex*)outSlopeX, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_plan pSlopeZ = fftwf_plan_dft_2d(m_M, m_N, (fftwf_complex*)inSlopeZ, (fftwf_complex*)outSlopeZ, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_plan pDX = fftwf_plan_dft_2d(m_M, m_N, (fftwf_complex*)inDX, (fftwf_complex*)outDX, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_plan pDZ = fftwf_plan_dft_2d(m_M, m_N, (fftwf_complex*)inDZ, (fftwf_complex*)outDZ, FFTW_FORWARD, FFTW_ESTIMATE);

	fftwf_execute(pTlide);
	fftwf_execute(pSlopeX);
	fftwf_execute(pSlopeZ);
	fftwf_execute(pDX);
	fftwf_execute(pDZ);
	maxHeight = 0.f;
	minHeight = 0.f;
	for (int j = 0; j < m_N; ++j)
	{
		for (int i = 0; i < m_M; ++i)
		{
			int index = j * m_M + i;
			Vertex& vertex = wave.vertices[index];
			float sign = 1.f;
			if ((j + i) / 2)
			{
				sign = -1.f;
			}
			float height = sign * outTilde[index].real();
			if (maxHeight < height)
			{
				maxHeight = height;
			}
			if (minHeight > height)
			{
				minHeight = height;
			}
			vertex.pos = { (i - m_M / 2) * m_length / m_M - sign * outDX[index].real(),
							   height,
							    (j - m_N / 2) * m_length / m_N - sign * outDZ[index].real()};
			vertex.normal = { sign * outSlopeX[index].real(), 1.f, sign * outSlopeZ[index].real() };
			vertex.normal.normalize();
			//qDebug() << outDX[index].real();
		}
	}
	
	delete[] inTilde;
	delete[] outTilde;
	delete[] inSlopeX;
	delete[] outSlopeX;
	delete[] inSlopeZ;
	delete[] outSlopeZ;
	delete[] inDX;
	delete[] outDX;
	delete[] inDZ;
	delete[] outDZ;
	
}

std::complex<float> OceanWave::computeTilde(int m, int n)
{
	float a = m_normalDist(m_generator);
	float b = m_normalDist(m_generator);
	
	return std::complex<float>(a, b) * sqrtf(phillips(m, n)) * sqrtf(0.5f);
}

std::complex<float> OceanWave::computeTilde(float t, int m, int n)
{
	int index = n * m_M + m;
	float omega = disperison(m, n) * t;
	//float cosVal = cosf(omega);
	//float sinVal = sinf(omega);
	std::complex<float> c0(0.f, omega);
	std::complex<float> c1(0.f, -omega);
	return m_tilde0[index] * c0 + m_conjTilde0[index] * c1;
}

float OceanWave::phillips(int m, int n)
{
	Vector2f k{ PI * (2 * m - m_M) / m_length, PI * (2 * n - m_N) / m_length, };
	if (k.x == 0.f && k.y == 0.f)
	{
		return 0.f;
	}
	float len = k.length();
	float len2 = powf(len, 2);
	float len4 = powf(len, 4);
	k.normalize();

	float dot = k.dot(m_W);
	float dot2 = powf(dot, 2);
	float L = powf(m_WLength, 2) / m_g;
	float L2 = powf(L, 2);

	float damping = 0.001f;
	float l2 = L2 * damping * damping;

	return m_A * exp(-1.0f / (len2 * L2)) / len4 * dot2 * exp(-len2 * l2);

}

float OceanWave::disperison(int m, int n)
{
	//int w = 2.f * PI / 200.f;
	float x = PI * (2 * m - m_M) / m_length;
	float z = PI * (2 * n - m_N) / m_length;
	return floorf(sqrt(m_g * sqrt(x * x + z * z)));
}
