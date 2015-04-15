#include "lumps.hpp"
#include "console.hpp"
#include "common.hpp"

#include <cmath>

using namespace BSP;

void write_raw_lumps(BSP_Handle *bsp, const std::string &path)
{
    printf("Exporting raw lumps...");
    retrieve_cursor_position();

    unsigned int n = 0;
    std::vector<Index>::iterator it;
    for (it = bsp->index.begin(); it < bsp->index.end(); it++)
    {
        restore_cursor_position();
        printf("%3.0f%%", ceil(((float) n / bsp->index.size()) * 100.0f));

        if ((*it).size)
        {
            LumpReference reference = UNKNOWN;
            if (bsp->lump_reference.find((*it).id) != bsp->lump_reference.end())
            {
                reference = bsp->lump_reference[(*it).id];
            }

            std::string filename(path);
            if ((*it).id < 10)
            {
                filename += '0';
            }
            filename += to_str((*it).id) + "_" + LumpName[reference] + ".lump";

            FILE *output = fopen(filename.c_str(), "wb+");
            if (!output)
            {
                fprintf(stderr, "ERROR: Could not write to `%s'.\n\n", filename.c_str());
            }
            else
            {
                fwrite(bsp->data.buffer + (*it).offset, 1, (*it).size, output);
                fclose(output);
            }
        }
        n++;
    }

    restore_cursor_position();
    printf("100%%\n");
}

void write_bsp(BSP_Handle *bsp, const std::string &path, std::string filename, bool export_html)
{
    printf("Exporting BSP...");
    retrieve_cursor_position();

    if (export_html)
    {
        filename = path + filename + ".html";
    }
    else
    {
        filename = path + filename + ".txt";
    }

    FILE *output = fopen(filename.c_str(), "wb+");
    if (!output)
    {
        fprintf(stderr, "ERROR: Could not write to `%s'.\n", filename.c_str());
    }
    else
    {
        std::vector<Index>::iterator it;
        if (export_html)
        {
            fprintf(output, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"de\">\n<head>\n<title>BSP export - %s</title>\n<link rel=\"stylesheet\" type=\"text/css\" media=\"screen,print\" href=\"../../export.css\">\n</head>\n<body>\n<div id=\"all\">\n<h1>%s</h1>\n", bsp->map.c_str(), bsp->map.c_str());

            fprintf(output, "<div>");
            for (it = bsp->index.begin(); it < bsp->index.end(); it++)
            {
                if (bsp->lump_reference.find((*it).id) != bsp->lump_reference.end() && (*it).size)
                {
                    fprintf(output, "<a href=\"#lump_%u\">%s</a> ", (*it).id, LumpName[bsp->lump_reference[(*it).id]]);
                }
            }
            fprintf(output, "</div>\n");
        }

        unsigned int n = 0;
        for (it = bsp->index.begin(); it < bsp->index.end(); it++)
        {
            restore_cursor_position();
            printf("%3.0f%%", ceil(((float) n / bsp->index.size()) * 100.0f));

            if (bsp->lump_reference.find((*it).id) != bsp->lump_reference.end())
            {
                LumpReference reference = bsp->lump_reference[(*it).id];
                if ((*it).size && bsp->size(reference))
                {
                    if (export_html)
                    {
                        fprintf(output, "<h2 id=\"lump_%u\">Lump %u - %s (%u)</h2><div><pre>", (*it).id, (*it).id, LumpName[reference], bsp->size(reference));
                        bsp->write(output, reference);
                        fprintf(output, "</pre></div>");
                    }
                    else
                    {
                        fprintf(output, "Lump %u - %s (%u)\r\n", (*it).id, LumpName[reference], bsp->size(reference));
                        bsp->write(output, reference);
                        fprintf(output, "\r\n\r\n");
                    }
                }
            }
            n++;
        }

        if (export_html)
        {
            fprintf(output, "</div></body></html>");
        }
        fclose(output);
    }

    restore_cursor_position();
    printf("100%%\n");
}

void write_lumps(BSP_Handle *bsp, const std::string &path, bool export_html)
{
    printf("Exporting lumps...");
    retrieve_cursor_position();

    unsigned int n = 0;
    std::vector<Index>::iterator it;
    for (it = bsp->index.begin(); it < bsp->index.end(); it++)
    {
        restore_cursor_position();
        printf("%3.0f%%", ceil(((float) n / bsp->index.size()) * 100.0f));

        if (bsp->lump_reference.find((*it).id) != bsp->lump_reference.end())
        {
            LumpReference reference = bsp->lump_reference[(*it).id];
            if ((*it).size && bsp->size(reference))
            {
                std::string tmp_filename(path);
                if ((*it).id < 10)
                {
                    tmp_filename += '0';
                }

                if (export_html)
                {
                    tmp_filename += to_str((*it).id) + "_" + LumpName[reference] + ".html";
                }
                else
                {
                    tmp_filename += to_str((*it).id) + "_" + LumpName[reference] + ".txt";
                }

                FILE *output = fopen(tmp_filename.c_str(), "wb+");
                if (!output)
                {
                    fprintf(stderr, "ERROR: Could not write to `%s'.\n", tmp_filename.c_str());
                }
                else
                {
                    if (export_html)
                    {
                        fprintf(output, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"de\">\n<head>\n<title>Lump %u export - %s</title>\n<link rel=\"stylesheet\" type=\"text/css\" media=\"screen,print\" href=\"../../../export.css\">\n</head>\n<body>\n<div id=\"all\">\n<h1>%s</h1>\n", (*it).id, bsp->map.c_str(), bsp->map.c_str());
                        fprintf(output, "<h2 id=\"lump_%u\">Lump %u - %s (%u)</h2><div><pre>", (*it).id, (*it).id, LumpName[reference], bsp->size(reference));

                        bsp->write(output, reference);

                        fprintf(output, "</pre></div>");
                        fprintf(output, "</div></body></html>");
                    }
                    else
                    {
                        fprintf(output, "Lump %u - %s (%u)\r\n", (*it).id, LumpName[reference], bsp->size(reference));

                        bsp->write(output, reference);
                    }

                    fclose(output);
                }
            }
        }
        n++;
    }

    restore_cursor_position();
    printf("100%%\n");
}
