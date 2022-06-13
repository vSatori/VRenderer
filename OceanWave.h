#pragma once
#include <complex>
#include <random>
#include "Mesh.h"
class OceanWave
{
public:
	OceanWave(int N, int M, float A, const Vector2f& W, float length);
public:
	void update(float time);
public:
	Mesh wave;
	float maxHeight;
	float minHeight;
private:
	std::complex<float> computeTilde(int m, int n);
	std::complex<float> computeTilde(float t, int m, int n);
	float phillips(int m, int n);
	float disperison(int m, int n);
	
private:
	const float m_g = 9.8f;
	int m_N;
	int m_M;
	float m_A;
	Vector2f m_W;
	float m_WLength;
	float m_length;
	std::complex<float>* m_tilde0;
	std::complex<float>* m_conjTilde0;

	std::default_random_engine m_generator;
	std::normal_distribution<float> m_normalDist;

};

