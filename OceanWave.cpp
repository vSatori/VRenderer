#include "OceanWave.h"
//#include "../FFT/fftw/fftw3.h"
#include <QtCore/qdebug.h>
OceanWave::OceanWave(int N, int M, float A, float length, const Vector2f& wind, float V):
	m_N(N), m_M(M), m_A(A), m_length(length), m_wind(wind), m_V(V)
{
	int size = M * N;
	wave.vertices.resize(size);
	int indexSize = (N - 1) * (M - 1) * 6;
	wave.indices.resize(indexSize);
	m_tilde0     = new std::complex<float>[M * N];
	m_conjTilde0 = new std::complex<float>[M * N];

	m_generator.seed();
	for (int n = 0; n < N; ++n)
	{
		for (int m = 0; m < M; ++m)
		{
			int index = m * N + n;
			m_tilde0[index] = computeTilde(n, m);
			m_conjTilde0[index] = std::conj(computeTilde(n, m));
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
	m_fft = new FFT(N, true);

}
#include <iostream>

void OceanWave::update(float time)
{
	int size = m_M * m_N;
	if (size <= 0)
	{
		return;
	}
	using stdComplex = std::complex<float>;
	stdComplex* inTilde = new stdComplex[size];
	stdComplex* outTilde = new stdComplex[size];
	stdComplex* inSlopeX = new stdComplex[size];
	stdComplex* outSlopeX = new stdComplex[size];
	stdComplex* inSlopeZ = new stdComplex[size];
	stdComplex* outSlopeZ = new stdComplex[size];
	stdComplex* inDX = new stdComplex[size];
	stdComplex* outDX = new stdComplex[size];
	stdComplex* inDZ = new stdComplex[size];
	stdComplex* outDZ = new stdComplex[size];
	for (int n = 0; n < m_N; ++n)
	{
		float kx = PI * 2.f * (n - m_N / 2) / m_length;
		for (int m = 0; m < m_M; ++m)
		{
			float kz = PI * 2.f * (m - m_M / 2) / m_length;
			Vector2f vec{ kx, kz };
			vec.normalize();
			
			int index = m * m_N + n;
			inTilde[index]  = computeTilde(time, n, m);
			inSlopeX[index] = inTilde[index] * stdComplex(0.f, kx);
			inSlopeZ[index] = inTilde[index] * stdComplex(0.f, kz);
			inDX[index]     = inTilde[index] * stdComplex(0.f, vec.x);
			inDZ[index]     = inTilde[index] * stdComplex(0.f, vec.y);
		}
	}

	for (int m = 0; m < m_N; ++m)
	{
		m_fft->execute((Complex*)(inTilde), 1, m_N * m);
		m_fft->execute((Complex*)(inSlopeX), 1, m_N * m);
		m_fft->execute((Complex*)(inSlopeZ), 1, m_N * m);
		m_fft->execute((Complex*)(inDX), 1, m_N * m);
		m_fft->execute((Complex*)(inDZ), 1, m_N * m);
	}

	for (int n = 0; n < m_N; ++n)
	{
		m_fft->execute((Complex*)(inTilde), m_N, n);
		m_fft->execute((Complex*)(inSlopeX), m_N, n);
		m_fft->execute((Complex*)(inSlopeZ), m_N, n);
		m_fft->execute((Complex*)(inDX), m_N, n);
		m_fft->execute((Complex*)(inDZ), m_N, n);
	}
	/*
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
	*/
	maxHeight = 0.f;
	minHeight = 0.f;
	for (int n = 0; n < m_N; ++n)
	{
		for (int m = 0; m < m_M; ++m)
		{
			int index = m * m_N + n;
			Vertex& vertex = wave.vertices[index];
			float sign = 1.f;
			if ((n + m) % 2)
			{
				sign = -1.f;
			}
			float height = sign * inTilde[index].real();
			if (maxHeight < height)
			{
				maxHeight = height;
			}
			if (minHeight > height)
			{
				minHeight = height;
			}
			
			
			vertex.pos = { (n - m_N / 2) * m_length / m_N + sign * inDX[index].real(),
							   height,
						   (m - m_M / 2) * m_length / m_M + sign * inDZ[index].real()};
			vertex.normal = { sign * inSlopeX[index].real(), 1.f, sign * inSlopeZ[index].real() };
			vertex.normal.normalize();
			//qDebug() << vertex.pos.x << vertex.pos.y;
		}
	}
	/*
	//qDebug() << time << maxHeight;
	fftwf_destroy_plan(pTlide);
	fftwf_destroy_plan(pSlopeX);
	fftwf_destroy_plan(pSlopeZ);
	fftwf_destroy_plan(pDX);
	fftwf_destroy_plan(pDZ);
	*/
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

std::complex<float> OceanWave::computeTilde(int n, int m)
{
	float a = m_normalDist(m_generator);
	float b = m_normalDist(m_generator);
	return std::complex<float>(a, b) * sqrtf(phillips(n, m)) * sqrtf(0.5f);
}

std::complex<float> OceanWave::computeTilde(float t, int n, int m)
{
	int index = m * m_N + n;
	float omega = disperison(n, m) * t;
	float cosVal = cosf(omega);
	float sinVal = sinf(omega);
	std::complex<float> c0(cosVal, sinVal);
	std::complex<float> c1(cosVal, -sinVal);

	return m_tilde0[index] * c0 + m_conjTilde0[index] * c1;
}

float OceanWave::phillips(int n, int m)
{
	
	Vector2f k{ PI * 2.f * (n - m_N / 2) / m_length, PI * 2.f * (m - m_M / 2) / m_length, };
	if (k.x == 0.f && k.y == 0.f)
	{
		return 0.f;
	}
	float len = k.length();
	float len2 = powf(len, 2);
	float len4 = powf(len, 4);
	k.normalize();

	float dot = k.dot(m_wind);
	float dot2 = powf(dot, 2);
	float L = powf(m_V, 2) / m_g;
	float L2 = powf(L, 2);

	float damping = 0.001f;
	float l2 = L2 * damping * damping;
	//float l2 = powf(0.1f, 2);

	return m_A * exp(-1.0f / (len2 * L2)) / len4 * dot2 * exp(-len2 * l2);


}

float OceanWave::disperison(int n, int m)
{
	//int w = 2.f * PI / 200.f;
	float x = PI * 2.f * (n - m_N / 2) / m_length;
	float z = PI * 2.f * (m - m_M / 2) / m_length;
	return (sqrt(m_g * sqrtf(x * x + z * z)));
}
