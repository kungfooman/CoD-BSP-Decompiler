#include <iostream>
#include <bsp/bsp.hpp>

namespace BSP
{
    BSP_Handle *bsp;

    ////////////////////////////////////////////////////////////////

    BSP_Handle::BSP_Handle(std::string filename) :
        cod_version(0),
        cod_size(0),
        success(false)
    {
        bsp = this;
        map = filename.substr(filename.find_last_of("/\\") + 1);

        // opening input file
        FILE *input;
        input = fopen(filename.c_str(), "rb+");
        if (!input)
        {
            printf("ERROR: Could not open input BSP file `%s'.\n", filename.c_str());
            return;
        }

        // reading input file
        fseek(input, 0, SEEK_END);
        data = Data(new char[ftell(input)], ftell(input));
        fseek(input, 0, SEEK_SET);

        if (fread(data.buffer, 1, data.size, input) != data.size)
        {
            printf("ERROR: Could not read BSP file `%s'.\n", filename.c_str());
            delete[] data.buffer;
            data.buffer = NULL;
            fclose(input);
            return;
        }
        fclose(input);

        // checking file format
        if (data.size < 9 || data.buffer[0] != 'I' || data.buffer[1] != 'B' || data.buffer[2] != 'S' || data.buffer[3] != 'P')
        {
            printf("ERROR: The BSP's file format is unsupported.\n");
            delete[] data.buffer;
            data.buffer = NULL;
            return;
        }

        // initialising data access variable
        i = data.buffer;
        i += 4;

        // version check
        DoubleWord version;
        version.b[0] = *i; i++;
        version.b[1] = *i; i++;
        version.b[2] = *i; i++;
        version.b[3] = *i; i++;

        // check if this is CoD1, CoD2, ...
        switch (version.u)
        {
            case COD1_VERSION:
            {
                cod1();
                break;
            }
            case COD2_VERSION:
            {
                cod2();
                break;
            }
            case COD4_VERSION:
            {
                cod4();
                break;
            }
            case COD5_VERSION:
            {
                cod5();
                break;
            }
            default:
            {
                printf("ERROR: The BSP's file version is unsupported.\n");
                delete[] data.buffer;
                data.buffer = NULL;
                return;
            }
        }

        read();
    }

    BSP_Handle::~BSP_Handle()
    {
        delete[] data.buffer;

        std::map<LumpReference, _Lump *>::iterator it;
        for (it = lumps.begin(); it != lumps.end(); it++)
        {
            delete (*it).second;
        }
    }

    void BSP_Handle::cod1()
    {
        cod_version = 1;
        cod_size = 33;

        lump_reference[ 0] = MATERIALS;
        lump_reference[ 2] = PLANES;
        lump_reference[ 3] = BRUSHSIDES;
        lump_reference[ 4] = BRUSHES;
        lump_reference[ 6] = TRIANGLESOUPS;
        lump_reference[ 7] = DRAWVERTICES;
        lump_reference[ 8] = DRAWINDICES;
        /*lump_reference[11] = PORTALVERTICES;
        lump_reference[16] = AABBTREES;
        lump_reference[17] = CELLS;
        lump_reference[18] = PORTALS;
        lump_reference[20] = NODES;
        lump_reference[21] = LEAVES;
        lump_reference[22] = LEAFBRUSHES;
        //lump_reference[29] = COLLISIONVERTICES;
        //lump_reference[30] = COLLISIONEDGES;
        //lump_reference[31] = COLLISIONTRIANGLES;
        //lump_reference[32] = COLLISIONBORDERS;
        //lump_reference[33] = COLLISIONPARTS;
        //lump_reference[34] = COLLISIONAABBS;*/
        lump_reference[27] = MODELS;
        lump_reference[29] = ENTITIES;

        lumps[MATERIALS] =          new Lump<Material>;
        lumps[PLANES] =             new Lump<Plane>;
        lumps[BRUSHSIDES] =         new Lump<Brushside>;
        lumps[BRUSHES] =            new Lump<Brush>;
        lumps[TRIANGLESOUPS] =      new Lump<TriangleSoup>;
        lumps[DRAWVERTICES] =       new Lump<DrawVertex1>;
        lumps[DRAWINDICES] =        new Lump<DrawIndex>;
        /*lumps[PORTALVERTICES] =     new Lump<PortalVertex>;
        lumps[AABBTREES] =          new Lump<AABBTree>;
        lumps[CELLS] =              new Lump<Cell>;
        lumps[PORTALS] =            new Lump<Portal>;
        lumps[NODES] =              new Lump<Node>;
        lumps[LEAVES] =             new Lump<Leaf>;
        lumps[LEAFBRUSHES] =        new Lump<LeafBrush>;
        //lumps[COLLISIONVERTICES] =  new Lump<CollisionVertex>;
        //lumps[COLLISIONEDGES] =     new Lump<CollisionEdge>;
        //lumps[COLLISIONTRIANGLES] = new Lump<CollisionTriangle>;
        //lumps[COLLISIONBORDERS] =   new Lump<CollisionBorder>;
        //lumps[COLLISIONPARTS] =     new Lump<CollisionPart>;
        //lumps[COLLISIONAABBS] =     new Lump<CollisionAABB>;*/
        lumps[MODELS] =             new Lump<Model>;
        lumps[ENTITIES] =           new Lump<Entity>;
    }

