#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <array>
#include <vector>
#include <exception>
#include <optional>
#include <algorithm>
#include <functional>
#include "pico8.cpp"
#include "celeste.hpp"
#include <cstdint>
#include <climits>
#include <unordered_map>
#include <unordered_set>
using namespace std;
using namespace picosystem;
using namespace picomath;

string message2; // debug
bool hasexception = false;

// *************************
// * PICO8SYSTEM ADDITIONS *
// *************************
std::array<std::array<int, 3>, 32> leveloffsets{
		std::array<int, 3>{5, 4, 0}, // 0
		std::array<int, 3>{5, 4, 0}, // 1
		std::array<int, 3>{5, 4, 0}, // 2
		std::array<int, 3>{5, 4, 1}, // 3
		std::array<int, 3>{5, 4, 1}, // 4
		std::array<int, 3>{5, 4, 0}, // 5
		std::array<int, 3>{5, 4, 0}, // 6
		std::array<int, 3>{5, 4, 1}, // 7
		std::array<int, 3>{5, 4, 1}, // 8
		std::array<int, 3>{5, 4, 0}, // 9
		std::array<int, 3>{5, 6, 1}, // 10
		std::array<int, 3>{5, 4, 0}, // 11
		std::array<int, 3>{5, 4, 1}, // 12
		std::array<int, 3>{5, 4, 1}, // 13
		std::array<int, 3>{5, 4, 0}, // 14
		std::array<int, 3>{5, 4, 1}, // 15
		std::array<int, 3>{5, 4, 1}, // 16
		std::array<int, 3>{5, 4, 1}, // 17
		std::array<int, 3>{5, 4, 1}, // 18
		std::array<int, 3>{5, 4, 1}, // 19
		std::array<int, 3>{5, 4, 1}, // 20
		std::array<int, 3>{5, 4, 1}, // 21
		std::array<int, 3>{5, 4, 0}, // 22
		std::array<int, 3>{5, 4, 0}, // 23
		std::array<int, 3>{5, 4, 1}, // 24
		std::array<int, 3>{5, 4, 1}, // 25
		std::array<int, 3>{5, 4, 1}, // 26
		std::array<int, 3>{5, 4, 1}, // 27
		std::array<int, 3>{5, 4, 1}, // 28
		std::array<int, 3>{5, 6, 1}, // 29
		std::array<int, 3>{5, 8, 0}, // 30
		std::array<int, 3>{5, 4, 0}	 // 31 (Title screen)
};

number playerx = number{0};

int viewportx = 0;
int viewporty = 0;

number screenshakex = number{0};
number screenshakey = number{0};

enum direction
{
	neutral = 0,
	left = 1,
	right = 2
};

typedef enum
{
	game = 0,
	menu = 1
} gamestate;

gamestate currentgamestate;

static struct
{
	int x0, x1, target1, target2, target3;
	direction findDirection(number playerx)
	{
		auto framedelay = 0;
		auto direction = direction::neutral;
		this->x0 = 0;
		this->x1 = playerx.round();
		if (this->x1 > this->x0 + framedelay)
		{
			direction = direction::right;
		}
		if (this->x1 < this->x0 - framedelay)
		{
			direction = direction::left;
		}
		if (this->x0 == this->x1)
		{
			direction = direction::neutral;
		}
		return direction;
	};
	int findTarget(number playerx)
	{
		auto target = 0; // 40 - 48 - 40 px regions
		if (playerx >= 0 and playerx <= 39)
		{
			target = this->target1;
		}
		if (playerx > 39 and playerx <= 86)
		{
			target = this->target2;
		}
		if (playerx > 86)
		{
			target = this->target3;
		}
		return target;
	};
} movetarget = {.x0 = 0, .x1 = 0, .target1 = 0, .target2 = 5, .target3 = 9};

// *****************
// * END ADDITIONS *
// *****************

/// <summary>
/// Attempting to Copy the Celeste Classic line-for-line here.
/// Obviously some differences due to converting from LUA to C#
///
/// This is not how I would recommend implementing a game like this from scratch in C#!
/// It's simply trying to be 1-1 with the LUA version
/// </summary>

// ~ celeste ~
// maddy thorson + noel berry

// forward declarations
class ClassicObject;
class player;
class player_hair;
class platform;
class fall_floor_spring;
class lifeup;
class fake_wall;
class smoke;
class orb;
struct Vect;
void load_room(int x, int y);
void next_room();
void restart_room();
void title_screen();
void draw_hair(player_hair &hair, Vect pos, number facing, number djump);
void psfx(number num);
void draw_time(number x, number y);
void refresh_objects(bool all = 0);
number tile_at(number x, number y);
bool ice_at(number x, number y, number w, number h);
bool solid_at(number x, number y, number w, number h);
bool spikes_at(number x, number y, number w, number h, number xspd, number yspd);
bool tile_flag_at(number x, number y, number w, number h, number flag);
bool maybe();
number appr(number val, number target, number amount);

// Additions
auto objectindex = 0;
vector<int8_t> markedfordelete;
vector<int8_t> markedparticlesfordelete;

enum class ObjType
{
	Player,
	Player_Spawn,
	Spring,
	Balloon,
	Smoke,
	Platform,
	Fall_Floor,
	Fruit,
	Fly_Fruit,
	Fake_Wall,
	Key,
	Chest,
	Lifeup,
	Message,
	Big_Chest,
	Orb,
	Flag,
	Room_Title,
};

// End additions

// *** "global" variables

struct Point
{
	int x;
	int y;

	Point()
	{
		x = 0;
		y = 0;
	}
	Point(int x_, int y_)
	{
		x = x_;
		y = y_;
	}
};

struct Vect
{
	number x;
	number y;

	Vect()
	{
		x = number{0};
		y = number{0};
	}
	Vect(number x_, number y_)
	{
		x = x_;
		y = y_;
	}
};

struct Rectangle
{
	number x = number{0}, y = number{0}, Width = number{0}, Height = number{0};
	Rectangle(number x_, number y_, number Width_, number Height_)
	{
		x = x_;
		y = y_;
		Width = Width_;
		Height = Height_;
	}
};

Point room;
unordered_map<int, shared_ptr<ClassicObject>> objects;
shared_ptr<ClassicObject> playerinstance = nullptr;
shared_ptr<player_hair> hair;
number freeze = number{0};
number shake = number{0};
bool will_restart = false;
number delay_restart = number{0};
array<bool, 32> got_fruit;
void reset_fruit()
{
	got_fruit = {false, false, false, false, false, false, false, false,
							 false, false, false, false, false, false, false, false,
							 false, false, false, false, false, false, false, false,
							 false, false, false, false, false, false, false, false};
}
bool has_dashed = false;
number sfx_timer = number{0};
bool has_key = false;
bool pause_player = false;
bool flash_bg = false;
number music_timer = number{0};
bool new_bg = false;

int k_left = 0;
int k_right = 1;
int k_up = 2;
int k_down = 3;
int k_jump = 5; // CHANGE was 4
int k_dash = 4; // CHANGE was number{5}

number frames = number{0};
number seconds = number{0};
number minutes = number{0};
number deaths = number{0};
number max_djump = number{0};
bool start_game = false;
number start_game_flash = number{0};

// ***

// *** effects

class Cloud
{
public:
	number x = number{0};
	number y = number{0};
	number spd = number{0};
	number w = number{0};
	Cloud(number x_, number y_, number spd_, number w_)
	{
		x = x_;
		y = y_;
		spd = spd_;
		w = w_;
	}
	~Cloud()
	{
	}
};
vector<shared_ptr<Cloud>> clouds;

class Particle
{
public:
	number x = number{0};
	number y = number{0};
	number s = number{0};
	number spd = number{0};
	number off = number{0};
	number c = number{0};
	Particle(number x_, number y_, number s_, number spd_, number off_, number c_)
	{
		x = x_;
		y = y_;
		s = s_;
		spd = spd_;
		off = off_;
		c = c_;
	}
	~Particle()
	{
	}
};
vector<shared_ptr<Particle>> particles;

class DeadParticle
{
public:
	number x = number{0};
	number y = number{0};
	number t = number{0};
	int id;
	Vect spd;
	DeadParticle()
	{
		id = objectindex;
		objectindex += 1;
	}
	DeadParticle(number x_, number y_, number t_, Vect spd_)
	{
		x = x_;
		y = y_;
		t = t_;
		spd = spd_;
	}
	~DeadParticle()
	{
	}
};
unordered_map<int8_t, shared_ptr<DeadParticle>> dead_particles;

// *** entry point

