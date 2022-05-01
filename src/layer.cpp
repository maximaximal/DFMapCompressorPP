#include "layer.hpp"
#include "tile.hpp"
#include "tileset.hpp"

namespace dfmapcompressorpp {
layer::layer(std::shared_ptr<tileset> tileset, size_t width, size_t height)
  : tileset_(tileset)
  , width_(width)
  , height_(height) {
  tiles_.reserve(width * height);
}
layer::~layer() {}

void
layer::add_tile(tileref t) {
  tiles_.emplace_back(t);
}
}
