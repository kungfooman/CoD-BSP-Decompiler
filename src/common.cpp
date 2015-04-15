#include "common.hpp"
#include "cod.hpp"

#include <sstream>
#include <string.h>

std::string to_str(unsigned int number)
{
    std::string text;
    std::stringstream stream;
    stream << number;
    stream >> text;
    return text;
}

int to_int(const std::string &text)
{
    int number;
    std::stringstream stream(text);
    stream >> number;
    return number;
}

void write_vertex(FILE *output, const BSP::DrawVertex1 &vertex, const Settings &settings)
{
    if (settings.map_version == 1)
    {
        fprintf(
            output,
            " ( %g %g %g %g %g %u %u %u %u 0 )",
            vertex.position[0],
            vertex.position[1],
            vertex.position[2],
            vertex.unknown[0] * 1024,
            vertex.unknown[1] * 1024,
            (vertex.rgba & 0x0000ff00) >> 8, // B
            (vertex.rgba & 0x00ff0000) >> 16, // G
            (vertex.rgba & 0xff000000) >> 24, // R
            (vertex.rgba & 0x000000ff) >> 0 // A
        );
    }
    else if (settings.map_version == 2)
    {
        fprintf(
            output,
            "    v %g %g %g c %u %u %u %u t %g %g %g %g\n",
            vertex.position[0],
            vertex.position[1],
            vertex.position[2],
            (vertex.rgba & 0x0000ff00) >> 8, // B
            (vertex.rgba & 0x00ff0000) >> 16, // G
            (vertex.rgba & 0xff000000) >> 24, // R
            (vertex.rgba & 0x000000ff) >> 0, // A
            vertex.unknown[1] * 1024,
            vertex.unknown[2] * 1024,
            vertex.unknown[1] * 1024,
            vertex.unknown[2] * 1024
        );
    }
    else if (settings.map_version > 3)
    {
        fprintf(
            output,
            "    v %g %g %g t %g %g\n",
            vertex.position[0],
            vertex.position[1],
            vertex.position[2],
            vertex.unknown[1] * 1024,
            vertex.unknown[2] * 1024
        );
    }
}

void write_vertex(FILE *output, const BSP::DrawVertex2 &vertex, const Settings &settings)
{
    if (settings.map_version == 1)
    {
        fprintf(
            output,
            " ( %g %g %g %g %g %u %u %u %u 0 )",
            vertex.position[0],
            vertex.position[1],
            vertex.position[2],
            vertex.uv[0] * 1024,
            vertex.uv[1] * 1024,
            (vertex.rgba & 0x0000ff00) >> 8, // B
            (vertex.rgba & 0x00ff0000) >> 16, // G
            (vertex.rgba & 0xff000000) >> 24, // R
            (vertex.rgba & 0x000000ff) >> 0 // A
        );
    }
    else if (settings.map_version == 2)
    {
        fprintf(
            output,
            "    v %g %g %g c %u %u %u %u t %g %g %g %g\n",
            vertex.position[0],
            vertex.position[1],
            vertex.position[2],
            (vertex.rgba & 0x0000ff00) >> 8, // B
            (vertex.rgba & 0x00ff0000) >> 16, // G
            (vertex.rgba & 0xff000000) >> 24, // R
            (vertex.rgba & 0x000000ff) >> 0, // A
            vertex.uv[0] * 1024,
            vertex.uv[1] * 1024,
            (vertex.st[0] * 1024) + 2,
            -((vertex.st[1] * 1024) + 2)
        );
    }
    else
    {
        fprintf(
            output,
            "    v %g %g %g t %g %g %g %g\n",
            vertex.position[0],
            vertex.position[1],
            vertex.position[2],
            vertex.uv[0] * 1024,
            vertex.uv[1] * 1024,
            (vertex.st[0] * 1024) + 2,
            -((vertex.st[1] * 1024) + 2)
        );
    }
}

void write_vertex(FILE *output, double x, double y, double z)
{
    fprintf(
        output,
        "    v %g %g %g t 0 0 0 0\n",
        x,
        y,
        z
    );
}

void write_brush(FILE *output, Position position1, Position position2, Position position3, const char *texture, const Settings &settings)
{
    bool is_tool = false;
    if (tool_texture(texture, settings.map_version))
    {
        is_tool = true;
    }
    else if (str_prefix("textures/common/", texture))
    {
        texture += strlen("textures/common/");
        if (tool_texture(texture, settings.map_version))
        {
            is_tool = true;
        }
    }

    if (settings.map_version == 1 && str_prefix("textures/", texture))
    {
        texture += strlen("textures/");
    }

    if ((settings.caulk_brushes && settings.caulk_tool_brushes) || (settings.caulk_brushes && !is_tool))
    {
        texture = "caulk";
    }

    if (settings.map_version > 1)
    {
        fprintf(output, " ( %g %g %g ) ( %g %g %g ) ( %g %g %g ) %s 128 128 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0\n",
            position1.x, position1.y, position1.z,
            position2.x, position2.y, position2.z,
            position3.x, position3.y, position3.z,
            texture
        );
    }
    else
    {
        fprintf(output, " ( %g %g %g ) ( %g %g %g ) ( %g %g %g ) %s 0 0 0 1 1 0 0 0 0\n",
            position1.x, position1.y, position1.z,
            position2.x, position2.y, position2.z,
            position3.x, position3.y, position3.z,
            texture
        );
    }
}

void push_if_unique(std::vector<Position> &positions, const Position &position)
{
    bool found = false;
    std::vector<Position>::iterator it;
    for (it = positions.begin(); it < positions.end(); it++)
    {
        if (*it == position)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        positions.push_back(position);
    }
}

bool str_prefix(const char *prefix, const char *string)
{
    while (*prefix)
    {
        if (*prefix != *string)
        {
            return false;
        }
        prefix++;
        string++;
    }
    return true;
}
