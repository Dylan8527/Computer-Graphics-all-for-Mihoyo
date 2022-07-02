#ifndef _MESH_H_
#define _MESH_H_
#include "defines.h"
#include <vector>

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

class Mesh {
public:
    // construct a sphere
    Mesh();
    // construct a mesh from a obj file
    Mesh(const std::string &path);

    void draw() const;

    //  mesh data
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    size_t num_face;

private:
    void loadDataFromFile(const std::string &path);
    void loadDataFromManual();
};

#endif