void ClassicInit()
{
	objects.clear();
	objects.reserve(30); // about 30 objects at the most
	room = Point(0, 0);
	freeze = number{0};
	will_restart = false;
	delay_restart = number{0};
	reset_fruit();
	has_dashed = false;
	sfx_timer = number{0};
	has_key = false;
	pause_player = false;
	flash_bg = false;
	music_timer = number{0};
	new_bg = false;

	frames = number{0};
	seconds = number{0};
	minutes = number{0};
	deaths = number{0};
	max_djump = number{1};
	start_game = false;
	start_game_flash = number{0};

	clouds.clear();
	for (auto i = 0; i <= 16; i++)
	{
		clouds.push_back(make_shared<Cloud>(
				rnd(number{128}),
				rnd(number{128}),
				number{1} + rnd(number{4}),
				number{32} + rnd(number{32})));
	}
	particles.clear();
	particles.reserve(50);
	for (auto i = 0; i <= 32; i++)
	{
		particles.push_back(make_shared<Particle>(
				rnd(number{128}),
				rnd(number{128}),
				number{1} + floor(rnd(number{5}) / number{4}), // change addition added 2
				(number{0.25} + rnd(number{5})),
				rnd(number{1}),
				number{6} + floor(number{0.5} + rnd(number{1}))));
	}

	dead_particles.clear();

	title_screen();
}

void title_screen()
{
	reset_fruit();
	frames = number{0};
	deaths = number{0};
	max_djump = number{1};
	start_game = false;
	start_game_flash = number{0};
	pico8::music(40, 0, 7);
	load_room(7, 3);
}

void begin_game()
{
	frames = number{0};
	seconds = number{0};
	minutes = number{0};
	music_timer = number{0};
	start_game = false;
	pico8::music(0, 0, 7);
	load_room(0, 0);
}

number level_index()
{
	return mod(room.x, number{8}) + room.y * number{8};
}

bool is_title()
{
	return level_index() == number{31};
}

// ***

// *** object functions

class ClassicObject
{
public:
	int id;
	bool isActive = true;
	ObjType type;
	bool collideable = true;
	bool solids = true;
	number spr = number{0};
	bool flipX = false;
	bool flipY = false;
	number x = number{0};
	number y = number{0};
	Rectangle hitbox = Rectangle(number{0}, number{0}, number{8}, number{8});
	Vect spd = Vect(number{0}, number{0});
	Vect rem = Vect(number{0}, number{0});
	ClassicObject()
	{
		id = objectindex;
		objectindex += 1;
	};
	virtual ~ClassicObject(){};

	virtual void init()
	{
	}

	virtual void update()
	{
	}

	void setInactive()
	{
		markedfordelete.push_back(this->id);
		this->isActive = false;
	}

	virtual void break_spring()
	{
	}

	virtual void draw()
	{
		if (this->isActive == false)
			return;
		if (spr > number{0})
		{
			pico8::spr(spr, x, y, number{1}, number{1}, flipX, flipY);
		}
	}

	bool is_solid(number ox, number oy)
	{
		if (this->isActive == false)
			return false;

		if (oy > number{0} && !check(ObjType::Platform, ox, number{0}) && check(ObjType::Platform, ox, oy))
		{
			return true;
		}
		return solid_at(x + hitbox.x + ox, y + hitbox.y + oy, hitbox.Width, hitbox.Height) ||
					 check(ObjType::Fall_Floor, ox, oy) ||
					 check(ObjType::Fake_Wall, ox, oy);
	}

	bool is_ice(number ox, number oy)
	{
		if (this->isActive == false)
			return false;
		return ice_at(x + hitbox.x + ox, y + hitbox.y + oy, hitbox.Width, hitbox.Height);
	}
	shared_ptr<ClassicObject> collide(ObjType newtype, number ox, number oy)
	{
		if (this->isActive == false)
			return nullptr;
		// destroy_objects(); // destroy objects already marked for delete
		// reflection is really slow so we use the type enum to check if two classic objects are of the same subtype
		for (const auto &otherpair : objects)
		{
			auto other = otherpair.second;
			if (other->type == newtype && other.get() != this && other->collideable && other->isActive == true &&
					other->x + other->hitbox.x + other->hitbox.Width > x + hitbox.x + ox &&
					other->y + other->hitbox.y + other->hitbox.Height > y + hitbox.y + oy &&
					other->x + other->hitbox.x < x + hitbox.x + hitbox.Width + ox &&
					other->y + other->hitbox.y < y + hitbox.y + hitbox.Height + oy)
				return other;
		}
		return nullptr;
	}

	bool check(ObjType newtype2, number ox, number oy)
	{
		if (this->isActive == false)
			return false;
		return collide(newtype2, ox, oy) != nullptr;
	}

	void move(number ox, number oy)
	{
		if (this->isActive == false)
			return;
		number amount = number{0};
		// [x] get move amount
		rem.x += ox;
		amount = floor(rem.x + number{0.5});
		rem.x -= amount;
		move_x(amount, number{0});

		// [y] get move amount
		rem.y += oy;
		amount = floor(rem.y + number{0.5});
		rem.y -= amount;
		move_y(amount);
	}

	void move_x(number amount, number start)
	{
		if (this->isActive == false)
			return;
		if (solids)
		{
			auto step = sign(amount);
			for (auto i = start; i <= picomath::abs(amount); i += number{1})
			{
				if (!is_solid(step, number{0}))
					x += step;
				else
				{
					spd.x = number{0};
					rem.x = number{0};
					break;
				}
			}
		}
		else
			x += amount;
	}

	void move_y(number amount)
	{
		if (this->isActive == false)
			return;
		if (solids)
		{
			auto step = sign(amount);
			for (auto i = number{0}; i <= picomath::abs(amount); i += number{1})
				if (!is_solid(number{0}, step))
					y += step;
				else
				{
					spd.y = number{0};
					rem.y = number{0};
					break;
				}
		}
		else
			y += amount;
	}
};

template <class T>
void init_object(number x, number y, optional<number> tile = nullopt, optional<bool> isSpring = nullopt, optional<number> direction = nullopt)
{
	auto obj = make_shared<T>();
	if (tile.has_value())
	{
		obj->spr = tile.value();
	}
	if (isSpring.has_value() && isSpring.value() == true)
	{ // special case for fall_floor_spring combo class
		obj->type = ObjType::Spring;
	}
	obj->x = x;
	obj->y = y;
	auto convertedobj = static_pointer_cast<ClassicObject>(obj);
	objects.emplace(convertedobj->id, convertedobj);
	if (convertedobj->type == ObjType::Player_Spawn) {
		playerinstance = convertedobj;
	}
	auto convertback = static_pointer_cast<T>(objects[convertedobj->id]);
	convertback->init();
	// return *convertback.get();
}

void markparticlesfordelete(int id)
{
	markedparticlesfordelete.push_back(id);
}

void refresh_objects(bool all)
{
	if (all == true)
	{
		objects.clear();
		hair.reset();
		markedfordelete.clear();
		return;
	}

	for (auto todelete : markedfordelete)
	{
		objects.erase(todelete);
	}
	markedfordelete.clear();
}

void destroy_particles(bool all = 0)
{
	if (all)
	{
		markedparticlesfordelete.clear();
		dead_particles.clear();
		return;
	}
	for (auto todelete : markedparticlesfordelete)
	{
		dead_particles.erase(todelete);
	}
	markedparticlesfordelete.clear();
}

void psfx(number num)
{
	if (sfx_timer <= number{0})
		pico8::sfx(num.floor());
}

// *** objects

class smoke : public ClassicObject
{
public:
	smoke()
	{
		type = ObjType::Smoke;
	}
	void init() override
	{
		spr = number{29};
		spd.y = number{-0.1};
		spd.x = number{0.3} + rnd(number{0.2});
		x += number{-1} + rnd(number{2});
		y += number{-1} + rnd(number{2});
		flipX = maybe(); // todo check is maybe a pico8 function?
		flipY = maybe(); // todo check is maybe a pico8 function?
		solids = false;
	}
	void update() override
	{
		if (this->isActive == false)
			return;
		spr += number{0.2};
		if (spr >= number{32})
			this->setInactive();
	}
};

class lifeup : public ClassicObject
{
	number duration;
	number flash;

public:
	lifeup()
	{
		type = ObjType::Lifeup;
	}
	void init() override
	{
		spd.y = number{-0.25};
		duration = number{30};
		x -= number{2};
		y -= number{4};
		flash = number{0};
		solids = false;
	}

	void update() override
	{
		if (this->isActive == false)
			return;
		duration -= number{1};
		if (duration <= number{0})
			this->setInactive();
	}

	void draw() override
	{
		if (this->isActive == false)
			return;
		flash += number{0.5};
		pico8::print("1000", x - number{2}, y, mod(number{7} + flash, number{2}));
	}
};

class player_hair
{
public:
	struct node
	{
	public:
		Vect pos;
		number size;
		node() = default;
		node(Vect pos_, number size_)
		{
			pos = pos_;
			size = size_;
		}
	};

	node hair[5];

	player_hair(Vect pos)
	{
		for (auto i = 0; i <= 4; i += 1)
			hair[i] = node(pos, picomath::max(number{1}, picomath::min(number{2}, number{3} - i)));
	};
};

