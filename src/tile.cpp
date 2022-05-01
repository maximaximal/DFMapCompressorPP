#include <limits>

#include "tile.hpp"
#include "tileset.hpp"

namespace dfmapcompressorpp {
thread_local tile* tile::current_tile = nullptr;

tile::operator tileref() {
  current_tile = this;
  return tileref(std::numeric_limits<TileId>::max());
}

size_t
tileref::len() const noexcept {
  tileset* t = tileset::current_tileset;
  size_t width = t->tile_width;
  return width * 3 * t->tile_height;
}

const Pixel*
tileref::data() const noexcept {
  if(id == std::numeric_limits<TileId>::max()) {
    assert(tile::current_tile);
    return tile::current_tile->data;
  }

  tileset* t = tileset::current_tileset;
  assert(t->data_.size() > len() * id);
  return &t->data_[len() * id];
}

bool
tileref::operator==(const tileref& o) const noexcept {
  if(id == o.id) return true;
  else if(id == std::numeric_limits<TileId>::max() ||
          o.id == std::numeric_limits<TileId>::max()) {
    return memcmp(data(), o.data(), len()) == 0;
  } else {
    return false;
  }
}

bool
tileref::operator==(const tile& o) const noexcept {
  tileset* t = tileset::current_tileset;
  return t->tile_width == o.w && t->tile_height == o.h &&
         memcmp(data(), o.data, len()) == 0;
}
}
