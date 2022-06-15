#pragma once
#include <complex>
#include <random>
#include "Mesh.h"
#include "FFT.h"
class OceanWave
{
public:
	OceanWave(int N, int M, float A, float length, const Vector2f& W, float V);
	~OceanWave();
public:
	void update(float time);
public:
	Mesh wave;
	float maxHeight;
	float minHeight;
private:
	Complex computeTilde(int n, int m);
	Complex computeTilde(float t, int n, int m);
	float phillips(int n, int m);
	float disperison(int n, int m);
private:
	const float m_g = 9.8f;
	int m_N;
	int m_M;
	float m_A;
	Vector2f m_wind;
	float m_V;
	float m_length;
	FFT* m_fftM;
	FFT* m_fftN;
	Complex* m_tilde0;
	Complex* m_conjTilde0;
	std::default_random_engine m_generator;
	std::normal_distribution<float> m_normalDist;
};