// moved outside class player_hair because of circular definitions
void draw_hair(player_hair &hair, Vect pos, number facing, number djump)
{
	return;
	auto c = (djump == number{1} ? number{8} : (djump == number{2} ? (number{7} + floor(mod((frames / number{3}), number{2})) * number{4}) : number{12}));
	auto last = Vect(pos.x + number{4} - facing * number{2}, pos.y + (pico8::btn(k_down) ? number{4} : number{3}));
	for (auto &h : hair.hair)
	{
		h.pos.x += (last.x - h.pos.x) / number{1.5};
		h.pos.y += (last.y + number{0.5} - h.pos.y) / number{1.5};
		pico8::circfill(h.pos.x, h.pos.y, h.size, c);
		last = h.pos;
	}
}

class player_spawn : public ClassicObject
{
private:
	Vect target;
	number state;
	number delay;

public:
	bool p_jump;
	bool p_dash;
	number grace;
	number jbuffer;
	number djump;
	number dash_time;
	number dash_effect_time;
	Vect dash_target;
	Vect dash_accel;
	number spr_off;
	bool was_on_ground;
	player_spawn()
	{
		type = ObjType::Player_Spawn;
	};
	~player_spawn()
	{
		hair.reset();
	}
	void init() override
	{
		dead_particles.clear();
		this->spr = number{3};
		this->target = Vect(this->x, this->y);
		this->y = number{128};
		this->spd.y = number{-4};
		this->state = number{0};
		this->delay = number{0};
		this->solids = false;
		// hair = make_shared<player_hair>(Vect(this->x, this->y)); // now referring to global hair, not a member of player_spawn anymore. //debug
		pico8::sfx(4);
	}
	void playerinit()
	{
		this->state = 2;
		this->spd.y = 0;
		this->spd.x = 0;

		collideable = true;
		solids = true;
		flipX= false;
		flipY = false;
		
		spd.x = 0;
		spd.y = 0;
		rem.x = 0;
		rem.y = 0;


		type = ObjType::Player;
		p_jump = false;
		p_dash = false;
		grace = number{0};
		jbuffer = number{0};
		djump = number{0};
		dash_time = number{0};
		dash_effect_time = number{0};
		dash_target = Vect(number{0}, number{0});
		dash_accel = Vect(number{0}, number{0});
		spr_off = number{0};
		was_on_ground = false;

		spr = number{1};
		djump = max_djump;
		hitbox = Rectangle(number{1}, number{3}, number{6}, number{5});
	}

	void kill_player()
	{
		sfx_timer = number{12};
		pico8::sfx(0);
		deaths += number{1};
		shake = number{10};
		// Stats.Increment(Stat.PICO_DEATHS);
		this->setInactive();
		dead_particles.clear();
		for (auto dir = 0; dir <= 7; dir += 1)
		{
			auto angle = (dir / number{8.0});
			auto dead_particle = make_shared<DeadParticle>(
					this->x + number{4},
					this->y + number{4},
					10,
					Vect(picomath::cos(angle) * number{3}, picomath::sin(angle + number{0.5}) * number{3}));
			dead_particles.emplace(dead_particle->id, dead_particle);
		}
		// restart_room(); // debug
		// next_room(); // debug
		load_room(room.x, room.y);
		shake = 0;
	}

	void update() override
	{
		if (this->isActive == false)
			return;
		if (this->type == ObjType::Player_Spawn)
		{
			// jumping up
			if (state == number{0})
			{
				if (y < target.y + number{16})
				{
					state = number{1};
					delay = number{3};
				}
			}
			// falling
			else if (state == number{1})
			{
				spd.y += number{0.5};
				if (spd.y > number{0} && delay > number{0})
				{
					spd.y = number{0};
					delay -= number{1};
				}
				if (spd.y > number{0} && y > target.y)
				{
					y = target.y;
					spd = Vect(number{0}, number{0});
					state = number{2};
					delay = number{5};
					shake = number{5};
					init_object<smoke>(x, y + number{4});
					pico8::sfx(5);
				}
			}
			// landing
			else if (state == number{2})
			{
				delay -= number{1};
				spr = number{6};
				if (delay < number{0})
				{
					this->playerinit(); // todo: alle gemeinsamen felder auf initialwert setzen
				}
			}
		}
		if (this->type == ObjType::Player)
		{
			if (this->isActive == false)
				return;
			if (pause_player)
				return;
			auto input = pico8::btn(k_right) ? number{1} : (pico8::btn(k_left) ? number{-1} : number{0});

			// spikes collide
			if (spikes_at(x + hitbox.x, y + hitbox.y, hitbox.Width, hitbox.Height, spd.x, spd.y))
			{
				kill_player();
			}

			// bottom death
			if (y > number{128})
			{
				kill_player();
			}

			auto on_ground = is_solid(number{0}, number{1});
			auto on_ice = is_ice(number{0}, number{1});

			// smoke particles
			if (on_ground && !was_on_ground) {
				init_object<smoke>(x, y + number{4});
			}

			auto jump = pico8::btnp(k_jump) && !p_jump; // change addition, was btn(k_jump)
			p_jump = pico8::btn(k_jump);
			if (jump)
			{
				jbuffer = number{4};
			}
			else if (jbuffer > number{0})
			{
				jbuffer -= number{1};
			}

			auto dash = pico8::btnp(k_dash) && !p_dash; // change addition, was btn(k_dash)
			p_dash = pico8::btn(k_dash);

			if (on_ground)
			{
				grace = number{6};
				if (djump < max_djump)
				{
					psfx(number{54});
					djump = max_djump;
				}
			}
			else if (grace > number{0})
				grace -= number{1};

			dash_effect_time -= number{1};
			if (dash_time > number{0} && has_dashed)
			{
				init_object<smoke>(x, y);
				dash_time -= number{1};
				spd.x = appr(spd.x, dash_target.x, dash_accel.x);
				spd.y = appr(spd.y, dash_target.y, dash_accel.y);
			}
			else
			{
				// move
				auto maxrun = number{1};
				auto accel = number{0.6};
				auto deccel = number{0.15};

				if (!on_ground)
					accel = number{0.4};
				else if (on_ice)
				{
					accel = number{0.05};
					if (input == (flipX ? number{-1} : number{1})) // this it how it was in the pico-8 cart but is redundant?
						accel = number{0.05};
				}

				if (picomath::abs(spd.x) > maxrun)
					spd.x = appr(spd.x, sign(spd.x) * maxrun, deccel);
				else
					spd.x = appr(spd.x, input * maxrun, accel);

				// facing
				if (spd.x != number{0})
					flipX = (spd.x < number{0});

				// gravity
				auto maxfall = number{2.0};
				auto gravity = number{0.21};

				if (picomath::abs(spd.y) <= number{0.15})
					gravity *= number{0.5};

				// wall slide
				if (input != number{0} && is_solid(input, number{0}) && !is_ice(input, number{0}))
				{
					maxfall = number{0.4};
					if (rnd(number{10}) < number{2}) {
						init_object<smoke>(x + input * number{6}, y);
						}
				}

				if (!on_ground)
					spd.y = appr(spd.y, maxfall, gravity);

				// jump
				if (jbuffer > number{0})
				{
					if (grace > number{0})
					{
						// normal jump
						psfx(number{1});
						jbuffer = number{0};
						grace = number{0};
						spd.y = number{-2};
						init_object<smoke>(x, y + number{4});
					}
					else
					{
						// wall jump
						auto wall_dir = (is_solid(number{-3}, number{0}) ? number{-1} : (is_solid(number{3}, number{0}) ? number{1} : number{0}));
						if (wall_dir != number{0})
						{
							psfx(number{2});
							jbuffer = number{0};
							spd.y = number{-2};
							spd.x = -wall_dir * (maxrun + number{1});
							if (is_ice(wall_dir * number{3}, number{0})) {
								init_object<smoke>(x + wall_dir * number{6}, y);
							}
						}
					}
				}

				// dash
				auto d_full = number{5};
				auto d_half = d_full * number{0.70710678118};

				if (djump > number{0} && dash)
				{
					init_object<smoke>(x, y);
					djump -= number{1};
					dash_time = number{4};
					has_dashed = true;
					dash_effect_time = number{10};

					auto dash_x_input = (pico8::btn(k_left) ? number{-1} : (pico8::btn(k_right) ? number{1} : number{0})); // todo check again
					auto dash_y_input = (pico8::btn(k_up) ? number{-1} : (pico8::btn(k_down) ? number{1} : number{0}));		 // todo check again

					if (dash_x_input != number{0} && dash_y_input != number{0})
					{
						spd.x = dash_x_input * d_half;
						spd.y = dash_y_input * d_half;
					}
					else if (dash_x_input != number{0})
					{
						spd.x = dash_x_input * d_full;
						spd.y = number{0};
					}
					else
					{
						spd.x = number{0};
						spd.y = dash_y_input * d_full;
					}

					psfx(number{3});
					freeze = number{2};
					shake = number{6};
					dash_target.x = number{2} * sign(spd.x);
					dash_target.y = number{2} * sign(spd.y);
					dash_accel.x = number{1.5};
					dash_accel.y = number{1.5};

					if (spd.y < number{0})
						dash_target.y *= number{0.75};
					if (spd.y != number{0})
						dash_accel.x *= number{0.70710678118};
					if (spd.x != number{0})
						dash_accel.y *= number{0.70710678118};
				}
				else if (dash && djump <= number{0})
				{
					psfx(number{9});
					init_object<smoke>(x, y);
				}
			}

			// animation
			spr_off += number{0.25};
			if (!on_ground)
			{
				if (is_solid(input, number{0}))
					spr = number{5};
				else
					spr = number{3};
			}
			else if (pico8::btn(k_down))
				spr = number{6};
			else if (pico8::btn(k_up))
				spr = number{7};
			else if (spd.x == number{0} || (!pico8::btn(k_left) && !pico8::btn(k_right)))
				spr = number{1};
			else
				spr = number{1} + mod(spr_off, number{4});

			// next level
			if (y < number{-4} && level_index() < number{30})
				next_room();

			// was on the ground
			was_on_ground = on_ground;
		}
	}
	void draw_player()
	{
		if (this->isActive == false)
			return;

		auto localjump = this->type == ObjType::Player_Spawn ? max_djump : this->djump;

		auto spritePush = number{0};
		if (localjump == number{2})
		{
			if (mod((frames / number{3}), number{2}).floor() == 0)
				spritePush = number{10} * number{16};
			else
				spritePush = number{9} * number{16};
		}
		else if (localjump == number{0})
		{
			spritePush = number{8} * number{16};
		}
		pico8::spr(this->spr + spritePush, this->x, this->y -1 /* debug debug */, number{1}, number{1}, this->flipX, this->flipY);
	}
	void draw() override
	{
		if (this->isActive == false)
			return;
		auto localjump = this->type == ObjType::Player_Spawn ? max_djump : this->djump;

		// clamp in screen
		if (this->type == ObjType::Player && (x < number{-1} || x > number{121}))
		{
			x = clamp(x, number{-1}, number{121});
			spd.x = number{0};
		}
		playerx = this->x; // save current player position for Celeste additions; ADDITION

		draw_hair(*hair.get(), Vect(this->x, this->y), flipX ? number{-1} : number{1}, localjump);
		draw_player();
	}
};

