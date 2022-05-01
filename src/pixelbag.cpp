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
  filename_ = p.filename();

  int x = 0;
  int y = 0;
  int n = 0;
  data_.reset(stbi_load(path.c_str(), &x, &y, &n, 3));

  width_ = x;
  height_ = y;

  std::cout << "Filename: " << filename_ << std::endl;

  std::cout << "X: " << x << ", Y: " << y << std::endl;
  std::cout << "X/48: " << x / 48 << ", Y/48: " << y / 48 << std::endl;

  std::cout << "X/48%16: " << x / 48 % 16 << ", Y/48%24: " << y / 48 % 24
            << std::endl;

  return true;
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
  size_t pos = bagtilenum_ * RGB * t_.w;

  for(size_t line = 0; line < t_.h; ++line) {
    const uint8_t* begin = bag_.data_.get() + pos;
    const uint8_t* end = begin + RGB * t_.w;

    uint8_t* tgt = &pixels_[line * t_.w * RGB];
    std::copy(begin, end, tgt);

    pos += bag_.width_ * RGB;
  }
}
}
