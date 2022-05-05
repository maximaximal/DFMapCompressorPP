#pragma once

#include <limits>
#include <memory>
#include <string>

#include "tile.hpp"

extern "C" void
stbi_image_free(void*);

namespace dfmapcompressorpp {
class pixelbag {
  public:
  struct STBIDeleter {
    void operator()(unsigned char i[]) { stbi_image_free(i); }
  };
  using STBIPtr = std::unique_ptr<unsigned char[], STBIDeleter>;

  struct tile_iterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = tile;
    using pointer = tile*;
    using reference = tile&;

    tile_iterator(pixelbag& bag);
    tile_iterator(pixelbag& bag, TileSize width, TileSize height);
    tile_iterator(const tile_iterator& o);
    ~tile_iterator();

    reference operator*() {
      if(dirty_)
        read_pixels();
      return t_;
    }
    pointer operator->() {
      if(dirty_)
        read_pixels();
      return &t_;
    }

    tile_iterator& operator++();

    tile_iterator operator++(int) {
      tile_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const tile_iterator& a, const tile_iterator& b) {
      return &a.bag_ == &b.bag_ && a.bagtilenum_ == b.bagtilenum_;
    };
    friend bool operator!=(const tile_iterator& a, const tile_iterator& b) {
      return &a.bag_ != &b.bag_ || a.bagtilenum_ != b.bagtilenum_;
    };

    private:
    pixelbag& bag_;
    std::unique_ptr<Pixel[]> pixels_;
    tile t_;
    size_t bagtilenum_ = std::numeric_limits<size_t>::max();
    size_t maxbagtilenum_ = 0;
    bool dirty_ = true;

    void read_pixels();
  };

  struct tile_range {
    tile_range(pixelbag& bag, TileSize w, TileSize h)
      : bag(bag)
      , w(w)
      , h(h) {}

    tile_iterator begin() { return tile_iterator(bag, w, h); }
    tile_iterator end() { return tile_iterator(bag); }

    pixelbag& bag;
    TileSize w, h;
  };

  tile_range tiles(TileSize w, TileSize h) { return tile_range(*this, w, h); }

  inline size_t tiles_x(TileSize w) const { return width_ / w; }
  inline size_t tiles_y(TileSize h) const { return height_ / h; }

  pixelbag();

  bool load_from_file(const std::string& path);

  int parse_filename_to_layernum() const;
  std::string parse_filename_to_fortress_name() const;

  private:
  STBIPtr data_;
  int width_ = 0;
  int height_ = 0;
  std::string filename_;
  std::string path_;
};
}
