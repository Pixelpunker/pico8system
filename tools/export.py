from picotool import *
from sfx import *
filename = "assets.hpp"
with open("assets/celeste.p8", 'rb') as fh:
  data = get_raw_data_from_p8_file(fh, filename="celeste.p8")
# print(data.section_lines['gfx'][1])
converted = "#include <array>\n#include \"../sfx.hpp\"\nusing namespace std;\nnamespace p8 = pico8;"
converted += read_map(data.section_lines['map'], 
	data.section_lines['gfx'], 
	version=data.version)

gfx = gfx_lines(data.section_lines['gfx'], data.version)
converted = converted + "\n" + gfx

sfx = sfx_convert(data.section_lines['sfx'])
converted = converted + "\n" + sfx

file1 = open("assets/"+filename, "w")
file1.write(converted)
file1.close
print("Written "+filename)