class fall_floor_spring : public ClassicObject
{
public:
	number hide_in = number{0};
	number state = number{0};
	bool solid = true;
	number delay = number{0};

private:
	number hide_for = number{0};

public:
	fall_floor_spring()
	{
		type = ObjType::Fall_Floor; // needs to be set to spring to function as a spring
	}
	void break_fall_floor()
	{
		if (this->isActive == false)
			return;
		if (this->state == number{0})
		{
			psfx(number{15});
			this->state = number{1};
			this->delay = number{15}; // how long until it falls
			init_object<smoke>(this->x, this->y);
			auto hit = static_pointer_cast<fall_floor_spring>(this->collide(ObjType::Spring, number{0}, number{-1}));
			if (hit != nullptr)
				hit->break_spring();
		}
	}
	void update() override
	{
		if (this->isActive == false)
			return;
		if (type == ObjType::Spring)
		{
			if (hide_for > number{0})
			{
				hide_for -= number{1};
				if (hide_for <= number{0})
				{
					spr = number{18};
					delay = number{0};
				}
			}
			else if (spr == number{18})
			{
				auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{0}));
				if (hit != nullptr && hit->spd.y >= number{0})
				{
					spr = number{19};
					hit->y = y - number{4};
					hit->spd.x *= number{0.2};
					hit->spd.y = number{-3};
					hit->djump = max_djump;
					delay = number{10};
					init_object<smoke>(x, y);

					// breakable below us
					auto below = static_pointer_cast<fall_floor_spring>(collide(ObjType::Fall_Floor, number{0}, number{1}));
					if (below != nullptr)
						below->break_fall_floor();

					psfx(number{8});
				}
			}
			else if (delay > number{0})
			{
				delay -= number{1};
				if (delay <= number{0})
					spr = number{18};
			}

			// begin hiding
			if (hide_in > number{0})
			{
				hide_in -= number{1};
				if (hide_in <= number{0})
				{
					hide_for = number{60};
					spr = number{0};
				}
			}
		}
		if (type == ObjType::Fall_Floor)
		{
			if (state == number{0})
			{
				if (check(ObjType::Player, number{0}, number{-1}) || check(ObjType::Player, number{-1}, number{0}) || check(ObjType::Player, number{1}, number{0}))
					break_fall_floor();
			}
			else if (state == number{1})
			{
				delay -= number{1};
				if (delay <= number{0})
				{
					state = number{2};
					delay = number{60}; // how long it hides for
					collideable = false;
				}
			}
			else if (state == number{2})
			{
				delay -= number{1};
				if (delay <= number{0} && !check(ObjType::Player, number{0}, number{0}))
				{
					psfx(number{7});
					state = number{0};
					collideable = true;
					init_object<smoke>(x, y);
				}
			}
		}
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		if (type == ObjType::Fall_Floor)
		{
			if (state != number{2})
			{
				if (state != number{1})
					pico8::spr(number{23}, x, y);
				else
					pico8::spr((number{23} + (number{15} - delay) / number{5}), x, y);
			}
		}
		if (type == ObjType::Spring)
		{ // same as base class
			if (spr > number{0})
			{
				pico8::spr(spr, x, y, number{1}, number{1}, flipX, flipY);
			}
		}
	}
	void break_spring() override
	{
		this->hide_in = number{15};
	}
};

class balloon : public ClassicObject
{
private:
	number offset;
	number start;
	number timer;

public:
	balloon()
	{
		type = ObjType::Balloon;
	}
	void init() override
	{
		offset = rnd(number{1.0});
		start = y;
		hitbox = Rectangle(number{-1}, number{-1}, number{10}, number{10});
	}
	void update() override
	{
		if (this->isActive == false)
			return;
		if (spr == number{22})
		{
			offset += number{0.01};
			y = start + picomath::sin(offset) * number{2};
			auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{0}));
			if (hit != nullptr && hit->djump < max_djump)
			{
				psfx(number{6});
				init_object<smoke>(x, y);
				hit->djump = max_djump;
				spr = number{0};
				timer = number{60};
			}
		}
		else if (timer > number{0})
			timer -= number{1};
		else
		{
			psfx(number{7});
			init_object<smoke>(x, y);
			spr = number{22};
		}
	}
	void draw() override
	{
		if (this->isActive == false)
			return;
		if (spr == number{22})
		{
			pico8::spr(number{13} + mod((offset * number{8}), number{3}), x, y + number{6});
			pico8::spr(spr, x, y);
		}
	}
};

class fruit : public ClassicObject
{
	number start;
	number off;

public:
	fruit()
	{
		type = ObjType::Fruit;
	}
	void init() override
	{
		spr = number{26};
		start = y;
		off = number{0};
	}

	// todo enable stats

	void update() override
	{
		if (this->isActive == false)
			return;

		auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{0}));
		if (hit != nullptr)
		{
			hit->djump = max_djump;
			sfx_timer = number{20};
			pico8::sfx(13);
			got_fruit.at(level_index().floor()) = true;
			init_object<lifeup>(x, y);
			this->setInactive();
			// Stats.Increment(Stat.PICO_BERRIES);
		}
		off += number{1};
		y = start + picomath::sin(off / number{40.0}) * number{2.5};
	}
};

class fly_fruit : public ClassicObject
{
	number start;
	bool fly = false;
	number step = number{0.5};
	number sfx_delay = number{8};

public:
	fly_fruit()
	{
		type = ObjType::Fly_Fruit;
	}
	void init() override
	{
		start = y;
		solids = false;
	}

	void update() override
	{
		if (this->isActive == false)
			return;

		// fly away
		if (fly)
		{
			if (sfx_delay > number{0})
			{
				sfx_delay -= number{1};
				if (sfx_delay <= number{0})
				{
					sfx_timer = number{20};
					pico8::sfx(14);
				}
			}
			spd.y = appr(spd.y, number{-3.5}, number{0.25});
			if (y < number{-16})
				this->setInactive();
		}
		// wait
		else
		{
			if (has_dashed)
				fly = true;
			step += number{0.05};
			spd.y = picomath::sin(step) * number{0.5};
		}
		// collect
		auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{0}));
		if (hit != nullptr)
		{
			hit->djump = max_djump;
			sfx_timer = number{20};
			pico8::sfx(13);
			got_fruit.at(level_index().floor()) = true;
			init_object<lifeup>(x, y);
			this->setInactive();
			// Stats.Increment(Stat.PICO_BERRIES);
		}
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		number off = number{0.0};
		if (!fly)
		{
			auto dir = picomath::sin(step);
			if (dir < number{0})
				off = number{1} + picomath::max(number{0}, sign(y - start));
		}
		else
			off = mod((off + number{0.25}), number{3.0});
		pico8::spr(number{45} + off, x - number{6}, y - number{2}, number{1}, number{1}, true, false);
		pico8::spr(spr, x, y);
		pico8::spr(number{45} + off, x + number{6}, y - number{2});
	}
};

