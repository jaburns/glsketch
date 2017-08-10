#pragma once

#include "gfx.hpp"
#include <memory>
#include <vector>
#include <string>
#include <glm/vec3.hpp>

class GLuintResource
{
protected:
    GLuint m_id = 0;
public:
    operator GLuint() const { return m_id; }
};

struct ShaderProgram : public GLuintResource
{
    ShaderProgram(const char *vert_path, const char *frag_path);
    ~ShaderProgram();
};

struct Texture : public GLuintResource
{
    Texture(const char *png_path);
    ~Texture();
};

struct CubeMap : public GLuintResource
{
    CubeMap(const char *r, const char *l, const char *t, const char *bo, const char *ba, const char *f);
    ~CubeMap();
};
