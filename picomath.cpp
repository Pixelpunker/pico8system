#include <cstdint>
#include <climits>

namespace picomath
{
	///////PICO-8 functions

	// fixedpoint "floats" hack enabled by default here for two reasons:
	//   1. performance on the picosystem that does not have a FPU
	//   2. exact same behaviour as real PICO-8

	// very ugly hack:
	// in order to switch to fixed point type and arithmetic, without having
	// to replace every arithmetic operator with a macro call, use a C++
	// struct with operator overloading and #define float to it...
	// it works, i guess!
	struct number
	{
		int32_t n;
		static const int32_t FACTOR = (1 << 16);

		// constructor
		template <typename T>
		number(T v) { *this = v; /* use operator= */ }
		number()
		{ /*undefined*/
		}

		// construct number from raw integer value
		static number from_bits(int32_t i)
		{
			number tmp;
			tmp.n = i;
			return tmp;
		}

		// fractional part
		inline uint16_t frac() { return static_cast<uint16_t>(this->n); }

		inline int32_t floor()
		{
			return static_cast<int32_t>(static_cast<unsigned>(this->n) & 0xFFFF0000) / (1 << 16);
		}

		inline int32_t round()
		{
			auto result = static_cast<int32_t>(static_cast<unsigned>(this->n) & 0xFFFF0000) / (1 << 16);
			if (static_cast<uint16_t>(this->n) >= 32768)
			{
				result += 1;
			}
			return result;
		}

		inline int32_t ceil()
		{
			auto result = static_cast<int32_t>(static_cast<unsigned>(this->n) & 0xFFFF0000) / (1 << 16);
			if (static_cast<uint16_t>(this->n) > 0)
			{
				result += 1;
			}
			return result;
		}

		// T -> number
		template <typename T>
		static inline number from(T n) { return number::from_bits(n * T(FACTOR)); }
		// number -> T
		template <typename T>
		inline T to() const { return T(this->n) / T(FACTOR); }

		template <typename T>
		inline number &operator=(T n)
		{
			this->n = number::from<T>(n).n;
			return *this;
		}

		// same as .to<T>()
		template <typename T>
		inline explicit operator T() const { return this->to<T>(); }

		// arithmetic operator overloading
		inline friend number operator+(number a, number b) { return from_bits(a.n + b.n); }
		inline friend number operator-(number a, number b) { return from_bits(a.n - b.n); }
		inline friend number operator-(number a) { return from_bits(-a.n); }
		inline friend number operator*(number a, number b)
		{
			return from_bits(int64_t(a.n) * int64_t(b.n) / int64_t(FACTOR));
		}
		inline friend number operator/(number a, number b)
		{ // pico8 decomp'd
			if (b == 0)
				return a > 0 ? FACTOR : -FACTOR; // +inf / -inf
			if (b.frac() == 0 && ((int32_t)b > 0))
			{
				return number::from_bits(int64_t(a.n) / int64_t(b));
			}
			return from_bits((int64_t(a.n) * FACTOR) / int64_t(b.n));
		}

		inline friend number &operator+=(number &a, number b) { return a = a + b; }
		inline friend number &operator-=(number &a, number b) { return a = a - b; }
		inline friend number &operator*=(number &a, number b) { return a = a * b; }
		inline friend number &operator/=(number &a, number b) { return a = a / b; }
		inline friend bool operator==(number a, number b) { return a.n == b.n; }
		inline friend bool operator!=(number a, number b) { return a.n != b.n; }
		inline friend bool operator<(number a, number b) { return a.n < b.n; }
		inline friend bool operator>(number a, number b) { return a.n > b.n; }
		inline friend bool operator<=(number a, number b) { return a.n <= b.n; }
		inline friend bool operator>=(number a, number b) { return a.n >= b.n; }
	};

	static_assert(sizeof(number) == 4, "bad");