// todo: check if all floats are actually the pico8::number type

class fake_wall : public ClassicObject
{
public:
	fake_wall()
	{
		type = ObjType::Fake_Wall;
	}
	void update() override
	{
		if (this->isActive == false)
			return;

		hitbox = Rectangle(number{-1}, number{-1}, number{18}, number{18});
		auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{0}));
		if (hit != nullptr && hit->dash_effect_time > number{0})
		{
			hit->spd.x = -sign(hit->spd.x) * number{1.5};
			hit->spd.y = number{-1.5};
			hit->dash_time = number{-1};
			sfx_timer = number{20};
			pico8::sfx(16);
			this->setInactive();
			init_object<smoke>(x, y);
			init_object<smoke>(x + number{8}, y);
			init_object<smoke>(x, y + number{8});
			init_object<smoke>(x + number{8}, y + number{8});
			init_object<fruit>(x + number{4}, y + number{4});
		}
		hitbox = Rectangle(number{0}, number{0}, number{16}, number{16});
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		pico8::spr(number{64}, x, y);
		pico8::spr(number{65}, x + number{8}, y);
		pico8::spr(number{80}, x, y + number{8});
		pico8::spr(number{81}, x + number{8}, y + number{8});
	}
};

class key : public ClassicObject
{
public:
	key()
	{
		type = ObjType::Key;
	}
	void update() override
	{
		if (this->isActive == false)
			return;

		auto was = floor(spr);
		spr = number{9} + (picomath::sin(frames / number{30.0}) + number{0.5}) * number{1};
		auto current = floor(spr);
		if (current == number{10} && current != was)
			flipX = !flipX;
		if (check(ObjType::Player, number{0}, number{0}))
		{
			pico8::sfx(23);
			sfx_timer = number{20};
			this->setInactive();
			has_key = true;
		}
	}
};

class chest : public ClassicObject
{
	number start;
	number timer;

public:
	chest()
	{
		type = ObjType::Chest;
	}
	void init() override
	{
		x -= number{4};
		start = x;
		timer = number{20};
	}
	void update() override
	{
		if (this->isActive == false)
			return;

		if (has_key)
		{
			timer -= number{1};
			x = start - number{1} + rnd(number{3});
			if (timer <= number{0})
			{
				sfx_timer = number{20};
				pico8::sfx(16);
				init_object<fruit>(x, y - number{4});
				this->setInactive();
			}
		}
	}
};

class platform : public ClassicObject
{
public:
	number dir;
	number last;
	platform()
	{
		type = ObjType::Platform;
	}

	void init() override
	{
		x -= number{4};
		solids = false;
		hitbox.Width = number{16};
		last = x;
	}
	void update() override
	{
		if (this->isActive == false)
			return;
		spd.x = dir * number{0.65};
		if (x < number{-16})
			x = number{128};
		if (x > number{128})
			x = number{-16};
		if (!check(ObjType::Player, number{0}, number{0}))
		{
			auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{-1}));
			if (hit != nullptr)
				hit->move_x((x - last), number{1});
		}
		last = x;
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		pico8::spr(number{11}, x, y - number{1});
		pico8::spr(number{12}, x + number{8}, y - number{1});
	}
};

class message : public ClassicObject
{
	number last = number{0};
	number index = number{0};

public:
	message()
	{
		type = ObjType::Message;
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		// todo check why text in level is unreadable
		string text = "-=number{1} celeste mountain -=number{1}#this memorial to those# perished on the climb";
		if (check(ObjType::Player, number{4}, number{0}))
		{
			if (index < text.length())
			{
				index += number{0.5};
				if (index >= last + number{1})
				{
					last += number{1};
					pico8::sfx(35);
				}
			}

			auto off = Vect(number{8}, number{96});
			for (auto i = 0; i < index; i += 1)
			{
				if (text[i] != '#')
				{
					pico8::rectfill(off.x - number{2}, off.y - number{2}, off.x + number{7}, off.y + number{6}, number{7});
					pico8::print("" + text[i], off.x, off.y, number{0});
					off.x += number{5};
				}
				else
				{
					off.x = number{8};
					off.y += number{7};
				}
			}
		}
		else
		{
			index = number{0};
			last = number{0};
		}
	}
};

class orb : public ClassicObject
{
public:
	orb()
	{
		type = ObjType::Orb;
	}
	void init() override
	{
		spd.y = number{-4};
		solids = false;
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		spd.y = appr(spd.y, number{0}, number{0.5});
		auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{0}));
		if (spd.y == number{0} && hit != nullptr)
		{
			music_timer = number{45};
			pico8::sfx(51);
			freeze = number{10};
			shake = number{10};
			this->setInactive();
			max_djump = number{2};
			hit->djump = number{2};
		}

		pico8::spr(number{102}, x, y);
		auto off = frames / number{30.0};
		for (auto i = 0; i <= 7; i += 1)
			pico8::circfill(x + number{4} + picomath::cos(off + i / number{8.0}) * number{8}, y + number{4} + picomath::sin(off + i / number{8.0}) * number{8}, number{1}, number{7});
	}
};

class big_chest : public ClassicObject
{
	number state = number{0};
	number timer;

private:
	class particle
	{
	public:
		number x;
		number y;
		number h;
		number spd;
		particle(number x, number y, number h, number spd)
		{
			this->x = x;
			this->y = y;
			this->h = h;
			this->spd = spd;
		}
	};

	vector<shared_ptr<particle>> particles;

public:
	big_chest()
	{
		type = ObjType::Big_Chest;
	}
	~big_chest()
	{
		particles.clear();
		particles.reserve(50);
	}
	void init()
	{
		hitbox.Width = number{16};
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		if (state == number{0})
		{
			auto hit = static_pointer_cast<player_spawn>(collide(ObjType::Player, number{0}, number{8}));
			if (hit != nullptr && hit->is_solid(number{0}, number{1}))
			{
				pico8::music(-1, 500, 7);
				pico8::sfx(37);
				pause_player = true;
				hit->spd.x = number{0};
				hit->spd.y = number{0};
				state = number{1};
				init_object<smoke>(x, y);
				init_object<smoke>(x + number{8}, y);
				timer = number{60};
				particles.clear();
				particles.reserve(50);
			}
			pico8::spr(number{96}, x, y);
			pico8::spr(number{97}, x + number{8}, y);
		}
		else if (state == number{1})
		{
			timer -= number{1};
			shake = number{5};
			flash_bg = true;
			if (timer <= number{45} && particles.size() < number{50})
			{
				particles.push_back(make_shared<particle>(
						number{1} + rnd(number{14}),
						number{0},
						number{32} + rnd(number{32}),
						number{8} + rnd(number{8})));
			}
			if (timer < number{0})
			{
				state = number{2};
				particles.clear();
				particles.reserve(50);
				flash_bg = false;
				new_bg = true;
				init_object<orb>(x + number{4}, y + number{4});
				pause_player = false;
			}
			for (auto &p : particles)
			{
				p->y += p->spd;
				pico8::rectfill(x + p->x, y + number{8} - p->y, x + p->x + number{1}, picomath::min(y + number{8} - p->y + p->h, y + number{8}), number{7});
			}
		}

		pico8::spr(number{112}, x, y + number{8});
		pico8::spr(number{113}, x + number{8}, y + number{8});
	}
};

class flag : public ClassicObject
{
	number score = number{0};
	bool show = false;

public:
	flag()
	{
		type = ObjType::Flag;
	}
	void init()
	{
		x += number{5};
		for (auto fruit : got_fruit)
		{
			if (fruit == true)
			{
				score = score + 1;
			}
		}

		// Stats.Increment(Stat.PICO_COMPLETES);
		// Achievements.Register(Achievement.PICO8);
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		spr = number{118} + mod((frames / number{5.0}), number{3});
		pico8::spr(spr, x, y);
		if (show)
		{
			pico8::rectfill(number{32}, number{2}, number{96}, number{31}, number{0});
			pico8::spr(number{26}, number{55}, number{6});
			pico8::print("x" + to_string(score.floor()), number{64}, number{9}, number{7});
			draw_time(number{49}, number{16});
			pico8::print("deaths:" + to_string(deaths.floor()), number{48}, number{24}, number{7});
		}
		else if (check(ObjType::Player, number{0}, number{0}))
		{
			pico8::sfx(55);
			sfx_timer = number{30};
			show = true;
		}
	}
};

