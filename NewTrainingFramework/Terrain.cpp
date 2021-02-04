#include "stdafx.h"
#include "Terrain.h"
#include "ResourceManager.h"
#include "SceneManager.h"

// TODO: refactor this method in 2 others
// static array is better
void Terrain::generateTerrain()
{
	const int INDICES_PER_CELL = 6;
	std::vector<Vertex> vertices((numberOfCells + 1)  * (numberOfCells + 1));
	std::vector<GLushort> indices(INDICES_PER_CELL * numberOfCells * numberOfCells);
	
	// generate vertices
	int inferiorLimit = -numberOfCells / 2 * cellDimension, superiorLimit = numberOfCells / 2 * cellDimension;
	for (int i = inferiorLimit, i_v = 0; i <= superiorLimit; i += cellDimension, i_v++)
		for (int j = inferiorLimit, j_v = 0; j <= superiorLimit; j += cellDimension, j_v++)
		{
			int currentIndex = (numberOfCells + 1) * i_v + j_v;
			vertices[currentIndex].pos.x = j;
			vertices[currentIndex].pos.y = -10;
			vertices[currentIndex].pos.z = i;

			vertices[currentIndex].uv2.x = static_cast<float>(j_v) / numberOfCells;
			vertices[currentIndex].uv2.y = static_cast<float>(i_v) / numberOfCells;

			vertices[currentIndex].uv.x = static_cast<float>(j_v);
			vertices[currentIndex].uv.y = static_cast<float>(i_v);
		}

	// generate indices
	int numberOfIndices = indices.size();
	for (int i = 0; i < numberOfIndices; i += INDICES_PER_CELL)
	{
		int specificToRow = i / (INDICES_PER_CELL * numberOfCells), specificToQuad = i / INDICES_PER_CELL;
		indices[i] = specificToQuad + specificToRow;
		indices[i + 1] = specificToQuad + 1 + specificToRow;
		indices[i + 2] = specificToQuad + numberOfCells + 1 + specificToRow;
		indices[i + 3] = specificToQuad + 1 + specificToRow;
		indices[i + 4] = specificToQuad + numberOfCells + 2 + specificToRow;
		indices[i + 5] = specificToQuad + numberOfCells + 1 + specificToRow;
	}

	GLuint terrainBIds[3];
	std::vector<GLushort> wiredIndices = getWiredModelIndices(indices);
	Model* terrainModel = new Model(vertices);
	ResourceManager::getInstance()->addGeneratedModel(terrainModel);
	
	terrainModel->setBufferSize(Model::Buffer::FILLED_INDICES, numberOfIndices);
	terrainModel->setBufferSize(Model::Buffer::WIRED_INDICES, wiredIndices.size());

	glGenBuffers(3, terrainBIds);

	glBindBuffer(GL_ARRAY_BUFFER, terrainBIds[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &(vertices[0]), GL_STATIC_DRAW);
	terrainModel->setBufferId(Model::Buffer::VERTICES, terrainBIds[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainBIds[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &(indices[0]), GL_STATIC_DRAW);
	terrainModel->setBufferId(Model::Buffer::FILLED_INDICES, terrainBIds[1]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainBIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, wiredIndices.size() * sizeof(GLushort), &(wiredIndices[0]), GL_STATIC_DRAW);
	terrainModel->setBufferId(Model::Buffer::WIRED_INDICES, terrainBIds[2]);

	setModel(terrainModel);
}

Terrain::Terrain(int numberOfCells, int cellDimension, int offsetY, Vector3& heights, int id, Vector3& position,
	Vector3& rotation, Vector3& scale, Vector3& color, Shader* shaderPtr, const std::vector<Texture*> vTexturePtr, bool wired)
	: SceneObject(id, position, rotation, scale, color, Vector3(0.0f,0.0f,0.0f) , nullptr, shaderPtr, vTexturePtr, wired),
	numberOfCells(numberOfCells), cellDimension(cellDimension), offsetY(offsetY), heights(heights), offsets(0,0)
{
	generateTerrain();
}

Terrain::Terrain(int numberOfCells, int cellDimension, int offsetY, Vector3& heights, int id_, Vector3& position_, Vector3& rotation_, Vector3& scale_, Vector3& color_, int shaderId, const std::vector<int> vTextureIds, bool wired)
	: SceneObject(id_, position_, rotation_, scale_, color_, Vector3(0.0f, 0.0f, 0.0f), ResourceManager::GENERATED_MODEL_ID, shaderId, vTextureIds, wired),
	numberOfCells(numberOfCells), cellDimension(cellDimension), offsetY(offsetY), heights(heights), offsets(0,0)
{
	generateTerrain();
	Vector3 newTerrainPosition = SceneManager::getInstance()->getActiveCamera()->getPosition();
	newTerrainPosition.y -= offsetY;
	changePosition(newTerrainPosition);
}

void Terrain::sendUniqueData()
{
	Shader* ptrToShader = SceneObject::getPtrToShader();
	if (ptrToShader->getAttrib("a_uv2") != -1) {
		glEnableVertexAttribArray(ptrToShader->getAttrib("a_uv2"));
		glVertexAttribPointer(ptrToShader->getAttrib("a_uv2"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<float*>(sizeof(Vector3) * 5 + sizeof(Vector2)));
	}

	if (ptrToShader->getUniform("u_height") != -1)
		glUniform3f(ptrToShader->getUniform("u_height"), heights.x, heights.y, heights.z);

	if (ptrToShader->getUniform("u_offset") != -1)
		glUniform2f(ptrToShader->getUniform("u_offset"), offsets.x, offsets.y);

	if (ptrToShader->getUniform("u_numberOfCells") != -1)
		glUniform1f(ptrToShader->getUniform("u_numberOfCells"), numberOfCells);
}

void Terrain::updateImpl()
{
	Vector3 cameraPosition = SceneManager::getInstance()->getActiveCamera()->getPosition();
	const Vector3& objectPosition = getObjectPosition();
	if (cameraPosition.x > objectPosition.x + cellDimension) {
		changePosition(Vector3(objectPosition.x + cellDimension, objectPosition.y, objectPosition.z));
		offsets.x++;
	}

	else if (cameraPosition.x < objectPosition.x - cellDimension) {
		changePosition(Vector3(objectPosition.x - cellDimension, objectPosition.y, objectPosition.z));
		offsets.x--;
	}

	if (cameraPosition.z > objectPosition.z + cellDimension) {
		changePosition(Vector3(objectPosition.x, objectPosition.y, objectPosition.z + cellDimension));
		offsets.y++;
	}

	else if (cameraPosition.z < objectPosition.z - cellDimension) {
		changePosition(Vector3(objectPosition.x, objectPosition.y, objectPosition.z - cellDimension));
		offsets.y--;
	}
}