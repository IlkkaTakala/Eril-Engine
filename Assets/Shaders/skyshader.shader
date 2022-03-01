2;0;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 in_position;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

uniform float time;

out float otime;
out vec3 fsun;
out vec3 pos;

uniform mat4 Model;

void main()
{
	fsun = normalize(vec3(0.0, 0.3, -1.0));
	pos = (Model * vec3(in_position, 1.0)).xyz;
	otime = time * 0.1;
	gl_Position = projection * view * Model * vec4(in_position, 1.0);
}
###END_VERTEX###
###FRAGMENT###
#version 430 core

in vec3 pos;
in vec3 fsun;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 BloomBuffer;
layout (location = 2) out vec4 accum;
layout (location = 3) out float reveal;

in float otime;
uniform float cirrus = 0.4;
uniform float cumulus = 0.8;

const float Br = 0.0005;
const float Bm = 0.0003;
const float g =  0.9200;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

float hash(float n)
{
	return fract(sin(n) * 43758.5453123);
}

float noise(vec3 x)
{
    vec3 f = fract(x);
    float n = dot(floor(x), vec3(1.0, 157.0, 113.0));
    return mix(mix(mix(hash(n +   0.0), hash(n +   1.0), f.x),
                   mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
               mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
                   mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

const mat3 m = mat3(0.0, 1.60,  1.20, -1.6, 0.72, -0.96, -1.2, -0.96, 1.28);
float fbm(vec3 p)
{
    float f = 0.0;
    f += noise(p) / 2; p = m * p * 1.1;
    f += noise(p) / 4; p = m * p * 1.2;
    f += noise(p) / 6; p = m * p * 1.3;
    f += noise(p) / 12; p = m * p * 1.4;
    f += noise(p) / 24;
    return f;
}

void main()
{
	// Atmosphere Scattering
	float mu = dot(normalize(pos), normalize(fsun));
	float rayleigh = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu);
	vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);

	vec3 day_extinction = exp(-exp(-((pos.y + fsun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
	vec3 night_extinction = vec3(1.0 - exp(fsun.y)) * 0.2;
	vec3 extinction = mix(day_extinction, night_extinction, -fsun.y * 0.2 + 0.5);
	color.rgb = rayleigh * mie * extinction;
	
	// Dithering Noise
	color.rgb += noise(pos * 1000) * 0.01;
	
	if (pos.y < 0.0)
		color = vec4(1.0);
	
	BloomBuffer = vec4(0.0);
}
###END_FRAGMENT###