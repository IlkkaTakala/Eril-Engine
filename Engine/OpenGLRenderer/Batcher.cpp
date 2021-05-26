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
	ActiveBuffer = 0;

	Buffers = new BufferPart[3]();
	Buffers[0].Initialize(size);
	Buffers[1].Initialize(size);
	Buffers[2].Initialize(size);
}

RenderBatch::~RenderBatch()
{
	delete[] Buffers;
}

void RenderBatch::begin()
{
	if (!drawing) {
		spriteMap.clear();
		vertex_index = 0;
		face_count = 0;
		drawing = true;
		Buffers[ActiveBuffer].Bind();
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

	}
	if (face_count > 0) render();
	spriteMap.clear();
	drawing = false;
	Buffers[ActiveBuffer].Unbind();
}

void RenderBatch::add(const Section* obj)
{
	spriteMap.push_back(obj);
}

void RenderBatch::drawSprite(const Section* obj, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	if (!drawing) return;

	glm::mat4 transform = obj->Parent->GetModelMatrix();

	const uint32* objIdx = obj->GetIndices();
	/* Transform the vertices */
	for (uint32 i = 0; i < obj->GetFaceCount(); i++) {
		Buffers[ActiveBuffer].indices[face_count * 3 + i * 3 + 0] = objIdx[i * 3 + 0] + vertex_index;
		Buffers[ActiveBuffer].indices[face_count * 3 + i * 3 + 1] = objIdx[i * 3 + 1] + vertex_index;
		Buffers[ActiveBuffer].indices[face_count * 3 + i * 3 + 2] = objIdx[i * 3 + 2] + vertex_index;
	}
	face_count += obj->GetFaceCount();

	const glm::mat4 normalMat = transpose(inverse(obj->Parent->GetModelMatrix()));

	const Vertex* vert_array = obj->GetVertices();
	for (uint32 i = 0; i < obj->GetVertexCount(); i++) {
		Vertex& next = Buffers[ActiveBuffer].verts[i + vertex_index];
		next.position = transform * glm::vec4(vert_array[i].position, 1.f);
		next.normal = normalize(glm::mat3(transform) * vert_array[i].normal);
		next.tangent = normalize(glm::mat3(transform) * vert_array[i].tangent);
		next.uv = vert_array[i].uv;
	}
	vertex_index += obj->GetVertexCount();
}

void RenderBatch::render()
{
	Buffers[ActiveBuffer].Unbind();
	glBindVertexArray(Buffers[ActiveBuffer].m_vao);
	glDrawElements(GL_TRIANGLES, face_count * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	vertex_index = 0;
	face_count = 0;

	ActiveBuffer = (ActiveBuffer + 1) % 3;
	Buffers[ActiveBuffer].Bind();
}

BufferPart::BufferPart()
{
	indices = nullptr;
	verts = nullptr;
	Size = 0;

	m_vao = 0;
	m_vbo = 0;
	m_ebo = 0;
}

BufferPart::~BufferPart()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteVertexArrays(1, &m_vao);
}

void BufferPart::Initialize(int size)
{
	Size = size;
	// Create Vertex Array Object
	glGenVertexArrays(1, &m_vao);

	// Create Vertex Buffer Object
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then 
	// configure vertex attributes(s).
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32), NULL, GL_DYNAMIC_DRAW);

	// Set buffer data to m_vbo-object (bind buffer first and then set the data), dynamic buffer with max size calculated from maximum batch size
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex) * 3, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void BufferPart::Bind()
{
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	indices = reinterpret_cast<uint32*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	verts = reinterpret_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE));*/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	indices = reinterpret_cast<uint32*>(glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, Size * sizeof(uint32), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	verts = reinterpret_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, Size * sizeof(Vertex) * 3, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT));
}

void BufferPart::Unbind()
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}
