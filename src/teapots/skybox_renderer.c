#include "skybox_renderer.h"

#include "../resources.h"

struct SkyboxRenderer
{
    GLuint vao;
    GLuint vertex_buffer;

    ShaderProgramRef program;
    CubeMapRef cubemap;
};

static GLfloat skybox_vertices[] = {
    -1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
};

CubeMapRef skybox_get_cubemap(SkyboxRenderer *renderer) 
{
    return renderer->cubemap;
}

SkyboxRenderer *skybox_renderer_create()
{
    SkyboxRenderer *renderer = (SkyboxRenderer*)malloc(sizeof(SkyboxRenderer));

    renderer->program = resources_load_shader("res/shaders/skybox.vert", "res/shaders/skybox.frag");
    renderer->cubemap = resources_load_cubemap(
        "res/hw_crater/craterlake_rt.png",
        "res/hw_crater/craterlake_lf.png",
        "res/hw_crater/craterlake_up.png",
        "res/hw_crater/craterlake_dn.png",
        "res/hw_crater/craterlake_bk.png",
        "res/hw_crater/craterlake_ft.png"
    );

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, &renderer->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(renderer->program, "position"));
    glVertexAttribPointer(glGetAttribLocation(renderer->program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    return renderer;
}

void skybox_renderer_draw_once(SkyboxRenderer *renderer, const mat4x4 *view, const mat4x4 *projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->cubemap);

    glDepthFunc(GL_LEQUAL);

    mat4x4 trunc_view;
    mat4x4_scale(trunc_view, *view, 1.0f);  // TODO write mat4x4_copy
    trunc_view[0][3] = 0.0f;
    trunc_view[1][3] = 0.0f;
    trunc_view[2][3] = 0.0f;
    trunc_view[3][0] = 0.0f;
    trunc_view[3][1] = 0.0f;
    trunc_view[3][2] = 0.0f;
    trunc_view[3][3] = 0.0f;

    glUseProgram(renderer->program);
    glUniformMatrix4fv(glGetUniformLocation(renderer->program, "view"), 1, GL_FALSE, trunc_view);
    glUniformMatrix4fv(glGetUniformLocation(renderer->program, "projection"), 1, GL_FALSE, projection);
    glUniform1i(glGetUniformLocation(renderer->program, "skybox"), 0);

    glBindVertexArray(renderer->vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

void skybox_renderer_destroy(SkyboxRenderer *renderer)
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vertex_buffer);

    resources_delete_shader(renderer->program);
    resources_delete_cubemap(renderer->cubemap);

    free(renderer);
}