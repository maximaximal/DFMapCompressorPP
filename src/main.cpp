#include <fstream>
#include <iostream>

extern "C" {
#include <unistd.h>
}

#include "indicators.hpp"

#include "fdf-map.hpp"
#include "map.hpp"
#include "pixelbag.hpp"
#include "tileset.hpp"

using namespace dfmapcompressorpp;

static void
help() {
  using std::cout;
  using std::endl;
  cout << "DFMapCompressorPP" << endl;
  cout << "USAGE:" << std::endl;
  cout << "  -h\t\topen help" << std::endl;
  cout << "  -w <tile width>\twidth of individual tiles" << std::endl;
  cout << "  -h <tile height>\theight of individual tiles" << std::endl;
  cout << "  -o <outfile>\tpath to output file (map name from .bmp files by "
          "default)"
       << std::endl;
  cout << "EXTRA:" << std::endl;
  cout
    << "  The name of the binary is parsed for _<width>x<height>.  This is the"
    << std::endl;
  cout << "  default for loaded images, even if no -w or -h were applied. This"
       << std::endl;
  cout
    << "  enables an easy shortcut: name the .exe\"dfmapcompressorpp_8x12.exe\""
    << std::endl;
  cout << "  and the parser will pick up the dimensions automatically."
       << std::endl;
}

int
main(int argc, char* argv[]) {
  using namespace indicators;

  ProgressBar bar{ option::BarWidth{ 60 },
                   option::Start{ "[" },
                   option::Fill{ "=" },
                   option::Lead{ ">" },
                   option::Remainder{ " " },
                   option::End{ " ]" },
                   option::PostfixText{ "Reading images" },
                   option::ForegroundColor{ Color::green },
                   option::FontStyles{
                     std::vector<FontStyle>{ FontStyle::bold } } };
  bar.set_progress(0);

  TileSize tileWidth, tileHeight;
  std::string outName;

  int c;
  opterr = 0;
  while((c = getopt(argc, argv, "o:w:h:")) != -1) {
    switch(c) {
      case 'w':
        tileWidth = std::atoi(optarg);
        break;
      case 'h':
        tileHeight = std::atoi(optarg);
        break;
      case 'o':
        outName = optarg;
        break;
      case '?':
        if(optopt == 'h')
          help();
        else if(optopt == 'w')
          help();
        else if(isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        abort();
    }
  }

  map m(std::make_shared<tileset>(tileWidth, tileHeight));

  for(size_t i = optind; i < argc; ++i) {
    int picture_count = argc - optind;
    double percentage_done =
      static_cast<double>(i - optind) / (picture_count + 1);
    bar.set_progress(percentage_done * 100);

    const char* input = argv[i];
    pixelbag b;
    if(b.load_from_file(input)) {
      m.add_layer(std::move(b));
    }
  }

  if(outName.empty())
    outName = m.fortress_name() + ".fdf-map";

  bar.set_option(option::PostfixText{ "Compressing to .fdf-map" });
  bar.set_progress(99);

  std::ofstream of(outName, std::ofstream::binary);
  fdfmap(m).write_to(of);
  of.close();

  return EXIT_SUCCESS;
}
