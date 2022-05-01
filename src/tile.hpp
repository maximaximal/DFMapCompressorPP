#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <iostream>

#include <xxhash.h>

namespace dfmapcompressorpp {
using Pixel = uint8_t;
using DataVec = std::vector<Pixel>;
using TileSize = uint16_t;
using TileId = uint32_t;

class tileset;
class tileref;

struct tile {
  friend class tileref;

  tile()
    : w(0)
    , h(0)
    , data(nullptr) {}
  tile(TileSize w, TileSize h, const Pixel* data)
    : w(w)
    , h(h)
    , data(data) {}

  const TileSize w, h;
  const Pixel* data;

  constexpr size_t len() const noexcept {
    size_t l = w;
    return l * h * 3;
  }

  operator tileref();

  private:
  static thread_local tile* current_tile;
};

struct tileref {
  tileref(TileId id)
    : id(id) {}

  TileId id;

  size_t len() const noexcept;
  const Pixel* data() const noexcept;

  bool operator==(const tileref& o) const noexcept;
  bool operator==(const tile& o) const noexcept;
};
}

namespace std {
template<>
struct hash<dfmapcompressorpp::tileref> {
  std::size_t operator()(const dfmapcompressorpp::tileref& k) const {
    return XXH64(k.data(), k.len(), 0);
  }
};
}