class room_title : public ClassicObject
{
	number delay = number{5};

public:
	room_title()
	{
		type = ObjType::Room_Title;
	}
	void draw() override
	{
		if (this->isActive == false)
			return;

		delay -= number{1};
		if (delay < number{-30})
			this->setInactive();
		else if (delay < number{0})
		{
			pico8::rectfill(number{24}, number{58}, number{104}, number{70}, number{0});
			if (room.x == 3 && room.y == 1)
				pico8::print("old site", number{48}, number{62}, number{7});
			else if (level_index() == number{30})
				pico8::print("summit", number{52}, number{62}, number{7});
			else
			{
				auto level = (number{1} + level_index()) * number{100};
				pico8::print(level.floor() + "m", number{52} + (level < number{1000} ? number{2} : number{0}), number{62}, number{7});
			}

			draw_time(number{4}, number{4});
		}
	}
};

// ***

// *** room functions

void restart_room()
{
	refresh_objects(true); // debug
	will_restart = true;
	delay_restart = number{15};
}

void next_room()
{
	if (room.x == 2 && room.y == 1)
		pico8::music(30, 500, 7);
	else if (room.x == 3 && room.y == 1)
		pico8::music(20, 500, 7);
	else if (room.x == 4 && room.y == 2)
		pico8::music(30, 500, 7);
	else if (room.x == 5 && room.y == 3)
		pico8::music(30, 500, 7);

	if (room.x == 7)
		load_room(0, room.y + 1);
	else
		load_room(room.x + 1, room.y);
}

void load_room(int x, int y)
{
	will_restart = false;
	// remove existing objects
	refresh_objects(true);
	// destroy_particles(true); // debug
	has_dashed = false;
	has_key = false;

	// current room
	room.x = x;
	room.y = y;

	// entities
	for (auto tx = 0; tx <= 15; tx += 1)
	{
		for (auto ty = 0; ty <= 15; ty += 1)
		{
			auto tile = pico8::mget(room.x * 16 + tx, room.y * 16 + ty);
			switch (tile)
			{
			case 11:
				init_object<platform>(tx * number{8}, ty * number{8}, nullopt, nullopt, number{-1});
				break;
			case 12:
				init_object<platform>(tx * number{8}, ty * number{8}, nullopt, nullopt, number{1});
				break;
			case 1:
				if (playerinstance == nullptr) {
					init_object<player_spawn>(tx * number{8}, ty * number{8}, tile);
				} else {
					playerinstance->x = tx * number{8};
					playerinstance->y = ty * number{8};
					playerinstance->spr = tile;
					playerinstance->isActive = true;
					objects.emplace(playerinstance->id, playerinstance);
				}
				break;
			case 18:
				init_object<fall_floor_spring>(tx * number{8}, ty * number{8}, tile, true); // spring (spring_fall_floor combo)
				break;
			case 22:
				init_object<balloon>(tx * number{8}, ty * number{8}, tile);
				break;
			case 23:
				init_object<fall_floor_spring>(tx * number{8}, ty * number{8}, tile, false); // fall_floor (spring_fall_floor combo)
				break;
			case 86:
				init_object<message>(tx * number{8}, ty * number{8}, tile);
				break;
			case 96:
				init_object<big_chest>(tx * number{8}, ty * number{8}, tile);
				break;
			case 118:
				init_object<flag>(tx * number{8}, ty * number{8}, tile);
				break;
			}
			if (got_fruit.at(level_index().floor()) == false)
			{
				switch (tile)
				{
				case 26:
					init_object<fruit>(tx * number{8}, ty * number{8}, tile);
					break;
				case 28:
					init_object<fly_fruit>(tx * number{8}, ty * number{8}, tile);
					break;
				case 64:
					init_object<fake_wall>(tx * number{8}, ty * number{8}, tile);
					break;
				case 8:
					init_object<key>(tx * number{8}, ty * number{8}, tile);
					break;
				case 20:
					init_object<chest>(tx * number{8}, ty * number{8}, tile);
					break;
				}
			}
		}
	}

	if (!is_title())
		init_object<room_title>(number{0}, number{0});

	// ADDITION
	movetarget.target1 = leveloffsets[level_index().floor()][0];
	// END ADDITION
}

// *** update

void ClassicUpdate()
{
	if (hasexception == true)
	{
		return;
	}
	if (pressed(X))
	{ // debug
		next_room();
	} // end debug;
	frames = mod(frames + number{1}, number{30});
	if (frames == number{0} && level_index() < number{30})
	{
		seconds = mod(seconds + number{1}, number{60});
		if (seconds == number{0})
			minutes += number{1};
	}

	if (music_timer > number{0})
	{
		music_timer -= number{1};
		if (music_timer <= number{0})
			pico8::music(10, 0, 7);
	}

	if (sfx_timer > number{0})
		sfx_timer -= number{1};

	// cancel if freeze
	if (freeze > number{0})
	{
		freeze -= number{1};
		return;
	}

	// screenshake
	if (shake > number{0})
	{
		shake -= number{1};
		// pico8::camera();
		if (shake > number{0})
		{
			screenshakex = -(number{-2} + rnd(number{5}));
			screenshakey = -(number{-2} + rnd(number{5}));
		}
		else
		{
			screenshakex = number{0};
			screenshakey = number{0};
		}
		// pico8::camera(number{-2} + rnd(number{5}), number{-2} + rnd(number{5}));
	}

	// restart(soon)
	if (will_restart && delay_restart > number{0})
	{
		delay_restart -= number{1};
		if (delay_restart <= number{0})
		{
			will_restart = true;
			load_room(room.x, room.y);
		}
	}

	// clear deleted objects
	refresh_objects();
	// update each object
	for (auto &obj : objects)
	{
		obj.second->move(obj.second->spd.x, obj.second->spd.y);
		obj.second->update();
	}
	// clear additional deleted objects
	refresh_objects();

	// start game
	if (is_title())
		if (!start_game && (pico8::btnp(k_jump) || pico8::btnp(k_dash) || pressed(X) || pressed(Y))) // addition change was pico8::btn(k_jump) || pico8::btn(k_dash)
		{
			pico8::music(-1, 0, 0);
			start_game_flash = number{50};
			start_game = true;
			pico8::sfx(38);
		}
	if (start_game)
	{
		start_game_flash -= number{1};
		if (start_game_flash <= number{-30})
			begin_game();
	}
}

// *** drawing

void draw_time(number x, number y)
{
	auto s = seconds;
	auto m = mod(minutes, number{60});
	auto h = floor(minutes / number{60});

	x += viewportx - number{4}; // addition
	y += viewporty - number{4}; // addition

	// pico8::rectfill(x, y, x + number{32}, y + number{6}, number{0});
	// pico8::print((h < number{10} ? "0" : "") + to_string(h) + ":" + (m < number{10} ? "0" : "") + to_string(m) + ":" + (s < number{10} ? "0" : "") + to_string(s), x + number{1}, y + number{1}, number{7});

	// begin addition / change
	char str[27];
	snprintf(str, sizeof(str), "%.2i:%.2i:%.2i", h, m, s);
	pico8::rectfill(x, y, x + pico8::gettextwidth(str) - number{6}, y + number{8}, number{0});
	pico8::print(str, x + number{1}, y + number{1}, number{7});
	// end addition / change
}

