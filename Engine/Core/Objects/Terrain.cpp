#include "Terrain.h"

namespace Noise {
	/* coherent noise function over 1, 2 or 3 dimensions */
	/* (copyright Ken Perlin) */

	#include <stdlib.h>
	#include <stdio.h>
	#include <math.h>

	#define B 0x100
	#define BM 0xff

	#define N 0x1000
	#define NP 12   /* 2^N */
	#define NM 0xfff

	static int p[B + B + 2];
	static float g3[B + B + 2][3];
	static float g2[B + B + 2][2];
	static float g1[B + B + 2];
	static int start = 1;

	static void init(void);

	#define s_curve(t) ( t * t * (3. - 2. * t) )

	#define lerp(t, a, b) ( a + t * (b - a) )

	#define setup(i,b0,b1,r0,r1)\
		t = vec[i] + N;\
		b0 = ((int)t) & BM;\
		b1 = (b0+1) & BM;\
		r0 = t - (int)t;\
		r1 = r0 - 1.;

		double noise1(double arg)
		{
			int bx0, bx1;
			float rx0, rx1, sx, t, u, v, vec[1];

			vec[0] = arg;
			if (start) {
				start = 0;
				init();
			}

			setup(0, bx0, bx1, rx0, rx1);

			sx = s_curve(rx0);

			u = rx0 * g1[p[bx0]];
			v = rx1 * g1[p[bx1]];

			return lerp(sx, u, v);
		}

		float noise2(float vec[2])
		{
			int bx0, bx1, by0, by1, b00, b10, b01, b11;
			float rx0, rx1, ry0, ry1, * q, sx, sy, a, b, t, u, v;
			int i, j;

			if (start) {
				start = 0;
				init();
			}

			setup(0, bx0, bx1, rx0, rx1);
			setup(1, by0, by1, ry0, ry1);

			i = p[bx0];
			j = p[bx1];

			b00 = p[i + by0];
			b10 = p[j + by0];
			b01 = p[i + by1];
			b11 = p[j + by1];

			sx = s_curve(rx0);
			sy = s_curve(ry0);

	#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

			q = g2[b00]; u = at2(rx0, ry0);
			q = g2[b10]; v = at2(rx1, ry0);
			a = lerp(sx, u, v);

			q = g2[b01]; u = at2(rx0, ry1);
			q = g2[b11]; v = at2(rx1, ry1);
			b = lerp(sx, u, v);

			return lerp(sy, a, b);
		}

		float noise3(float vec[3])
		{
			int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
			float rx0, rx1, ry0, ry1, rz0, rz1, * q, sy, sz, a, b, c, d, t, u, v;
			int i, j;

			if (start) {
				start = 0;
				init();
			}

			setup(0, bx0, bx1, rx0, rx1);
			setup(1, by0, by1, ry0, ry1);
			setup(2, bz0, bz1, rz0, rz1);

			i = p[bx0];
			j = p[bx1];

			b00 = p[i + by0];
			b10 = p[j + by0];
			b01 = p[i + by1];
			b11 = p[j + by1];

			t = s_curve(rx0);
			sy = s_curve(ry0);
			sz = s_curve(rz0);

	#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

			q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
			q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
			a = lerp(t, u, v);

			q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0);
			q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0);
			b = lerp(t, u, v);

			c = lerp(sy, a, b);

			q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1);
			q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1);
			a = lerp(t, u, v);

			q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1);
			q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1);
			b = lerp(t, u, v);

			d = lerp(sy, a, b);

			return lerp(sz, c, d);
		}

		static void normalize2(float v[2])
		{
			float s;

			s = sqrt(v[0] * v[0] + v[1] * v[1]);
			v[0] = v[0] / s;
			v[1] = v[1] / s;
		}

		static void normalize3(float v[3])
		{
			float s;

			s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
			v[0] = v[0] / s;
			v[1] = v[1] / s;
			v[2] = v[2] / s;
		}

		static void init(void)
		{
			int i, j, k;

			for (i = 0; i < B; i++) {
				p[i] = i;

				g1[i] = (float)((rand() % (B + B)) - B) / B;

				for (j = 0; j < 2; j++)
					g2[i][j] = (float)((rand() % (B + B)) - B) / B;
				normalize2(g2[i]);

				for (j = 0; j < 3; j++)
					g3[i][j] = (float)((rand() % (B + B)) - B) / B;
				normalize3(g3[i]);
			}

			while (--i) {
				k = p[i];
				p[i] = p[j = rand() % B];
				p[j] = k;
			}

			for (i = 0; i < B + 2; i++) {
				p[B + i] = p[i];
				g1[B + i] = g1[i];
				for (j = 0; j < 2; j++)
					g2[B + i][j] = g2[i][j];
				for (j = 0; j < 3; j++)
					g3[B + i][j] = g3[i][j];
			}
		}
}

