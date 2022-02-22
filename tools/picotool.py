import re
import collections
P8Char = collections.namedtuple('P8Char', ('p8scii', 'p8string', 'name'))
from sfx import *

# The P8SCII character set
P8SCII_CHARSET = [
		# Control codes
		P8Char(0, '\x00', 'Terminate printing'),
		P8Char(1, '\x01', 'Repeat next character P0 times'),
		P8Char(2, '\x02', 'Draw solid background with color P0'),
		P8Char(3, '\x03', 'Move cursor horizontally by P0-16 pixels'),
		P8Char(4, '\x04', 'Move cursor vertically by P0-16 pixels'),
		P8Char(5, '\x05', 'Move cursor by P0-16, P1-16 pixels'),
		P8Char(6, '\x06', 'Special command'),
		P8Char(7, '\x07', 'Audio command'),
		P8Char(8, '\x08', 'Backspace'),
		P8Char(9, '\x09', 'Tab'),
		P8Char(10, '\x0a', 'Newline'),
		P8Char(11, '\x0b', 'Decorate previous character command'),
		P8Char(12, '\x0c', 'Set foreground to color P0'),
		P8Char(13, '\x0d', 'Carriage return'),
		P8Char(14, '\x0e', 'Switch font defined at 0x5600'),
		P8Char(15, '\x0f', 'Switch font to default'),

		# Japanese punctuation
		P8Char(16, '▮', 'Vertical rectangle'),
		P8Char(17, '■', 'Filled square'),
		P8Char(18, '□', 'Hollow square'),
		P8Char(19, '⁙', 'Five dot'),
		P8Char(20, '⁘', 'Four dot'),
		P8Char(21, '‖', 'Pause'),
		P8Char(22, '◀', 'Back'),
		P8Char(23, '▶', 'Forward'),
		P8Char(24, '「', 'Japanese starting quote'),
		P8Char(25, '」', 'Japanese ending quote'),
		P8Char(26, '¥', 'Yen sign'),
		P8Char(27, '•', 'Interpunct'),
		P8Char(28, '、', 'Japanese comma'),
		P8Char(29, '。', 'Japanese full stop'),
		P8Char(30, '゛', 'Japanese dakuten'),
		P8Char(31, '゜', 'Japanese handakuten'),

		# ASCII
		P8Char(32, ' ', 'space'),
] + [
		P8Char(x, chr(x), chr(x)) for x in range(33, 127)
] + [
		P8Char(127, '○', 'Hollow circle'),

		# Symbols
		P8Char(128, '█', 'Rectangle'),
		P8Char(129, '▒', 'Checkerboard'),
		P8Char(130, '🐱', 'Jelpi'),
		P8Char(131, '⬇️', 'Down key'),
		P8Char(132, '░', 'Dot pattern'),
		P8Char(133, '✽', 'Throwing star'),
		P8Char(134, '●', 'Ball'),
		P8Char(135, '♥', 'Heart'),
		P8Char(136, '☉', 'Eye'),
		P8Char(137, '웃', 'Man'),
		P8Char(138, '⌂', 'House'),
		P8Char(139, '⬅️', 'Left key'),
		P8Char(140, '😐', 'Face'),
		P8Char(141, '♪', 'Musical note'),
		P8Char(142, '🅾️', 'O key'),
		P8Char(143, '◆', 'Diamond'),
		P8Char(144, '…', 'Ellipsis'),
		P8Char(145, '➡️', 'Right key'),
		P8Char(146, '★', 'Five-pointed star'),
		P8Char(147, '⧗', 'Hourglass'),
		P8Char(148, '⬆️', 'Up key'),
		P8Char(149, 'ˇ', 'Birds'),
		P8Char(150, '∧', 'Sawtooth'),
		P8Char(151, '❎', 'X key'),
		P8Char(152, '▤', 'Horiz lines'),
		P8Char(153, '▥', 'Vert lines'),

		# Hiragana
		P8Char(154, 'あ', 'Hiragana: a'),
		P8Char(155, 'い', 'i'),
		P8Char(156, 'う', 'u'),
		P8Char(157, 'え', 'e'),
		P8Char(158, 'お', 'o'),
		P8Char(159, 'か', 'ka'),
		P8Char(160, 'き', 'ki'),
		P8Char(161, 'く', 'ku'),
		P8Char(162, 'け', 'ke'),
		P8Char(163, 'こ', 'ko'),
		P8Char(164, 'さ', 'sa'),
		P8Char(165, 'し', 'si'),
		P8Char(166, 'す', 'su'),
		P8Char(167, 'せ', 'se'),
		P8Char(168, 'そ', 'so'),
		P8Char(169, 'た', 'ta'),
		P8Char(170, 'ち', 'chi'),
		P8Char(171, 'つ', 'tsu'),
		P8Char(172, 'て', 'te'),
		P8Char(173, 'と', 'to'),
		P8Char(174, 'な', 'na'),
		P8Char(175, 'に', 'ni'),
		P8Char(176, 'ぬ', 'nu'),
		P8Char(177, 'ね', 'ne'),
		P8Char(178, 'の', 'no'),
		P8Char(179, 'は', 'ha'),
		P8Char(180, 'ひ', 'hi'),
		P8Char(181, 'ふ', 'phu'),
		P8Char(182, 'へ', 'he'),
		P8Char(183, 'ほ', 'ho'),
		P8Char(184, 'ま', 'ma'),
		P8Char(185, 'み', 'mi'),
		P8Char(186, 'む', 'mu'),
		P8Char(187, 'め', 'me'),
		P8Char(188, 'も', 'mo'),
		P8Char(189, 'や', 'ya'),
		P8Char(190, 'ゆ', 'yu'),
		P8Char(191, 'よ', 'yo'),
		P8Char(192, 'ら', 'ra'),
		P8Char(193, 'り', 'ri'),
		P8Char(194, 'る', 'ru'),
		P8Char(195, 'れ', 're'),
		P8Char(196, 'ろ', 'ro'),
		P8Char(197, 'わ', 'wa'),
		P8Char(198, 'を', 'wo'),
		P8Char(199, 'ん', 'n'),
		P8Char(200, 'っ', 'Hiragana sokuon'),
		P8Char(201, 'ゃ', 'Hiragana digraphs: ya'),
		P8Char(202, 'ゅ', 'yu'),
		P8Char(203, 'ょ', 'yo'),

		# Katakana
		P8Char(204, 'ア', 'Katakana: a'),
		P8Char(205, 'イ', 'i'),
		P8Char(206, 'ウ', 'u'),
		P8Char(207, 'エ', 'e'),
		P8Char(208, 'オ', 'o'),
		P8Char(209, 'カ', 'ka'),
		P8Char(210, 'キ', 'ki'),
		P8Char(211, 'ク', 'ku'),
		P8Char(212, 'ケ', 'ke'),
		P8Char(213, 'コ', 'ko'),
		P8Char(214, 'サ', 'sa'),
		P8Char(215, 'シ', 'si'),
		P8Char(216, 'ス', 'su'),
		P8Char(217, 'セ', 'se'),
		P8Char(218, 'ソ', 'so'),
		P8Char(219, 'タ', 'ta'),
		P8Char(220, 'チ', 'chi'),
		P8Char(221, 'ツ', 'tsu'),
		P8Char(222, 'テ', 'te'),
		P8Char(223, 'ト', 'to'),
		P8Char(224, 'ナ', 'na'),
		P8Char(225, 'ニ', 'ni'),
		P8Char(226, 'ヌ', 'nu'),
		P8Char(227, 'ネ', 'ne'),
		P8Char(228, 'ノ', 'no'),
		P8Char(229, 'ハ', 'ha'),
		P8Char(230, 'ヒ', 'hi'),
		P8Char(231, 'フ', 'phu'),
		P8Char(232, 'ヘ', 'he'),
		P8Char(233, 'ホ', 'ho'),
		P8Char(234, 'マ', 'ma'),
		P8Char(235, 'ミ', 'mi'),
		P8Char(236, 'ム', 'mu'),
		P8Char(237, 'メ', 'me'),
		P8Char(238, 'モ', 'mo'),
		P8Char(239, 'ヤ', 'ya'),
		P8Char(240, 'ユ', 'yu'),
		P8Char(241, 'ヨ', 'yo'),
		P8Char(242, 'ラ', 'ra'),
		P8Char(243, 'リ', 'ri'),
		P8Char(244, 'ル', 'ru'),
		P8Char(245, 'レ', 're'),
		P8Char(246, 'ロ', 'ro'),
		P8Char(247, 'ワ', 'wa'),
		P8Char(248, 'ヲ', 'wo'),
		P8Char(249, 'ン', 'n'),
		P8Char(250, 'ッ', 'Katakana sokuon'),
		P8Char(251, 'ャ', 'Katakana digraphs: ya'),
		P8Char(252, 'ュ', 'yu'),
		P8Char(253, 'ョ', 'yo'),

		# Remaining symbols
		P8Char(254, '◜', 'Left arc'),
		P8Char(255, '◝', 'Right arc')
]

