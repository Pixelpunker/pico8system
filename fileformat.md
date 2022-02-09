# File Format

## Spritesheet

The spritesheet section begins with the delimiter __gfx__.

The spritesheet is represented in the .p8 file as 128 lines of 128 hexadecimal digits. Each line is a pixel row, and each hexadecimal digit is a pixel color value, in pixel order.

This differs from the in-memory representation in that the 4-bit hexadecimal digits appear in pixel order in the .p8 file, while pairs are swapped (least significant nybble first) in memory. This allows you to identify and draw images using hex digits with a text editor, if you like. If a manually edited file's __gfx__ section includes characters outside of the hexadecimal range, they're loaded as pixels of color 0.

A cart is allowed to use the bottommost 128 sprites as the bottommost 128x32 tiles of the map data. That is, if the cart calls map() with coordinates in that region, the data is read from the bottom of the spritesheet, and the map editor can view this memory either way.

In the .p8 file, this data is always saved in the __gfx__ section, even if the cart uses it as map data. Note that this is encoded as a linear series of 4-bit pixels, rather than a series of 8-bit bytes. Tools reading this section and converting to byte format should treat the first hex digit of each pair as the bottom 4 bits and the second digit as the top 4 bits.
