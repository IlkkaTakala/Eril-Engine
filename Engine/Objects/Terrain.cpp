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

#pragma optimize("", off)
void Terrain::InitTerrain(int r, Vector scale, Vector location)
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
			normals.emplace_back(Vector(0.f, 0.f, 1.f));
			tangents.emplace_back(Vector(1.f, 0.f, 0.f));
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
	Mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/Materials/ground"));
}
//
//float fract(float p)
//{
//	return p - floor(p);
//}
//
//float hash1(float px, float py)
//{
//	px = 50.0 * fract(px * 0.3183099);
//	py = 50.0 * fract(py * 0.3183099);
//	return fract(px * py * (px + py));
//}
//
//float clamp(float x, float lowerlimit, float upperlimit) {
//	if (x < lowerlimit)
//		x = lowerlimit;
//	if (x > upperlimit)
//		x = upperlimit;
//	return x;
//}
//
//float smoothstep(float edge0, float edge1, float x) {
//	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
//	return x * x * (3 - 2 * x);
//}
//
//float noise(float x, float y)
//{
//	float px = floor(x);
//	float py = floor(x);
//	float wx = fract(x);
//	float wy = fract(x);
//
//	float ux = wx * wx * wx * (wx * (wx * 6.0 - 15.0) + 10.0);
//	float uy = wy * wy * wy * (wy * (wy * 6.0 - 15.0) + 10.0);
//
//	float a = hash1(px, py);
//
//	float b = hash1(px + 1, py + 0);
//
//	float c = hash1(px + 0, py + 1);
//
//	float d = hash1(px + 1, py + 1);
//
//	return -1.0 + 2.0 * (a + (b - a) * ux + (c - a) * uy + (a - b - c + d) * ux * uy);
//}
//
//float fbm_9(float x, float y)
//{
//	float f = 1.9;
//	float s = 0.55;
//	float a = 0.0;
//	float b = 0.5;
//	for (int i = 0; i < 9; i++)
//	{
//		float n = noise(x, y);
//		a += b * n;
//	}
//	return a;
//}

float Terrain::GetHeight(float x, float y)
{
	//const float sca = 0.010;
	//const float amp = 10.0;
	//x *= sca;
	//y *= sca;
	//float e = fbm_9( x + 1.0, y + -2.0);
	//float a = 1.0 - smoothstep(0.12, 0.13, abs(e + 0.12)); // flag high-slope areas (-0.25, 0.0)
	//e = e + 0.15 * smoothstep(-0.08, -0.01, e);
	//e *= amp;
	//return e;
	x *= noise_scale;
	y *= noise_scale;
	float arr[] = { x, y };
	return Noise::noise2(arr) * amplitude + Mesh->GetLocation().Z;
}
#pragma optimize("", on)