	// math functions
	static number mod(number a, number b)
	{
		return number::from_bits(((a.n % b.n) + b.n) % b.n);
	}
	static number sin(number x)
	{ // pico8 decomp'd
		static const int32_t sin_tbl[4098] = {65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65534, 65534, 65534, 65534, 65534, 65533, 65533, 65533, 65533, 65532, 65532, 65532, 65532, 65531, 65531, 65531, 65530, 65530, 65530, 65529, 65529, 65529, 65528, 65528, 65527, 65527, 65527, 65526, 65526, 65525, 65525, 65524, 65524, 65523, 65523, 65522, 65522, 65521, 65521, 65520, 65520, 65519, 65519, 65518, 65517, 65517, 65516, 65516, 65515, 65514, 65514, 65513, 65512, 65512, 65511, 65510, 65510, 65509, 65508, 65507, 65507, 65506, 65505, 65504, 65504, 65503, 65502, 65501, 65500, 65500, 65499, 65498, 65497, 65496, 65495, 65494, 65493, 65493, 65492, 65491, 65490, 65489, 65488, 65487, 65486, 65485, 65484, 65483, 65482, 65481, 65480, 65479, 65478, 65477, 65476, 65474, 65473, 65472, 65471, 65470, 65469, 65468, 65467, 65465, 65464, 65463, 65462, 65461, 65460, 65458, 65457, 65456, 65455, 65453, 65452, 65451, 65449, 65448, 65447, 65446, 65444, 65443, 65442, 65440, 65439, 65437, 65436, 65435, 65433, 65432, 65430, 65429, 65428, 65426, 65425, 65423, 65422, 65420, 65419, 65417, 65416, 65414, 65413, 65411, 65410, 65408, 65406, 65405, 65403, 65402, 65400, 65398, 65397, 65395, 65393, 65392, 65390, 65388, 65387, 65385, 65383, 65382, 65380, 65378, 65376, 65375, 65373, 65371, 65369, 65368, 65366, 65364, 65362, 65360, 65358, 65357, 65355, 65353, 65351, 65349, 65347, 65345, 65343, 65341, 65339, 65338, 65336, 65334, 65332, 65330, 65328, 65326, 65324, 65322, 65320, 65317, 65315, 65313, 65311, 65309, 65307, 65305, 65303, 65301, 65299, 65296, 65294, 65292, 65290, 65288, 65286, 65283, 65281, 65279, 65277, 65275, 65272, 65270, 65268, 65265, 65263, 65261, 65259, 65256, 65254, 65252, 65249, 65247, 65245, 65242, 65240, 65237, 65235, 65233, 65230, 65228, 65225, 65223, 65220, 65218, 65215, 65213, 65210, 65208, 65205, 65203, 65200, 65198, 65195, 65193, 65190, 65188, 65185, 65182, 65180, 65177, 65175, 65172, 65169, 65167, 65164, 65161, 65159, 65156, 65153, 65150, 65148, 65145, 65142, 65139, 65137, 65134, 65131, 65128, 65126, 65123, 65120, 65117, 65114, 65111, 65109, 65106, 65103, 65100, 65097, 65094, 65091, 65088, 65085, 65082, 65079, 65076, 65073, 65070, 65067, 65064, 65061, 65058, 65055, 65052, 65049, 65046, 65043, 65040, 65037, 65034, 65031, 65028, 65025, 65021, 65018, 65015, 65012, 65009, 65006, 65002, 64999, 64996, 64993, 64989, 64986, 64983, 64980, 64976, 64973, 64970, 64967, 64963, 64960, 64957, 64953, 64950, 64947, 64943, 64940, 64936, 64933, 64930, 64926, 64923, 64919, 64916, 64912, 64909, 64905, 64902, 64899, 64895, 64892, 64888, 64884, 64881, 64877, 64874, 64870, 64867, 64863, 64859, 64856, 64852, 64849, 64845, 64841, 64838, 64834, 64830, 64827, 64823, 64819, 64816, 64812, 64808, 64804, 64801, 64797, 64793, 64789, 64786, 64782, 64778, 64774, 64770, 64766, 64763, 64759, 64755, 64751, 64747, 64743, 64739, 64735, 64732, 64728, 64724, 64720, 64716, 64712, 64708, 64704, 64700, 64696, 64692, 64688, 64684, 64680, 64676, 64672, 64667, 64663, 64659, 64655, 64651, 64647, 64643, 64639, 64635, 64630, 64626, 64622, 64618, 64614, 64609, 64605, 64601, 64597, 64593, 64588, 64584, 64580, 64575, 64571, 64567, 64563, 64558, 64554, 64550, 64545, 64541, 64536, 64532, 64528, 64523, 64519, 64514, 64510, 64506, 64501, 64497, 64492, 64488, 64483, 64479, 64474, 64470, 64465, 64461, 64456, 64452, 64447, 64443, 64438, 64433, 64429, 64424, 64420, 64415, 64410, 64406, 64401, 64396, 64392, 64387, 64382, 64378, 64373, 64368, 64363, 64359, 64354, 64349, 64344, 64340, 64335, 64330, 64325, 64320, 64316, 64311, 64306, 64301, 64296, 64291, 64287, 64282, 64277, 64272, 64267, 64262, 64257, 64252, 64247, 64242, 64237, 64232, 64227, 64222, 64217, 64212, 64207, 64202, 64197, 64192, 64187, 64182, 64177, 64172, 64167, 64161, 64156, 64151, 64146, 64141, 64136, 64131, 64125, 64120, 64115, 64110, 64105, 64099, 64094, 64089, 64084, 64078, 64073, 64068, 64062, 64057, 64052, 64047, 64041, 64036, 64031, 64025, 64020, 64014, 64009, 64004, 63998, 63993, 63987, 63982, 63976, 63971, 63966, 63960, 63955, 63949, 63944, 63938, 63933, 63927, 63922, 63916, 63910, 63905, 63899, 63894, 63888, 63882, 63877, 63871, 63866, 63860, 63854, 63849, 63843, 63837, 63832, 63826, 63820, 63814, 63809, 63803, 63797, 63792, 63786, 63780, 63774, 63768, 63763, 63757, 63751, 63745, 63739, 63733, 63728, 63722, 63716, 63710, 63704, 63698, 63692, 63686, 63680, 63674, 63668, 63663, 63657, 63651, 63645, 63639, 63633, 63627, 63621, 63614, 63608, 63602, 63596, 63590, 63584, 63578, 63572, 63566, 63560, 63554, 63547, 63541, 63535, 63529, 63523, 63517, 63510, 63504, 63498, 63492, 63486, 63479, 63473, 63467, 63461, 63454, 63448, 63442, 63435, 63429, 63423, 63416, 63410, 63404, 63397, 63391, 63385, 63378, 63372, 63365, 63359, 63353, 63346, 63340, 63333, 63327, 63320, 63314, 63307, 63301, 63294, 63288, 63281, 63275, 63268, 63262, 63255, 63248, 63242, 63235, 63229, 63222, 63215, 63209, 63202, 63195, 63189, 63182, 63175, 63169, 63162, 63155, 63149, 63142, 63135, 63128, 63122, 63115, 63108, 63101, 63095, 63088, 63081, 63074, 63067, 63060, 63054, 63047, 63040, 63033, 63026, 63019, 63012, 63005, 62998, 62992, 62985, 62978, 62971, 62964, 62957, 62950, 62943, 62936, 62929, 62922, 62915, 62908, 62901, 62894, 62886, 62879, 62872, 62865, 62858, 62851, 62844, 62837, 62830, 62822, 62815, 62808, 62801, 62794, 62787, 62779, 62772, 62765, 62758, 62750, 62743, 62736, 62729, 62721, 62714, 62707, 62699, 62692, 62685, 62677, 62670, 62663, 62655, 62648, 62641, 62633, 62626, 62618, 62611, 62604, 62596, 62589, 62581, 62574, 62566, 62559, 62551, 62544, 62536, 62529, 62521, 62514, 62506, 62499, 62491, 62483, 62476, 62468, 62461, 62453, 62445, 62438, 62430, 62423, 62415, 62407, 62400, 62392, 62384, 62376, 62369, 62361, 62353, 62346, 62338, 62330, 62322, 62314, 62307, 62299, 62291, 62283, 62275, 62268, 62260, 62252, 62244, 62236, 62228, 62220, 62212, 62205, 62197, 62189, 62181, 62173, 62165, 62157, 62149, 62141, 62133, 62125, 62117, 62109, 62101, 62093, 62085, 62077, 62069, 62061, 62053, 62045, 62036, 62028, 62020, 62012, 62004, 61996, 61988, 61979, 61971, 61963, 61955, 61947, 61939, 61930, 61922, 61914, 61906, 61897, 61889, 61881, 61873, 61864, 61856, 61848, 61839, 61831, 61823, 61814, 61806, 61798, 61789, 61781, 61772, 61764, 61756, 61747, 61739, 61730, 61722, 61713, 61705, 61697, 61688, 61680, 61671, 61663, 61654, 61646, 61637, 61628, 61620, 61611, 61603, 61594, 61586, 61577, 61568, 61560, 61551, 61543, 61534, 61525, 61517, 61508, 61499, 61491, 61482, 61473, 61464, 61456, 61447, 61438, 61429, 61421, 61412, 61403, 61394, 61386, 61377, 61368, 61359, 61350, 61341, 61333, 61324, 61315, 61306, 61297, 61288, 61279, 61270, 61261, 61253, 61244, 61235, 61226, 61217, 61208, 61199, 61190, 61181, 61172, 61163, 61154, 61145, 61136, 61127, 61117, 61108, 61099, 61090, 61081, 61072, 61063, 61054, 61045, 61035, 61026, 61017, 61008, 60999, 60990, 60980, 60971, 60962, 60953, 60943, 60934, 60925, 60916, 60906, 60897, 60888, 60879, 60869, 60860, 60851, 60841, 60832, 60823, 60813, 60804, 60794, 60785, 60776, 60766, 60757, 60747, 60738, 60728, 60719, 60710, 60700, 60691, 60681, 60672, 60662, 60653, 60643, 60634, 60624, 60614, 60605, 60595, 60586, 60576, 60567, 60557, 60547, 60538, 60528, 60518, 60509, 60499, 60490, 60480, 60470, 60460, 60451, 60441, 60431, 60422, 60412, 60402, 60392, 60383, 60373, 60363, 60353, 60343, 60334, 60324, 60314, 60304, 60294, 60284, 60275, 60265, 60255, 60245, 60235, 60225, 60215, 60205, 60195, 60185, 60175, 60166, 60156, 60146, 60136, 60126, 60116, 60106, 60096, 60086, 60075, 60065, 60055, 60045, 60035, 60025, 60015, 60005, 59995, 59985, 59975, 59964, 59954, 59944, 59934, 59924, 59914, 59903, 59893, 59883, 59873, 59863, 59852, 59842, 59832, 59822, 59811, 59801, 59791, 59781, 59770, 59760, 59750, 59739, 59729, 59719, 59708, 59698, 59687, 59677, 59667, 59656, 59646, 59635, 59625, 59615, 59604, 59594, 59583, 59573, 59562, 59552, 59541, 59531, 59520, 59510, 59499, 59489, 59478, 59468, 59457, 59446, 59436, 59425, 59415, 59404, 59393, 59383, 59372, 59362, 59351, 59340, 59330, 59319, 59308, 59297, 59287, 59276, 59265, 59255, 59244, 59233, 59222, 59212, 59201, 59190, 59179, 59168, 59158, 59147, 59136, 59125, 59114, 59103, 59093, 59082, 59071, 59060, 59049, 59038, 59027, 59016, 59005, 58994, 58983, 58972, 58962, 58951, 58940, 58929, 58918, 58907, 58896, 58885, 58873, 58862, 58851, 58840, 58829, 58818, 58807, 58796, 58785, 58774, 58763, 58751, 58740, 58729, 58718, 58707, 58696, 58685, 58673, 58662, 58651, 58640, 58628, 58617, 58606, 58595, 58583, 58572, 58561, 58550, 58538, 58527, 58516, 58504, 58493, 58482, 58470, 58459, 58448, 58436, 58425, 58414, 58402, 58391, 58379, 58368, 58356, 58345, 58334, 58322, 58311, 58299, 58288, 58276, 58265, 58253, 58242, 58230, 58219, 58207, 58195, 58184, 58172, 58161, 58149, 58138, 58126, 58114, 58103, 58091, 58079, 58068, 58056, 58045, 58033, 58021, 58009, 57998, 57986, 57974, 57963, 57951, 57939, 57927, 57916, 57904, 57892, 57880, 57869, 57857, 57845, 57833, 57821, 57809, 57798, 57786, 57774, 57762, 57750, 57738, 57726, 57714, 57703, 57691, 57679, 57667, 57655, 57643, 57631, 57619, 57607, 57595, 57583, 57571, 57559, 57547, 57535, 57523, 57511, 57499, 57487, 57475, 57463, 57450, 57438, 57426, 57414, 57402, 57390, 57378, 57366, 57353, 57341, 57329, 57317, 57305, 57293, 57280, 57268, 57256, 57244, 57231, 57219, 57207, 57195, 57182, 57170, 57158, 57145, 57133, 57121, 57109, 57096, 57084, 57072, 57059, 57047, 57034, 57022, 57010, 56997, 56985, 56972, 56960, 56948, 56935, 56923, 56910, 56898, 56885, 56873, 56860, 56848, 56835, 56823, 56810, 56798, 56785, 56773, 56760, 56747, 56735, 56722, 56710, 56697, 56684, 56672, 56659, 56647, 56634, 56621, 56609, 56596, 56583, 56571, 56558, 56545, 56533, 56520, 56507, 56494, 56482, 56469, 56456, 56443, 56431, 56418, 56405, 56392, 56379, 56367, 56354, 56341, 56328, 56315, 56302, 56289, 56277, 56264, 56251, 56238, 56225, 56212, 56199, 56186, 56173, 56160, 56147, 56134, 56121, 56108, 56095, 56082, 56069, 56056, 56043, 56030, 56017, 56004, 55991, 55978, 55965, 55952, 55939, 55926, 55913, 55900, 55887, 55873, 55860, 55847, 55834, 55821, 55808, 55794, 55781, 55768, 55755, 55742, 55728, 55715, 55702, 55689, 55675, 55662, 55649, 55636, 55622, 55609, 55596, 55582, 55569, 55556, 55542, 55529, 55516, 55502, 55489, 55476, 55462, 55449, 55435, 55422, 55409, 55395, 55382, 55368, 55355, 55341, 55328, 55314, 55301, 55288, 55274, 55260, 55247, 55233, 55220, 55206, 55193, 55179, 55166, 55152, 55139, 55125, 55111, 55098, 55084, 55071, 55057, 55043, 55030, 55016, 55002, 54989, 54975, 54961, 54948, 54934, 54920, 54906, 54893, 54879, 54865, 54852, 54838, 54824, 54810, 54796, 54783, 54769, 54755, 54741, 54727, 54714, 54700, 54686, 54672, 54658, 54644, 54630, 54617, 54603, 54589, 54575, 54561, 54547, 54533, 54519, 54505, 54491, 54477, 54463, 54449, 54435, 54421, 54407, 54393, 54379, 54365, 54351, 54337, 54323, 54309, 54295, 54281, 54267, 54253, 54239, 54224, 54210, 54196, 54182, 54168, 54154, 54140, 54125, 54111, 54097, 54083, 54069, 54054, 54040, 54026, 54012, 53998, 53983, 53969, 53955, 53941, 53926, 53912, 53898, 53883, 53869, 53855, 53840, 53826, 53812, 53797, 53783, 53769, 53754, 53740, 53726, 53711, 53697, 53682, 53668, 53653, 53639, 53625, 53610, 53596, 53581, 53567, 53552, 53538, 53523, 53509, 53494, 53480, 53465, 53451, 53436, 53422, 53407, 53392, 53378, 53363, 53349, 53334, 53319, 53305, 53290, 53276, 53261, 53246, 53232, 53217, 53202, 53188, 53173, 53158, 53144, 53129, 53114, 53099, 53085, 53070, 53055, 53040, 53026, 53011, 52996, 52981, 52967, 52952, 52937, 52922, 52907, 52892, 52878, 52863, 52848, 52833, 52818, 52803, 52788, 52773, 52759, 52744, 52729, 52714, 52699, 52684, 52669, 52654, 52639, 52624, 52609, 52594, 52579, 52564, 52549, 52534, 52519, 52504, 52489, 52474, 52459, 52444, 52429, 52414, 52398, 52383, 52368, 52353, 52338, 52323, 52308, 52293, 52277, 52262, 52247, 52232, 52217, 52202, 52186, 52171, 52156, 52141, 52126, 52110, 52095, 52080, 52065, 52049, 52034, 52019, 52003, 51988, 51973, 51957, 51942, 51927, 51911, 51896, 51881, 51865, 51850, 51835, 51819, 51804, 51789, 51773, 51758, 51742, 51727, 51711, 51696, 51681, 51665, 51650, 51634, 51619, 51603, 51588, 51572, 51557, 51541, 51526, 51510, 51495, 51479, 51463, 51448, 51432, 51417, 51401, 51386, 51370, 51354, 51339, 51323, 51307, 51292, 51276, 51260, 51245, 51229, 51213, 51198, 51182, 51166, 51151, 51135, 51119, 51104, 51088, 51072, 51056, 51041, 51025, 51009, 50993, 50977, 50962, 50946, 50930, 50914, 50898, 50882, 50867, 50851, 50835, 50819, 50803, 50787, 50771, 50756, 50740, 50724, 50708, 50692, 50676, 50660, 50644, 50628, 50612, 50596, 50580, 50564, 50548, 50532, 50516, 50500, 50484, 50468, 50452, 50436, 50420, 50404, 50388, 50372, 50356, 50340, 50324, 50307, 50291, 50275, 50259, 50243, 50227, 50211, 50195, 50178, 50162, 50146, 50130, 50114, 50097, 50081, 50065, 50049, 50033, 50016, 50000, 49984, 49968, 49951, 49935, 49919, 49902, 49886, 49870, 49854, 49837, 49821, 49805, 49788, 49772, 49756, 49739, 49723, 49706, 49690, 49674, 49657, 49641, 49624, 49608, 49592, 49575, 49559, 49542, 49526, 49509, 49493, 49476, 49460, 49443, 49427, 49410, 49394, 49377, 49361, 49344, 49328, 49311, 49295, 49278, 49262, 49245, 49228, 49212, 49195, 49179, 49162, 49145, 49129, 49112, 49095, 49079, 49062, 49045, 49029, 49012, 48995, 48979, 48962, 48945, 48929, 48912, 48895, 48878, 48862, 48845, 48828, 48811, 48795, 48778, 48761, 48744, 48727, 48711, 48694, 48677, 48660, 48643, 48626, 48610, 48593, 48576, 48559, 48542, 48525, 48508, 48491, 48474, 48458, 48441, 48424, 48407, 48390, 48373, 48356, 48339, 48322, 48305, 48288, 48271, 48254, 48237, 48220, 48203, 48186, 48169, 48152, 48135, 48118, 48101, 48084, 48067, 48049, 48032, 48015, 47998, 47981, 47964, 47947, 47930, 47912, 47895, 47878, 47861, 47844, 47827, 47809, 47792, 47775, 47758, 47741, 47723, 47706, 47689, 47672, 47654, 47637, 47620, 47603, 47585, 47568, 47551, 47534, 47516, 47499, 47482, 47464, 47447, 47430, 47412, 47395, 47378, 47360, 47343, 47325, 47308, 47291, 47273, 47256, 47238, 47221, 47204, 47186, 47169, 47151, 47134, 47116, 47099, 47081, 47064, 47046, 47029, 47011, 46994, 46976, 46959, 46941, 46924, 46906, 46889, 46871, 46853, 46836, 46818, 46801, 46783, 46765, 46748, 46730, 46713, 46695, 46677, 46660, 46642, 46624, 46607, 46589, 46571, 46554, 46536, 46518, 46501, 46483, 46465, 46447, 46430, 46412, 46394, 46376, 46359, 46341, 46323, 46305, 46288, 46270, 46252, 46234, 46216, 46199, 46181, 46163, 46145, 46127, 46109, 46091, 46074, 46056, 46038, 46020, 46002, 45984, 45966, 45948, 45930, 45912, 45895, 45877, 45859, 45841, 45823, 45805, 45787, 45769, 45751, 45733, 45715, 45697, 45679, 45661, 45643, 45625, 45607, 45589, 45571, 45552, 45534, 45516, 45498, 45480, 45462, 45444, 45426, 45408, 45390, 45371, 45353, 45335, 45317, 45299, 45281, 45262, 45244, 45226, 45208, 45190, 45172, 45153, 45135, 45117, 45099, 45080, 45062, 45044, 45026, 45007, 44989, 44971, 44953, 44934, 44916, 44898, 44879, 44861, 44843, 44824, 44806, 44788, 44769, 44751, 44733, 44714, 44696, 44677, 44659, 44641, 44622, 44604, 44585, 44567, 44549, 44530, 44512, 44493, 44475, 44456, 44438, 44419, 44401, 44382, 44364, 44345, 44327, 44308, 44290, 44271, 44253, 44234, 44216, 44197, 44179, 44160, 44141, 44123, 44104, 44086, 44067, 44049, 44030, 44011, 43993, 43974, 43955, 43937, 43918, 43899, 43881, 43862, 43843, 43825, 43806, 43787, 43769, 43750, 43731, 43713, 43694, 43675, 43656, 43638, 43619, 43600, 43581, 43562, 43544, 43525, 43506, 43487, 43469, 43450, 43431, 43412, 43393, 43374, 43356, 43337, 43318, 43299, 43280, 43261, 43242, 43223, 43205, 43186, 43167, 43148, 43129, 43110, 43091, 43072, 43053, 43034, 43015, 42996, 42977, 42958, 42939, 42920, 42901, 42882, 42863, 42844, 42825, 42806, 42787, 42768, 42749, 42730, 42711, 42692, 42673, 42654, 42635, 42616, 42597, 42578, 42558, 42539, 42520, 42501, 42482, 42463, 42444, 42424, 42405, 42386, 42367, 42348, 42329, 42309, 42290, 42271, 42252, 42233, 42213, 42194, 42175, 42156, 42136, 42117, 42098, 42079, 42059, 42040, 42021, 42002, 41982, 41963, 41944, 41924, 41905, 41886, 41866, 41847, 41828, 41808, 41789, 41770, 41750, 41731, 41711, 41692, 41673, 41653, 41634, 41614, 41595, 41576, 41556, 41537, 41517, 41498, 41478, 41459, 41439, 41420, 41401, 41381, 41362, 41342, 41323, 41303, 41283, 41264, 41244, 41225, 41205, 41186, 41166, 41147, 41127, 41108, 41088, 41068, 41049, 41029, 41010, 40990, 40970, 40951, 40931, 40912, 40892, 40872, 40853, 40833, 40813, 40794, 40774, 40754, 40735, 40715, 40695, 40675, 40656, 40636, 40616, 40597, 40577, 40557, 40537, 40518, 40498, 40478, 40458, 40439, 40419, 40399, 40379, 40359, 40340, 40320, 40300, 40280, 40260, 40241, 40221, 40201, 40181, 40161, 40141, 40121, 40102, 40082, 40062, 40042, 40022, 40002, 39982, 39962, 39942, 39922, 39902, 39882, 39863, 39843, 39823, 39803, 39783, 39763, 39743, 39723, 39703, 39683, 39663, 39643, 39623, 39603, 39583, 39563, 39543, 39523, 39503, 39482, 39462, 39442, 39422, 39402, 39382, 39362, 39342, 39322, 39302, 39282, 39261, 39241, 39221, 39201, 39181, 39161, 39141, 39120, 39100, 39080, 39060, 39040, 39020, 38999, 38979, 38959, 38939, 38919, 38898, 38878, 38858, 38838, 38817, 38797, 38777, 38757, 38736, 38716, 38696, 38675, 38655, 38635, 38615, 38594, 38574, 38554, 38533, 38513, 38493, 38472, 38452, 38432, 38411, 38391, 38370, 38350, 38330, 38309, 38289, 38269, 38248, 38228, 38207, 38187, 38166, 38146, 38126, 38105, 38085, 38064, 38044, 38023, 38003, 37982, 37962, 37941, 37921, 37900, 37880, 37859, 37839, 37818, 37798, 37777, 37757, 37736, 37716, 37695, 37674, 37654, 37633, 37613, 37592, 37572, 37551, 37530, 37510, 37489, 37469, 37448, 37427, 37407, 37386, 37365, 37345, 37324, 37303, 37283, 37262, 37241, 37221, 37200, 37179, 37159, 37138, 37117, 37097, 37076, 37055, 37034, 37014, 36993, 36972, 36951, 36931, 36910, 36889, 36868, 36848, 36827, 36806, 36785, 36764, 36744, 36723, 36702, 36681, 36660, 36639, 36619, 36598, 36577, 36556, 36535, 36514, 36493, 36473, 36452, 36431, 36410, 36389, 36368, 36347, 36326, 36305, 36284, 36263, 36243, 36222, 36201, 36180, 36159, 36138, 36117, 36096, 36075, 36054, 36033, 36012, 35991, 35970, 35949, 35928, 35907, 35886, 35865, 35844, 35823, 35802, 35781, 35759, 35738, 35717, 35696, 35675, 35654, 35633, 35612, 35591, 35570, 35549, 35527, 35506, 35485, 35464, 35443, 35422, 35401, 35380, 35358, 35337, 35316, 35295, 35274, 35252, 35231, 35210, 35189, 35168, 35146, 35125, 35104, 35083, 35062, 35040, 35019, 34998, 34977, 34955, 34934, 34913, 34892, 34870, 34849, 34828, 34806, 34785, 34764, 34743, 34721, 34700, 34679, 34657, 34636, 34615, 34593, 34572, 34551, 34529, 34508, 34486, 34465, 34444, 34422, 34401, 34380, 34358, 34337, 34315, 34294, 34272, 34251, 34230, 34208, 34187, 34165, 34144, 34122, 34101, 34079, 34058, 34037, 34015, 33994, 33972, 33951, 33929, 33908, 33886, 33865, 33843, 33821, 33800, 33778, 33757, 33735, 33714, 33692, 33671, 33649, 33628, 33606, 33584, 33563, 33541, 33520, 33498, 33476, 33455, 33433, 33412, 33390, 33368, 33347, 33325, 33303, 33282, 33260, 33238, 33217, 33195, 33173, 33152, 33130, 33108, 33087, 33065, 33043, 33022, 33000, 32978, 32956, 32935, 32913, 32891, 32870, 32848, 32826, 32804, 32783, 32761, 32739, 32717, 32695, 32674, 32652, 32630, 32608, 32586, 32565, 32543, 32521, 32499, 32477, 32456, 32434, 32412, 32390, 32368, 32346, 32324, 32303, 32281, 32259, 32237, 32215, 32193, 32171, 32149, 32127, 32106, 32084, 32062, 32040, 32018, 31996, 31974, 31952, 31930, 31908, 31886, 31864, 31842, 31820, 31798, 31776, 31754, 31732, 31710, 31688, 31666, 31644, 31622, 31600, 31578, 31556, 31534, 31512, 31490, 31468, 31446, 31424, 31402, 31380, 31358, 31336, 31314, 31292, 31270, 31248, 31225, 31203, 31181, 31159, 31137, 31115, 31093, 31071, 31049, 31026, 31004, 30982, 30960, 30938, 30916, 30893, 30871, 30849, 30827, 30805, 30783, 30760, 30738, 30716, 30694, 30672, 30649, 30627, 30605, 30583, 30560, 30538, 30516, 30494, 30472, 30449, 30427, 30405, 30382, 30360, 30338, 30316, 30293, 30271, 30249, 30226, 30204, 30182, 30160, 30137, 30115, 30093, 30070, 30048, 30026, 30003, 29981, 29959, 29936, 29914, 29891, 29869, 29847, 29824, 29802, 29780, 29757, 29735, 29712, 29690, 29668, 29645, 29623, 29600, 29578, 29555, 29533, 29511, 29488, 29466, 29443, 29421, 29398, 29376, 29353, 29331, 29308, 29286, 29264, 29241, 29219, 29196, 29174, 29151, 29129, 29106, 29083, 29061, 29038, 29016, 28993, 28971, 28948, 28926, 28903, 28881, 28858, 28835, 28813, 28790, 28768, 28745, 28723, 28700, 28677, 28655, 28632, 28610, 28587, 28564, 28542, 28519, 28496, 28474, 28451, 28429, 28406, 28383, 28361, 28338, 28315, 28293, 28270, 28247, 28225, 28202, 28179, 28156, 28134, 28111, 28088, 28066, 28043, 28020, 27998, 27975, 27952, 27929, 27907, 27884, 27861, 27838, 27816, 27793, 27770, 27747, 27725, 27702, 27679, 27656, 27633, 27611, 27588, 27565, 27542, 27519, 27497, 27474, 27451, 27428, 27405, 27382, 27360, 27337, 27314, 27291, 27268, 27245, 27223, 27200, 27177, 27154, 27131, 27108, 27085, 27062, 27040, 27017, 26994, 26971, 26948, 26925, 26902, 26879, 26856, 26833, 26810, 26787, 26765, 26742, 26719, 26696, 26673, 26650, 26627, 26604, 26581, 26558, 26535, 26512, 26489, 26466, 26443, 26420, 26397, 26374, 26351, 26328, 26305, 26282, 26259, 26236, 26213, 26190, 26167, 26144, 26121, 26098, 26075, 26051, 26028, 26005, 25982, 25959, 25936, 25913, 25890, 25867, 25844, 25821, 25798, 25774, 25751, 25728, 25705, 25682, 25659, 25636, 25613, 25589, 25566, 25543, 25520, 25497, 25474, 25451, 25427, 25404, 25381, 25358, 25335, 25312, 25288, 25265, 25242, 25219, 25196, 25172, 25149, 25126, 25103, 25080, 25056, 25033, 25010, 24987, 24963, 24940, 24917, 24894, 24870, 24847, 24824, 24801, 24777, 24754, 24731, 24708, 24684, 24661, 24638, 24614, 24591, 24568, 24545, 24521, 24498, 24475, 24451, 24428, 24405, 24381, 24358, 24335, 24311, 24288, 24265, 24241, 24218, 24195, 24171, 24148, 24124, 24101, 24078, 24054, 24031, 24008, 23984, 23961, 23937, 23914, 23891, 23867, 23844, 23820, 23797, 23774, 23750, 23727, 23703, 23680, 23656, 23633, 23610, 23586, 23563, 23539, 23516, 23492, 23469, 23445, 23422, 23398, 23375, 23351, 23328, 23304, 23281, 23257, 23234, 23210, 23187, 23163, 23140, 23116, 23093, 23069, 23046, 23022, 22999, 22975, 22952, 22928, 22905, 22881, 22858, 22834, 22810, 22787, 22763, 22740, 22716, 22693, 22669, 22645, 22622, 22598, 22575, 22551, 22527, 22504, 22480, 22457, 22433, 22409, 22386, 22362, 22339, 22315, 22291, 22268, 22244, 22220, 22197, 22173, 22149, 22126, 22102, 22078, 22055, 22031, 22007, 21984, 21960, 21936, 21913, 21889, 21865, 21842, 21818, 21794, 21771, 21747, 21723, 21699, 21676, 21652, 21628, 21604, 21581, 21557, 21533, 21510, 21486, 21462, 21438, 21415, 21391, 21367, 21343, 21320, 21296, 21272, 21248, 21224, 21201, 21177, 21153, 21129, 21106, 21082, 21058, 21034, 21010, 20987, 20963, 20939, 20915, 20891, 20867, 20844, 20820, 20796, 20772, 20748, 20724, 20701, 20677, 20653, 20629, 20605, 20581, 20557, 20534, 20510, 20486, 20462, 20438, 20414, 20390, 20366, 20343, 20319, 20295, 20271, 20247, 20223, 20199, 20175, 20151, 20127, 20103, 20080, 20056, 20032, 20008, 19984, 19960, 19936, 19912, 19888, 19864, 19840, 19816, 19792, 19768, 19744, 19720, 19696, 19672, 19648, 19624, 19600, 19577, 19553, 19529, 19505, 19481, 19457, 19433, 19409, 19385, 19361, 19337, 19313, 19288, 19264, 19240, 19216, 19192, 19168, 19144, 19120, 19096, 19072, 19048, 19024, 19000, 18976, 18952, 18928, 18904, 18880, 18856, 18832, 18808, 18783, 18759, 18735, 18711, 18687, 18663, 18639, 18615, 18591, 18567, 18543, 18518, 18494, 18470, 18446, 18422, 18398, 18374, 18350, 18325, 18301, 18277, 18253, 18229, 18205, 18181, 18156, 18132, 18108, 18084, 18060, 18036, 18012, 17987, 17963, 17939, 17915, 17891, 17867, 17842, 17818, 17794, 17770, 17746, 17721, 17697, 17673, 17649, 17625, 17600, 17576, 17552, 17528, 17504, 17479, 17455, 17431, 17407, 17382, 17358, 17334, 17310, 17285, 17261, 17237, 17213, 17188, 17164, 17140, 17116, 17091, 17067, 17043, 17019, 16994, 16970, 16946, 16922, 16897, 16873, 16849, 16824, 16800, 16776, 16751, 16727, 16703, 16679, 16654, 16630, 16606, 16581, 16557, 16533, 16508, 16484, 16460, 16435, 16411, 16387, 16362, 16338, 16314, 16289, 16265, 16241, 16216, 16192, 16168, 16143, 16119, 16095, 16070, 16046, 16021, 15997, 15973, 15948, 15924, 15900, 15875, 15851, 15826, 15802, 15778, 15753, 15729, 15704, 15680, 15656, 15631, 15607, 15582, 15558, 15534, 15509, 15485, 15460, 15436, 15411, 15387, 15363, 15338, 15314, 15289, 15265, 15240, 15216, 15192, 15167, 15143, 15118, 15094, 15069, 15045, 15020, 14996, 14971, 14947, 14922, 14898, 14874, 14849, 14825, 14800, 14776, 14751, 14727, 14702, 14678, 14653, 14629, 14604, 14580, 14555, 14531, 14506, 14482, 14457, 14433, 14408, 14384, 14359, 14334, 14310, 14285, 14261, 14236, 14212, 14187, 14163, 14138, 14114, 14089, 14065, 14040, 14016, 13991, 13966, 13942, 13917, 13893, 13868, 13844, 13819, 13794, 13770, 13745, 13721, 13696, 13672, 13647, 13622, 13598, 13573, 13549, 13524, 13499, 13475, 13450, 13426, 13401, 13376, 13352, 13327, 13303, 13278, 13253, 13229, 13204, 13180, 13155, 13130, 13106, 13081, 13056, 13032, 13007, 12983, 12958, 12933, 12909, 12884, 12859, 12835, 12810, 12785, 12761, 12736, 12711, 12687, 12662, 12638, 12613, 12588, 12564, 12539, 12514, 12490, 12465, 12440, 12415, 12391, 12366, 12341, 12317, 12292, 12267, 12243, 12218, 12193, 12169, 12144, 12119, 12095, 12070, 12045, 12020, 11996, 11971, 11946, 11922, 11897, 11872, 11847, 11823, 11798, 11773, 11749, 11724, 11699, 11674, 11650, 11625, 11600, 11575, 11551, 11526, 11501, 11476, 11452, 11427, 11402, 11377, 11353, 11328, 11303, 11278, 11254, 11229, 11204, 11179, 11155, 11130, 11105, 11080, 11056, 11031, 11006, 10981, 10956, 10932, 10907, 10882, 10857, 10833, 10808, 10783, 10758, 10733, 10709, 10684, 10659, 10634, 10609, 10585, 10560, 10535, 10510, 10485, 10461, 10436, 10411, 10386, 10361, 10336, 10312, 10287, 10262, 10237, 10212, 10188, 10163, 10138, 10113, 10088, 10063, 10039, 10014, 9989, 9964, 9939, 9914, 9890, 9865, 9840, 9815, 9790, 9765, 9740, 9716, 9691, 9666, 9641, 9616, 9591, 9566, 9542, 9517, 9492, 9467, 9442, 9417, 9392, 9367, 9343, 9318, 9293, 9268, 9243, 9218, 9193, 9168, 9144, 9119, 9094, 9069, 9044, 9019, 8994, 8969, 8944, 8919, 8895, 8870, 8845, 8820, 8795, 8770, 8745, 8720, 8695, 8670, 8646, 8621, 8596, 8571, 8546, 8521, 8496, 8471, 8446, 8421, 8396, 8371, 8346, 8322, 8297, 8272, 8247, 8222, 8197, 8172, 8147, 8122, 8097, 8072, 8047, 8022, 7997, 7972, 7947, 7923, 7898, 7873, 7848, 7823, 7798, 7773, 7748, 7723, 7698, 7673, 7648, 7623, 7598, 7573, 7548, 7523, 7498, 7473, 7448, 7423, 7398, 7373, 7348, 7323, 7298, 7273, 7249, 7224, 7199, 7174, 7149, 7124, 7099, 7074, 7049, 7024, 6999, 6974, 6949, 6924, 6899, 6874, 6849, 6824, 6799, 6774, 6749, 6724, 6699, 6674, 6649, 6624, 6599, 6574, 6549, 6524, 6499, 6474, 6449, 6424, 6399, 6374, 6349, 6324, 6299, 6274, 6249, 6224, 6199, 6173, 6148, 6123, 6098, 6073, 6048, 6023, 5998, 5973, 5948, 5923, 5898, 5873, 5848, 5823, 5798, 5773, 5748, 5723, 5698, 5673, 5648, 5623, 5598, 5573, 5548, 5523, 5498, 5473, 5448, 5422, 5397, 5372, 5347, 5322, 5297, 5272, 5247, 5222, 5197, 5172, 5147, 5122, 5097, 5072, 5047, 5022, 4997, 4972, 4946, 4921, 4896, 4871, 4846, 4821, 4796, 4771, 4746, 4721, 4696, 4671, 4646, 4621, 4596, 4570, 4545, 4520, 4495, 4470, 4445, 4420, 4395, 4370, 4345, 4320, 4295, 4270, 4244, 4219, 4194, 4169, 4144, 4119, 4094, 4069, 4044, 4019, 3994, 3969, 3943, 3918, 3893, 3868, 3843, 3818, 3793, 3768, 3743, 3718, 3693, 3667, 3642, 3617, 3592, 3567, 3542, 3517, 3492, 3467, 3442, 3417, 3391, 3366, 3341, 3316, 3291, 3266, 3241, 3216, 3191, 3165, 3140, 3115, 3090, 3065, 3040, 3015, 2990, 2965, 2940, 2914, 2889, 2864, 2839, 2814, 2789, 2764, 2739, 2714, 2688, 2663, 2638, 2613, 2588, 2563, 2538, 2513, 2488, 2462, 2437, 2412, 2387, 2362, 2337, 2312, 2287, 2261, 2236, 2211, 2186, 2161, 2136, 2111, 2086, 2061, 2035, 2010, 1985, 1960, 1935, 1910, 1885, 1860, 1834, 1809, 1784, 1759, 1734, 1709, 1684, 1659, 1633, 1608, 1583, 1558, 1533, 1508, 1483, 1458, 1432, 1407, 1382, 1357, 1332, 1307, 1282, 1257, 1231, 1206, 1181, 1156, 1131, 1106, 1081, 1056, 1030, 1005, 980, 955, 930, 905, 880, 854, 829, 804, 779, 754, 729, 704, 679, 653, 628, 603, 578, 553, 528, 503, 478, 452, 427, 402, 377, 352, 327, 302, 276, 251, 226, 201, 176, 151, 126, 101, 75, 50, 25, 0, 0};

		unsigned index = ((x.n + 0x4002) >> 2) & 0x3FFF;
		if (0x1FFF < index)
		{
			index = 0x4000 - index;
		}
		if ((int32_t)index < 0x1000)
		{
			return number::from_bits(sin_tbl[index]);
		}
		return number::from_bits(-sin_tbl[0x2000 - index]);
	}

