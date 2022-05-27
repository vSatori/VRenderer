#pragma once



#include <random>
#include <complex>
#include "MathUtil.h"
using namespace std;


class Wave
{
public:
	// N, M:      Resolution
	// L_x, L_z:  Actual lengths of the grid (as in meters)
	// omega_hat: Direction of wind
	// V:         Speed of wind	
	Wave(int N, int M, float L_x, float L_z, Vector2f omega, float V, float A, float lambda);
	~Wave();
	void buildField(float time);

	Vector3f* heightField;
	Vector3f* normalField;

private:
	complex<float> *value_h_twiddle_0 = NULL;
	complex<float> *value_h_twiddle_0_conj = NULL;
	complex<float> *value_h_twiddle = NULL;

	default_random_engine generator;
	normal_distribution<float> normal_dist;

	const float pi = float(PI);
	const float g = 9.8f; // Gravitational constant
	const float l = 0.1;
	float A;
	float V;
	int N, M;
	int kNum;
	Vector2f omega_hat;
	float lambda;
	float L_x, L_z;	

	inline float func_omega(float k) const;
	inline float func_P_h(Vector2f vec_k) const;
	inline complex<float> func_h_twiddle_0(Vector2f vec_k);
	inline complex<float> func_h_twiddle(int kn, int km, float t) const;
};
