#include "map.hpp"
#include "pixelbag.hpp"
#include "tileset.hpp"

#include <iostream>

#include <zstr.hpp>

namespace dfmapcompressorpp {
map::map(std::shared_ptr<tileset> t)
  : tileset_(t) {}
map::~map() {}

bool
map::add_layer(pixelbag&& bag) {
  auto tiles = bag.tiles(tileset_->tile_width, tileset_->tile_height);

  assert(tiles.begin() != tiles.end());

  int layernum = bag.parse_filename_to_layernum();

  if(fortress_name_.empty())
    fortress_name_ = bag.parse_filename_to_fortress_name();
  else if(fortress_name_ != bag.parse_filename_to_fortress_name())
    std::cerr << "!! Combining multiple fortress pictures!" << std::endl;

  size_t width = bag.tiles_x(tileset_->tile_width);
  size_t height = bag.tiles_x(tileset_->tile_height);

  auto layerIt =
    layers_.emplace(layernum, layer{ layernum, width, height });
  layer& l = layerIt.first->second;

  for(auto& t : tiles) {
    l.add_tile((*tileset_)[t]);
  }

  return true;
}
}