void ClassicDraw()
{
	if (hasexception == true)
	{
		return;
	}
	// reset all palette values
	pico8::pal();

	// start game flash
	if (start_game)
	{
		auto c = number{10};
		if (start_game_flash > number{10})
		{
			if (mod(frames, number{10}) < number{5})
				c = number{7};
		}
		else if (start_game_flash > number{5})
			c = number{2};
		else if (start_game_flash > number{0})
			c = number{1};
		else
			c = number{0};

		if (c < number{10})
		{
			pico8::pal(number{6}, c, number{1}); // TODO: workaround redirecting to screen palette
			pico8::pal(number{12}, c, number{1});
			pico8::pal(number{13}, c, number{1});
			pico8::pal(number{5}, c, number{1});
			pico8::pal(number{1}, c, number{1});
			pico8::pal(number{7}, c, number{1});
		}
	}

	// clear screen
	auto bg_col = number{0};
	if (flash_bg)
		bg_col = frames / number{5};
	else if (new_bg)
		bg_col = number{2};
	pico8::cls(bg_col); // todo for number{60}fps number{2}. draw: reuse bg_col

	// clouds
	if (!is_title())
	{
		for (auto &c : clouds) // todo: check all (auto &item : objects) loops
		{
#ifdef SIXTYFPSMODE
			c->x += c->spd / 2; // 60 fps
#else
			c->x += c->spd;
#endif
			pico8::rectfill(c->x, c->y, c->x + c->w, c->y + number{4} + (number{1} - c->w / number{64}) * number{12}, new_bg ? number{14} : number{1});
			if (c->x > number{128})
			{
				c->x = -c->w;
				c->y = rnd(number{128} - number{8});
			}
		}
	}

	// draw bg terrain
	pico8::map(room.x * number{16}, room.y * number{16}, number{0}, number{0}, number{16}, number{16}, number{2});

	// destroy_objects();
	// platforms / big chest
	for (auto &obj : objects)
	{
		if (obj.second->type == ObjType::Platform || obj.second->type == ObjType::Big_Chest)
			obj.second->draw();
	}

	// draw terrain
	auto off = is_title() ? number{-4} : number{0};
	pico8::map(room.x * number{16}, room.y * number{16}, off, number{0}, number{16}, number{16}, number{1});

	// draw objects
	// destroy_objects();
	for (auto &obj : objects)
	{
		if (obj.second->type != ObjType::Platform && obj.second->type != ObjType::Big_Chest)
			obj.second->draw();
	}

	// draw fg terrain
	pico8::map(room.x * number{16}, room.y * number{16}, number{0}, number{0}, number{16}, number{16}, number{3});

	// particles
	for (auto &p : particles)
	{
#ifdef SIXTYFPSMODE
		p->x += p->spd / 2;																							// 60 fps
		p->y += picomath::sin(p->off) / 2;															// 60 fps
		p->off += picomath::min(number{0.05}, p->spd / number{32}) / 2; // 60 fps
#else
		p->x += p->spd;
		p->y += picomath::sin(p->off);
		p->off += picomath::min(number{0.05}, p->spd / number{32});
#endif
		pico8::rectfill(p->x, p->y, p->x + p->s, p->y + p->s, p->c);
		if (p->x > number{132})
		{
			p->x = number{-4};
			p->y = rnd(number{128});
		}
	}

	// dead particles
	for (auto &p : dead_particles)
	{
		p.second->x += p.second->spd.x;
		p.second->y += p.second->spd.y;
		p.second->t -= number{1};
		if (p.second->t <= number{0})
		{
			markparticlesfordelete(p.second->id); // todo: check not sure if should be drawn anyway
		}
	}
	destroy_particles();
	for (auto &p : dead_particles)
	{
		pico8::rectfill(p.second->x - p.second->t / number{5}, p.second->y - p.second->t / number{5},
										p.second->x + p.second->t / number{5}, p.second->y + p.second->t / number{5}, number{14} + mod(p.second->t, number{2}));
	}

	// draw outside of the screen for screenshake
	/* 	pico8::rectfill(number{-5}, number{-5}, number{-1}, number{133}, number{0});
		pico8::rectfill(number{-5}, number{-5}, number{133}, number{-1}, number{0});
		pico8::rectfill(number{-5}, number{128}, number{133}, number{133}, number{0});
		pico8::rectfill(number{128}, number{-5}, number{133}, number{133}, number{0}); */

	// C# Change: "press button" instead to fit consoles
	// no need for credits here
	if (is_title())
	{
		// pico8::print("press button", number{42}, number{96}, number{5});
		// pico8::print("matt thorson", number{42}, number{96}, number{5});
		// pico8::print("noel berry", number{46}, number{102}, number{5});
		// pico8::center("A+B", number{80}, number{5});
		pico8::center("press button", number{80}, number{5});
		pico8::center("Maddy Thorson", number{96}, number{5});
		pico8::center("Noel Berry", number{104}, number{5});
	}

	if (level_index() == number{30})
	{
		// destroy_objects();
		for (auto &obj : objects)
		{
			if (obj.second->type == ObjType::Player)
			{
				auto diff = picomath::min(number{24}, number{40} - picomath::abs(obj.second->x + number{4} - number{64}));
				pico8::rectfill(number{0}, number{0}, diff, number{128}, number{0});
				pico8::rectfill(number{128} - diff, number{0}, number{128}, number{128}, number{0});
				break;
			}
		}
	}
	// draw_time(number{4},4); // debugging and number{30} fps test // ADDITION
}

// ***

// *** util

// todo check if these are already in picosystem::number

number clamp(number val, number a, number b)
{
	return picomath::max(a, picomath::min(b, val));
}

number appr(number val, number target, number amount)
{
	return (val > target ? picomath::max(val - amount, target) : picomath::min(val + amount, target));
}

bool maybe()
{
	return rnd(number{1}) < number{0.5};
}

bool solid_at(number x, number y, number w, number h)
{
	return tile_flag_at(x, y, w, h, number{0});
}

bool ice_at(number x, number y, number w, number h)
{
	return tile_flag_at(x, y, w, h, number{4});
}

bool tile_flag_at(number x, number y, number w, number h, number flag)
{
	for (auto i = picomath::max(number{0}, floor(x / number{8.0})); i <= picomath::min(number{15}, (x + w - number{1}) / number{8}); i += number{1})
		for (auto j = picomath::max(number{0}, floor(y / number{8.0})); j <= picomath::min(number{15}, (y + h - number{1}) / number{8}); j += number{1})
			if (pico8::fget(tile_at(i, j), flag))
				return true;
	return false;
}

number tile_at(number x, number y)
{
	return pico8::mget(room.x * number{16} + x, room.y * number{16} + y);
}

bool spikes_at(number x, number y, number w, number h, number xspd, number yspd)
{
	for (auto i = picomath::max(number{0}, floor(x / number{8.0})); i <= picomath::min(number{15}, (x + w - number{1}) / number{8}); i += number{1})
		for (auto j = picomath::max(number{0}, floor(y / number{8.0})); j <= picomath::min(number{15}, (y + h - number{1}) / number{8}); j += number{1})
		{
			auto tile = tile_at(i, j);
			if (tile == number{17} && (mod((y + h - number{1}), number{8}) >= number{6} || y + h == j * number{8} + number{8}) && yspd >= number{0})
				return true;
			else if (tile == number{27} && mod(y, number{8}) <= number{2} && yspd <= number{0})
				return true;
			else if (tile == number{43} && mod(x, number{8}) <= number{2} && xspd <= number{0})
				return true;
			else if (tile == number{59} && (mod((x + w - number{1}), number{8}) >= number{6} || x + w == i * number{8} + number{8}) && xspd >= number{0})
				return true;
		}
	return false;
}

// *************************
// * PICO8SYSTEM ADDITIONS *
// *************************

static struct
{
	int x, state, target;
	void update()
	{
		if (this->target > this->x)
		{
			this->x += 1;
		}
		if (this->target > this->x + 4)
		{
			this->x += 1;
		}
		if (this->target < this->x)
		{
			this->x -= 1;
		}
		if (this->target < this->x - 4)
		{
			this->x -= 1;
		}
		viewportx = this->x;
	};
} cam = {
		.x = 5,
		.state = neutral,
		.target = 5};

// only move camera at all if camera is set to movable
int secondaryCamera()
{
	auto currentoffsets = leveloffsets[level_index().floor()];
	if (currentoffsets[2] == 1)
	{
		return cam.x;
	}
	else
	{
		return currentoffsets[0];
	}
}

enum setting
{
	none,
	off,
	on,
	strawberry,
	raspberry
};

static string settings_to_string(setting setting)
{
	switch (setting)
	{
	case none:
		return "";
	case off:
		return ": off";
	case on:
		return ": on";
	case strawberry:
		return ": strawberry";
	case raspberry:
		return ": raspberry";
	default:
		return "";
	}
}

typedef struct
{
	string text;
	setting selected;
	vector<setting> *settings;
	function<void()> a_button_action;
} menuentry;

static auto menupage = 0;
static auto selectedmenuindex = 0;
static auto menutime = 0;
static uint_fast8_t backlightlevel = 75;

static void return_to_game()
{
	backlightlevel = 75;
	led(0, 0, 0);
	picosystem::backlight(backlightlevel);
	blend(pico8::PALETTE);
	currentgamestate = game;
	spritesheet(pico8::celeste);
}

static void switch_to_menu()
{
	menutime = time();
	backlightlevel = 75;
	blend(picosystem::COPY);
	menupage = 0;
	selectedmenuindex = 0;
	currentgamestate = menu;
	spritesheet(pico8::mountain);
}

static auto menu1 = new vector<menuentry>{
		{.text = "resume",
		 .selected = none,
		 .settings = new vector<setting>{none},
		 .a_button_action = return_to_game},

		{.text = "return to title",
		 .selected = none,
		 .settings = new vector<setting>{none},
		 .a_button_action = init},

		{.text = "options",
		 .selected = none,
		 .settings = new vector<setting>{none},
		 .a_button_action = []()
		 { menupage = 1; selectedmenuindex = 0; }},

		{.text = "credits",
		 .selected = none,
		 .settings = new vector<setting>{none},
		 .a_button_action = []()
		 { menupage = 2; selectedmenuindex = 0; }}};
