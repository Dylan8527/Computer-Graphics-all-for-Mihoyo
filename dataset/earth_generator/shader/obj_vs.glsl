#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;  

out vec3 fragment_position;
out vec3 normal;
out vec2 TexCoord;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragment_position = vec3(world * vec4(vertex_position, 1.0f));
    normal = mat3(transpose(inverse(world))) * vertex_normal;
    gl_Position = projection * view * vec4(fragment_position, 1.0);

    TexCoord = vertex_uv;
}