HEADER_TITLE_STR = b'pico-8 cartridge // http://www.pico-8.com\n'
HEADER_VERSION_RE = re.compile(br'version (\d+)\n')
SECTION_DELIM_RE = re.compile(br'__(\w+)__\n')

# Map of Unicode strings to P8SCII characters
UNICODE_TO_P8SCII = dict((c.p8string, c.p8scii) for c in P8SCII_CHARSET)
UNICODE_CHAR_WIDTHS = dict((k[0], len(k)) for k in UNICODE_TO_P8SCII.keys())


class Error(Exception):
		"""A base class for all errors in the picotool libraries."""
		pass


class InvalidP8DataError(Error):
		"""A base class for all invalid game file errors."""
		pass


class InvalidP8HeaderError(InvalidP8DataError):
		"""Exception for invalid .p8 file header."""

		def __str__(self):
				return 'Invalid .p8: missing or corrupt header'


def unicode_to_p8scii(s):
		"""Convert a Unicode string to P8SCII.

		Args:
				s: A Unicode string.

		Returns:
				A bytestring of P8SCII codes.
		"""
		result = []
		idx = 0
		while idx < len(s):
				char_width = UNICODE_CHAR_WIDTHS[s[idx]]
				result.append(UNICODE_TO_P8SCII[s[idx:idx+char_width]])
				idx += char_width
		return bytes(result)


