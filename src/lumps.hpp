#ifndef LUMPS_H
#define LUMPS_H

#include <bsp/bsp.hpp>

void write_raw_lumps(BSP::BSP_Handle *bsp, const std::string &path);
void write_bsp(BSP::BSP_Handle *bsp, const std::string &path, std::string filename, bool export_html);
void write_lumps(BSP::BSP_Handle *bsp, const std::string &path, bool export_html);

#endif // LUMPS_H
