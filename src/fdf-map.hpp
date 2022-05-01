#pragma once

#include <ostream>

namespace dfmapcompressorpp {
class map;
class tileset;

/** @brief Write to an .fdf-map file.

   Documented here:
   https://github.com/Ramblurr/df-map-compressor/blob/master/DwarfFortressMapViewer/DFMapViewer_FileFormat_extended.txt
*/
class fdfmap {
  public:
  fdfmap(const map& m);
  ~fdfmap();

  void write_to(std::ostream& os);

  private:
  const map& m;
  const tileset& t;

  static const int32_t TILE_ID_INFO = 0x01;
  static const int32_t RLE = 0x02;
  static const int32_t CLIPPING = 0x04;
};
}