def get_raw_data_from_p8_file(instr, filename=None):
		header_title_str = instr.readline()
		if header_title_str != HEADER_TITLE_STR:
				raise InvalidP8HeaderError()
		header_version_str = instr.readline()
		version_m = HEADER_VERSION_RE.match(header_version_str)
		if version_m is None:
				raise InvalidP8HeaderError()
		version = int(version_m.group(1))

		# (section is a text str.)
		section = None
		section_lines = {}
		while True:
				line = instr.readline()
				if not line:
						break
				section_delim_m = SECTION_DELIM_RE.match(line)
				if section_delim_m:
						section = str(section_delim_m.group(1), encoding='utf-8')
						section_lines[section] = []
				elif section:
						p8scii_line = unicode_to_p8scii(
								str(line, encoding='utf-8'))
						section_lines[section].append(p8scii_line)

		class P8Data(object):
				pass
		data = P8Data()
		data.version = version
		data.section_lines = section_lines

		return data


def from_lines(lines, version):
		"""Create an instance based on .p8 data lines.

		The base implementation reads lines of ASCII-encoded hexadecimal bytes.

		Args:
						lines: .p8 lines for the section.
						version: The PICO-8 data version from the game file header.

		Returns:
						A Gfx instance.
		"""
		datastrs = []
		for line in lines:
				if len(line) != 129:
						continue

				larray = list(line.rstrip())
				for i in range(0, 128, 2):
						(larray[i], larray[i+1]) = (larray[i+1], larray[i])

				larray_str = str(bytes(larray), encoding='ascii')
				datastrs.append(bytearray.fromhex(larray_str))

		data = b''.join(datastrs)
		return bytearray(b'\x00' * 128 * 64)


def mapbytes_from_lines(lines, version):
		"""Create an instance based on .p8 data lines.

		The base implementation reads lines of ASCII-encoded hexadecimal bytes.

		Args:
						lines: .p8 lines for the section.
						version: The PICO-8 data version from the game file header.

		Returns:
						A Gfx instance.
		"""
		datastrs = ""
		for line in lines:
				if len(line) != 129:
						continue

				larray = list(line.rstrip())
				for i in range(0, 128, 2):
						(larray[i], larray[i+1]) = (larray[i+1], larray[i])
				substring = ", ".join(("0x{0:X}".format(element))
															for element in larray)
				datastrs += substring + "\n"
		return datastrs

