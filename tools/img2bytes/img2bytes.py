import PIL.Image

if __name__ == "__main__":
    img = PIL.Image.open("../../assets/abc.png")
    get_pixel = lambda x, y: img.getdata()[y * img.width + x]
    assert img.width % 8 == 0 and img.height % 8 == 0, "Invalid file dimensions"

    def extract_tile(k):
        tile = []
        for i in range(8):
            for j in range(8):
                tile.append(get_pixel(8 * k + j, i))
        return tile

    def to_hex(tile):
        result = ""
        v = ""
        for i, t in enumerate(tile):
            v += '1' if t[0] == 255 else '0'
            if (i + 1) % 4 == 0:
                result += f"0x{v[::-1]}, "
                v = ""
        return result

    for i in range(img.width // 8):
      print(to_hex(extract_tile(i)))

