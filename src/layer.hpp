#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "tile.hpp"

namespace dfmapcompressorpp {
class tileset;

class layer {
  public:
  layer(std::shared_ptr<tileset> tileset, int l, size_t width, size_t height);
  ~layer();

  void add_tile(tileref t);

  const size_t width, height, l;

  inline tileref operator[](size_t i) const { return tiles_[i]; }

  private:
  std::shared_ptr<tileset> tileset_;
  std::vector<tileref> tiles_;
};
}
