#include "RayTracingScene.h"
#include "RenderContext.h"
#include <random>

static float random()
{
	std::random_device rd;
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	std::mt19937 gen(rd());
	float r = dist(gen);
	return r;
}

static Vector3f random_vec()
{
	Vector3f p;
	do
	{
		p = Vector3f{ random(), random(), random() } * 2 - Vector3f{ 1.f, 1.f, 1.f };
	} while (p.dot(p) >= 1.f);
	return p;
}

void RayTracingScene::render()
{
	RenderContext::clear();
	Vector3f origin{ 0.f, 0.f, 0.f };
	Vector3f horizental{ 2.f, 0.f, 0.f };
	Vector3f vertical{ 0.f, 1.5f, 0.f };
	Vector3f lbCorner = origin - horizental / 2.f + vertical / 2.f - Vector3f{ 0.f, 0.f, 1.f };

	std::shared_ptr<HitSphere> sp1 = std::make_shared<HitSphere>();
	sp1->center = Vector3f{ 0.f, 0.f, -1.f };
	sp1->radius = 0.3f;
	LAmbiention* sp1Ma = new LAmbiention;
	sp1Ma->albedo = Vector3f{ 0.8f, 0.3f, 0.3f };
	sp1->material = sp1Ma;

	std::shared_ptr<HitSphere> sp2 = std::make_shared<HitSphere>();
	sp2->center = Vector3f{ 0.f, -100.3f, -1.f };
	sp2->radius = 100.f;
	LAmbiention* sp2Ma = new LAmbiention;
	sp2Ma->albedo = Vector3f{ 0.8f, 0.8, 0.0f };
	sp2->material = sp2Ma;

	std::shared_ptr<HitSphere> sp3 = std::make_shared<HitSphere>();
	sp3->center = Vector3f{ 0.8f, 0.f, -1.f };
	sp3->radius = 0.3f;
	Metal* sp3Ma = new Metal;
	sp3Ma->albedo = Vector3f{ 0.8f, 0.6f, 0.2f };
	sp3Ma->fuzz = 0.3f;
	sp3->material = sp3Ma;

	std::shared_ptr<HitSphere> sp4 = std::make_shared<HitSphere>();
	sp4->center = Vector3f{ -0.8f, 0.f, -1.f };
	sp4->radius = 0.3f;
	Metal* sp4Ma = new Metal;
	sp4Ma->albedo = Vector3f{ 0.8f, 0.8f, 0.8f };
	sp4Ma->fuzz = 1.f;
	sp4->material = sp4Ma;

	HitableList hits;
	hits.hitables.push_back(sp1);
	hits.hitables.push_back(sp2);
	hits.hitables.push_back(sp3);
	hits.hitables.push_back(sp4);


	for (int y = 0; y < RenderContext::height; ++y)
	{
		for (int x = 0; x < RenderContext::width; ++x)
		{
			Vector3f color;
			float v = (float)y / float(RenderContext::height);
			float u = (float)x / float(RenderContext::width);
			Vector3f dir = lbCorner + horizental * u - vertical * v - origin;
			Ray ray{ origin,  dir };
			color += hitColor(ray, hits);
			color.x = sqrt(color.x);
			color.y = sqrt(color.y);
			color.z = sqrt(color.z);
			int red   = static_cast<int>(color.x * 255.f);
			int green = static_cast<int>(color.y * 255.f);
			int blue  = static_cast<int>(color.z * 255.f);
			int colorValue = (red << 16) + (green << 8) + blue;
			RenderContext::renderTarget[y * RenderContext::width + x] = colorValue;
		}
	}
}

bool hit(const Vector3f& center, float radius, const Ray& ray)
{
	Vector3f oc = ray.origin - center;
	float a = ray.direction.dot(ray.direction);
	float b = 2.f * oc.dot(ray.direction);
	float c = oc.dot(oc) - radius * radius;
	float d = b * b - 4 * a * c;
	return d > 0;
}

Vector3f hitColor(const Ray& ray, const HitableList& hits)
{
	HitRecord record;
	if (hits.hit(ray, 0.001f, std::numeric_limits<float>::max(), record))
	{
		Ray scattered;
		Vector3f attenuation;
		if (record.material->scatter(ray, record, attenuation, scattered))
		{
			return hitColor(scattered, hits) * attenuation;
		}
		Vector3f target = record.p + record.n + random_vec();
		//Vector3f vec{ record.n.x + 1, record.n.y + 1 , record.n.z + 1 };
		
		return hitColor(Ray{ record.p, target - record.p }, hits) * 0.5f;
	}
	Vector3f dir = ray.direction;
	dir.normalize();
	float t = 0.5f * (dir.y + 1.f);
	Vector3f c1{ 1.f, 1.f, 1.f };
	Vector3f c2{ 0.5f, 0.7f, 1.f };
	return c1 * (1.f - t) + c2 * t;
}

bool HitSphere::hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const
{
	Vector3f oc = ray.origin - center;
	float a = ray.direction.dot(ray.direction);
	float hb = oc.dot(ray.direction);
	float c = oc.dot(oc) - radius * radius;
	float d = hb * hb - a * c;
	if (d < 0)
	{
		return false;
	}
	float sqrtd = sqrt(d);
	float root = (-hb - sqrtd) / a;
	if (root < tMin || root > tMax)
	{
		root - (-hb + sqrtd) / a;
		if (root < tMin || root > tMax)
		{
			return false;
		}
	}
	record.t = root;
	record.p = ray.at(root);
	record.material = material;
	Vector3f n = (record.p - center) / radius;
	record.setNormal(ray, n);
	return true;
}

bool HitableList::hit(const Ray& ray, float tMin, float tMax, HitRecord& record) const
{
	HitRecord tempRecord;
	bool hitAnything = false;
	float tFar = tMax;
	int size = hitables.size();
	for (auto& hitable : hitables)
	{
		if (hitable->hit(ray, tMin, tFar, tempRecord))
		{
			tFar = tempRecord.t;
			record = tempRecord;
			hitAnything = true;

		}
	}
	return hitAnything;
}

bool LAmbiention::scatter(const Ray& ray, const HitRecord& record, Vector3f& attenuation, Ray& scatter) const
{
	Vector3f target = record.p + record.n + random_vec();
	//Vector3f vec{ record.n.x + 1, record.n.y + 1 , record.n.z + 1 };

	scatter = Ray{ record.p, target - record.p };
	attenuation = albedo;
	return true;
}

bool Metal::scatter(const Ray& ray, const HitRecord& record, Vector3f& attenuation, Ray& scatter) const
{
	Vector3f dir = ray.direction;
	dir.normalize();
	Vector3f reflected = dir.reflect(record.n);
	scatter = Ray{record.p, reflected + random_vec() * fuzz};
	attenuation = albedo;
	return scatter.direction.dot(record.n) > 0;
}
