#ifndef MAP_H
#define MAP_H

#include <cmath>
#include <list>

#include <bsp/bsp.hpp>

#include "settings.hpp"

void write_map(BSP::BSP_Handle *bsp, const std::string &bsp_path, const std::string &bsp_name, const Settings &settings);
void map_prepare(BSP::BSP_Handle *bsp, FILE *output, std::list<BSP::Entity> &entities, std::map<int, BSP::Entity> &brushmodels);
void map_brushes(BSP::BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i, std::map<int, BSP::Entity> &brushmodels);
void map_triangles(BSP::BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i);
//void map_patches(BSP::BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i);
void map_entities(BSP::BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i, std::list<BSP::Entity> &entities);

#endif // MAP_H
