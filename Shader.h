#pragma once
#include <vector>
#include "Light.h"
#include "MathUtility.h"
class Shader
{
public:
	Vector3f eye;
	std::vector<Light> ligths;
};