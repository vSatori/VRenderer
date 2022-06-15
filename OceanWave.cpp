#include "OceanWave.h"
OceanWave::OceanWave(int N, int M, float A, float length, const Vector2f& wind, float V):
	m_N(N), m_M(M), m_A(A), m_length(length), m_wind(wind), m_V(V)
{
	int size = N * M;
	wave.vertices.resize(size);
	m_tilde0     = new Complex[size];
	m_conjTilde0 = new Complex[size];

	m_generator.seed();
	for (int m = 0; m < M; ++m)
	{
		for (int n = 0; n < N; ++n)
		{
			int index = m * N + n;
			m_tilde0[index] = computeTilde(n, m);
			m_conjTilde0[index] = computeTilde(n, m).conjugate();
		}
	}
	int index = -1;
	for (int m = 0; m < M - 1; ++m)
	{
		for (int n = 0; n < N - 1; ++n)
		{
			int index = m * N + n;
			
			wave.indices.push_back({ index, index + N, index + N + 1 });
			wave.indices.push_back({ index, index + N + 1, index + 1 });
		}
	}
	m_fftM = new FFT(M, true);
	m_fftN = new FFT(N, true);

	Material& material = wave.material;
	material.ambient = { 0.f, 0.2f, 0.6f };
	material.diffuse = { 0.f, 0.4f, 0.6f };
	material.specular = { 0.7f, 0.7f, 0.9f };
	material.shininess = 32.f;

}
OceanWave::~OceanWave()
{
	delete m_tilde0;
	delete m_conjTilde0;

	delete m_fftM;
	delete m_fftN;
}
#include <iostream>

void OceanWave::update(float time)
{
	int size = m_M * m_N;
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
	for (int m = 0; m < m_N; ++m)
	{
		float kz = PI * (2.f * m - m_N) / m_length;
		for (int n = 0; n < m_N; ++n)
		{
			float kx = PI * (2.f * n - m_N) / m_length;
			Vector2f vec{ kx, kz };
			vec.normalize();
			
			int index = m * m_N + n;
			inTilde[index]  = computeTilde(time, n, m);
			inSlopeX[index] = inTilde[index] * Complex(0.f, kx);
			inSlopeZ[index] = inTilde[index] * Complex(0.f, kz);
			inDX[index]     = inTilde[index] * Complex(0.f, vec.x);
			inDZ[index]     = inTilde[index] * Complex(0.f, vec.y);
		}
	}
	
	for (int m = 0; m < m_M; ++m)
	{
		m_fftM->execute(inTilde,  outTilde, 1, m_N * m);
		m_fftM->execute(inSlopeX, outSlopeX, 1, m_N * m);
		m_fftM->execute(inSlopeZ, outSlopeZ, 1, m_N * m);
		m_fftM->execute(inDX,     outDX, 1, m_N * m);
		m_fftM->execute(inDZ,     outDZ, 1, m_N * m);
	}

	for (int n = 0; n < m_N; ++n)
	{
		m_fftN->execute(outTilde, outTilde, m_N, n);
		m_fftN->execute(outSlopeX, outSlopeX, m_N, n);
		m_fftN->execute(outSlopeZ, outSlopeZ, m_N, n);
		m_fftN->execute(outDX, outDX, m_N, n);
		m_fftN->execute(outDZ, outDZ, m_N, n);
	}
	
	maxHeight = 0.f;
	minHeight = 0.f;
	for (int m = 0; m < m_M; ++m)
	{
		for (int n = 0; n < m_N; ++n)
		{
			int index = m * m_N + n;
			int index1 = m * (m_N) + n;
			Vertex& vertex = wave.vertices[index1];
			float sign = 1.f;
			if ((n + m) % 2)
			{
				sign = -1.f;
			}
			float height = sign * outTilde[index].real;
			if (maxHeight < height)
			{
				maxHeight = height;
			}
			if (minHeight > height)
			{
				minHeight = height;
			}
			
			
			vertex.pos = { (n - m_N / 2) * m_length / m_N + sign * outDX[index].real,
							   height,
						   (m - m_M / 2) * m_length / m_M + sign * outDZ[index].real};
						   
			vertex.normal = { sign * outSlopeX[index].imag, 1.f, sign * outSlopeZ[index].imag };
			vertex.normal.normalize();
			//qDebug() << vertex.pos.x << vertex.pos.y;
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

Complex OceanWave::computeTilde(int n, int m)
{
	float a = m_normalDist(m_generator);
	float b = m_normalDist(m_generator);
	return Complex(a, b) * sqrtf(phillips(n, m) / 2.f);
}

Complex OceanWave::computeTilde(float t, int n, int m)
{
	int index = m * (m_N)+ n;
	float omega = disperison(n, m) * t;
	float cosVal = cosf(omega);
	float sinVal = sinf(omega);
	Complex c0(cosVal, sinVal);
	Complex c1(cosVal, -sinVal);
	return m_tilde0[index] * c0 + m_conjTilde0[index] * c1;
}

float OceanWave::phillips(int n, int m)
{
	
	Vector2f k{ PI * (2.f * n - m_N) / m_length, PI * (2.f * m - m_M) / m_length};
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
	float x = PI * (2.f * n - m_N) / m_length;
	float z = PI * (2.f * m - m_M) / m_length;
	return (sqrt(m_g * sqrtf(x * x + z * z)));
}
