#include "console.hpp"
#include "map.hpp"
#include "math.hpp"
#include "common.hpp"
#include "cod.hpp"

#include <set>
#include <vector>

using namespace BSP;

void write_map(BSP_Handle *bsp, const std::string &bsp_path, const std::string &bsp_name, const Settings &settings)
{
    printf("Writing map file...\n");

    std::string output_filename = bsp_path + bsp_name + ".map";
    FILE *output = fopen(output_filename.c_str(), "wb+");
    if (!output)
    {
        fprintf(stderr, "ERROR: Could not write to `%s'.\n", output_filename.c_str());
    }
    else
    {
        if (settings.map_version > 1)
        {
            fprintf(output, "iwmap 4\n");

            if (settings.map_version > 2)
            {
                fprintf(output, "\"000_Global\" flags  active\n\"The Map\" flags expanded\n");

                if (settings.map_layers)
                {
                    fprintf(output, "\"Brushes\" flags expanded\n\"Brushes/Tools\" flags\n\"Triangles\" flags expanded\n\"Entities\" flags expanded\n\"Entities/Models\" flags\n\"Entities/Spawns\" flags\n");
                }
            }
        }

        unsigned int entity_i = 0;
        std::list<Entity> entities;
        std::map<int, Entity> brushmodels;

        if (bsp->size(ENTITIES))
        {
            map_prepare(bsp, output, entities, brushmodels);
        }

        if (bsp->size(MODELS))
        {
            if (settings.map_version > 1)
            {
                fprintf(output, "// entity %u\n", entity_i);
            }
            fprintf(output, "{\n");
            entity_i++;

            if (brushmodels.find(0) != brushmodels.end())
            {
                std::map<int, Entity>::iterator entity = brushmodels.find(0);
                std::map<std::string, std::string>::iterator pair;
                for (pair = (*entity).second.pairs.begin(); pair != (*entity).second.pairs.end(); pair++)
                {
                    fprintf(output, "\"%s\" \"%s\"\n", (*pair).first.c_str(), (*pair).second.c_str());
                }
            }

            if (settings.map_triangles && bsp->size(TRIANGLESOUPS) && bsp->size(DRAWINDICES) && bsp->size(DRAWVERTICES))
            {
                map_triangles(bsp, output, settings, entity_i);
            }

            if (settings.map_brushes && bsp->size(BRUSHES) && bsp->size(BRUSHSIDES) && bsp->size(PLANES) && bsp->size(TRIANGLESOUPS) && bsp->size(DRAWINDICES) && bsp->size(DRAWVERTICES))
            {
                map_brushes(bsp, output, settings, entity_i, brushmodels);
            }
            fprintf(output, "}\n");
        }

        /*#ifndef RELEASE
        if (settings.map_patches && bsp->size(COLLISIONTRIANGLES) && bsp->size(COLLISIONVERTICES) && bsp->size(MATERIALS))
        {
            map_patches(bsp, output, settings, entity_i);
        }
        #endif*/

        if ((settings.map_entities || settings.map_models) && bsp->size(ENTITIES) > 1)
        {
            map_entities(bsp, output, settings, entity_i, entities);
        }

        fclose(output);
    }
}

void map_prepare(BSP_Handle *bsp, FILE *output, std::list<Entity> &entities, std::map<int, Entity> &brushmodels)
{
    printf("  Preparing...");
    retrieve_cursor_position();

    entities.assign(bsp->array<Entity>(ENTITIES) + 1, bsp->array<Entity>(ENTITIES) + bsp->size(ENTITIES));
    brushmodels[0] = bsp->array<Entity>(ENTITIES)[0];

    unsigned int n = 0;
    bool remove_entity = false;
    std::list<Entity>::iterator entity;
    for (entity = entities.begin(); entity != entities.end(); entity++)
    {
        restore_cursor_position();
        printf("%3.1f%%", ceil(((float) n / entities.size()) * 1000.0f) / 10.0f);

        bool remove_pair = false;
        std::map<std::string, std::string>::iterator terminal;

        std::map<std::string, std::string>::iterator pair;
        for (pair = (*entity).pairs.begin(); pair != (*entity).pairs.end(); pair++)
        {
            if (remove_pair)
            {
                (*entity).pairs.erase(terminal);
                remove_pair = false;
            }

            if ((*pair).first == "model" && (*pair).second[0] == '*')
            {
                std::string id = (*pair).second.substr(1);
                (*entity).pairs.erase(pair);
                brushmodels[to_int(id)] = (*entity);
                remove_entity = true;
                break;
            }
            else if ((*pair).first == "gndLt")
            {
                terminal = pair;
                remove_pair = true;
            }
        }

        if (remove_entity)
        {
            entity = entities.erase(entity);
            entity--;
            remove_entity = false;
        }
        n++;
    }

    restore_cursor_position();
    printf("100.0%%\n");
}

