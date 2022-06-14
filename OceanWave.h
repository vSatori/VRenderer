#pragma once
#include <complex>
#include <random>
#include "Mesh.h"
#include "FFT.h"
class OceanWave
{
public:
	OceanWave(int N, int M, float A, float length, const Vector2f& W, float V);
public:
	void update(float time);
public:
	Mesh wave;
	float maxHeight;
	float minHeight;
private:
	std::complex<float> computeTilde(int n, int m);
	std::complex<float> computeTilde(float t, int n, int m);
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
	FFT* m_fft;
	std::complex<float>* m_tilde0;
	std::complex<float>* m_conjTilde0;

	std::default_random_engine m_generator;
	std::normal_distribution<float> m_normalDist;

};

