#include <fstream>

#include <emscripten/bind.h>

#include "fdf-map.hpp"
#include "map.hpp"
#include "pixelbag.hpp"
#include "tileset.hpp"

using namespace dfmapcompressorpp;
using namespace emscripten;

std::string
process_images(int tileWidth,
               int tileHeight,
               std::vector<std::string> files,
               emscripten::val progress) {
  try {
  map m(std::make_shared<tileset>(tileWidth, tileHeight));

  if(files.size() == 0)
    return nullptr;

  float steps = files.size() + 1;

  for(size_t i = 0; i < files.size(); ++i) {
    const auto& f = files[i];
    pixelbag b;
    if(b.load_from_file(f)) {
      if(!m.add_layer(std::move(b))) {
        return nullptr;
      }
    }
    progress(i / steps * 100);
  }

  static std::string outName;
  outName = m.fortress_name() + ".fdf-map";
  std::ofstream of(outName, std::ofstream::binary);
  fdfmap(m).write_to(of);
  of.close();

  progress(100);

  return outName.c_str();
  } catch(std::exception& e) {
    return nullptr;
  } catch(...) {
    return nullptr;
  }
}

EMSCRIPTEN_BINDINGS(dfmapcompressorpp) {
  register_vector<std::string>("StringList");
  function("process_images", &process_images);
}