void map_brushes(BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i, std::map<int, Entity> &brushmodels)
{
    printf("  Brushes...");
    retrieve_cursor_position();

    std::list<Brushside> brushsides;
    brushsides.assign(bsp->array<Brushside>(BRUSHSIDES), bsp->array<Brushside>(BRUSHSIDES) + bsp->size(BRUSHSIDES));

    Brushside *brushside = bsp->array<Brushside>(BRUSHSIDES);

    unsigned int brushmodel = 0;
    for (Model *model = bsp->array<Model>(MODELS); model < bsp->array<Model>(MODELS) + bsp->size(MODELS); model++)
    {
        if (entity_i && model != bsp->array<Model>(MODELS))
        {
            fprintf(output, "// entity %u\n", entity_i);
            fprintf(output, "{\n");
            entity_i++;

            if (brushmodels.find(brushmodel) != brushmodels.end())
            {
                std::map<int, Entity>::iterator entity = brushmodels.find(brushmodel);
                std::map<std::string, std::string>::iterator pair;
                for (pair = (*entity).second.pairs.begin(); pair != (*entity).second.pairs.end(); pair++)
                {
                    fprintf(output, "\"%s\" \"%s\"\n", (*pair).first.c_str(), (*pair).second.c_str());
                }
            }
        }

        for (Brush *brush = bsp->array<Brush>(BRUSHES) + model->brushes_offset; brush < bsp->array<Brush>(BRUSHES) + model->brushes_offset + model->brushes_size; brush++)
        {
            restore_cursor_position();
            printf("%3.1f%%", ceil(((float) (brush - bsp->array<Brush>(BRUSHES)) / bsp->size(BRUSHES)) * 1000.0f) / 10.0f);

            std::vector<Brushplane> brushplanes;
            for (unsigned int i = 0; i < (*brush).sides; i++)
            {
                Brushplane brushplane;
                if (i < 6)
                {
                    switch (i)
                    {
                        case 0:
                        {
                            brushplane.normal.x = -1.0;
                            brushplane.normal.y = 0.0;
                            brushplane.normal.z = 0.0;
                            break;
                        }
                        case 1:
                        {
                            brushplane.normal.x = 1.0;
                            brushplane.normal.y = 0.0;
                            brushplane.normal.z = 0.0;
                            break;
                        }
                        case 2:
                        {
                            brushplane.normal.x = 0.0;
                            brushplane.normal.y = -1.0;
                            brushplane.normal.z = 0.0;
                            break;
                        }
                        case 3:
                        {
                            brushplane.normal.x = 0.0;
                            brushplane.normal.y = 1.0;
                            brushplane.normal.z = 0.0;
                            break;
                        }
                        case 4:
                        {
                            brushplane.normal.x = 0.0;
                            brushplane.normal.y = 0.0;
                            brushplane.normal.z = -1.0;
                            break;
                        }
                        case 5:
                        {
                            brushplane.normal.x = 0.0;
                            brushplane.normal.y = 0.0;
                            brushplane.normal.z = 1.0;
                            break;
                        }
                    }

                    brushplane.distance = (*brushside).column1.distance;
                    brushplane.material_id = (*brushside).material_id;
                    brushplanes.push_back(brushplane);
                }
                else
                {
                    //if (unique_brushsides.find((*brushside).material_id) == unique_brushsides.end() || unique_brushsides[(*brushside).material_id].find((*brushside).column1.plane) == unique_brushsides[(*brushside).material_id].end())
                    //{
                        //unique_brushsides[(*brushside).material_id].insert((*brushside).column1.plane);

                    Plane *plane = bsp->array<Plane>(PLANES) + (*brushside).column1.plane;
                    brushplane.normal.x = (*plane).normal[0];
                    brushplane.normal.y = (*plane).normal[1];
                    brushplane.normal.z = (*plane).normal[2];
                    brushplane.distance = (*plane).distance;
                    brushplane.material_id = (*brushside).material_id;

                    bool unique = true;
                    std::vector<Brushplane>::iterator other_brushplane;
                    for (other_brushplane = brushplanes.begin(); other_brushplane < brushplanes.end(); other_brushplane++)
                    {
                        if (brushplane.material_id == (*other_brushplane).material_id)
                        {
                            fprintf(stderr, " id");
                        }
                        fprintf(stderr, " %g %g", brushplane.distance, (*other_brushplane).distance);
                        if ((brushplane.distance - (*other_brushplane).distance) > 0.0f)
                        {
                            fprintf(stderr, " distance");
                        }
                        if (brushplane.normal == (*other_brushplane).normal)
                        {
                            fprintf(stderr, " normal");
                        }

                        if (brushplane.material_id == (*other_brushplane).material_id && ((brushplane.distance - (*other_brushplane).distance) > 0.0f) && brushplane.normal == (*other_brushplane).normal)
                        {
                            unique = false;
                            break;
                        }
                        else if (brushplane.material_id == (*other_brushplane).material_id && ((brushplane.distance - (*other_brushplane).distance) < 0.0f) && brushplane.normal == (*other_brushplane).normal)
                        {
                            unique = false;
                            (*other_brushplane) = brushplane;
                            break;
                        }
                        fprintf(stderr, "\n");
                    }

                    if (unique)
                    {
                        brushplanes.push_back(brushplane);
                        fprintf(stderr, "Brushside: %4u %8.4g %8.4g %8.4g %8.4g %3u\n", (*brushside).column1.plane, brushplane.normal.x, brushplane.normal.y, brushplane.normal.z, brushplane.distance, brushplane.material_id);
                    }
                    else
                    {
                        fprintf(stderr, "Discarting brushside %u;\r\n", (brushside - bsp->array<Brushside>(BRUSHSIDES)));
                    }
                }
                brushside++;
            }
            fprintf(stderr, "\n");

            std::map<unsigned int, std::vector<Position> > brushplane_intersections;

            unsigned int n = 0;
            std::vector<Brushplane>::iterator brushplane;
            // print out the results for the other dynamic sides
            for (brushplane = (brushplanes.begin() + 6); brushplane < brushplanes.end(); brushplane++)
            {
                Position position;
                double xn = (*brushplane).normal.x,
                       yn = (*brushplane).normal.y,
                       zn = (*brushplane).normal.z,
                       x0 = xn * (*brushplane).distance,
                       y0 = yn * (*brushplane).distance,
                       z0 = zn * (*brushplane).distance;

                // find intersections to the static sides
                std::vector<Position> intersections;
                if (xn)
                {
                    //fprintf(error, "On X: %6g %6g  %6g %6g:", brushplanes[2].distance, brushplanes[3].distance, brushplanes[4].distance, brushplanes[5].distance);

                    position(0.0, brushplanes[2].distance, brushplanes[4].distance);
                    position.x = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.y * yn) - (position.z * zn)) / xn;
                    if (within(brushplanes[0].distance, position.x, brushplanes[1].distance))
                    {
                        //fprintf(error, " %6g", position.x);
                        push_if_unique(intersections, position);
                    }

                    position(0.0, brushplanes[2].distance, brushplanes[5].distance);
                    position.x = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.y * yn) - (position.z * zn)) / xn;
                    if (within(brushplanes[0].distance, position.x, brushplanes[1].distance))
                    {
                        //fprintf(error, " %6g", position.x);
                        push_if_unique(intersections, position);
                    }

                    position(0.0, brushplanes[3].distance, brushplanes[4].distance);
                    position.x = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.y * yn) - (position.z * zn)) / xn;
                    if (within(brushplanes[0].distance, position.x, brushplanes[1].distance))
                    {
                        //fprintf(error, " %6g", position.x);
                        push_if_unique(intersections, position);
                    }

                    position(0.0, brushplanes[3].distance, brushplanes[5].distance);
                    position.x = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.y * yn) - (position.z * zn)) / xn;
                    if (within(brushplanes[0].distance, position.x, brushplanes[1].distance))
                    {
                        //fprintf(error, " %6g", position.x);
                        push_if_unique(intersections, position);
                    }

                    //fprintf(error, "\r\n");
                }

                if (yn)
                {
                    //fprintf(error, "On Y: %6g %6g  %6g %6g:", brushplanes[0].distance, brushplanes[1].distance, brushplanes[4].distance, brushplanes[5].distance);

                    position(brushplanes[0].distance, 0.0, brushplanes[4].distance);
                    position.y = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.z * zn)) / yn;
                    if (within(brushplanes[2].distance, position.y, brushplanes[3].distance))
                    {
                        //fprintf(error, " %6g", position.y);
                        push_if_unique(intersections, position);
                    }

                    position(brushplanes[0].distance, 0.0, brushplanes[5].distance);
                    position.y = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.z * zn)) / yn;
                    if (within(brushplanes[2].distance, position.y, brushplanes[3].distance))
                    {
                        //fprintf(error, " %6g", position.y);
                        push_if_unique(intersections, position);
                    }

                    position(brushplanes[1].distance, 0.0, brushplanes[4].distance);
                    position.y = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.z * zn)) / yn;
                    if (within(brushplanes[2].distance, position.y, brushplanes[3].distance))
                    {
                        //fprintf(error, " %6g", position.y);
                        push_if_unique(intersections, position);
                    }

                    position(brushplanes[1].distance, 0.0, brushplanes[5].distance);
                    position.y = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.z * zn)) / yn;
                    if (within(brushplanes[2].distance, position.y, brushplanes[3].distance))
                    {
                        //fprintf(error, " %6g", position.y);
                        push_if_unique(intersections, position);
                    }

                    //fprintf(error, "\r\n");
                }

                if (zn)
                {
                    //fprintf(error, "On Z: %6g %6g  %6g %6g:", brushplanes[0].distance, brushplanes[1].distance, brushplanes[2].distance, brushplanes[3].distance);

                    position(brushplanes[0].distance, brushplanes[2].distance, 0.0);
                    position.z = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.y * yn)) / zn;
                    if (within(brushplanes[4].distance, position.z, brushplanes[5].distance))
                    {
                        //fprintf(error, " %6g", position.z);
                        push_if_unique(intersections, position);
                    }

                    position(brushplanes[0].distance, brushplanes[3].distance, 0.0);
                    position.z = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.y * yn)) / zn;
                    if (within(brushplanes[4].distance, position.z, brushplanes[5].distance))
                    {
                        //fprintf(error, " %6g", position.z);
                        push_if_unique(intersections, position);
                    }

                    position(brushplanes[1].distance, brushplanes[2].distance, 0.0);
                    position.z = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.y * yn)) / zn;
                    if (within(brushplanes[4].distance, position.z, brushplanes[5].distance))
                    {
                        //fprintf(error, " %6g", position.z);
                        push_if_unique(intersections, position);
                    }

                    position(brushplanes[1].distance, brushplanes[3].distance, 0.0);
                    position.z = ((x0 * xn) + (y0 * yn) + (z0 * zn) - (position.x * xn) - (position.y * yn)) / zn;
                    if (within(brushplanes[4].distance, position.z, brushplanes[5].distance))
                    {
                        //fprintf(error, " %6g", position.z);
                        push_if_unique(intersections, position);
                    }

                    //fprintf(error, "\r\n");
                }

                /*fprintf(error, "Max: %u\r\n", intersections.size());

                std::vector<Position>::iterator intersection;
                for (intersection = intersections.begin(); intersection < intersections.end(); intersection++)
                {
                    fprintf(error, "- %6g %6g %6g\r\n", (*intersection).x, (*intersection).y, (*intersection).z);
                }*/

                unsigned int max = intersections.size();
                if (max < 3)
                {
                    #ifndef RELEASE
                    fprintf(stderr, "Ignoring brushside %6u of brush %6u: less then 3 intersections found with the bounding box\n", std::distance(brushplanes.begin(), brushplane), (brush - bsp->array<Brush>(BRUSHES)));
                    #endif
                    n++;
                    continue;
                }
                else
                {
                    //fprintf(error, "\r\n\r\n");
                    double least_difference = -1.0;
                    unsigned int least_i = 0, least_j = 0, least_k = 0;

                    double tmp_difference = 0.0f;
                    unsigned int i = 0, j = 0, k = 0;
                    //bool stop = false;
                    for (i = 0; i < max; i++)
                    {
                        for (j = 0; j < max; j++)
                        {
                            if (i != j)
                            {
                                for (k = 0; k < max; k++)
                                {
                                    if (i != k && j != k)
                                    {
                                        Normal normal(triangle_normal(intersections[i], intersections[j], intersections[k]));
                                        tmp_difference = difference(normal.x, (*brushplane).normal.x) + difference(normal.y, (*brushplane).normal.y) + difference(normal.z, (*brushplane).normal.z);
                                        if (tmp_difference < LEAST_VALUE_EPSILON && (tmp_difference < least_difference || least_difference == -1.0))
                                        {
                                            least_difference = tmp_difference;
                                            least_i = i;
                                            least_j = j;
                                            least_k = k;
                                        }

                                        //fprintf(error, "Epsilon indication: %g %g %g with normals: %g %g %g  %g %g %g", abs(normal.x - (*brushplane).normal[0]), abs(normal.y - (*brushplane).normal[1]), abs(normal.z - (*brushplane).normal[2]), normal.x, normal.y, normal.z, (*brushplane).normal[0], (*brushplane).normal[1], (*brushplane).normal[2]);
                                        /*if (equal(normal.x, (*brushplane).normal[0]) && equal(normal.y, (*brushplane).normal[1]) && equal(normal.z, (*brushplane).normal[2]))
                                        {
                                            //fprintf(error, "...succes\r\n");
                                            stop = true;
                                            break;
                                        }*/
                                        //fprintf(error, "\r\n");
                                    }
                                }
                            }
                        }
                    }

                    if (least_difference == -1.0)
                    {
                        #ifndef RELEASE
                        fprintf(stderr, "\nIgnoring brushside %u of brush %u: no sequence found of 3 intersections giving a correct normal of (%g %g %g) out of these intersections:\n", std::distance(brushplanes.begin(), brushplane), (brush - bsp->array<Brush>(BRUSHES)), (*brushplane).normal.x, (*brushplane).normal.y, (*brushplane).normal.z);
                        std::vector<Position>::iterator intersection;
                        for (intersection = intersections.begin(); intersection < intersections.end(); intersection++)
                        {
                            fprintf(stderr, "(%g %g %g)\n", (*intersection).x, (*intersection).y, (*intersection).z);
                        }

                        fprintf(stderr, "Those interactions give these combinations and their normal:\n");

                        for (i = 0; i < max; i++)
                        {
                            for (j = 0; j < max; j++)
                            {
                                if (i != j)
                                {
                                    for (k = 0; k < max; k++)
                                    {
                                        if (i != k && j != k)
                                        {
                                            Normal normal(triangle_normal(intersections[i], intersections[j], intersections[k]));
                                            fprintf(stderr, "(%g %g %g) (%g %g %g) (%g %g %g) normal: (%g %g %g)\n", intersections[i].x, intersections[i].y, intersections[i].z, intersections[j].x, intersections[j].y, intersections[j].z, intersections[k].x, intersections[k].y, intersections[k].z, normal.x, normal.y, normal.z);
                                        }
                                    }
                                }
                            }
                        }

                        fprintf(stderr, "\n");
                        #endif
                        n++;
                        continue;
                    }

                    brushplane_intersections[n].push_back(intersections[least_i]);
                    brushplane_intersections[n].push_back(intersections[least_j]);
                    brushplane_intersections[n].push_back(intersections[least_k]);
                }
                n++;
            }

            #ifndef RELEASE
            fprintf(output, "// ###: %2u %2u\n", (brush - bsp->array<Brush>(BRUSHES)), std::distance(brushplanes.begin(), brushplane));
            #endif

            /*for (brushplane = brushplanes.begin(); brushplane < brushplanes.end(); brushplane++)
            {
                if (((*brushplane).normal.x + (*brushplane).normal.y + (*brushplane).normal.z) < 0.0)
                {
                    (*brushplane).distance = -(*brushplane).distance;
                }
            }

            n = 0;
            for (brushplane = brushplanes.begin(); brushplane < brushplanes.end(); brushplane++)
            {
                fprintf(output, "\r\n///////////////////////////////////////////////////////////////////\r\n");
                std::list<Line> lines;
                for (std::vector<Brushplane>::iterator brushplane2 = brushplanes.begin(); brushplane2 < brushplanes.end(); brushplane2++)
                {
                    if (brushplane != brushplane2)
                    {
                        if (plane_plane_intersection((*brushplane), (*brushplane2), lines))
                        {
                            fprintf(output, "// slope (%4g, %4g, %4g) %4g || (%4g, %4g, %4g) %4g\r\n", (*brushplane).normal.x, (*brushplane).normal.y, (*brushplane).normal.z, (*brushplane).distance, (*brushplane2).normal.x, (*brushplane2).normal.y, (*brushplane2).normal.z, (*brushplane2).distance);
                        }
                    }
                }

                unsigned int size = lines.size();

                for (std::list<Line>::iterator line = lines.begin(); line != lines.end(); line++)
                {
                    fprintf(output, "// line (%4g, %4g, %4g) + t(%4g, %4g, %4g)\r\n", (*line).point.x, (*line).point.y, (*line).point.z, (*line).slope.x, (*line).slope.y, (*line).slope.z);
                }

                unsigned int v = 0;
                Vertex *convex = new Vertex[size], closest_vertex, tmp_vertex;

                bool first_t;
                double t, tmp_t;
                Line first_line = *(lines.begin());
                std::list<Line>::iterator line = lines.begin(), closest_line;

                for (unsigned int i = 0; i < (size - 1); i++)
                {
                    first_t = true;
                    closest_line = line;
                    for (std::list<Line>::iterator iterator = lines.begin(); iterator != lines.end(); iterator++)
                    {
                        if (line != iterator && dot((*brushplane).normal, cross((*line).slope, (*iterator).slope)) > 0.0 && line_line_intersection(*line, *iterator, tmp_vertex, tmp_t))
                        {
                            if (first_t || tmp_t > t)
                            {
                                t = tmp_t;
                                first_t = false;

                                closest_line = iterator;
                                closest_vertex = tmp_vertex;
                            }
                        }
                    }

                    if (closest_line != line)
                    {
                        fprintf(output, "// xline (%4g, %4g, %4g) (%4g, %4g, %4g)\r\n", (*line).point.x, (*line).point.y, (*line).point.z, (*line).slope.x, (*line).slope.y, (*line).slope.z);

                        lines.erase(line);
                        line = closest_line;

                        convex[v] = closest_vertex;
                        v++;
                    }
                    else
                    {
                        fprintf(error, "No intersection found between the boundary lines of a flat, convex brushside. %u\r\n", i);
                        break;
                    }
                }

                if (line_line_intersection(first_line, *(lines.begin()), tmp_vertex, t))
                {
                    fprintf(output, "// xline (%4g, %4g, %4g) (%4g, %4g, %4g)\r\n", (*lines.begin()).point.x, (*lines.begin()).point.y, (*lines.begin()).point.z, (*lines.begin()).slope.x, (*lines.begin()).slope.y, (*lines.begin()).slope.z);
                    convex[v] = tmp_vertex;
                    v++;
                }

                fprintf(output, "// convex");
                for (unsigned int i = 0; i < v; i++)
                {
 ( -1061.09 -2848.28 113 ) ( -1111.3199 -2834.8201 49 ) ( -1111.3199 -2834.8201 113 ) caulk 128 128 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0
                    fprintf(output, " (%4g, %4g, %4g)", convex[i].x, convex[i].y, convex[i].z);
                }
                fprintf(output, "\r\n");

                // see which triangles share the same position as the brush sides
                TriangleSoup *triangle_soup;
                for (triangle_soup = bsp->array<TriangleSoup>(TRIANGLESOUPS); triangle_soup < bsp->array<TriangleSoup>(TRIANGLESOUPS) + bsp->size(TRIANGLESOUPS); triangle_soup++)
                {
                    if ((*triangle_soup).material_id == (*brushplane).material_id && !((*triangle_soup).triangle_length % 3) && !((*triangle_soup).triangle_offset % 3))
                    {
                        for (unsigned int i = 0; i < ((*triangle_soup).triangle_length / 3); i++)
                        {
                            unsigned int offset = ((*triangle_soup).triangle_offset / 3);

                            DrawVertex *drawvertex[3] = {
                                &bsp->array<DrawVertex>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[0]],
                                &bsp->array<DrawVertex>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[1]],
                                &bsp->array<DrawVertex>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]]
                            };

                            if (drawvertex[0]->rgba == 0xffffffff && drawvertex[1]->rgba == 0xffffffff && drawvertex[2]->rgba == 0xffffffff)
                            {
                                Vertex triangle[3] = {
                                    Vertex(drawvertex[0]->position[0], drawvertex[0]->position[1], drawvertex[0]->position[2]),
                                    Vertex(drawvertex[1]->position[0], drawvertex[1]->position[1], drawvertex[1]->position[2]),
                                    Vertex(drawvertex[2]->position[0], drawvertex[2]->position[1], drawvertex[2]->position[2])
                                };

                                Normal normal(drawvertex[0]->normal[0], drawvertex[0]->normal[1], drawvertex[0]->normal[2]);
                                normal /= normal.length();

                                Normal convex_normal = cross(convex[1] - convex[0], convex[2] - convex[0]);
                                convex_normal /= convex_normal.length();

                                //fprintf(output, "// normal2 (%4g, %4g, %4g)\r\n", normal2.x, normal2.y, normal2.z);
                                fprintf(output, "// normal (%4g, %4g, %4g) %4g == (%4g, %4g, %4g) %4g\r\n", normal.x, normal.y, normal.z, dot(normal, triangle[0]), (*brushplane).normal.x, (*brushplane).normal.y, (*brushplane).normal.z, (*brushplane).distance);
                                if (coplanar(triangle, normal, *brushplane))//in_brushplane(triangle[0], normal[0], *brushplane) && in_brushplane(triangle[1], normal[1], *brushplane) && in_brushplane(triangle[2], normal[2], *brushplane))
                                {
                                    //fprintf(output, "// #%2u: %2u %2u  %2u %2u", n, (triangle_soup - bsp->array<TriangleSoup>(TRIANGLESOUPS)), i, (brush - bsp->array<Brush>(BRUSHES)), std::distance(brushplanes.begin(), brushplane));
                                    fprintf(output, "// coplanar (%4g, %4g, %4g) (%4g, %4g, %4g) (%4g, %4g, %4g)\r\n", triangle[0].x, triangle[0].y, triangle[0].z, triangle[1].x, triangle[1].y, triangle[1].z, triangle[2].x, triangle[2].y, triangle[2].z);
                                    if (triangle_overlaps_convex(triangle, normal, convex, v, convex_normal))
                                    {
                                        fprintf(output, "YES!\r\n");
                                    }
                                    n++;
                                }
                            }
                        }
                    }
                }

                delete[] convex;
                fprintf(output, "\r\n");
            }*/

            // print out the results of the 6 static sides
            fprintf(output, "// brush %u\n{\n", (brush - bsp->array<Brush>(BRUSHES)));

            if (settings.map_layers && settings.map_version > 2)
            {
                bool tool = true;
                for (brushplane = brushplanes.begin(); brushplane < brushplanes.end(); brushplane++)
                {
                    if (!tool_texture(bsp->array<Material>(MATERIALS)[(*brushplane).material_id].name, settings.map_version))
                    {
                        tool = false;
                        break;
                    }
                }

                if (tool)
                {
                    fprintf(output, "layer \"Brushes/Tools\"\n");
                }
                else
                {
                    fprintf(output, "layer \"Brushes\"\n");
                }
            }

            write_brush(output,
                Position(brushplanes[1].distance, brushplanes[3].distance, brushplanes[4].distance),
                Position(brushplanes[0].distance, brushplanes[3].distance, brushplanes[4].distance),
                Position(brushplanes[0].distance, brushplanes[2].distance, brushplanes[4].distance),
                bsp->array<Material>(MATERIALS)[brushplanes[4].material_id].name,
                settings
            );

            write_brush(output,
                Position(brushplanes[0].distance, brushplanes[2].distance, brushplanes[5].distance),
                Position(brushplanes[0].distance, brushplanes[3].distance, brushplanes[5].distance),
                Position(brushplanes[1].distance, brushplanes[3].distance, brushplanes[5].distance),
                bsp->array<Material>(MATERIALS)[brushplanes[5].material_id].name,
                settings
            );

            write_brush(output,
                Position(brushplanes[0].distance, brushplanes[2].distance, brushplanes[5].distance),
                Position(brushplanes[1].distance, brushplanes[2].distance, brushplanes[5].distance),
                Position(brushplanes[1].distance, brushplanes[2].distance, brushplanes[4].distance),
                bsp->array<Material>(MATERIALS)[brushplanes[3].material_id].name,
                settings
            );

            write_brush(output,
                Position(brushplanes[1].distance, brushplanes[2].distance, brushplanes[5].distance),
                Position(brushplanes[1].distance, brushplanes[3].distance, brushplanes[5].distance),
                Position(brushplanes[1].distance, brushplanes[3].distance, brushplanes[4].distance),
                bsp->array<Material>(MATERIALS)[brushplanes[1].material_id].name,
                settings
            );

            write_brush(output,
                Position(brushplanes[1].distance, brushplanes[3].distance, brushplanes[5].distance),
                Position(brushplanes[0].distance, brushplanes[3].distance, brushplanes[5].distance),
                Position(brushplanes[0].distance, brushplanes[3].distance, brushplanes[4].distance),
                bsp->array<Material>(MATERIALS)[brushplanes[2].material_id].name,
                settings
            );

            write_brush(output,
                Position(brushplanes[0].distance, brushplanes[3].distance, brushplanes[5].distance),
                Position(brushplanes[0].distance, brushplanes[2].distance, brushplanes[5].distance),
                Position(brushplanes[0].distance, brushplanes[2].distance, brushplanes[4].distance),
                bsp->array<Material>(MATERIALS)[brushplanes[0].material_id].name,
                settings
            );

            n = 0;
            for (brushplane = (brushplanes.begin() + 6); brushplane < brushplanes.end(); brushplane++)
            {
                if (brushplane_intersections.find(n) != brushplane_intersections.end())
                {
                    write_brush(output,
                        Position(brushplane_intersections[n][0].x, brushplane_intersections[n][0].y, brushplane_intersections[n][0].z),
                        Position(brushplane_intersections[n][1].x, brushplane_intersections[n][1].y, brushplane_intersections[n][1].z),
                        Position(brushplane_intersections[n][2].x, brushplane_intersections[n][2].y, brushplane_intersections[n][2].z),
                        bsp->array<Material>(MATERIALS)[(*brushplane).material_id].name,
                        settings
                    );
                }
                n++;
            }

            fprintf(output, "}\n");
        }

        if ((model + 1) < bsp->array<Model>(MODELS) + bsp->size(MODELS))
        {
            fprintf(output, "}\n");
        }
        brushmodel++;
    }

    restore_cursor_position();
    printf("100.0%%\n");
}

