#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "layer.hpp"

namespace dfmapcompressorpp {
class pixelbag;
class tileset;

class map {
  public:
  map(std::shared_ptr<tileset> t);
  ~map();

  bool add_layer(pixelbag&& bag);

  private:
  std::unordered_map<int32_t, layer> layers_;
  std::shared_ptr<tileset> tileset_;
};
}
