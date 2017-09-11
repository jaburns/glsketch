#pragma once

#include "gfx.h"

typedef GLuint ShaderProgramRef;
typedef GLuint TextureRef;
typedef GLuint CubeMapRef;

ShaderProgramRef resources_load_shader(const char *vert_path, const char *frag_path);
void resources_delete_shader(ShaderProgramRef ref);

TextureRef resources_load_texture(const char *png_path);
void resources_delete_texture(TextureRef ref);

CubeMapRef resources_load_cubemap(const char *r, const char *l, const char *t, const char *bo, const char *ba, const char *f);
void resources_delete_cubemap(CubeMapRef ref);