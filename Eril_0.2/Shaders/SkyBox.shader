2;2;
###VERTEX###
#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140, binding = 0) uniform Globals
{
    mat4 projection;
    mat4 view;
	vec4 viewPos;
	ivec2 screenSize;
	int sceneLightCount;
};

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
} 
###END_VERTEX###
###FRAGMENT###
#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    vec3 envColor = texture(skybox, TexCoords).rgb;
    
    //envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2)); 
  
    FragColor = vec4(envColor, 1.0);
	BloomColor = vec4(0.0);
}
###END_FRAGMENT###