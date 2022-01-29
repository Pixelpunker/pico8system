from picotool import *
filename = "map_data.hpp"
with open("assets/celeste.p8", 'rb') as fh:
  data = get_raw_data_from_p8_file(fh, filename="celeste.p8")
# print(data.section_lines['gfx'][1])
converted = from_lines(data.section_lines['map'], version=data.version)
file1 = open("assets/"+filename, "w")
file1.write(converted)
file1.close
print("Written "+filename)
