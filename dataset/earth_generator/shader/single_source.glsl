#version 330 core

struct LightSource {
    vec3 position;
    vec3 color;
};

in vec3 normal;
in vec3 fragment_position;

out vec4 fragment_color;

uniform LightSource light_source;
uniform vec3 object_color;
uniform vec3 camera_position;

void main() {

    float ambient_factor = 0.1f;
    float diffuse_factor = 1.5f;
    float specular_factor = 0.3f;

    vec3 fragment_normal = normalize(normal);

    vec3 result = vec3(0.0f);
    vec3 ambient = ambient_factor * light_source.color;

    vec3 light_direction = normalize(light_source.position - fragment_position);
    float diffuse_strength = max(dot(fragment_normal, light_direction), 0.0f);
    vec3 diffuse = diffuse_factor * diffuse_strength * light_source.color;

    vec3 view_direction = normalize(camera_position - fragment_position);
    vec3 reflect_direction = reflect(-light_direction, fragment_normal);
    float specular_strength = pow(max(dot(view_direction, reflect_direction), 0.0f), 32);
    vec3 specular = specular_strength * specular_factor * light_source.color;

    result = (ambient + diffuse + specular) * object_color;



    fragment_color = vec4(result, 1.0);

}

