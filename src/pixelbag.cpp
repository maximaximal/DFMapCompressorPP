#include <filesystem>
#include <iostream>
#include <memory>

#include "stb_image.h"

#include "pixelbag.hpp"

struct STBIDeleter {
  void operator()(stbi_uc* i) { stbi_image_free(i); }
};

namespace dfmapcompressorpp {
pixelbag::pixelbag() {}

bool
pixelbag::load_from_file(const std::string& file) {
  if(!std::filesystem::exists(file)) {
    return false;
  }

  int x = 0;
  int y = 0;
  int n = 0;
  std::unique_ptr<stbi_uc, STBIDeleter> img(
    stbi_load(file.c_str(), &x, &y, &n, 3));

  std::cout << "X: " << x << ", Y: " << y << std::endl;
  std::cout << "X/48: " << x / 48 << ", Y/48: " << y / 48 << std::endl;

  std::cout << "X/48%16: " << x / 48 % 16 << ", Y/48%24: " << y / 48 % 24
            << std::endl;

  return false;
}
}
