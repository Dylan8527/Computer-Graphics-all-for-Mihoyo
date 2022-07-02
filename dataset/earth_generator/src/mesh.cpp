#include <mesh.h>
#include <utils.h>
#include <fstream>
#include <vector>

Mesh::Mesh(const std::string &path) : num_face(0) {
    loadDataFromFile(getPath(path));
}

Mesh::Mesh() {
    loadDataFromManual();
}

inline vec2 getTextureCoordinate(vec3& normal) {
    return vec2 {
        (std::atan2(normal[1], normal[0])) / (2.0 * M_PI),
        std::acos(normal[2]) / M_PI
    };
}

/**
 * TODO: implement load mesh data from file here
 * you need to open file with [path]
 *
 * File format
 * First line contains 3 integers: vertex count, normal count, face count
 *
 * For the next [vertex count] lines, each line contains 3 floats, which
 * represents the position of a vertex
 *
 * For the next [normal count] lines, each line contains 3 floats, which
 * represents the normal of a vertex
 *
 * For the next [face count] lines, each line contains 6 integers
 * [v0, n0, v1, n1, v2, n2], which represents the triangle face
 * v0, n0 means the vertex index and normal index of the first vertex
 * v1, n1 means the vertex index and normal index of the second vertex
 * v2, n2 means the vertex index and normal index of the third vertex
 */
void Mesh::loadDataFromFile(const std::string &path) {

    std::ifstream file(path);
    std::string line;
    size_t num_vertex, num_normal;
    float x, y, z;
    GLuint index;

    // first line
    file >> num_vertex >> num_normal >> this->num_face;
    // blank line
    std::getline(file, line);

    std::vector<vec3> vertex;
    vertex.reserve(num_vertex);
    std::vector<vec3> normal;
    normal.reserve(num_normal);

    // read vertex
    for (size_t i = 0; i < num_vertex; ++i) {
        file >> x >> y >> z;
        vertex.emplace_back(vec3 {x, y, z});
    }
    std::getline(file, line);

    // read normal
    for(size_t i = 0; i < num_normal; ++i) {
        file >> x >> y >> z;
        normal.emplace_back(glm::normalize(vec3 {x, y, z}));
    }
    std::getline(file, line);

    // read face indices
    this->indices.reserve(this->num_face * 6);
    for(size_t i = 0; i < this->num_face * 6; ++i) {
        file >> index;
        this->indices.emplace_back(index);
    }

    // construct Vertex
    this->vertices.reserve(this->num_face * 3);
    for(size_t i = 0; i < this->num_face * 3; ++i) {
        this->vertices.emplace_back(Vertex {
            vertex[this->indices[2 * i + 0]],
            normal[this->indices[2 * i + 1]],
            getTextureCoordinate(normal[this->indices[2 * i + 1]])
        });
    }

}