system_palette = [
		0x0000, #rgb2(0, 0, 0),         # 0 	black (also transparent by default for sprites)
		0x2511, #rgb2(29, 43, 83),      # 1 	dark-blue
		0x2527, #rgb2(126, 37, 83),     # 2 	dark-purple
		0x8530, #rgb2(0, 135, 81),      # 3 	dark-green
		0x534A, #rgb2(171, 82, 54),     # 4 	brown
		0x5455, #rgb2(95, 87, 79),      # 5 	dark-grey
		0xCC6C, #rgb2(194, 195, 199),   # 6 	light-grey
		0xFE7F, #rgb2(255, 241, 232),   # 7 	white
		0x048F, #rgb2(255, 0, 77),      # 8 	red
		0xA09F, #rgb2(255, 163, 0),     # 9	orange
		0xE2AF, #rgb2(255, 236, 39),    # 10	yellow
		0xE3B0, #rgb2(0, 228, 54),      # 11 green
		0xAFC2, #rgb2(41, 173, 255),    # 12 blue
		0x79D8, #rgb2(131, 118, 156),   # 13 lavender
		0x7AEF, #rgb2(255, 119, 168),   # 14 pink
		0xCAFF, #rgb2(255, 204, 170),   # 15 light-peach
		0x1102, #rgb2(41, 24, 20),    # 128 	brownish-black
		0x1311, #rgb2(17, 29, 53),    # 129 	darker-blue
		0x2324, #rgb2(66, 33, 54),    # 130 	darker-purple
		0x5531, #rgb2(18, 83, 89),    # 131 	blue-green
		0x2247, #rgb2(116, 47, 41),   # 132 	dark-brown
		0x3354, #rgb2(73, 51, 59),    # 133 	darker-grey
		0x876A, #rgb2(162, 136, 121), # 134 	medium-grey
		0xE77F, #rgb2(243, 239, 125), # 135 	light-yellow
		0x158B, #rgb2(190, 18, 80),   # 136 	dark-red
		0x629F, #rgb2(255, 108, 36),  # 137 	dark-orange
		0xE2AA, #rgb2(168, 231, 46),  # 138 	lime-green
		0xB4B0, #rgb2(0, 181, 67),    # 139 	medium-green
		0x5BC0, #rgb2(6, 90, 181),    # 140	true-blue
		0x46D7, #rgb2(117, 70, 101),  # 141 	mauve
		0x65EF, #rgb2(255, 110, 89),  # 142 	dark-peach
		0x98FF #rgb2(255, 157, 129), # 143 	peach    
]

def read_map(lines, gfxlines, version):
	"""Create an instance based on .p8 data lines.

	The base implementation reads lines of ASCII-encoded hexadecimal bytes.

	Args:
			lines: .p8 lines for the section.
			version: The PICO-8 data version from the game file header.
	"""
	arrayofbytes = []
	formattedlines = []
	for line in lines:
		myarray = bytearray.fromhex(str(line.rstrip(), encoding='ascii'))
		for element in myarray:
			arrayofbytes.append(element)
	for j in range(64, 128, 1):
		gfxline = gfxlines[j]
		if len(gfxline) == 129:
			datastrs = []
			larray = list(gfxline.rstrip())
			for i in range(0, 128, 2):
				(larray[i], larray[i+1]) = (larray[i+1], larray[i])
			larray_str = str(bytes(larray), encoding='ascii')
			for i in range(0, 128, 2):
				arrayofbytes.append(int(larray_str[i : i + 2], 16))
	chunk_size = 16
	chunked_list = [arrayofbytes[i:i+chunk_size] for i in range(0, len(arrayofbytes), chunk_size)]
	for i in range(len(chunked_list)):
		substring = ", ".join(("0x{0:02X}".format(element))
													for element in chunked_list[i])
		if i < len(chunked_list) - 1:
				substring = "\t" + substring + ",\n"
		else:
				substring = "\t" + substring
		formattedlines.append(substring)
	return "\nconst array<uint_fast8_t, 16384> map_data =\n{\n"+("".join(formattedlines))+"\n};"

