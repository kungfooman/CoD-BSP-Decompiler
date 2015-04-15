#include "settings.hpp"

#include <stdio.h>

extern "C"
{
    #include <lua/lua.h>
    #include <lua/lualib.h>
    #include <lua/lauxlib.h>
}

int initialise_settings(Settings &settings, const char *filename)
{
    lua_State *lua_state = lua_open();
    if (luaL_loadfile(lua_state, filename) || lua_pcall(lua_state, 0, 0, 0))
    {
        fprintf(stderr, "ERROR: Lua says `%s'\n", lua_tostring(lua_state, -1));
        lua_close(lua_state);
        return 1;
    }
    else
    {
        ////////////////

        lua_getglobal(lua_state, "pause");
        if (lua_isnumber(lua_state, -1))
        {
            settings.pause = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `pause' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        ////////////////

        lua_getglobal(lua_state, "batch_file");
        if (lua_isnumber(lua_state, -1))
        {
            settings.batch_file = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `batch_file' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "statistics");
        if (lua_isnumber(lua_state, -1))
        {
            settings.statistics = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `statistics' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        ////////////////

        lua_getglobal(lua_state, "export_raw_lumps");
        if (lua_isnumber(lua_state, -1))
        {
            settings.export_raw_lumps = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `export_raw_lumps' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "export_html");
        if (lua_isnumber(lua_state, -1))
        {
            settings.export_html = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `export_html' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "export_bsp");
        if (lua_isnumber(lua_state, -1))
        {
            settings.export_bsp = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `export_bsp' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "export_lumps");
        if (lua_isnumber(lua_state, -1))
        {
            settings.export_lumps = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `export_lumps' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        ////////////////

        lua_getglobal(lua_state, "map");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "map_version");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_version = lua_tonumber(lua_state, -1);
        }

        if (settings.map_version != 0 && settings.map_version != 1 && settings.map_version != 2 && settings.map_version != 4 && settings.map_version != 5)
        {
            fprintf(stderr, "WARNING: No setting for `map_version' given, defaulting to 0\n");
            settings.map_version = 0;
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "map_layers");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_layers = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_layers' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "map_brushes");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_brushes = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_brushes' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "map_triangles");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_triangles = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_triangles' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        /*lua_getglobal(lua_state, "map_patches");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_patches = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_patches' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);*/

        lua_getglobal(lua_state, "map_entities");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_entities = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_entities' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "map_models");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_models = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_models' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "map_spawns");
        if (lua_isnumber(lua_state, -1))
        {
            settings.map_spawns = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `map_spawns' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        ////////////////

        lua_getglobal(lua_state, "caulk_brushes");
        if (lua_isnumber(lua_state, -1))
        {
            settings.caulk_brushes = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `caulk_brushes' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "caulk_tool_brushes");
        if (lua_isnumber(lua_state, -1))
        {
            settings.caulk_tool_brushes = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `caulk_tool_brushes' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        lua_getglobal(lua_state, "caulk_triangles");
        if (lua_isnumber(lua_state, -1))
        {
            settings.caulk_triangles = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `caulk_triangles' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        ////////////////

        lua_getglobal(lua_state, "layer_split");
        if (lua_isnumber(lua_state, -1))
        {
            settings.layer_split = lua_tonumber(lua_state, -1);
        }
        else
        {
            fprintf(stderr, "WARNING: No setting for `layer_split' given, defaulting to 0\n");
        }
        lua_pop(lua_state, 1);

        ////////////////

    }
    lua_close(lua_state);
    return 0;
}
