#pragma once
#include "Scene.h"
class Ray;
struct HitRecord;
class RtMaterial;
bool hit(const Vector3f& center, float radius, const Ray& ray);
class Ray
{
public:
	inline Vector3f at(float t)const
	{
		return origin + direction * t;
	}

	inline Vector3f color()const
	{
		if (hit({ 0.f, 0.f, -1.f, }, 0.5f, *this))
		{
			return Vector3f{ 1.f, -0.f, 0.f };
		}
		Vector3f dir = direction;
		dir.normalize();
		float t = 0.5f * (dir.y + 1.f);
		Vector3f c1{ 1.f, 1.f, 1.f };
		Vector3f c2{ 0.5f, 0.7f, 1.f };
		return c1 * (1.f - t) + c2 * t;
	}
public:
	Vector3f origin;
	Vector3f direction;
};

struct HitRecord
{
	Vector3f p;
	Vector3f n;
	float t;
	bool front;
	RtMaterial* material;

	inline void setNormal(const Ray& ray, const Vector3f& out)
	{
		front = ray.direction.dot(out);
		n = front ? out : out * -1;
	}
};

class Hitable
{
public:
	virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const = 0;
};

class HitableList : public Hitable
{
public:
	virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
public:
	std::vector<std::shared_ptr<Hitable>> hitables;

};



class HitSphere : public Hitable
{
public:
	virtual bool hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const override;
public:
	Vector3f center;
	float radius;
	RtMaterial* material;
};
Vector3f hitColor(const Ray& ray, const HitableList& hits);


class RtMaterial
{
public:
	virtual bool scatter(const Ray& ray, const HitRecord& record, Vector3f& attenuation, Ray& scatter) const = 0;
};

class LAmbiention : public RtMaterial
{
public:
	virtual bool scatter(const Ray& ray, const HitRecord& record, Vector3f& attenuation, Ray& scatter) const;
public:
	Vector3f albedo;
};

class Metal : public RtMaterial
{
public:
	virtual bool scatter(const Ray& ray, const HitRecord& record, Vector3f& attenuation, Ray& scatter) const;
public:
	Vector3f albedo;
	float fuzz;

};


class RayTracingScene : public Scene
{
public:
	virtual void render() override;
};



