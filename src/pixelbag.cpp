#include <filesystem>
#include <iostream>
#include <memory>

#include "stb_image.h"

#include "pixelbag.hpp"

namespace dfmapcompressorpp {
pixelbag::pixelbag() {}

bool
pixelbag::load_from_file(const std::string& path) {
  std::filesystem::path p(path);

  if(!std::filesystem::exists(p)) {
    return false;
  }

  path_ = path;
  filename_ = p.filename().string();

  int x = 0;
  int y = 0;
  int n = 0;
  data_.reset(stbi_load(path.c_str(), &x, &y, &n, 3));

  width_ = x;
  height_ = y;

  return width_ * height_ > 10;
}

pixelbag::tile_iterator::tile_iterator(pixelbag& bag)
  : bag_(bag) {}
pixelbag::tile_iterator::tile_iterator(pixelbag& bag,
                                       TileSize width,
                                       TileSize height)
  : bag_(bag)
  , pixels_(std::make_unique<Pixel[]>(static_cast<size_t>(width) * height * 3))
  , t_(width, height, pixels_.get()) {
  if(width > 0) {
    maxbagtilenum_ = bag_.width_ / width;
    maxbagtilenum_ *= bag_.height_ / height;
  } else {
    maxbagtilenum_ = 0;
  }
  bagtilenum_ = 0;
}

pixelbag::tile_iterator::tile_iterator(const tile_iterator& o)
  : tile_iterator(o.bag_, o.t_.w, o.t_.h) {
  if(o.t_.w > 0)
    memcpy(pixels_.get(), o.pixels_.get(), t_.len());
}

pixelbag::tile_iterator::~tile_iterator() {}

pixelbag::tile_iterator&
pixelbag::tile_iterator::operator++() {
  if(bagtilenum_ < maxbagtilenum_ - 1) {
    ++bagtilenum_;
    dirty_ = true;
  } else {
    bagtilenum_ = std::numeric_limits<size_t>::max();
  }

  return *this;
}

void
pixelbag::tile_iterator::read_pixels() {
  assert(bagtilenum_ < maxbagtilenum_);
  assert(t_.w > 0);

  const size_t RGB = 3;

  size_t tiles_x = bag_.tiles_x(t_.w);
  size_t tiles_y = bag_.tiles_y(t_.h);

  size_t tile_x = bagtilenum_ % tiles_x;
  size_t tile_y = bagtilenum_ / tiles_x;

  size_t tile_px_xy0 =
    (tile_y * t_.w * t_.h * tiles_x * RGB) + tile_x * t_.w * RGB;

  for(size_t line = 0; line < t_.h; ++line) {
    const uint8_t* begin =
      bag_.data_.get() + tile_px_xy0 + (line * tiles_x * t_.w * RGB);
    const uint8_t* end = begin + RGB * t_.w;

    uint8_t* tgt = &pixels_[line * t_.w * RGB];
    std::copy(begin, end, tgt);
  }
}

int
pixelbag::parse_filename_to_layernum() const {
  auto layernumstart = filename_.find('-') + 1;
  auto layernumend = filename_.find('-', layernumstart);

  std::string layernum =
    filename_.substr(layernumstart, layernumend - layernumstart);

  return atoi(layernum.c_str());
}

std::string
pixelbag::parse_filename_to_fortress_name() const {
  auto fortressname_end = filename_.find('-');
  auto regionname_start = filename_.find('-', fortressname_end + 1) + 1;
  auto regionname_end = filename_.find('-', regionname_start);

  std::string fortress_name =
    filename_.substr(0, fortressname_end) + "-" +
    filename_.substr(regionname_start, regionname_end - regionname_start);

  return fortress_name;
}
}
