#include <limits>
#include <cstdint>

#include "fdf-map.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "tileset.hpp"

#include <zstr.hpp>

template<typename T>
class AsBin {
  private:
  T i;

  public:
  AsBin(T i)
    : i(i) {}
  friend std::ostream& operator<<(std::ostream& os, const AsBin& b) {
    os.write(reinterpret_cast<const char*>(&(b.i)), sizeof(T));
    return os;
  }
  T val() { return i; }
};

namespace dfmapcompressorpp {
fdfmap::fdfmap(const map& m)
  : m(m)
  , t(m.get_tileset()) {}

fdfmap::~fdfmap() {}

template<typename UnitType>
static void
write_layer_as_units(const layer& l, zstr::ostream& zos) {
  UnitType col[l.height];
  for(size_t x = 0; x < l.width; ++x) {
    for(size_t y = 0; y < l.height; ++y) {
      auto id = l[y * l.width + x].id;
      assert(std::numeric_limits<UnitType>::max() >= id);
      col[y] = id;
    }
    zos.write(reinterpret_cast<char*>(col), sizeof(col));
  }
}

static void
write_run_length_encoded_px_arr(const Pixel* first_px,
                                int32_t n,
                                zstr::ostream& zos) {
  const Pixel* px = first_px;
  const Pixel* next = first_px + 3;

  uint8_t num = 1;
  int32_t outputted = 0;
  for(int32_t i = 0; i < n; ++i) {

    if(px[0] != next[0] || px[1] != next[1] || px[2] != next[2] || i == n - 1 ||
       i == std::numeric_limits<decltype(num)>::max()) {
      // As BGR not RGB!
      zos << AsBin(num) << AsBin(px[2]) << AsBin(px[1]) << AsBin(px[0]);
      outputted += num;
      num = 1;
    } else {
      ++num;
    }

    px = next;
    next += 3;
  }
  assert(outputted == n);
}

void
fdfmap::write_to(std::ostream& os) {
  zstr::ostream zos(os, zstr::default_buff_size, Z_DEFAULT_COMPRESSION, 15);

  int32_t version = -1;
  zos << AsBin(version);

  int32_t number_of_tiles = t.number_of_tiles();
  int32_t tile_width = t.tile_width;
  int32_t tile_height = t.tile_height;
  zos << AsBin(number_of_tiles) << AsBin(tile_width) << AsBin(tile_height);

  int32_t number_of_map_layers = m.layers().size();
  zos << AsBin(number_of_map_layers);

  // Layer Info
  for(const auto& it : m.layers()) {
    const layer& l = it.second;
    int32_t layer_depth = l.l;
    int32_t layer_width = l.width;
    int32_t layer_height = l.height;
    zos << AsBin(layer_depth) << AsBin(layer_width) << AsBin(layer_height);
  }

  // Tile Info
  for(size_t tileid = 0; tileid < number_of_tiles; ++tileid) {
    // TileID, but this is ignored by the web viewer.
    uint8_t character_code = 0xff, background_color = 0xff,
            foreground_color = 0xff;
    //zos << AsBin(character_code) << AsBin(background_color)
    //    << AsBin(foreground_color);

    int32_t number_of_pixels = tile_width * tile_height;

    // Pixel data
    const Pixel* px = t[tileid];
    write_run_length_encoded_px_arr(px, number_of_pixels, zos);
  }

  // Layer Data
  for(const auto& it : m.layers()) {
    const layer& l = it.second;
    if(number_of_tiles <= 127) {
      write_layer_as_units<uint8_t>(l, zos);
    } else if(number_of_tiles <= 32767) {
      write_layer_as_units<uint16_t>(l, zos);
    } else {
      write_layer_as_units<uint32_t>(l, zos);
    }
  }
}
}