void Mesh::loadDataFromManual(){
    size_t sqrt_n = 1000;
    float inv_sqrt_n = 1.0 / sqrt_n;

    float x, y, z;

    std::vector<vec3> vertex;
    std::vector<vec3> normal;
    std::vector<size_t> n_vertex_per_circle;

    //* Generate `sqrt_n * sqrt_n` vertices uniformly distributed on sphere
    for (size_t i = 0; i <= sqrt_n; ++i) {
        float u1 = i * inv_sqrt_n;
        // we add vertices circle by circle
        float phi = acos(1 - 2 * u1);
        size_t n_vetx = 0;
        for(size_t j = 0; j < sqrt_n; ++j){
            ++n_vetx;
            float u2 = j * inv_sqrt_n;
            float theta = 2 * M_PI * u2;
            // sphere's origin set at (0, 0, 0) with radius 1.0f
            auto point = glm::vec3{
                1.0f * cos(theta) * sin(phi),
                1.0f * sin(theta) * sin(phi),
                1.0f * cos(phi)
            };
            vertex.emplace_back(point);
            normal.emplace_back(point); 
            if (i == 0 || i == sqrt_n)
                break;
        }
        n_vertex_per_circle.emplace_back(n_vetx);
    }

    //* construct face indices

    // north pole
    for(size_t nxt_cir = 0; nxt_cir < n_vertex_per_circle[1]; ++nxt_cir) {
        this->indices.emplace_back(0); 
        this->indices.emplace_back(0); 
        this->indices.emplace_back(1 + (nxt_cir % n_vertex_per_circle[1])); 
        this->indices.emplace_back(1 + (nxt_cir % n_vertex_per_circle[1])); 
        this->indices.emplace_back(1 + ((nxt_cir + 1) % n_vertex_per_circle[1])); 
        this->indices.emplace_back(1 + ((nxt_cir + 1) % n_vertex_per_circle[1])); 
    }

    size_t now_pos = 0;
    size_t nxt_pos = n_vertex_per_circle[0];
    // left triangles
    for(size_t idx_cir = 1; idx_cir < n_vertex_per_circle.size() - 1; ++idx_cir) {
        size_t now_n_vtx = n_vertex_per_circle[idx_cir];
        size_t nxt_n_vtx = n_vertex_per_circle[idx_cir + 1];
        now_pos = nxt_pos;
        nxt_pos += n_vertex_per_circle[idx_cir];

        for(size_t idx_vtx = 0; idx_vtx < now_n_vtx; ++idx_vtx){
            this->indices.emplace_back(now_pos + idx_vtx); 
            this->indices.emplace_back(now_pos + idx_vtx); 
            this->indices.emplace_back(nxt_pos + (idx_vtx % nxt_n_vtx)); 
            this->indices.emplace_back(nxt_pos + (idx_vtx % nxt_n_vtx)); 
            this->indices.emplace_back(nxt_pos + ((idx_vtx + 1) % nxt_n_vtx)); 
            this->indices.emplace_back(nxt_pos + ((idx_vtx + 1) % nxt_n_vtx)); 
        }
    }

    // right triangles
    now_pos = 0;
    nxt_pos = n_vertex_per_circle[0];
    for(size_t idx_cir = 1; idx_cir < n_vertex_per_circle.size() - 1; ++idx_cir){
        size_t* now_n_vtx = &n_vertex_per_circle[idx_cir];
        size_t* nxt_n_vtx = &n_vertex_per_circle[idx_cir + 1];
        now_pos = nxt_pos;
        nxt_pos += n_vertex_per_circle[idx_cir];

        for(size_t idx_vtx = 0; idx_vtx < *now_n_vtx; ++idx_vtx){
            this->indices.emplace_back(nxt_pos + ((idx_vtx + 1) % *nxt_n_vtx)); 
            this->indices.emplace_back(nxt_pos + ((idx_vtx + 1) % *nxt_n_vtx)); 
            this->indices.emplace_back(now_pos + (idx_vtx % *now_n_vtx)); 
            this->indices.emplace_back(now_pos + (idx_vtx % *now_n_vtx)); 
            this->indices.emplace_back(now_pos + ((idx_vtx + 1) % *now_n_vtx)); 
            this->indices.emplace_back(now_pos + ((idx_vtx + 1) % *now_n_vtx)); 
        }
    }

    // south pole
    now_pos = vertex.size() - n_vertex_per_circle.back();
    nxt_pos = now_pos - n_vertex_per_circle[n_vertex_per_circle.size() - 2];
    for(size_t nxt_cir = 0; nxt_cir < n_vertex_per_circle[n_vertex_per_circle.size() - 2]; ++nxt_cir){
        this->indices.emplace_back(now_pos); 
        this->indices.emplace_back(now_pos); 
        this->indices.emplace_back(nxt_pos + (nxt_cir % n_vertex_per_circle[n_vertex_per_circle.size() - 2])); 
        this->indices.emplace_back(nxt_pos + (nxt_cir % n_vertex_per_circle[n_vertex_per_circle.size() - 2])); 
        this->indices.emplace_back(nxt_pos + ((nxt_cir + 1) % n_vertex_per_circle[n_vertex_per_circle.size() - 2])); 
        this->indices.emplace_back(nxt_pos + ((nxt_cir + 1) % n_vertex_per_circle[n_vertex_per_circle.size() - 2])); 
    }

    // construct Vertex
    for(size_t i = 0; i < this->indices.size() / 2; ++i) {
        this->vertices.emplace_back(Vertex {
            vertex[this->indices[2 * i + 0]],
            normal[this->indices[2 * i + 1]],
            getTextureCoordinate(normal[this->indices[2 * i + 1]])
        });
    }

}

void Mesh::draw() const {
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
}