    void BSP_Handle::cod2()
    {
        cod_version = 2;
        cod_size = -1;

        lump_reference[ 0] = MATERIALS;
        lump_reference[ 4] = PLANES;
        lump_reference[ 5] = BRUSHSIDES;
        lump_reference[ 6] = BRUSHES;
        lump_reference[ 7] = TRIANGLESOUPS;
        lump_reference[ 8] = DRAWVERTICES;
        lump_reference[ 9] = DRAWINDICES;
        lump_reference[17] = PORTALVERTICES;
        lump_reference[22] = AABBTREES;
        lump_reference[23] = CELLS;
        lump_reference[24] = PORTALS;
        lump_reference[25] = NODES;
        lump_reference[26] = LEAVES;
        lump_reference[27] = LEAFBRUSHES;
        lump_reference[29] = COLLISIONVERTICES;
        lump_reference[30] = COLLISIONEDGES;
        lump_reference[31] = COLLISIONTRIANGLES;
        lump_reference[32] = COLLISIONBORDERS;
        lump_reference[33] = COLLISIONPARTS;
        lump_reference[34] = COLLISIONAABBS;
        lump_reference[35] = MODELS;
        lump_reference[37] = ENTITIES;

        lumps[MATERIALS] =          new Lump<Material>;
        lumps[PLANES] =             new Lump<Plane>;
        lumps[BRUSHSIDES] =         new Lump<Brushside>;
        lumps[BRUSHES] =            new Lump<Brush>;
        lumps[TRIANGLESOUPS] =      new Lump<TriangleSoup>;
        lumps[DRAWVERTICES] =       new Lump<DrawVertex2>;
        lumps[DRAWINDICES] =        new Lump<DrawIndex>;
        lumps[PORTALVERTICES] =     new Lump<PortalVertex>;
        lumps[AABBTREES] =          new Lump<AABBTree>;
        lumps[CELLS] =              new Lump<Cell>;
        lumps[PORTALS] =            new Lump<Portal>;
        lumps[NODES] =              new Lump<Node>;
        lumps[LEAVES] =             new Lump<Leaf>;
        lumps[LEAFBRUSHES] =        new Lump<LeafBrush>;
        lumps[COLLISIONVERTICES] =  new Lump<CollisionVertex>;
        lumps[COLLISIONEDGES] =     new Lump<CollisionEdge>;
        lumps[COLLISIONTRIANGLES] = new Lump<CollisionTriangle>;
        lumps[COLLISIONBORDERS] =   new Lump<CollisionBorder>;
        lumps[COLLISIONPARTS] =     new Lump<CollisionPart>;
        lumps[COLLISIONAABBS] =     new Lump<CollisionAABB>;
        lumps[MODELS] =             new Lump<Model>;
        lumps[ENTITIES] =           new Lump<Entity>;
    }

