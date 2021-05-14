#include "Batcher.h"
#include <GLFW/glfw3.h>

#include "Core.h"
#include "Material.h"
#include "Mesh.h"

RenderBatch::RenderBatch(int size)
{
	vertex_index = 0;
	face_count = 0;
	max_index = size;
	drawing = false;

	// Create Vertex Array Object
	glGenVertexArrays(1, &m_vao);

	// Create Vertex Buffer Object
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then 
	// configure vertex attributes(s).
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_index * sizeof(uint32), NULL, GL_DYNAMIC_DRAW);

	// Set buffer data to m_vbo-object (bind buffer first and then set the data), dynamic buffer with max size calculated from maximum batch size
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, max_index * sizeof(Vertex) * 3, NULL, GL_DYNAMIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

RenderBatch::~RenderBatch()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteVertexArrays(1, &m_vao);
}

void RenderBatch::begin()
{
	if (!drawing) {
		spriteMap.clear();
		vertex_index = 0;
		face_count = 0;
		drawing = true;
	}
}

void RenderBatch::end(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	if (spriteMap.size() == 0 || !drawing) return;

	/* Iterate through sprites and render all with the same texture in one draw call */
	for (auto i : spriteMap) {
		if (i->GetFaceCount() * 3 > max_index) continue;
		if ((i->GetFaceCount() + face_count) * 3 >= max_index || i->GetVertexCount() + vertex_index >= max_index)
			render();

		drawSprite(i, viewMatrix, projectionMatrix);

		//if (face_count >= max_index) render();
	}
	if (face_count > 0) render();
	spriteMap.clear();
	drawing = false;
}

void RenderBatch::add(const Section* obj)
{
	spriteMap.push_back(obj);
}

void RenderBatch::drawSprite(const Section* obj, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	if (!drawing) return;

	glm::mat4 transform = obj->Parent->GetModelMatrix();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	uint32* indices = reinterpret_cast<uint32*>(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, face_count * 3 * sizeof(uint32), (max_index - (face_count * 3)) * sizeof(uint32), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT));
	if (indices == nullptr) return;
	const uint32* objIdx = obj->GetIndices();
	/* Transform the vertices */
	for (uint32 i = 0; i < obj->GetFaceCount(); i++) {
		indices[i * 3 + 0] = objIdx[i * 3 + 0] + vertex_index;
		indices[i * 3 + 1] = objIdx[i * 3 + 1] + vertex_index;
		indices[i * 3 + 2] = objIdx[i * 3 + 2] + vertex_index;
	}
	face_count += obj->GetFaceCount();
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	Vertex* verts = reinterpret_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, vertex_index * sizeof(Vertex), (max_index - vertex_index) * sizeof(Vertex), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT));
	
	glm::mat3 normalMat = glm::mat3(transpose(inverse(obj->Parent->GetModelMatrix())));

	const Vertex* vert_array = obj->GetVertices();
	Vertex next;
	for (uint32 i = 0; i < obj->GetVertexCount(); i++) {
		next.position = transform * glm::vec4(vert_array[i].position, 1.f);
		next.normal = normalMat * vert_array[i].normal;
		next.uv = vert_array[i].uv;
		verts[i] = next;
	}
	vertex_index += obj->GetVertexCount();

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindVertexArray(0);
}

void RenderBatch::render()
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, face_count * 3, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	vertex_index = 0;
	face_count = 0;
}