void map_triangles(BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i)
{
    printf("  Triangles...");
    retrieve_cursor_position();

    /*if (entity_i || (!entity_i && (settings.map_version == 4 || settings.map_version == 5)))
    {
        fprintf(output, "// entity %u\n", entity_i);
    }
    fprintf(output, "{\n");
    entity_i++;*/

    TriangleSoup *triangle_soup;
    for (triangle_soup = bsp->array<TriangleSoup>(TRIANGLESOUPS); triangle_soup < bsp->array<TriangleSoup>(TRIANGLESOUPS) + bsp->size(TRIANGLESOUPS); triangle_soup++)
    {
        restore_cursor_position();
        printf("%3.1f%%", ceil(((float) (triangle_soup - bsp->array<TriangleSoup>(TRIANGLESOUPS)) / bsp->size(TRIANGLESOUPS)) * 1000.0f) / 10.0f);

        if (!((*triangle_soup).triangle_length % 3) && !((*triangle_soup).triangle_offset % 3))
        {
            for (unsigned int i = 0; i < ((*triangle_soup).triangle_length / 3); i++)
            {
                unsigned int offset = ((*triangle_soup).triangle_offset / 3);

                if (settings.map_version > 1)
                {
                    fprintf(output, " {\n  mesh\n  {\n");

                    if (settings.map_layers && settings.map_version > 2)
                    {
                        fprintf(output, "layer \"Triangles\"\n");
                    }

                    if (settings.map_version > 2)
                    {
                        fprintf(output, "   toolFlags splitGeo;\n");
                    }

                    if (settings.caulk_triangles)
                    {
                        fprintf(output, "   caulk\n");
                    }
                    else
                    {
                        fprintf(output, "   %s\r\n", bsp->array<Material>(MATERIALS)[(*triangle_soup).material_id].name);
                    }
                    fprintf(output, "   lightmap_gray\n   2 2 16 8\n   (\n");

                    if (bsp->cod_version == 1)
                    {
                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);
                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);

                        fprintf(output, "   )\n   (\n");

                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[1]], settings);
                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[0]], settings);
                    }
                    else if (bsp->cod_version == 2)
                    {
                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);
                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);

                        fprintf(output, "   )\n   (\n");

                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[1]], settings);
                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[0]], settings);
                    }

                    fprintf(output, "   )\n  }\n }\n");
                }
                else
                {
                    fprintf(output, " {\n  patchTerrainDef3\n  {\n");

                    if (settings.caulk_triangles)
                    {
                        fprintf(output, "   caulk\n");
                    }
                    else
                    {
                        char *texture = bsp->array<Material>(MATERIALS)[(*triangle_soup).material_id].name;
                        if (str_prefix("textures/", texture))
                        {
                            texture += strlen("textures/");
                        }
                        fprintf(output, "   %s\r\n", texture);
                    }
                    fprintf(output, "   ( 2 2 0 0 0 15 4 )\n(\n(");

                    if (bsp->cod_version == 1)
                    {
                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);
                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);

                        fprintf(output, " )\n(");

                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[1]], settings);
                        write_vertex(output, bsp->array<DrawVertex1>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[0]], settings);
                    }
                    else if (bsp->cod_version == 2)
                    {
                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);
                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[2]], settings);

                        fprintf(output, " )\n(");

                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[1]], settings);
                        write_vertex(output, bsp->array<DrawVertex2>(DRAWVERTICES)[(*triangle_soup).vertex_offset + bsp->array<DrawIndex>(DRAWINDICES)[offset + i].vertex[0]], settings);
                    }

                    fprintf(output, " )\n)\n  }\n }\n");
                }
            }
        }
    }

    //fprintf(output, "}\n");

    restore_cursor_position();
    printf("100.0%%\n");
}

