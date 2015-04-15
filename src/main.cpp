#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <boost/filesystem.hpp>
#include <bsp/bsp.hpp>

#include "console.hpp"
#include "settings.hpp"
#include "statistics.hpp"
#include "lumps.hpp"
#include "map.hpp"

using namespace BSP;

int main(int argc, char *argv[])
{
    initialise_console("BSP Decompiler - by Daevius & CoDEmanX");

    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Nothing to decompile, drop one or multiple BSP files onto the executable to decompile\n\n");
        system("pause");
        return 1;
    }

    ////////////////

    Settings settings;
    settings.executable = argv[0];

    std::string root_path(settings.executable);
    root_path = root_path.substr(0, root_path.find_last_of("/\\") + 1);

    std::string data_path = root_path + "data\\";
    boost::filesystem::path bfs_data_path(data_path);
    if (!boost::filesystem::exists(bfs_data_path) || !boost::filesystem::is_directory(bfs_data_path))
    {
        boost::filesystem::create_directory(bfs_data_path);
    }

    ////////////////

    freopen((root_path + "error.log").c_str(), "w", stderr);

    ////////////////

    if (initialise_settings(settings, (root_path + "settings.lua").c_str()))
    {
        printf("\n");
        system("pause");
        return 1;
    }

    ////////////////

    for (int i = 1; i < argc; i++)
    {
        clock_t clock = std::clock();

        printf("File %i out of %i:\n\n", i, argc - 1);

        std::string bsp_filename(argv[i]);
        bsp_filename = bsp_filename.substr(bsp_filename.find_last_of("/\\") + 1);

        std::string bsp_name = bsp_filename.substr(0, bsp_filename.find_first_of("."));
        std::string bsp_path = data_path;/* + bsp_name + '\\';

        boost::filesystem::path bfs_bsp_path(bsp_path);
        if (!boost::filesystem::exists(bfs_bsp_path) || !boost::filesystem::is_directory(bfs_bsp_path))
        {
            boost::filesystem::create_directory(bfs_bsp_path);
        }*/

        ////////////////

        if (settings.batch_file)
        {
            printf("Generating batch file...");
            if (!boost::filesystem::exists(boost::filesystem::path(bsp_path + bsp_filename)))
            {
                boost::filesystem::copy_file(argv[i], bsp_path + bsp_filename);
            }
            printf("done\n");

            printf("Copying BSP file `%s'...", bsp_filename.c_str());
            std::string batch_filename = bsp_path + "decompile.bat";
            if (!boost::filesystem::exists(boost::filesystem::path(batch_filename)))
            {
                FILE *batch_file = fopen(batch_filename.c_str(), "wb+");
                if (!batch_file)
                {
                    fprintf(stderr, "ERROR: Could not write to `%s'.\n\n", batch_filename.c_str());
                }
                else
                {
                    fprintf(batch_file, "@echo off\r\n\"%s\" \"%s\"", settings.executable.c_str(), (bsp_path + bsp_filename).c_str());
                    fclose(batch_file);
                }
            }
            printf("done\n");
        }

        ////////////////

        printf("Parsing BSP file `%s'...\n", bsp_filename.c_str());
        BSP_Handle *bsp = new BSP_Handle(argv[i]);
        if (!bsp->success)
        {
            printf("\n\n");

            printf("\nFinished in %g seconds\n\n", (float) (std::clock() - clock) / CLOCKS_PER_SEC);
            printf("---------------------------------------------\n\n");
            continue;
        }
        printf("done\n\n");

        if (!settings.map_version)
        {
            settings.map_version = bsp->cod_version;
        }

        ////////////////

        if (settings.statistics)
        {
            write_statistics(bsp, bsp_path);
        }

        if (settings.export_raw_lumps)
        {
            boost::filesystem::path bfs_bsp_path_raw_lumps(bsp_path + "raw_lumps\\");
            if (!boost::filesystem::exists(bfs_bsp_path_raw_lumps) || !boost::filesystem::is_directory(bfs_bsp_path_raw_lumps))
            {
                boost::filesystem::create_directory(bfs_bsp_path_raw_lumps);
            }

            write_raw_lumps(bsp, bsp_path + "raw_lumps\\");
        }

        if (settings.export_bsp)
        {
            write_bsp(bsp, bsp_path, bsp_name, settings.export_html);
        }

        if (settings.export_lumps)
        {
            boost::filesystem::path bfs_bsp_path_lumps(bsp_path + "lumps\\");
            if (!boost::filesystem::exists(bfs_bsp_path_lumps) || !boost::filesystem::is_directory(bfs_bsp_path_lumps))
            {
                boost::filesystem::create_directory(bfs_bsp_path_lumps);
            }

            write_lumps(bsp, bsp_path + "lumps\\", settings.export_html);
        }

        if (settings.map)
        {
            write_map(bsp, bsp_path, bsp_name, settings);
        }

        ////////////////

        printf("\nFinished in %g seconds\n\n", (float) (std::clock() - clock) / CLOCKS_PER_SEC);
        printf("---------------------------------------------\n\n");
    }

    if (settings.pause)
    {
        system("pause");
    }
    return 0;
}
