import PIL.Image
import argparse

def color24to15(c: int):
    '''
    Transform a 24bit (RGB) color into a GBA 15bit color.
    '''
    return (
      ( ((c >> (0  + 3)) & 0x1f) << 10) |
      ( ((c >> (8  + 3)) & 0x1f) << 5) |
      ( ((c >> (16 + 3)) & 0x1f) << 0)
    )

class Tileset:
    '''
    Utility class to load an image file as a gba tileset.
    '''
    def __init__(self, img_path, tile_size):
        self.img = PIL.Image.open(args.input)
        assert (
            self.img.width % tile_size == 0 and
            self.img.height % tile_size == 0
        ), f"Invalid file dimension.\n"\
           f"Expected tile size = ({tile_size}x{tile_size})\n" \
           f"Image size = ({self.img.width}x{self.img.height})."
        self.tile_size = tile_size
        self._compute_palette()
        self._compute_tiles()

    def _compute_palette(self):
        self.palette = []
        for rgb in self.img.getdata():
            c24 = (rgb[0] << 16) | (rgb[1] << 8) | (rgb[2] << 0)
            c15 = color24to15(c24)
            if c15 in self.palette:
                continue
            self.palette.append(c15)
        assert len(self.palette) < 16, \
            f"Max n.colors is 16, but this image has {len(self.palette)}"

    def _compute_tiles(self):
        self.tiles = []
        get_pixel = lambda x, y: self.img.getdata()[y * self.img.width + x]
        # TODO: Assume 1d tileset image - 2D Support must be implemented
        for tile_id in range(self.img.width // tile_size):
            tile = []
            for i in range(self.tile_size):
                for j in range(self.tile_size):
                    rgb = get_pixel(tile_size * tile_id + j, i)
                    c24 = (rgb[2] << 0) | (rgb[1] << 8) | (rgb[0] << 16)
                    color_id = self.palette.index(color24to15(c24))
                    tile.append(color_id)
            self.tiles.append(tile)


def tile_as_hex(tile: [int], tile_size: int):
    '''
    Takes a single tile and creates a GBA hex representation of it.
    Currently only supports 16x16 tiles.
    '''
    assert tile_size == 16, "tile_as_hex only support 16x16 tiles"

    # Transforms the tile into hexadecimal color indexes
    hex_image = []
    v = ""
    for i, cid in enumerate(tile):
        v += str(hex(cid)[2:]) # Ignore '0x'
        if (i + 1) % 4 == 0:
            hex_image.append(f"0x{v[::-1]}")
            v = ""

    # Rearrange data to fit into GBA's expected order. Assume 1D 16x16 mode.
    result = []
    for k in [0, 2, 32, 34]:
        for i in range(8):
            result += [hex_image[4*i  + k], hex_image[4*i + 1 + k]]

    return ',\n'.join(result)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    parser.add_argument("tile_size")
    # TODO: Support for 2d tiling
    args = parser.parse_args()
    tile_size = int(args.tile_size)
    tileset = Tileset(args.input, tile_size)

    print(f"static u16_t const PALETTE_SIZE = {len(tileset.palette)};")
    print("static u16_t const PALETTE[] = {")
    for c15 in tileset.palette:
        print(f"  {c15:#0{6}x}, ")
    print("};");

    print("static u16_t const TILESET[] = {")
    for tile in tileset.tiles:
        print(tile_as_hex(tile, tile_size))
    print("};")