/*void map_patches(BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i)
{
    printf("  Patches...");
    retrieve_cursor_position();

    fprintf(output, "// entity %u\n{\n", entity_i);
    entity_i++;

    std::list<Triangle> triangles;

    CollisionTriangle *collision_triangle;
    for (collision_triangle = bsp->array<CollisionTriangle>(COLLISIONTRIANGLES); collision_triangle < bsp->array<CollisionTriangle>(COLLISIONTRIANGLES) + bsp->size(COLLISIONTRIANGLES); collision_triangle++)
    {
        restore_cursor_position();
        printf("%3.1f%%", ceil(((float) (collision_triangle - bsp->array<CollisionTriangle>(COLLISIONTRIANGLES)) / bsp->size(COLLISIONTRIANGLES)) * 1000.0f) / 10.0f);

        Triangle new_triangle((*collision_triangle).vertex_id[0], (*collision_triangle).vertex_id[1], (*collision_triangle).vertex_id[2]);

        bool found_shared_diagonal = false;
        std::list<Triangle>::iterator triangle;
        for (triangle = triangles.begin(); triangle != triangles.end(); triangle++)
        {
            unsigned int vertices[4] = {0, 0, 0, 0};
            if ((*triangle).share_diagonal(new_triangle, vertices))
            {
                fprintf(output, " {\n  mesh\n  {\n");
                if (settings.map_version == 4 || settings.map_version == 5)
                {
                    fprintf(output, "   toolFlags splitGeo;\n");
                }
                fprintf(output, "   caulk\n   lightmap_gray\n   2 2 16 8\n   (\n");

                write_vertex(output, bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[0]].position[0], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[0]].position[1], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[0]].position[2]);
                write_vertex(output, bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[1]].position[0], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[1]].position[1], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[1]].position[2]);

                fprintf(output, "   )\n   (\n");
                write_vertex(output, bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[2]].position[0], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[2]].position[1], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[2]].position[2]);
                write_vertex(output, bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[3]].position[0], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[3]].position[1], bsp->array<CollisionVertex>(COLLISIONVERTICES)[vertices[3]].position[2]);

                fprintf(output, "   )\n  }\n }\n");

                found_shared_diagonal = true;
                triangles.erase(triangle);
                break;
            }
        }

        if (!found_shared_diagonal)
        {
            triangles.push_back(new_triangle);
        }
    }

    fprintf(output, "}\n");

    restore_cursor_position();
    printf("100.0%%\n");
}*/

