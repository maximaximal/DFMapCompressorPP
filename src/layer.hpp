#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace dfmapcompressorpp {
struct tileref;
class tileset;

class layer {
  public:
  layer(std::shared_ptr<tileset> tileset, size_t width, size_t height);
  ~layer();

  void add_tile(tileref t);

  private:
  std::shared_ptr<tileset> tileset_;
  const size_t width_, height_;
  std::vector<tileref> tiles_;
};
}
