#ifndef BSP_HEADER
#define BSP_HEADER

#define COD1_VERSION 59
#define COD2_VERSION 4
#define COD4_VERSION 22
#define COD5_VERSION 31

namespace BSP
{
    ////////////////////////////////////////////////////////////////

    // doesn't correspond as how it's stored in the BSP
    struct Index
    {
        unsigned int id;
        unsigned int offset;
        unsigned int size;

        Index(unsigned int id = 0, unsigned int offset = 0, unsigned int size = 0) :
            id(id),
            offset(offset),
            size(size)
        {}
    };

    ////////////////////////////////////////////////////////////////

    union DoubleWord
    {
        unsigned char b[4];
        unsigned short s[2];
        int i;
        unsigned int u, x;
        float f, g;

        DoubleWord()
        {
            b[0] = 0;
            b[1] = 0;
            b[2] = 0;
            b[3] = 0;

            s[0] = 0;
            s[1] = 0;
        }
    };

    struct Data
    {
        char *buffer;
        size_t size;

        Data(char *buffer = NULL, size_t size = 0) : buffer(buffer), size(size) {}
    };

    ////////////////////////////////////////////////////////////////
}

#endif // BSP_HEADER
