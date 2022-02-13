def convertColor(r, g, b, a):
	r = r >> 4
	g = g >> 4
	b = b >> 4
	# a = a >> 4

	g = g << 12
	b = b << 8
	a = a << 4

	# c = (g << 12) | (b << 8) | (a << 4) | r
	c = g | b
	c = c | a
	c= c | r
	return "0x{0:04X}".format(c)

print(convertColor(0, 0, 0, 0))     
print(convertColor(29, 43, 83, 1))  
print(convertColor(126, 37, 83, 2)) 
print(convertColor(0, 135, 81, 3))  
print(convertColor(171, 82, 54, 4)) 
print(convertColor(95, 87, 79, 5))  
print(convertColor(194, 195, 199, 6))
print(convertColor(255, 241, 232, 7))
print(convertColor(255, 0, 77, 8))  
print(convertColor(255, 163, 0, 9)) 
print(convertColor(255, 236, 39, 10))
print(convertColor(0, 228, 54, 11))  
print(convertColor(41, 173, 255, 12))
print(convertColor(131, 118, 156, 13))
print(convertColor(255, 119, 168, 14))
print(convertColor(255, 204, 170, 15))
print(convertColor(41, 24, 20, 0))  
print(convertColor(17, 29, 53, 1))  
print(convertColor(66, 33, 54, 2))  
print(convertColor(18, 83, 89, 3))  
print(convertColor(116, 47, 41, 4)) 
print(convertColor(73, 51, 59, 5))  
print(convertColor(162, 136, 121, 6))
print(convertColor(243, 239, 125, 7))
print(convertColor(190, 18, 80, 8)) 
print(convertColor(255, 108, 36, 9))
print(convertColor(168, 231, 46, 10))
print(convertColor(0, 181, 67, 11))  
print(convertColor(6, 90, 181, 12))  
print(convertColor(117, 70, 101, 13))
print(convertColor(255, 110, 89, 14))
print(convertColor(255, 157, 129, 15))