// Tessellated Plane Mesh, slightly adapted from the original Plane mesh to fit into the Tessellator stage of the DX11 Graphics Pipeline
#include "Tplane.h"

// Initialise buffer and load texture.
TPlane::TPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	resolution = lresolution;
	initBuffers(device);
}

// Release resources.
TPlane::~TPlane()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Generate plane (including texture coordinates and normals).
void TPlane::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;


	// Calculate the number of vertices in the terrain mesh.
	vertexCount = (resolution - 1) * (resolution - 1) * 6;

	// Set the index and vertice count
	indexCount = vertexCount;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// Initialize U and V texture coords and the increment, depending on the resolution of the plane
	u = 0;
	v = 0;
	increment = 1.0f / resolution;

	for (j = 0; j < (resolution - 1); j++)
	{
		for (i = 0; i < (resolution - 1); i++)
		{
			// Determine the indexes based on the specific spot of the quad
			int verticeIndex = (i * resolution + j) * 4;
			int indiceIndex = (i * resolution + j) * 4;

			// lower left (upper left)
			positionX = (float)(i);
			positionZ = (float)(j + 1);

			vertices[verticeIndex].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex].texture = XMFLOAT2(u, v + increment);
			vertices[verticeIndex].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex] = verticeIndex;

			// Upper left (bottom left)
			positionX = (float)(i);
			positionZ = (float)(j);

			vertices[verticeIndex + 1].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex + 1].texture = XMFLOAT2(u, v);
			vertices[verticeIndex + 1].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex + 1] = verticeIndex + 1;

			// Bottom right (upper right)
			positionX = (float)(i + 1);
			positionZ = (float)(j);

			vertices[verticeIndex + 2].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex + 2].texture = XMFLOAT2(u + increment, v);
			vertices[verticeIndex + 2].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex + 2] = verticeIndex + 2;

			// Upper right (bottom right)
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[verticeIndex + 3].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex + 3].texture = XMFLOAT2(u + increment, v + increment);
			vertices[verticeIndex + 3].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex + 3] = verticeIndex + 3;

			// Increment the U texture coord, which is equivalent to the X value
			u += increment;

		}

		// Reset the U value to 0, as the next row of quads is being generated
		u = 0;

		// Increment the V texture coord, which is equivalent to the Y value
		// Effectively moves to the Y coord to create the next row of quads
		v += increment;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}