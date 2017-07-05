#include "teapot.hpp"
#include "teapot_mesh.hpp" 

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

TeapotRenderer::TeapotRenderer(std::shared_ptr<const CubeMap> skybox_cubemap)
{
    m_skybox_cubemap = skybox_cubemap;
    m_program = std::make_unique<const ShaderProgram>("res/shaders/teapot.vert", "res/shaders/teapot.frag");
    m_texture = std::make_unique<const Texture>("res/texture.png");

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, TEAPOT_VERTEX_COUNT * sizeof(glm::vec3), teapot_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    glGenBuffers(1, &m_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, TEAPOT_NORMAL_COUNT * sizeof(glm::vec3), teapot_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "normal"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, TEAPOT_INDEX_COUNT * sizeof(unsigned int), teapot_indices, GL_STATIC_DRAW);
}

TeapotRenderer::~TeapotRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_normal_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}

void TeapotRenderer::use(const glm::vec3& camera_pos, const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *m_skybox_cubemap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *m_texture);

    glUseProgram(*m_program);
    glUniform3fv(glGetUniformLocation(*m_program, "camera_pos"), 1, glm::value_ptr(camera_pos));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(*m_program, "skybox"), 0);
    glUniform1i(glGetUniformLocation(*m_program, "surf_texture"), 1);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
}

void TeapotRenderer::draw(const Transform& transform)
{
    auto m = glm::scale(glm::translate(glm::mat4(1.0f), transform.position), transform.scale) 
        * glm::mat4_cast(transform.rotation);

    glUniformMatrix4fv(glGetUniformLocation(*m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, TEAPOT_INDEX_COUNT, GL_UNSIGNED_INT, (void*)0);
}