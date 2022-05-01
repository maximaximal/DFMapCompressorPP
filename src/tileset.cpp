#include "tileset.hpp"

namespace dfmapcompressorpp {
tileset::tileset(TileSize tileWidth, TileSize tileHeight)
  : tile_width(tileWidth)
  , tile_height(tileHeight) {}
tileset::~tileset() {}

thread_local tileset* tileset::current_tileset = nullptr;
}
