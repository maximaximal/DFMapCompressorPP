#pragma once

#include <cstdint>
#include <map>
#include <memory>

#include "layer.hpp"

namespace dfmapcompressorpp {
class pixelbag;
class tileset;

class map {
  public:
  using LayerMap = std::map<int32_t, layer>;

  map(std::shared_ptr<tileset> t);
  ~map();

  bool add_layer(pixelbag&& bag);

  const LayerMap& layers() const { return layers_; }
  const tileset& get_tileset() const { return *tileset_; }
  const std::string& fortress_name() const { return fortress_name_; }

  private:
  LayerMap layers_;
  std::shared_ptr<tileset> tileset_;
  std::string fortress_name_;
};
}