	static number cos(number x)
	{
		return sin(x + 0.25f); // cos(x) = sin(x+pi/2)
	}

	static int32_t floor(number x)
	{
		return static_cast<int32_t>(static_cast<unsigned>(x.n) & 0xFFFF0000) / (1 << 16);
	}

	static number min(number a, number b)
	{
		return a > b ? b : a;
	}
	static number max(number a, number b)
	{
		return a > b ? a : b;
	}
	static number abs(number x)
	{
		return x >= 0 ? x : -x;
	}

	// these values dont matter as set_rndseed should be called before init, as long as they arent both zero
	static unsigned rnd_seed_lo = 0, rnd_seed_hi = 1;
	static number rnd(number moritz)
	{ // decomp'd pico-8
		int32_t max = moritz.n;
		if (!max)
			return 0;
		rnd_seed_hi = ((rnd_seed_hi << 16) | (rnd_seed_hi >> 16)) + rnd_seed_lo;
		rnd_seed_lo += rnd_seed_hi;
		return number::from_bits(rnd_seed_hi % (unsigned)max);
	};
	static void pico8_srand(unsigned seed)
	{ // also decomp'd
		if (seed == 0)
		{
			rnd_seed_hi = 0x60009755;
			seed = 0xdeadbeef;
		}
		else
		{
			rnd_seed_hi = seed ^ 0xbead29ba;
		}
		for (int32_t i = 0x20; i > 0; i--)
		{
			rnd_seed_hi = ((rnd_seed_hi << 16) | (rnd_seed_hi >> 16)) + seed;
			seed += rnd_seed_hi;
		}
		rnd_seed_lo = seed;
	}
	static number sign(number v)
	{
		return v > 0 ? 1 : (v < 0 ? -1 : 0);
	}
	static int sign(int v)
	{
		return v > 0 ? 1 : (v < 0 ? -1 : 0);
	}
}