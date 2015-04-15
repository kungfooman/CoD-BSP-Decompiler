#ifndef BSP_BSP
#define BSP_BSP

#include <cstdio>
#include <vector>
#include <map>

#include "common.hpp"
#include "lumps.hpp"

namespace BSP
{
    class BSP;

    ////////////////////////////////////////////////////////////////

    class _Lump // base class, to allow heterogeneous arrays (of type Lump *)
    {
    public:
        virtual void *array() = 0;
        virtual unsigned int size() const = 0;

        virtual void read(char *i, unsigned int isize) = 0;
        virtual void write(FILE *output) const = 0;
    };

    ////////////////////////////////////////////////////////////////

    template <typename T> // T = parameter type
    class Lump: public _Lump // derived class, allows the use of any class (using templates)
    {
    public:
        Lump() :
            _array(NULL),
            _size(0)
        {
        }

        virtual ~Lump()
        {
        }

        void *array()
        {
            return _array;
        }

        unsigned int size() const
        {
            return _size;
        }

        ////////////////

        void read(char *i, unsigned int isize)
        {
            _size = (isize - (isize % sizeof(T))) / sizeof(T);
            _array = (T *) i;
        }

        void write(FILE *output) const
        {
            unsigned int n = 0;
            T *it;
            for (it = _array; it < _array + _size; it++)
            {
                fprintf(output, "[%6u] ", n);
                it->write(output);
                fprintf(output, "\r\n");
                n++;
            }
        }

        ////////////////

        T *_array;
        unsigned int _size;
    };

    ////////////////////////////////////////////////////////////////

    class BSP_Handle
    {
    public:
        BSP_Handle(std::string filename);
        ~BSP_Handle();

    private:
        void cod1();
        void cod2();
        void cod4();
        void cod5();

        void read();

    public:
        template <typename T>
        T *array(LumpReference reference)
        {
            if (exists(reference) && size(reference))
            {
                return (T *) lumps[reference]->array();
            }
            else
            {
                fprintf(stderr, "ERROR: Lump reference `%u' does not exist.\n", reference);
                return NULL;
            }
        }

        unsigned int size(LumpReference reference);

        bool exists(LumpReference reference);
        void write(FILE *output, LumpReference reference);

        std::map<LumpReference, _Lump *> lumps; // holds Lump instance which will parse the lump according to the struct passed
        std::map<unsigned int, LumpReference> lump_reference; // each different lump in any game has a reference ID

        std::string map;
        int cod_version;
        int cod_size;

        Data data;
        char *i;

        std::vector<Index> index;

        bool success;
    };

    ////////////////////////////////////////////////////////////////
}

#endif // BSP_BSP
