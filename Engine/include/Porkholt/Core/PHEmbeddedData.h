/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHEMBEDDEDDATA_H
#define PHEMBEDDEDDATA_H

#include <Porkholt/Core/PHMain.h>

extern const char   scripts_alloc_log_lua[];
extern const size_t scripts_alloc_log_lua_size;

extern const char   scripts_lua_material_lua[];
extern const size_t scripts_lua_material_lua_size;

extern const char   rsrc_shaders_color_notex_sprites_vsh[];
extern const size_t rsrc_shaders_color_notex_sprites_vsh_size;

extern const char   rsrc_shaders_deferred_diffuse_point_fsh[];
extern const size_t rsrc_shaders_deferred_diffuse_point_fsh_size;

extern const char   rsrc_shaders_missingnormals_sprites_fsh[];
extern const size_t rsrc_shaders_missingnormals_sprites_fsh_size;

extern const char   rsrc_shaders_deferred_ambient_vsh[];
extern const size_t rsrc_shaders_deferred_ambient_vsh_size;

extern const char   rsrc_shaders_deferred_diffuse_normal_point_fsh[];
extern const size_t rsrc_shaders_deferred_diffuse_normal_point_fsh_size;

extern const char   rsrc_shaders_deferred_ambient_fsh[];
extern const size_t rsrc_shaders_deferred_ambient_fsh_size;

extern const char   rsrc_shaders_sprites_fsh[];
extern const size_t rsrc_shaders_sprites_fsh_size;

extern const char   rsrc_shaders_sprites_lua[];
extern const size_t rsrc_shaders_sprites_lua_size;

extern const char   rsrc_shaders_color_sprites_vsh[];
extern const size_t rsrc_shaders_color_sprites_vsh_size;

extern const char   rsrc_shaders_sprites_vsh[];
extern const size_t rsrc_shaders_sprites_vsh_size;

extern const char   rsrc_shaders_deferred_diffuse_normal_point_vsh[];
extern const size_t rsrc_shaders_deferred_diffuse_normal_point_vsh_size;

extern const char   rsrc_shaders_missingnormals_sprites_lua[];
extern const size_t rsrc_shaders_missingnormals_sprites_lua_size;

extern const char   rsrc_shaders_color_sprites_lua[];
extern const size_t rsrc_shaders_color_sprites_lua_size;

extern const char   rsrc_shaders_deferred_ambient_lua[];
extern const size_t rsrc_shaders_deferred_ambient_lua_size;

extern const char   rsrc_shaders_deferred_diffuse_point_lua[];
extern const size_t rsrc_shaders_deferred_diffuse_point_lua_size;

extern const char   rsrc_shaders_notex_sprites_lua[];
extern const size_t rsrc_shaders_notex_sprites_lua_size;

extern const char   rsrc_shaders_text_vsh[];
extern const size_t rsrc_shaders_text_vsh_size;

extern const char   rsrc_shaders_color_notex_sprites_lua[];
extern const size_t rsrc_shaders_color_notex_sprites_lua_size;

extern const char   rsrc_shaders_notex_sprites_fsh[];
extern const size_t rsrc_shaders_notex_sprites_fsh_size;

extern const char   rsrc_shaders_text_lua[];
extern const size_t rsrc_shaders_text_lua_size;

extern const char   rsrc_shaders_text_fsh[];
extern const size_t rsrc_shaders_text_fsh_size;

extern const char   rsrc_shaders_notex_sprites_vsh[];
extern const size_t rsrc_shaders_notex_sprites_vsh_size;

extern const char   rsrc_shaders_color_sprites_fsh[];
extern const size_t rsrc_shaders_color_sprites_fsh_size;

extern const char   rsrc_shaders_deferred_diffuse_normal_point_lua[];
extern const size_t rsrc_shaders_deferred_diffuse_normal_point_lua_size;

extern const char   rsrc_shaders_deferred_diffuse_point_vsh[];
extern const size_t rsrc_shaders_deferred_diffuse_point_vsh_size;

extern const char   rsrc_shaders_color_notex_sprites_fsh[];
extern const size_t rsrc_shaders_color_notex_sprites_fsh_size;

extern const char   rsrc_fnt_Helvetica_phf[];
extern const size_t rsrc_fnt_Helvetica_phf_size;


extern map<PHHashedString, pair<const char*, size_t> > * PHEmbedFiles;

#define PHEmbedFile_(ident, path, size) \
PH_INITIALIZER(PH_TOKENPASTE(PHEmbedInit_, PH_UNIQUE_TOKEN)) \
{ \
    if (!PHEmbedFiles) \
        PHEmbedFiles = new map<PHHashedString, pair<const char*, size_t> >; \
    PHEmbedFiles->insert(make_pair(PHHashedString(path), make_pair(ident, size))); \
}

#define PHEmbedFile(ident, path, size, contents) \
const char   ident[] = contents; \
const size_t ident ## _size = size; \
PHEmbedFile_(ident, path, size)

#define PHEmbedDir(path) PHEmbedFile_((const char*)NULL, path, 0)

#endif
