#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

struct Settings
{
    std::string executable;

    bool pause;

    bool batch_file;
    bool statistics;

    bool export_raw_lumps;
    bool export_html;
    bool export_bsp;
    bool export_lumps;

    bool map;
    int map_version;
    bool map_layers;
    bool map_brushes;
    bool map_triangles;
    //bool map_patches;
    bool map_entities;
    bool map_models;
    bool map_spawns;

    bool caulk_brushes;
    bool caulk_tool_brushes;
    bool caulk_triangles;

    bool layer_split;

    Settings() :
        pause(false),

        batch_file(false),
        statistics(false),

        export_raw_lumps(false),
        export_html(false),
        export_bsp(false),
        export_lumps(false),

        map(false),
        map_version(0),
        map_layers(false),
        map_brushes(false),
        map_triangles(false),
        //map_patches(false),
        map_entities(false),
        map_models(false),
        map_spawns(false),

        caulk_brushes(false),
        caulk_tool_brushes(false),
        caulk_triangles(false),

        layer_split(false)
    {
    }
};

int initialise_settings(Settings &settings, const char *filename);

#endif // SETTINGS_H