void map_entities(BSP_Handle *bsp, FILE *output, const Settings &settings, unsigned int &entity_i, std::list<Entity> &entities)
{
    printf("  Entities...");
    retrieve_cursor_position();

    unsigned int n = 1;
    std::list<Entity>::iterator entity;
    for (entity = entities.begin(); entity != entities.end(); entity++)
    {
        restore_cursor_position();
        printf("%3.1f%%", ceil(((float) (n - 1) / (bsp->size(ENTITIES) - 1)) * 1000.0f) / 10.0f);

        if (entity_i || (!entity_i && settings.map_version > 1))
        {
            fprintf(output, "// entity %u\n", entity_i);
        }
        fprintf(output, "{\n");
        entity_i++;

        if (settings.map_layers && settings.map_version > 2)
        {
            if ((*entity).pairs.find("classname")->second == "misc_model" || (*entity).pairs.find("classname")->second == "script_model")
            {
                if (settings.map_models)
                {
                    fprintf(output, "layer \"Entities/Models\"\n");
                }
            }
            else if (spawn_classname((*entity).pairs.find("classname")->second.c_str(), settings.map_version))
            {
                if (settings.map_spawns)
                {
                    fprintf(output, "layer \"Entities/Spawns\"\n");
                }
            }
            else if (settings.map_entities)
            {
                fprintf(output, "layer \"Entities\"\n");
            }
        }

        std::map<std::string, std::string>::iterator pair;
        for (pair = (*entity).pairs.begin(); pair != (*entity).pairs.end(); pair++)
        {
            fprintf(output, " \"%s\" \"%s\"\n", (*pair).first.c_str(), (*pair).second.c_str());
        }

        fprintf(output, "}\n");
        n++;
    }

    restore_cursor_position();
    printf("100.0%%\n");
}