    void BSP_Handle::cod4()
    {
        cod_version = 4;

        DoubleWord size;
        size.b[0] = *i; i++;
        size.b[1] = *i; i++;
        size.b[2] = *i; i++;
        size.b[3] = *i; i++;

        cod_size = size.i;

        lump_reference[ 0] = MATERIALS;
        lump_reference[ 1] = LIGHTMAPS;
        lump_reference[ 2] = LIGHTGRIDPONTS;
        lump_reference[ 3] = LIGHTGRIDCOLOURS;
        lump_reference[ 4] = PLANES;
        lump_reference[ 5] = BRUSHSIDES;
        lump_reference[ 6] = UNKNOWN;
        lump_reference[ 7] = UNKNOWN;
        lump_reference[ 8] = BRUSHES;
        lump_reference[ 9] = LAYEREDTRIANGLESOUPS;
        lump_reference[10] = LAYEREDDRAWVERTICES;
        lump_reference[11] = LAYEREDDRAWINDICES;
        lump_reference[19] = PORTALVERTICES;
        lump_reference[24] = LAYEREDAABBTREES;
        lump_reference[25] = CELLS;
        lump_reference[26] = PORTALS;
        lump_reference[27] = NODES;
        lump_reference[28] = LEAVES;
        lump_reference[29] = LEAFBRUSHES;
        lump_reference[31] = COLLISIONVERTICES;
        lump_reference[32] = COLLISIONTRIANGLES;
        lump_reference[33] = COLLISIONEDGES;
        lump_reference[34] = COLLISIONBORDERS;
        lump_reference[35] = COLLISIONPARTS;
        lump_reference[36] = COLLISIONAABBS;
        lump_reference[37] = MODELS;
        lump_reference[39] = ENTITIES;
        lump_reference[40] = PATHS;
        lump_reference[41] = REFLECTIONPROBES;
        lump_reference[42] = LAYEREDDATA;
        lump_reference[43] = PRIMARYLIGHTS;
        lump_reference[44] = LIGHTGRIDHEADER;
        lump_reference[45] = LIGHTGRIDROWS;
        lump_reference[47] = SIMPLETRIANGLESOUPS;
        lump_reference[48] = SIMPLEVERTICES;
        lump_reference[49] = SIMPLEINDICES;
        lump_reference[51] = SIMPLEAABBTREES;
        lump_reference[52] = LIGHTREGIONS;
        lump_reference[53] = LIGHTREGIONHULLS;
        lump_reference[54] = LIGHTREGIONAXES;
    }

    void BSP_Handle::cod5()
    {
        cod_version = 5;

        DoubleWord size;
        size.b[0] = *i; i++;
        size.b[1] = *i; i++;
        size.b[2] = *i; i++;
        size.b[3] = *i; i++;

        cod_size = size.i;
    }

    void BSP_Handle::read()
    {
        DoubleWord offset, size, id;
        offset.u = id.u = 0;

        unsigned int padded = 0, n = 0;

        // read size and offset of each lump
        while (((size_t) (i + 7) - (size_t) data.buffer) < data.size)
        {
            if (cod_version < 3)
            {
                if (index.size())
                {
                    id.u++;
                }

                size.b[0] = *i; i++;
                size.b[1] = *i; i++;
                size.b[2] = *i; i++;
                size.b[3] = *i; i++;

                offset.b[0] = *i; i++;
                offset.b[1] = *i; i++;
                offset.b[2] = *i; i++;
                offset.b[3] = *i; i++;
            }
            else if (cod_version > 3)
            {
                if (!index.size())
                {
                    offset.i += 12 + ((cod_size * 2) * 4);
                }
                else
                {
                    unsigned int length = index[index.size() - 1].offset + index[index.size() - 1].size;
                    unsigned int padding = 0;

                    // variance
                    if (id.u != 7 && id.u != 39)
                    {
                        padding = (length % 4); // make it a multiple of 4 bytes

                        if (padding)
                        {
                            padding = (4 - padding);
                        }
                    }
                    else if (id.u == 39)
                    {
                        padding = 1;
                    }

                    offset.i = length + padding;
                    padded += padding;
                }

                id.b[0] = *i; i++;
                id.b[1] = *i; i++;
                id.b[2] = *i; i++;
                id.b[3] = *i; i++;

                size.b[0] = *i; i++;
                size.b[1] = *i; i++;
                size.b[2] = *i; i++;
                size.b[3] = *i; i++;

                if (id.u == 39)
                {
                    size.u--; // we know your length, no need for 0x00
                }
            }

            // if we reached the number of lumps used by this CoD
            if (cod_size == -1 && offset.u == 0 && size.u == 0)
            {
                break;
            }

            // as long as the end of the file is not too close...
            if ((offset.u + size.u) <= data.size)
            {
                if (size.u)
                {
                    index.push_back(Index(id.u, offset.u, size.u));
                }
            }
            else
            {
                printf("WARNING: The bsp's file is too short, the lump's directory is pointing outside the file by `%u' bytes, starting at lump `%u'.\n", ((offset.u + size.u) - data.size), id.u);
                break;
            }

            if (cod_size != -1 && (n + 1) == (unsigned int) cod_size)
            {
                break;
            }
            n++;
        }

        ////////

        // run a function for each lump
        std::vector<Index>::iterator it;
        for (it = index.begin(); it < index.end(); it++)
        {
            if (lump_reference.find((*it).id) != lump_reference.end())
            {
                LumpReference reference = lump_reference[(*it).id];
                if ((*it).size && exists(reference))
                {
                    lumps[reference]->read(data.buffer + (*it).offset, (*it).size);
                }
            }
        }

        success = true;
    }

