#pragma once
#include "Core.h"
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <vector>

class Shader;
class Section;
struct Vertex;

class BufferPart
{
public:
	BufferPart();
	~BufferPart();
	void Initialize(int size);
	void Bind();
	void Unbind();
	int Size;

	uint32* indices;
	Vertex* verts;

	/* Buffers */
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
};

class RenderBatch
{
public:
	RenderBatch(int size);
	~RenderBatch();

	/* Begin the sprite processing */
	void begin();

	/* End the processing, sort and render the batches */
	void end(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	/* Add new point to be rendered as a sprite */
	void add(const Section* obj);

private:
	/* Render the current batch */
	void render();

	/* Draw sprite to the current batch */
	void drawSprite(const Section* obj, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	bool drawing;

	/* These handle the bounds of the batch */
	uint32 max_index;
	uint32 vertex_index;
	uint32 face_count;

	uint8 ActiveBuffer;

	BufferPart* Buffers;

	/* Store and sort the sprites until we want to draw them into batches */
	std::vector<const Section*> spriteMap;
};
