#ifndef COMMON_H
#define COMMON_H

#include "settings.hpp"
#include "math.hpp"

#include <bsp/bsp.hpp>

struct Brushplane
{
    Normal normal;
    double distance;
    unsigned int material_id;

    Brushplane()
    {
    }
};

std::string to_str(unsigned int number);
int to_int(const std::string &text);

void write_vertex(FILE *output, const BSP::DrawVertex1 &vertex, const Settings &settings);
void write_vertex(FILE *output, const BSP::DrawVertex2 &vertex, const Settings &settings);
void write_vertex(FILE *output, double x, double y, double z);

void write_brush(FILE *output, Position position1, Position position2, Position position3, const char *texture, const Settings &settings);

void push_if_unique(std::vector<Position> &positions, const Position &position);

bool str_prefix(const char *prefix, const char *string);

#endif // COMMON_H
