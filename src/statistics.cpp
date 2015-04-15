#include "console.hpp"
#include "statistics.hpp"
#include "math.hpp"

#include <ctime>

using namespace BSP;

void write_statistics(BSP_Handle *bsp, const std::string &path)
{
    printf("Writing statistics...");

    const char *filename = (path + "statistics.txt").c_str();
    FILE *output = fopen(filename, "wb+");
    if (!output)
    {
        fprintf(stderr, "ERROR: Could not write to `%s'.\n\n", filename);
    }
    else
    {
        retrieve_cursor_position();
        printf("0%%");

        time_t rawtime;
        time(&rawtime);
        char *time_string = ctime(&rawtime);
        time_string[strlen(time_string) - 1] = 0x00;

        fprintf(output, "Filename: %s\r\n", bsp->map.c_str());
        fprintf(output, "Decompile data: %s\r\n", time_string);
        fprintf(output, "CoD Version: %i\r\n\r\n", bsp->cod_version);

        if (bsp->cod_version < 3)
        {
            fprintf(output, "##      Offset         Size      Entries    Name\r\n----------------------------------------------------------------------\r\n");

            std::vector<Index>::iterator it;
            for (it = bsp->index.begin(); it < bsp->index.end(); it++)
            {
                fprintf(output, "%02u    0x%08x    0x%08x    %6u    %s\r\n", (*it).id, (*it).offset, (*it).size, bsp->size(bsp->lump_reference[(*it).id]), LumpName[bsp->lump_reference[(*it).id]]);
            }
        }
        else if (bsp->cod_version > 3)
        {
            fprintf(output, "##      Offset         Size      Entries    Name\r\n----------------------------------------------------------------------\r\n");

            std::vector<Index>::iterator it;
            for (it = bsp->index.begin(); it < bsp->index.end(); it++)
            {
                fprintf(output, "%02u    0x%08x    0x%08x    %6u    %s\r\n", (*it).id, (*it).offset, (*it).size, bsp->size(bsp->lump_reference[(*it).id]), LumpName[bsp->lump_reference[(*it).id]]);
            }
        }

        fprintf(output, "\r\nTotal size: 0x%08x; Last lump offset + size: 0x%08x;\r\n", bsp->data.size, bsp->index.back().offset + bsp->index.back().size);

        unsigned int nills = 0;
        unsigned int non_ones = 0;

        Plane *plane;
        for (plane = bsp->array<Plane>(PLANES); plane < bsp->array<Plane>(PLANES) + bsp->size(PLANES); plane++)
        {
            double length = sqrt((plane->normal[0] * plane->normal[0]) + (plane->normal[1] * plane->normal[1]) + (plane->normal[2] * plane->normal[2]));

            if (equal(length, 0.0))
            {
                nills++;
            }
            else if (!equal(length, 1.0))
            {
                non_ones++;
            }
        }

        fprintf(output, "\r\nPlane normals: non-length(%u); non-normalized(%u);\r\n", nills, non_ones);

        restore_cursor_position();
        printf("100%%");

        fclose(output);
    }

    printf("\n");
}
