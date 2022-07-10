<?php

require __DIR__ . '/vendor/autoload.php';

$input_map = '../build/Counciltools-region1.fdf-map';
$print_debug_info = FALSE;
$chosen_map_layer = 0;
$display_tileset = FALSE;

if(isset($_GET["m"]))
	$input_map = urldecode($_GET["m"]);
else {
	echo("Require some map using the m GET param!<br/>");
	exit();
}
if(isset($_GET["t"]))
	$display_tileset = TRUE;
if(isset($_GET["l"]))
	$chosen_map_layer = $_GET["l"];
if(isset($_GET["d"]))
	$print_debug_info = TRUE;

$compressed = file_get_contents($input_map, 'rb');
$uncompressed = zlib_decode($compressed);
$br = new PhpBinaryReader\BinaryReader($uncompressed, PhpBinaryReader\Endian::ENDIAN_LITTLE);

// GENERAL DATA
// ------------

$version = $br->readInt32();
$number_of_tiles = $br->readInt32();
$tile_width = $br->readInt32();
$tile_height = $br->readInt32();
$number_of_map_layers = $br->readInt32();

$has_tile_color_info = $version == -2 || $version == -4;

$layers = [];
$pixels = [];
$layers_backmap = [];

$tileset_width = $tile_width * 128;
$tileset_height = $tile_height * (intval(($number_of_tiles - 1) / 128) + 1);

if($print_debug_info) {
	echo ("Version: $version<br>");
	echo ("Number of tiles: $number_of_tiles<br>");
	echo ("Tile Width: $tile_width<br>");
	echo ("Tile Height: $tile_height<br>");
	echo ("Number of map layers: $number_of_map_layers<br>");
	echo ("Tileset width: $tileset_width, height: $tileset_height<br>");
	exit();
}

$tileset = imagecreatetruecolor($tileset_width, $tileset_height);

// LAYER DATA
// ----------

for($i = 0; $i < $number_of_map_layers; ++$i) {
	$l = array(
		"depth" => $br->readInt32(),
		"width" => $br->readInt32(),
		"height" => $br->readInt32(),
	);
	$depth = $l["depth"];
	$layers_backmap[$depth] = $i;
	array_push($layers, $l);
}

$chosen_layer = 0;
if(isset($layers_backmap[$chosen_map_layer]))
	$chosen_layer = $layers_backmap[$chosen_map_layer];

if(isset($_GET["cl"]))
	$chosen_layer = intval($_GET["cl"]);

// TILE DEFINITIONS
// ----------------

$number_of_pixels = $tile_width * $tile_height;

for($i = 0; $i < $number_of_tiles; ++$i) {
	if($has_tile_color_info) {
		// Tile info not used here.
		$br->readUInt8();
		$br->readUInt8();
		$br->readUInt8();
	}

	$tx = (intval($i % 128)) * $tile_width;
	$ty = (intval($i / 128)) * $tile_height;

	$n = 0;
	$b = 0;
	$g = 0;
	$r = 0;
	$color = -1;

	for($px = 0; $px < $number_of_pixels; ++$px) {
		$px_x = intval($px % $tile_width);
		$px_y = intval($px / $tile_width);
		if($n == 0) {
			if($color != -1)
				imagecolordeallocate($tileset, $color);

			$n = $br->readUInt8(); // Index in pixeldata: 0
			$b = $br->readUInt8(); // Index in pixeldata: 1
			$g = $br->readUInt8(); // Index in pixeldata: 2
			$r = $br->readUInt8(); // Index in pixeldata: 3
			$color = imagecolorallocate($tileset, $r, $g, $b);
		}
		$res = imagesetpixel($tileset, $tx + $px_x, $ty + $px_y, $color);
		if($res === FALSE) {
			throw new Exception("Could not set pixel!");
		}
		--$n;
	}

	imagecolordeallocate($tileset, $color);
}

if($display_tileset) {
	header("Content-type: image/png");
	imagepng($tileset);
	exit();
}

// LAYER DATA READER
// -----------------

$read_tile = NULL;

if($number_of_tiles <= 127)
	$read_tile = function($br) { return $br->readUInt8(); };
else if($number_of_tiles <= 32767)
	$read_tile = function($br) { return $br->readUInt16(); };
else
	$read_tile = function($br) { return $br->readUInt32(); };

// LAYER DATA
// ----------
$map_width = $layers[$chosen_layer]['width'];
$map_height = $layers[$chosen_layer]['height'];

$map = imagecreatetruecolor($map_width * $tile_width, $map_height * $tile_height);

for($i = 0; $i < $number_of_map_layers; ++$i) {
	$map_width = $layers[$i]['width'];
	$map_height = $layers[$i]['height'];
	if($i != $chosen_layer) {
		for($t = 0; $t < $map_width * $map_height; ++$t) {
			$tile = $read_tile($br);
		}
		continue;
	}

	// Only need to draw one layer.
	// Format is columnwise!
	for($t = 0; $t < $map_width * $map_height; ++$t) {
		$tile = $read_tile($br);
		$y = intval($t % $map_height) * $tile_height;
		$x = intval($t / $map_height) * $tile_width;
		$tx = intval($tile % 128) * $tile_width;
		$ty = intval($tile / 128) * $tile_height;
		assert($tile < $number_of_tiles);
		// echo("$tile, from tx:$tx,ty:$ty to x:$x,y:$y<br>");
		imagecopy($map, $tileset, $x, $y, $tx, $ty, $tile_width, $tile_height);
	}
	break;
}

header("Content-type: image/png");
imagepng($map);