    bool BSP_Handle::exists(LumpReference reference)
    {
        return (lumps.find(reference) != lumps.end());
    }

    unsigned int BSP_Handle::size(LumpReference reference)
    {
        if (exists(reference))
        {
            return lumps[reference]->size();
        }
        return 0;
    }

    void BSP_Handle::write(FILE *output, LumpReference reference)
    {
        if (exists(reference) && size(reference))
        {
            lumps[reference]->write(output);
        }
    }

    template<>
    void Lump<Entity>::read(char *i, unsigned int isize)
    {
        std::vector<Entity> vector;
        char *size = i + isize;

        bool in_entity = false;
        while (i < size)
        {
            bool in_string = false;
            bool has_key = false;
            Entity entity;
            std::string key, value;
            while (i < size && in_entity)
            {
                while (i < size && in_string)
                {
                    if (*i == '"')
                    {
                        i++;
                        in_string = false;
                        if (!has_key)
                        {
                            has_key = true;
                        }
                        else
                        {
                            entity.pairs[key] = value;
                            has_key = false;
                            key = value = "";
                        }
                        break;
                    }
                    else if (!has_key)
                    {
                        key.push_back(*i);
                    }
                    else if (has_key)
                    {
                        value.push_back(*i);
                    }
                    i++;
                }

                if (*i == '"')
                {
                    in_string = true;
                }
                else if (*i == '}')
                {
                    in_entity = false;
                    vector.push_back(entity);
                    break;
                }
                i++;
            }

            if (*i == '{')
            {
                in_entity = true;
            }
            i++;
        }

        _size = vector.size();
        _array = new Entity[_size];

        for (unsigned int n = 0; n < _size; n++)
        {
            _array[n] = vector[n];
        }
    }

    template<>
    Lump<Entity>::~Lump()
    {
        delete[] _array;
    }

    template<>
    void Lump<Brushside>::write(FILE *output) const
    {
        unsigned int n = 0, offset = 0, side = 0;

        Brush *brush;
        for (brush = bsp->array<Brush>(BRUSHES); brush < bsp->array<Brush>(BRUSHES) + bsp->size(BRUSHES); brush++)
        {
            side = 0;
            Brushside *brushside;
            for (brushside = _array + offset; brushside < (_array + offset + (*brush).sides); brushside++)
            {
                if (side < 6)
                {
                    fprintf(output, "[%6u] %12g %6u\r\n", n, (*brushside).column1.distance, (*brushside).material_id);
                }
                else
                {
                    fprintf(output, "[%6u] %11u# %6u\r\n", n, (*brushside).column1.plane, (*brushside).material_id);
                }
                side++;
                n++;
            }
            offset += (*brush).sides;
        }
    }

    template<>
    void Lump<Entity>::write(FILE *output) const
    {
        unsigned int n = 0;
        Entity *entity;
        for (entity = _array; entity < (_array + _size); entity++)
        {
            unsigned int m = 0;
            std::map<std::string, std::string>::const_iterator it;
            for (it = (*entity).pairs.begin(); it != (*entity).pairs.end(); it++)
            {
                fprintf(output, "[%6u.%02u] %s %s\r\n", n, m, (*it).first.c_str(), (*it).second.c_str());
                m++;
            }
            fprintf(output, "\r\n");
            n++;
        }
    }

    ////////////////////////////////////////////////////////////////
}
