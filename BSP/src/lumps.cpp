#include "bsp/lumps.hpp"

namespace BSP
{
    ////////////////////////////////////////////////////////////////

    const char *LumpName[] =
    {
        "Unknown",
        "Materials",
        "LightMaps",
        "LightGridHash",
        "LightGridValues",
        "Planes",
        "Brushsides",
        "Brushes",
        "TriangleSoups",
        "DrawVertices",
        "DrawIndices",
        "CullGroups",
        "CullGroupIndices",
        "Unknown12",
        "Unknown13",
        "Unknown14",
        "Unknown15",
        "Unknown16",
        "PortalVertices",
        "Occluder",
        "OccluderPlanes",
        "OccluderEdges",
        "OccluderIndices",
        "AABBTrees",
        "Cells",
        "Portals",
        "Nodes",
        "Leaves",
        "LeafBrushes",
        "Unknown28",
        "CollisionVertices",
        "CollisionEdges",
        "CollisionTriangles",
        "CollisionBorders",
        "CollisionParts",
        "CollisionAABBs",
        "Models",
        "Visibility",
        "Entities",

        // cod4
        "LightGridPoints",
        "LightGridColours",
        "LayeredTriangleSoups",
        "LayeredDrawVertices",
        "LayeredDrawIndices",
        "LayeredAABBTrees",
        "Paths",
        "ReflectionProbes",
        "LayeredData",
        "PrimaryLights",
        "LightGridHeader",
        "LightGridRows",
        "SimpleTriangleSoups",
        "SimpleVertices",
        "SimpleIndices",
        "SimpleAABBTrees",
        "LightRegions",
        "LightRegionHulls",
        "LightRegionAxes"
    };

    ////////////////////////////////////////////////////////////////

    void Material::write(FILE *output) const
    {
        for (unsigned int a = 0; a < 64; a++)
        {
            if (name[a])
            {
                fputc(name[a], output);
            }
            else
            {
                fputc(' ', output);
            }
        }

        fprintf(output, "0x%08x 0x%08x", flags[0], flags[1]);
    }

    void Plane::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g %6g", normal[0], normal[1], normal[2], distance);
    }

    void Brushside::write(FILE *output) const
    {
        // exception!!!
    }

    void Brush::write(FILE *output) const
    {
        fprintf(output, "%6u %6u", sides, material_id);
    }

    void TriangleSoup::write(FILE *output) const
    {
        fprintf(output, "%6u %6u %6u %6u %6u %6u", material_id, draw_order, vertex_offset, vertex_length, triangle_offset, triangle_length);
    }

    void DrawVertex1::write(FILE *output) const
    {
        fprintf(output, "%6g %6g %6g %12g %12g %12g 0x%08x %12g %12g %12g %12g", position[0], position[1], position[2], normal[0], normal[1], normal[2], rgba, unknown[0], unknown[1], unknown[2], unknown[3]);
    }

    void DrawVertex2::write(FILE *output) const
    {
        fprintf(output, "%6g %6g %6g %12g %12g %12g 0x%08x %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g", position[0], position[1], position[2], normal[0], normal[1], normal[2], rgba, uv[0], uv[1], st[0], st[1], unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5]);
    }

    void DrawIndex::write(FILE *output) const
    {
        fprintf(output, "%6u %6u %6u", vertex[0], vertex[1], vertex[2]);
    }

    void PortalVertex::write(FILE *output) const
    {
         fprintf(output, "%12g %12g %12g", unknown[0], unknown[1], unknown[2]);
    }

    void AABBTree::write(FILE *output) const
    {
        fprintf(output, "%6u %6u %6u", unknown[0], unknown[1], unknown[2]);
    }

    void Cell::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g", unknown[0], unknown[1], unknown[2]);
    }
    void Portal::write(FILE *output) const
    {
        fprintf(output, "%6u %6u %6u", unknown[0], unknown[1], unknown[2]);
    }

    void Node::write(FILE *output) const
    {
        fprintf(output, "%6i %6i %6i %6i %6i %6i %6i %6i %6i", unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5], unknown[6], unknown[7], unknown[8]);
    }

    void Leaf::write(FILE *output) const
    {
        fprintf(output, "%6i %6i %6i %6i %6i %6i %6i %6i %6i", unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5], unknown[6], unknown[7], unknown[8]);
    }

    void LeafBrush::write(FILE *output) const
    {
        fprintf(output, "%6i", unknown);
    }

    void CollisionVertex::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g %12g", unknown, position[0], position[1], position[2]);
    }

    void CollisionEdge::write(FILE *output) const
    {
        fprintf(output, "%4u %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g", unknown, position[0], position[1], position[2], normal[0][0], normal[0][1], normal[0][2], normal[1][0], normal[1][1], normal[1][2], normal[2][0], normal[2][1], normal[2][2], distance);
    }

    void CollisionTriangle::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %12g %6u %6u %6u %6i %6i %6i", normal[0], normal[1], normal[2], distance, unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5], unknown[6], unknown[7], vertex_id[0], vertex_id[1], vertex_id[2], edge_id[0], edge_id[1], edge_id[2]);
    }

    void CollisionBorder::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g %12g %12g %12g %12g", unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5], unknown[6]);
    }

    void CollisionPart::write(FILE *output) const
    {
        fprintf(output, "%4i %4i %4i %4i %4i %4i", unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5]);
    }

    void CollisionAABB::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g %12g %12g %12g %4u %4u %4u %4u", unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5], unknown2[0], unknown2[1], unknown2[2], unknown2[3]);
    }

    void Model::write(FILE *output) const
    {
        fprintf(output, "%12g %12g %12g %12g %12g %12g %6i %6i %6i %6i %6i %6i", position[0][0], position[0][1], position[0][2], position[1][0], position[1][1], position[1][2], trianglesoups_offset, trianglesoups_size, collisionaabbs_offset, collisionaabbs_size, brushes_offset, brushes_size);
    }

    void Entity::write(FILE *output) const
    {
        // exception
    }
}
