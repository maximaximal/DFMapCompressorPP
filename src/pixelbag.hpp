#pragma once

#include <string>

namespace dfmapcompressorpp {
  class pixelbag {
  public:
    pixelbag();

    bool load_from_file(const std::string &file);
  };
}