def gfx_lines(lines, version):
		"""Create an instance based on .p8 data lines.

		The base implementation reads lines of ASCII-encoded hexadecimal bytes.

		Args:
						lines: .p8 lines for the section.
						version: The PICO-8 data version from the game file header.

		Returns:
						A Gfx instance.
		"""
		arrayofbytes = []
		formattedlines = []
		for gfxline in lines:
				if len(gfxline) == 129:
						datastrs = []
						larray = gfxline.rstrip()
						for i in range(0, 128, 1):
							arrayofbytes.append(system_palette[int(larray[i : i + 1], 16)])
		chunk_size = 12
		chunked_list = [arrayofbytes[i:i+chunk_size] for i in range(0, len(arrayofbytes), chunk_size)]
		for i in range(len(chunked_list)):
			substring = ", ".join(("0x{0:04X}".format(element))
														for element in chunked_list[i])
			if i < len(chunked_list) - 1:
					substring = "\t" + substring + ",\n"
			else:
					substring = "\t" + substring
			formattedlines.append(substring)
		return "\nstatic color_t spritedata[16384] =\n{\n"+("".join(formattedlines))+"\n};"

effects = [
	"p8::effect::none",
	"p8::effect::slide",
	"p8::effect::vibrato",
	"p8::effect::drop",
	"p8::effect::fade_in",
	"p8::effect::fade_out",
	"p8::effect::arp_fast",
	"p8::effect::arp_slow"
]

waveforms = [
    "p8::waveform::sine",
    "p8::waveform::triangle",
    "p8::waveform::sawtooth",
    "p8::waveform::square",
    "p8::waveform::pulse",
    "p8::waveform::ringing",
    "p8::waveform::noise",
    "p8::waveform::ringing_sine",
    "p8::waveform::custom_0",
    "p8::waveform::custom_1",
    "p8::waveform::custom_2",
    "p8::waveform::custom_3",
    "p8::waveform::custom_4",
    "p8::waveform::custom_5",
    "p8::waveform::custom_6",
    "p8::waveform::custom_7"
  ]

def sfx_convert(datasection):
	formattedlines = []
	s = Sfx.from_lines(datasection, 4)
	# sfxnotes = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, 16, 23, 35, 37, 38, 51, 54, 55]
	for id in range(64):
		notes = []
		a, b, c, d = s.get_properties(id)
		formattedlines.append("\t{{ {0}, {1}, {2}, {3}, {{\n".format(a,b,c,d))
		lastindex = 0
		for n in range(31, -1, -1):
			note = s.get_note(id, n)
			if not (note[2] == 0):
				lastindex = n
				break
		for m in range(0, lastindex + 1, 1):
			note2 = s.get_note(id, m)
			if (m < lastindex):
				formattedlines.append("\t\t{{ {0}, {1}, {2}, {3} }}, \n".format(note2[0], waveforms[note2[1]], note2[2], effects[note2[3]]))
			else:
				formattedlines.append("\t\t{{ {0}, {1}, {2}, {3} }}}}\n\t}},\n".format(note2[0], waveforms[note2[1]], note2[2], effects[note2[3]]))
		lastindex = 0
	return "\nstatic const vector<p8::pattern> patterns = {\n"+("".join(formattedlines))+"};"
	# note: pitch waveform volume effect
	# pattern: editormode speed loopstart loopend

	# effekte fadeout und slide umsetzen
	# waveforms: 
	# long und short square, ringing, sawtooth, ringing sinus, 
	# triangle, noise
	# --> long and short square, noise, ringing square

	# 0->SQUARE
	# 1->SQUARE
	# 2->SQUARE
	# 3->LONG_SQUARE
	# 4->SHORT_SQUARE
	# 5->SQUARE with RINGING
	# 6->SQUARE with NOISE
	# 7->SQUARE with RINGING

	# für slide note-funktion erweitern

	# jede note dauert 7812 Mikrosekunden mal sfx-Speed

	# keine sfx benutzen loops


# keine timer, einfach nur WARTEN (läuft ja auf dem 2. Core...)

# wo ist die formel zur frequenzberechnung?
