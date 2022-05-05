#include "layer.hpp"
#include "tile.hpp"
#include "tileset.hpp"

namespace dfmapcompressorpp {
layer::layer(std::shared_ptr<tileset> tileset,
             int l,
             size_t width,
             size_t height)
  : tileset_(tileset)
  , l(l)
  , width(width)
  , height(height) {
  tiles_.reserve(width * height);
}
layer::~layer() {}

void
layer::add_tile(tileref t) {
  tiles_.emplace_back(t);
}
}