Terrain::Terrain()
{
	isTexture = false;
	Heightmap = nullptr;

	resolution = 0;
	noise_scale = 0.010f;
	amplitude = 10.0f;

	Mesh = SpawnObject<VisibleObject>();
}

void Terrain::LoadWithParameters(const String& args)
{
	BaseObject::LoadWithParameters(args);
	auto data = ParseOptions(args);

	auto res = data.find("Resolution");
	auto sca = data.find("Scale");
	auto loc = data.find("Location");
	auto mat = data.find("Material");
	int Resolution = 100;
	Vector Scale = Vector(1.f);
	Vector Location = Vector();
	String Material = "Assets/Materials/ground";
	if (res != data.end()) Resolution = atoi(res->second.c_str());
	if (sca != data.end()) Scale = Vector(sca->second);
	if (loc != data.end()) Location = Vector(loc->second);
	if (mat != data.end()) Material = mat->second;

	InitTerrain(Resolution, Scale, Location, Material);
}

void Terrain::InitTerrain(int r, Vector scale, Vector location, String material)
{
	resolution = r;
	Scale = scale / r;
	Mesh->SetLocation(location);

	std::vector<Vector> pos;
	std::vector<Vector> uvs;
	std::vector<Vector> normals;
	std::vector<Vector> tangents;
	std::vector<uint32> inds;

	pos.reserve(r * r);

	Vector offset = scale / 2;
	offset.Z = 0.f;

	for (int32 y = 0; y < r + 1; y++) {
		for (int32 x = 0; x < r + 1; x++) {
			pos.emplace_back(Vector(Scale.X * x, Scale.Y * y, GetHeight(Scale.X * x + location.X, Scale.Y * y + location.Y)));
			uvs.emplace_back(Vector((x - 1) / (float)r, (y - 1) / (float)r, 0.f) * 10.f);
			Vector normal = GetNormal(Scale.X * x + location.X, Scale.Y * y + location.Y);
			normals.emplace_back(normal);
			Vector tangent;
			Vector t1 = Vector::Cross(normal, Vector(1.f, 0.f, 0.f));
			Vector t2 = Vector::Cross(normal, Vector(0.f, 1.f, 0.f));
			if (t1.Length() > t2.Length())
			{
				tangent = t1;
			}
			else
			{
				tangent = t2;
			}

			tangents.emplace_back(tangent);
			if (y < r && x < r) {
				inds.emplace_back(y * (r + 1) + x);
				inds.emplace_back((y + 1) * (r + 1) + x);
				inds.emplace_back((y + 1) * (r + 1) + x + 1);

				inds.emplace_back(y * (r + 1) + x);
				inds.emplace_back((y + 1) * (r + 1) + x + 1);
				inds.emplace_back(y * (r + 1) + x + 1);
			}
		}
	}

	normals.resize(pos.size());
	tangents.resize(pos.size());

	Mesh->SetModel(MI->CreateProcedural(Mesh, "Terrain_" + std::to_string(GetRecord()), pos, uvs, normals, tangents, inds));
	Mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName(material == "" ? "Assets/Materials/ground" : material));
}

float Terrain::GetHeight(float x, float y)
{
	x *= noise_scale;
	y *= noise_scale;
	float arr[] = { x, y };
	return Noise::noise2(arr) * amplitude + Mesh->GetLocation().Z;
}

Vector Terrain::GetNormal(float x, float y)
{
	Vector first(x - 0.1f, y - 0.1f, 0.f);
	Vector second(x + 0.1f, y + 0.1f, 0.f);

	Vector x_dir(0.2f, 0.f, GetHeight(second.X, y) - GetHeight(first.X, y));
	Vector y_dir(0.f, 0.2f, GetHeight(x, second.Y) - GetHeight(x, first.Y));

	return Vector::Cross(x_dir.Normalize(), y_dir.Normalize());
}
