#ifndef COD_H
#define COD_H

bool tool_texture(const char *texture, int map_version);
bool spawn_classname(const char *classname, int map_version);

extern const char **tool_textures[];
extern const char *cod1_tool_textures[];
extern const char *cod2_tool_textures[];
extern const char *cod4_tool_textures[];
extern const char *cod5_tool_textures[];

extern const char **spawn_classnames[];
extern const char *cod1_spawn_classnames[];
extern const char *cod2_spawn_classnames[];
extern const char *cod4_spawn_classnames[];
extern const char *cod5_spawn_classnames[];

#endif // COD_H

