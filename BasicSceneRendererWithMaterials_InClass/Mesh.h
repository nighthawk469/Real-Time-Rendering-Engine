#ifndef MESH_H_
#define MESH_H_

#include "glshell.h"
#include "Vertex.h"
#include <vector>
#include "Vertex.h"

class Mesh {
    
public:
    const VertexFormat* mFormat;        // format of vertices in the vertex buffer

    GLenum              mMode;          // drawing mode
    GLsizei             mNumVertices;   // number of vertices



    Mesh();
    ~Mesh();

	GLuint              mVBO;           // id of vertex buffer containing vertex data

    bool loadFromData(const void* data,
                      GLsizei numVertices,
                      GLsizei vertexSize,
                      GLenum mode,
                      const VertexFormat* format);

    void activate() const;
    void deactivate() const;

    void draw() const;

	std::vector<VertexPositionNormal> mVertices;
};

//
// Load mesh from a Wavefront OBJ file
//
Mesh* LoadMesh(const std::string& path);

#endif
