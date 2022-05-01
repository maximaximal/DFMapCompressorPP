#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "tile.hpp"

namespace dfmapcompressorpp {
class tileset {
  public:
  friend struct tileref;

  tileset(TileSize tileWidth, TileSize tileHeight);
  ~tileset();

  tileref operator[](tile& t) {
    current_tileset = this;
    tileref rt = t;
    auto it = lookup_.find(rt);
    if(it != lookup_.end()) {
      return it->first;
    }

    // Insert new entry
    std::copy(t.data, t.data + t.len(), std::back_inserter(data_));
    rt.id = tileCount_++;
    return lookup_.emplace(rt, tileCount_).first->first;
  }

  const TileSize tile_width, tile_height;

  size_t number_of_tiles() const { return tileCount_; }
  const Pixel* operator[](size_t i) const {
    return &data_[i * 3 * tile_width * tile_height];
  }

  private:
  size_t tileCount_ = 0;
  DataVec data_;
  std::unordered_map<tileref, size_t> lookup_;

  static thread_local tileset* current_tileset;
};
}
