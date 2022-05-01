#include <iostream>

#include <boost/program_options.hpp>

#include "pixelbag.hpp"
#include "map.hpp"
#include "tileset.hpp"

using namespace dfmapcompressorpp;

int
main(int argc, char* argv[]) {
  namespace po = boost::program_options;
  po::options_description options("Allowed Options for DF Map CompressPP");
  // clang-format off
  options.add_options()
    ("help", "produce help message")
    ("input", po::value<std::vector<std::string>>()->multitoken(), "input files to analyze and compress")
    ("tilewidth,w", po::value<TileSize>()->required(), "tile width")
    ("tileheight,h", po::value<TileSize>()->required(), "tile height");
  // clang-format on
  po::positional_options_description positional_options;
  positional_options.add("input", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
              .options(options)
              .positional(positional_options)
              .run(),
            vm);
  po::notify(vm);

  if(vm.count("help")) {
    std::cerr << options << std::endl;
    return EXIT_SUCCESS;
  }

  TileSize tileWidth, tileHeight;

  tileWidth = vm["tilewidth"].as<TileSize>();
  tileHeight = vm["tileheight"].as<TileSize>();

  if(!vm["input"].empty()) {
    const auto& inputs = vm["input"].as<std::vector<std::string>>();

    map m(std::make_shared<tileset>(tileWidth, tileHeight));

    for(const auto& input : inputs) {
      pixelbag b;
      b.load_from_file(input);

      m.add_layer(std::move(b));
    }
  }

  return 0;
}
