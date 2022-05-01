#include "pixelbag.hpp"

using namespace dfmapcompressorpp;

int main(int argc, char *argv[]) {
  if(argc < 2)
    return 1;
  pixelbag b;
  b.load_from_file(argv[1]);
  return 0;
}
