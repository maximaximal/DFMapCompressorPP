#include "map.hpp"
#include "pixelbag.hpp"
#include "tileset.hpp"

#include <iostream>

namespace dfmapcompressorpp {
map::map(std::shared_ptr<tileset> t)
  : tileset_(t) {}
map::~map() {}

bool
map::add_layer(pixelbag&& bag) {
  auto tiles = bag.tiles(tileset_->tile_width, tileset_->tile_height);
  std::cout << std::distance(tiles.begin(), tiles.end()) << std::endl;

  assert(tiles.begin() != tiles.end());

  for(auto &t : tiles) {
    tileref ref = (*tileset_)[t];
  }

  return true;
}
}
