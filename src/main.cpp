#include <fstream>
#include <iostream>
#include <regex>

extern "C" {
#include <unistd.h>
}

#if !defined(_WIN32) && !defined(WIN32)
#include "indicators.hpp"
#endif

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

  if(argc == 0)
    return EXIT_FAILURE;

  TileSize tileWidth = 16, tileHeight = 16;

  {
    std::regex rgx("(\\d+)x(\\d+)");
    std::cmatch c;

    if(std::regex_search(argv[0], c, rgx)) {
      tileWidth = stoi(c[1].str());
      tileHeight = stoi(c[2].str());
    }
  }

#if !defined(_WIN32) && !defined(WIN32)
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
  bar.set_progress(1);
#endif

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
#if !defined(_WIN32) && !defined(WIN32)
    bar.set_progress(percentage_done * 100);
#else
    std::cerr << static_cast<int>(percentage_done * 100) << " %" << std::endl;
#endif

    const char* input = argv[i];
    pixelbag b;
    if(b.load_from_file(input)) {
      m.add_layer(std::move(b));
    }
  }

  if(m.layers().size() == 0) {
    std::cerr << "No layers parsed!" << std::endl;
    return EXIT_FAILURE;
  }

  if(outName.empty())
    outName = m.fortress_name() + ".fdf-map";

#if !defined(_WIN32) && !defined(WIN32)
  bar.set_option(option::PostfixText{ "Compressing to .fdf-map" });
  bar.set_progress(99);
#endif

  std::ofstream of(outName, std::ofstream::binary);
  fdfmap(m).write_to(of);
  of.close();

  return EXIT_SUCCESS;
}
