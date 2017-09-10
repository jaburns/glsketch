#include "teapot_renderer.h"

#include "../gfx.h"
#include "../resources.h"
#include "teapot_mesh.h" 

struct TeapotRenderer
{
    GLuint vao;
    GLuint vertex_buffer;
    GLuint normal_buffer;
    GLuint index_buffer;

    ShaderProgramRef program;
    TextureRef texture;

    CubeMapRef skybox_cubemap; // borrowed
};

TeapotRenderer *teapot_renderer_create(CubeMapRef skybox_cubemap)
{
    TeapotRenderer *renderer = (TeapotRenderer*)malloc(sizeof(TeapotRenderer));

    renderer->skybox_cubemap = skybox_cubemap;
    renderer->program = resources_load_shader("res/shaders/teapot.vert", "res/shaders/teapot.frag");
    renderer->texture = resources_load_texture("res/texture.png");

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, TEAPOT_VERTEX_COUNT * sizeof(vec3), teapot_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(renderer->program, "position"));
    glVertexAttribPointer(glGetAttribLocation(renderer->program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) 0);

    glGenBuffers(1, &renderer->normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, TEAPOT_NORMAL_COUNT * sizeof(vec3), teapot_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(renderer->program, "normal"));
    glVertexAttribPointer(glGetAttribLocation(renderer->program, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) 0);

    glGenBuffers(1, &renderer->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TEAPOT_INDEX_COUNT * sizeof(unsigned int), teapot_indices, GL_STATIC_DRAW);

    return renderer;
}

void teapot_renderer_destroy(TeapotRenderer *renderer)
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vertex_buffer);
    glDeleteBuffers(1, &renderer->normal_buffer);
    glDeleteBuffers(1, &renderer->index_buffer);

    resources_delete_shader(renderer->program);
    resources_delete_texture(renderer->texture);

    free(renderer);
}

void teapot_renderer_use(TeapotRenderer *renderer, const vec3 *camera_pos, const mat4x4 *view, const mat4x4 *projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->skybox_cubemap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderer->texture);

    glUseProgram(renderer->program);
    glUniform3fv(glGetUniformLocation(renderer->program, "camera_pos"), 1, camera_pos);
    glUniformMatrix4fv(glGetUniformLocation(renderer->program, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(renderer->program, "projection"), 1, GL_FALSE, projection);
    glUniform1i(glGetUniformLocation(renderer->program, "skybox"), 0);
    glUniform1i(glGetUniformLocation(renderer->program, "surf_texture"), 1);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->index_buffer);
}

void teapot_renderer_draw(TeapotRenderer *renderer, const Transform *transform)
{
    mat4x4 m, r;

    mat4x4_identity(m);
    mat4x4_translate_in_place(m, transform->position[0], transform->position[1], transform->position[2]);
    mat4x4_scale_aniso(m, m, transform->scale[0], transform->scale[1], transform->scale[2]);
    mat4x4_from_quat(r, transform->rotation);
    mat4x4_mul(m, m, r);

    glUniformMatrix4fv(glGetUniformLocation(renderer->program, "model"), 1, GL_FALSE, m);

    glDrawElements(GL_TRIANGLES, TEAPOT_INDEX_COUNT, GL_UNSIGNED_INT, (void*)0);
}