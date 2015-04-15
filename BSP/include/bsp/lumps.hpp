#ifndef BSP_LUMPS
#define BSP_LUMPS

#include <cstdio>
#include <string>
#include <map>

#undef PLANES

namespace BSP
{
    ////////////////////////////////////////////////////////////////

    extern const char *LumpName[];

    ////////////////////////////////////////////////////////////////

    enum LumpReference
    {
        // cod1
        UNKNOWN,
        MATERIALS,
        LIGHTMAPS,
        LIGHTGRIDHASH,
        LIGHTGRIDVALUES,
        PLANES,
        BRUSHSIDES,
        BRUSHES,
        TRIANGLESOUPS,
        DRAWVERTICES,
        DRAWINDICES,
        CULLGROUPS,
        CULLGROUPINDICES,
        UNKNOWN12,
        UNKNOWN13,
        UNKNOWN14,
        UNKNOWN15,
        UNKNOWN16,
        PORTALVERTICES,
        OCCLUDER,
        OCCLUDERPLANES,
        OCCLUDEREDGES,
        OCCLUDERINDICES,
        AABBTREES,
        CELLS,
        PORTALS,
        NODES,
        LEAVES,
        LEAFBRUSHES,
        UNKNOWN28,
        COLLISIONVERTICES,
        COLLISIONEDGES,
        COLLISIONTRIANGLES,
        COLLISIONBORDERS,
        COLLISIONPARTS,
        COLLISIONAABBS,
        MODELS,
        VISIBILITY,
        ENTITIES,

        // cod4
        LIGHTGRIDPONTS,
        LIGHTGRIDCOLOURS,
        LAYEREDTRIANGLESOUPS,
        LAYEREDDRAWVERTICES,
        LAYEREDDRAWINDICES,
        LAYEREDAABBTREES,
        PATHS,
        REFLECTIONPROBES,
        LAYEREDDATA,
        PRIMARYLIGHTS,
        LIGHTGRIDHEADER,
        LIGHTGRIDROWS,
        SIMPLETRIANGLESOUPS,
        SIMPLEVERTICES,
        SIMPLEINDICES,
        SIMPLEAABBTREES,
        LIGHTREGIONS,
        LIGHTREGIONHULLS,
        LIGHTREGIONAXES
    };

    ////////////////////////////////////////////////////////////////

    struct Material
    {
        char name[64];
        unsigned int flags[2];

        void write(FILE *output) const;
    };

    struct Plane
    {
        float normal[3];
        float distance;

        void write(FILE *output) const;
    };

    struct Brushside
    {
        union Column1
        {
            unsigned int plane;
            float distance;
        } column1;

        unsigned int material_id;

        void write(FILE *output) const;
    };

    struct Brush
    {
        unsigned short sides;
        unsigned short material_id;

        void write(FILE *output) const;
    };

    struct TriangleSoup
    {
        unsigned short material_id;
        unsigned short draw_order;

        unsigned int vertex_offset;
        unsigned short vertex_length;

        unsigned short triangle_length;
        unsigned int triangle_offset;

        void write(FILE *output) const;
    };

    /*struct DrawVertex
    {
        float position[3];
        float normal[3];

        unsigned int rgba;

        virtual void write(FILE *output) const = 0;
    };*/

    struct DrawVertex1
    {
        float position[3];
        float normal[3];

        unsigned int rgba;

        float unknown[4];

        void write(FILE *output) const;
    };

    struct DrawVertex2
    {
        float position[3];
        float normal[3];

        unsigned int rgba;

        float uv[2];
        float st[2];

        float unknown[6];

        void write(FILE *output) const;
    };

    struct DrawIndex
    {
        unsigned short vertex[3];

        void write(FILE *output) const;
    };

    struct PortalVertex
    {
        float unknown[3];

        void write(FILE *output) const;
    };

    struct AABBTree
    {
        unsigned int unknown[3];

        void write(FILE *output) const;
    };

    struct Cell
    {
        float unknown[3];

        void write(FILE *output) const;
    };

    struct Portal
    {
        unsigned int unknown[4];

        void write(FILE *output) const;
    };

    struct Node
    {
        int unknown[9];

        void write(FILE *output) const;
    };

    struct Leaf
    {
        int unknown[9];

        void write(FILE *output) const;
    };

    struct LeafBrush
    {
        int unknown;

        void write(FILE *output) const;
    };

    struct CollisionVertex
    {
        float unknown;
        float position[3];

        void write(FILE *output) const;
    };

    struct CollisionEdge
    {
        unsigned int unknown;

        float position[3];
        float normal[3][3];
        float distance;

        void write(FILE *output) const;
    };

    struct CollisionTriangle
    {
        float normal[3];
        float distance;
        float unknown[8];
        unsigned int vertex_id[3];
        int edge_id[3];

        void write(FILE *output) const;
    };

    struct CollisionBorder
    {
        float unknown[7];

        void write(FILE *output) const;
    };

    struct CollisionPart
    {
        short unknown[6];

        void write(FILE *output) const;
    };

    struct CollisionAABB
    {
        float unknown[6];
        unsigned short unknown2[4];

        void write(FILE *output) const;
    };

    struct Model
    {
        float position[2][3];

        unsigned int trianglesoups_offset;
        unsigned int trianglesoups_size;
        unsigned int collisionaabbs_offset;
        unsigned int collisionaabbs_size;
        unsigned int brushes_offset;
        unsigned int brushes_size;

        void write(FILE *output) const;
    };

    struct Entity
    {
        std::map<std::string, std::string> pairs;

        void write(FILE *output) const;
    };

    ////////////////////////////////////////////////////////////////
}

#endif // BSP_LUMPS