static auto menu2 = new vector<menuentry>{
		{.text = "back",
		 .selected = none,
		 .settings = new vector<setting>{none},
		 .a_button_action = []()
		 { 	pico8::writeSettingsToFlash();
		menupage = 0;
		selectedmenuindex = 0; }},

		{.text = "sound",
		 .selected = on,
		 .settings = new vector<setting>{off, on},
		 .a_button_action = []() {}},

		{.text = "fruit",
		 .selected = raspberry,
		 .settings = new vector<setting>{strawberry, raspberry},
		 .a_button_action = []() {}}};

static void restoresettings()
{
	if (pico8::sound == true)
	{
		menu2->at(1).selected = on;
	}
	if (pico8::sound == false)
	{
		menu2->at(1).selected = off;
	}
	if (pico8::berries == 0)
	{
		menu2->at(2).selected = strawberry;
		pico8::high_color_mode = false;
	}
	if (pico8::berries == 1)
	{
		menu2->at(2).selected = raspberry;
		pico8::high_color_mode = true;
	}
}

static void menu_update()
{
	if (button(A) || button(B) || button(X) || button(Y) || button(LEFT) || button(RIGHT) || button(UP) || button(DOWN))
	{
		menutime = time();
		backlightlevel = 75;
	}
	if (time() - menutime > 1000 * 20 && backlightlevel > 0)
	{
		backlightlevel--;
	}

	if (menu2->at(1).selected == on)
	{
		pico8::sound = true;
	}
	if (menu2->at(1).selected == off)
	{
		pico8::sound = false;
	}
	if (menu2->at(2).selected == strawberry)
	{
		pico8::berries = 0;
		pico8::high_color_mode = false;
	}
	if (menu2->at(2).selected == raspberry)
	{
		pico8::berries = 1;
		pico8::high_color_mode = true;
	}

	if (pressed(UP))
	{
		if (selectedmenuindex > 0)
		{
			selectedmenuindex--;
		}
	}
	else if (pressed(DOWN))
	{
		if (menupage == 0)
		{
			if (selectedmenuindex < menu1->size() - 1)
			{
				selectedmenuindex++;
			}
		}
		else if (menupage == 1)
		{
			if (selectedmenuindex < menu2->size() - 1)
			{
				selectedmenuindex++;
			}
		}
	}
	else if (pressed(LEFT))
	{
		menu2->at(selectedmenuindex).selected = menu2->at(selectedmenuindex).settings->front();
	}
	else if (pressed(RIGHT))
	{
		menu2->at(selectedmenuindex).selected = menu2->at(selectedmenuindex).settings->back();
	}
	else if (pressed(A))
	{
		if (menupage == 0)
		{
			menu1->at(selectedmenuindex).a_button_action();
		}
		else if (menupage == 1)
		{
			menu2->at(selectedmenuindex).a_button_action();
		}
		else if (menupage == 2)
		{
			menupage = 0;
			selectedmenuindex = 0;
		}
	}
	else if (pressed(B))
	{
		if (menupage == 0)
		{
			return_to_game();
		}
		else if (menupage == 1)
		{
			pico8::writeSettingsToFlash();
			menupage = 0;
			selectedmenuindex = 0;
		}
		else if (menupage == 2)
		{
			menupage = 0;
			selectedmenuindex = 0;
		}
	}
}

static void menu_draw(uint32_t tick)
{
	picosystem::backlight(backlightlevel);
	if (backlightlevel == 0)
	{								 // show the led to indicate that
		led(0, 15, 0); // picosystem is still on when screen is "off"
	}
	else
	{
		led(0, 0, 0);
	}
	target();
	picosystem::pen(0x00f0);
	clear();
	picosystem::sprite(2, 27, 72, 10, 7);
	picosystem::sprite(56, 19, 112, 1, 1);
	if (menupage == 0)
	{
		for (auto i = 0; i < menu1->size(); i++)
		{
			if (i == selectedmenuindex)
			{
				picosystem::pen(0xFFFF);
			}
			else
			{
				picosystem::pen(0x54F5);
			}
			text(menu1->at(i).text + settings_to_string(menu1->at(i).selected), 15, 15 + i * 10);
		}
	}
	else if (menupage == 1)
	{
		for (auto i = 0; i < menu2->size(); i++)
		{
			if (i == selectedmenuindex)
			{
				picosystem::pen(0xFFFF);
			}
			else
			{
				picosystem::pen(0x54F5);
			}
			text(menu2->at(i).text + settings_to_string(menu2->at(i).selected), 15, 15 + i * 10);
		}
	}
	else if (menupage == 2)
	{ // credits
		picosystem::pen(0x8df9);
		text("Celeste", 15, 15);
		text("Celeste", 16, 15);
		picosystem::pen(0x54F5);
		text("by Maddy Thorson", 15, 25);
		text("& Noel Berry", 15, 35);
		text("Picosystem version", 15, 50);
		text("by Pixelpunker", 15, 60);
		text("Uses", 15, 75);
		text("CCeleste/Lemon32767", 15, 85);
		text("Picotool/Sanderson", 15, 95);
		text("Picosystem SDK", 15, 105);
	}
}

void init()
{
	picomath::pico8_srand(rand()); // seed pico-8 random function with c+=number{1} random function
	pico8::init(true);
	restoresettings();
	currentgamestate = game;
	ClassicInit();
}

void update(uint32_t tick)
{
	if (hasexception == true)
	{
		return;
	}
	// try
	// {
	if (currentgamestate == game)
	{
		try
		{
			ClassicUpdate();
		}
		catch (...)
		{
			hasexception = true;
			std::exception_ptr p = std::current_exception();
			target();
			pen(0, 0, 0, 15);
			picosystem::frect(0, 39, 128, 21);
			pen(0, 15, 0, 15);
			text((p ? p.__cxa_exception_type()->name() : "null"), 0, 40);
			text("update", 0, 50);
			_flip();
			return;
		}
		auto direction = movetarget.findDirection(playerx);
		if (direction == direction::left)
		{
			movetarget.target1 = 0;
		}
		auto target = movetarget.findTarget(playerx);
		cam.x = target;
		cam.update();
	}
	else
	{
		menu_update();
	}

	if (pressed(Y) && !is_title())
	{
		if (currentgamestate == game)
		{
			switch_to_menu();
		}
		else
		{
			return_to_game();
		}
	}
	// }
	// catch (const std::exception &e)
	// {
	// 	errormessage = e.what();
	// 	error = true;
	// 	target();
	// 	pen(0, 15, 0, 15);
	// 	text(errormessage, 0, 40);
	// 	text("update", 0, 50);
	// 	_flip();
	// }
}

void draw(uint32_t tick)
{
	if (hasexception == true)
	{
		return;
	}
	if (currentgamestate == game)
	{
		blend(pico8::PALETTE);
		target(pico8::PICO8SCREEN);
		clip(0, 0, 128, 128);
		ClassicDraw();
		target();
		blend(pico8::CONVERT);
		viewportx = secondaryCamera();
		viewporty = leveloffsets[level_index().floor()][1];

		// blit(pico8::PICO8SCREEN, 4, 4, 120, 120, -10, -10, 120, 120);
		// blit(pico8::PICO8SCREEN, 4, 4, 120, 120, 10, 10, 110, 110);
		auto widthx = screenshakex.round() > 0 ? 120 - screenshakex.round() : 120;
		auto widthy = screenshakey.round() > 0 ? 120 - screenshakey.round() : 120;
		blit(pico8::PICO8SCREEN, viewportx, viewporty, 120 - screenshakex.round(), 120 - screenshakey.round(), screenshakex.round(), screenshakey.round(), widthx, widthy);
		blend(picosystem::COPY);
		target();
		pen(0, 0, 0, 15);
		text("obj:" + to_string(objects.size()), -1, 0);
		text("obj:" + to_string(objects.size()), 1, 0);
		pen(15, 15, 15, 15);
		text("obj:" + to_string(objects.size()), 0, 0);
		text(message2, 0, 10);
	}
	else
	{
		menu_draw(tick);
	}

	// Start Debug stats
	//

	// Debug saving
	/* 	text(to_string(pico8::flash_target_contents[0]), 60, 86);			 // DEBUG
		text(to_string(pico8::sound), 40, 96);								 // DEBUG
		text(to_string(pico8::berries), 40, 106);							 // DEBUG
		text("snd: " + to_string(pico8::flash_target_contents[1]), 60, 96);	 // DEBUG
		text("ber: " + to_string(pico8::flash_target_contents[2]), 60, 106); // DEBUG */

	// Debug FPS
	/* 	blend(picosystem::COPY); // DEBUG
		auto drawstat = stats.draw_us/100; // DEBUG
		auto updatestat = stats.update_us/100; // DEBUG
		auto combined = drawstat + updatestat; // DEBUG
	pen(15, 0, 0); // DEBUG

	text("fps: " + str(stats.fps), 60, 86); // DEBUG
	text("draw: " + str(drawstat), 60, 94); // DEBUG
	text("update: " + str(updatestat), 60, 102); // DEBUG
	text("combined: " + str(combined), 60, 110); // DEBUG */
}
