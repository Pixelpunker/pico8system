#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <unordered_map>
#include "pico8.cpp"
#include "celeste.hpp"
using namespace std;
using namespace picosystem;
using namespace picomath;

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

number playerx;

int viewportx = 0;
int viewporty = 0;

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
		this->x1 = (int)playerx;
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
class fall_floor;
class lifeup;
class fake_wall;
class smoke;
class orb;
void load_room(int x, int y);
void next_room();
void restart_room();
void title_screen();
void draw_player(shared_ptr<ClassicObject> obj, int djump);
void kill_player(shared_ptr<player> obj);
void draw_hair(shared_ptr<player_hair> hair, shared_ptr<player> obj, int facing, int djump);
void psfx(int num);
void break_fall_floor(shared_ptr<fall_floor> obj);
void draw_time(int x, int y);
void destroy_objects();
int tile_at(int x, int y);
bool ice_at(number x, number y, number w, number h);
bool solid_at(number x, number y, number w, number h);
bool spikes_at(number x, number y, int w, int h, number xspd, number yspd);
bool tile_flag_at(number x, number y, number w, number h, int flag);
bool maybe();
number appr(number val, number target, number amount);

// Additions
auto objectmasterindex = 0;
auto objectcount = 0;
vector<int> markedfordelete;
vector<int> markedparticlesfordelete;

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
	Point(int x, int y)
	{
		x = x;
		y = y;
	}
};

struct Vect
{
	number x;
	number y;

	Vect()
	{
		x = 0;
		y = 0;
	}
	Vect(number x, number y)
	{
		x = x;
		y = y;
	}
};

struct Rectangle
{
	int x, y, Width, Height;
	Rectangle(int x, int y, int Width, int Height)
	{
		x = x;
		y = y;
		Width = Width;
		Height = Height;
	}
};

Point room;
unordered_map<int, unique_ptr<ClassicObject>> objects;
int freeze;
int shake;
bool will_restart;
int delay_restart;
unordered_set<int> got_fruit;
bool has_dashed;
int sfx_timer;
bool has_key;
bool pause_player;
bool flash_bg;
int music_timer;
bool new_bg;

int k_left = 0;
int k_right = 1;
int k_up = 2;
int k_down = 3;
int k_jump = 5; // CHANGE was 4
int k_dash = 4; // CHANGE was 5

int frames;
int seconds;
int minutes;
int deaths;
int max_djump;
bool start_game;
int start_game_flash;

// ***

// *** effects

class Cloud
{
public:
	number x;
	number y;
	number spd;
	number w;
	Cloud(number x, number y, number spd, number w)
	{
		x = x;
		y = y;
		spd = spd;
		w = w;
		objectcount++;
	}
	~Cloud()
	{
		objectcount--;
	}
};
vector<shared_ptr<Cloud>> clouds;

class Particle
{
public:
	number x;
	number y;
	int s;
	number spd;
	number off;
	int c;
	int id;
	Particle(number x, number y, int s, number spd, number off, int c)
	{
		x = x;
		y = y;
		s = s;
		spd = spd;
		off = off;
		c = c;
		id = objectmasterindex;
		objectmasterindex++;
		objectcount++;
	}
	~Particle()
	{
		objectcount--;
	}
};
vector<shared_ptr<Particle>> particles;

class DeadParticle
{
public:
	number x;
	number y;
	int t;
	int id;
	Vect spd;
	DeadParticle(number x, number y, int t, Vect spd)
	{
		x = x;
		y = y;
		t = t;
		spd = spd;
		id = objectmasterindex;
		objectmasterindex++;
		objectcount++;
	}
	~DeadParticle()
	{
		objectcount--;
	}
};
unordered_map<int, shared_ptr<DeadParticle>> dead_particles;

// *** entry point

void ClassicInit()
{
	objects.clear();
	objects.reserve(30); // about 30 objects at the most
	room = Point(0, 0);
	freeze = 0;
	will_restart = false;
	delay_restart = 0;
	got_fruit.clear();
	has_dashed = false;
	sfx_timer = 0;
	has_key = false;
	pause_player = false;
	flash_bg = false;
	music_timer = 0;
	new_bg = false;

	frames = 0;
	seconds = 0;
	minutes = 0;
	deaths = 0;
	max_djump = 1;
	start_game = false;
	start_game_flash = 0;

	clouds.clear();
	for (int i = 0; i <= 16; i++)
	{
		clouds.push_back(make_shared<Cloud>(
				rnd(128),
				rnd(128),
				1 + rnd(4),
				32 + rnd(32)));
	}
	particles.clear();
	particles.reserve(50);
	for (int i = 0; i <= 32; i++)
	{
		particles.push_back(make_shared<Particle>(
				rnd(128),
				rnd(128),
				floor(rnd(5) / 4), // todo minimum size 2? was invisible in ccleste
				0.25f + rnd(5),
				rnd(1),
				6 + floor(0.5f + rnd(1))));
	}
	dead_particles.clear();

	title_screen();
}

void title_screen()
{
	got_fruit.clear();
	got_fruit.reserve(30);
	frames = 0;
	deaths = 0;
	max_djump = 1;
	start_game = false;
	start_game_flash = 0;
	pico8::music(40, 0, 7);
	load_room(7, 3);
}

void begin_game()
{
	frames = 0;
	seconds = 0;
	minutes = 0;
	music_timer = 0;
	start_game = false;
	pico8::music(0, 0, 7);
	load_room(0, 0);
}

int level_index()
{
	return room.x % 8 + room.y * 8;
}

bool is_title()
{
	return level_index() == 31;
}

// ***

// *** object functions

class ClassicObject
{
public:
	int id;
	ObjType type;
	bool collideable = true;
	bool solids = true;
	number spr;
	bool flipX;
	bool flipY;
	number x;
	number y;
	Rectangle hitbox = Rectangle(0, 0, 8, 8);
	Vect spd = Vect(0, 0);
	Vect rem = Vect(0, 0);
	ClassicObject()
	{
		objectmasterindex++;
		id = objectmasterindex;
		objectcount++;
	}
	virtual ~ClassicObject() { objectcount--; };

	virtual void init()
	{
	}

	virtual void update()
	{
	}

	virtual void draw()
	{
		if (spr > 0)
		{
			pico8::spr(spr, x, y, 1, 1, flipX, flipY);
		}
	}

	bool is_solid(int ox, int oy)
	{
		if (oy > 0 && !check<platform>(ox, 0) && check<platform>(ox, oy))
		{
			return true;
		}
		return solid_at(x + hitbox.x + ox, y + hitbox.y + oy, hitbox.Width, hitbox.Height) ||
					 check<fall_floor>(ox, oy) ||
					 check<fake_wall>(ox, oy);
	}

	bool is_ice(int ox, int oy)
	{
		return ice_at(x + hitbox.x + ox, y + hitbox.y + oy, hitbox.Width, hitbox.Height);
	}

	template <typename T>
	optional<T> collide(int ox, int oy)
	{
		optional<T> result;
		destroy_objects(); // destroy objects already marked for delete
		// we don't have reflection in embedded c++ while we would have it on the desktop
		// we therefore use the type enum to check if two classic objects are of the same subtype
		for (auto &other : objects)
		{
			if (other.second->type == type && other.second.get() != this && other.second->collideable &&
					other.second->x + other.second->hitbox.x + other.second->hitbox.Width > x + hitbox.x + ox &&
					other.second->y + other.second->hitbox.y + other.second->hitbox.Height > y + hitbox.y + oy &&
					other.second->x + other.second->hitbox.x < x + hitbox.x + hitbox.Width + ox &&
					other.second->y + other.second->hitbox.y < y + hitbox.y + hitbox.Height + oy)
				result.value() = *(static_cast<T *>(other.second.get()));
		}
		return result;
	}

	template <class T>
	bool check(int ox, int oy)
	{
		return collide<T>(ox, oy).has_value();
	}

	void move(number ox, number oy) // todo // check if i can reduce the speed with these move functions
	{
		int amount = 0;
		// [x] get move amount
		rem.x += ox;
		amount = floor(rem.x + 0.5f);
		rem.x -= amount;
		move_x(amount, 0);

		// [y] get move amount
		rem.y += oy;
		amount = floor(rem.y + 0.5f);
		rem.y -= amount;
		move_y(amount);
	}

	void move_x(int amount, int start) // todo // check if i can reduce the speed with these move functions
	{
		if (solids)
		{
			auto step = sign(amount);
			for (int i = start; i <= picomath::abs(amount); i++)
			{
				if (!is_solid(step, 0))
					x += step;
				else
				{
					spd.x = 0;
					rem.x = 0;
					break;
				}
			}
		}
		else
			x += amount;
	}

	void move_y(int amount) // todo // check if i can reduce the speed with these move functions
	{
		if (solids)
		{
			auto step = sign(amount);
			for (auto i = 0; i <= picomath::abs(amount); i++)
				if (!is_solid(0, step))
					y += step;
				else
				{
					spd.y = 0;
					rem.y = 0;
					break;
				}
		}
		else
			y += amount;
	}
};

template <class T>
T init_object(number x, number y, optional<int> tile = nullopt)
{
	objects.emplace(objectmasterindex + 1, unique_ptr<ClassicObject>(static_cast<ClassicObject *>(new T())));
	auto obj = static_cast<T *>(objects[objectmasterindex + 1].get());
	if (tile.has_value())
		obj->spr = tile.value();
	obj->x = (int)x;
	obj->y = (int)y;
	obj->init();
	return *obj;
}

void markfordelete(shared_ptr<ClassicObject> obj)
{
	markedfordelete.push_back(obj->id);
}

void markfordelete(shared_ptr<DeadParticle> particle)
{
	markedparticlesfordelete.push_back(particle->id);
}

void destroy_objects()
{
	for (auto todelete : markedfordelete)
	{
		objects.erase(todelete);
	}
	markedfordelete.clear();
}

void destroy_particles()
{
	for (auto todelete : markedparticlesfordelete)
	{
		dead_particles.erase(todelete);
	}
	markedparticlesfordelete.clear();
}

void psfx(int num)
{
	if (sfx_timer <= 0)
		pico8::sfx(num);
}

// *** objects

class smoke : public ClassicObject, enable_shared_from_this<smoke>
{
public:
	smoke()
	{
		type = ObjType::Smoke;
	}
	void init() override
	{
		spr = 29;
		spd.y = -0.1f;
		spd.x = 0.3f + rnd(0.2f);
		x += -1 + rnd(2);
		y += -1 + rnd(2);
		flipX = maybe(); // todo check is maybe a pico8 function?
		flipY = maybe(); // todo check is maybe a pico8 function?
		solids = false;
	}
	void update() override
	{
		spr += 0.2f;
		if (spr >= 32)
			markfordelete(shared_from_this());
	}
};

class lifeup : public ClassicObject, enable_shared_from_this<lifeup>
{
	int duration;
	number flash;

public:
	lifeup()
	{
		type = ObjType::Lifeup;
	}
	void init() override
	{
		spd.y = -0.25f;
		duration = 30;
		x -= 2;
		y -= 4;
		flash = 0;
		solids = false;
	}

	void update() override
	{
		duration -= 1;
		if (duration <= 0)
			markfordelete(shared_from_this());
	}

	void draw() override
	{
		flash += 0.5f;
		pico8::print("1000", x - 2, y, mod(7 + flash, 2));
	}
};

class player : public ClassicObject, enable_shared_from_this<player>
{
public:
	bool p_jump = false;
	bool p_dash = false;
	int grace = 0;
	int jbuffer = 0;
	int djump = 0;
	int dash_time = 0;
	int dash_effect_time = 0;
	Vect dash_target = Vect(0, 0);
	Vect dash_accel = Vect(0, 0);
	number spr_off = 0;
	bool was_on_ground = false;
	shared_ptr<player_hair> hair;
	player()
	{
		type = ObjType::Player;
	}
	void init() override
	{
		spr = 1;
		djump = max_djump;
		hitbox = Rectangle(1, 3, 6, 5);
	}

	void update() override
	{
		if (pause_player)
			return;
		auto input = pico8::btn(k_right) ? 1 : (pico8::btn(k_left) ? -1 : 0);

		// spikes collide
		if (spikes_at(x + hitbox.x, y + hitbox.y, hitbox.Width, hitbox.Height, spd.x, spd.y))
			kill_player(shared_from_this());

		// bottom death
		if (y > 128)
			kill_player(shared_from_this());

		auto on_ground = is_solid(0, 1);
		auto on_ice = is_ice(0, 1);

		// smoke particles
		if (on_ground && !was_on_ground)
			init_object<smoke>(x, y + 4);

		auto jump = pico8::btnp(k_jump) && !p_jump; // change addition, was btn(k_jump)
		p_jump = pico8::btn(k_jump);
		if (jump)
		{
			jbuffer = 4;
		}
		else if (jbuffer > 0)
		{
			jbuffer -= 1;
		}

		auto dash = pico8::btnp(k_dash) && !p_dash; // change addition, was btn(k_dash)
		p_dash = pico8::btn(k_dash);

		if (on_ground)
		{
			grace = 6;
			if (djump < max_djump)
			{
				psfx(54);
				djump = max_djump;
			}
		}
		else if (grace > 0)
			grace -= 1;

		dash_effect_time -= 1;
		if (dash_time > 0)
		{
			init_object<smoke>(x, y);
			dash_time -= 1;
			spd.x = appr(spd.x, dash_target.x, dash_accel.x);
			spd.y = appr(spd.y, dash_target.y, dash_accel.y);
		}
		else
		{
			// move
			auto maxrun = 1;
			auto accel = 0.6f;
			auto deccel = 0.15f;

			if (!on_ground)
				accel = 0.4f;
			else if (on_ice)
			{
				accel = 0.05f;
				if (input == (flipX ? -1 : 1)) // this it how it was in the pico-8 cart but is redundant?
					accel = 0.05f;
			}

			if (picomath::abs(spd.x) > maxrun)
				spd.x = appr(spd.x, sign(spd.x) * maxrun, deccel);
			else
				spd.x = appr(spd.x, input * maxrun, accel);

			// facing
			if (spd.x != 0)
				flipX = (spd.x < 0);

			// gravity
			auto maxfall = 2.0f;
			auto gravity = 0.21f;

			if (picomath::abs(spd.y) <= 0.15f)
				gravity *= 0.5f;

			// wall slide
			if (input != 0 && is_solid(input, 0) && !is_ice(input, 0))
			{
				maxfall = 0.4f;
				if (rnd(10) < 2)
					init_object<smoke>(x + input * 6, y);
			}

			if (!on_ground)
				spd.y = appr(spd.y, maxfall, gravity);

			// jump
			if (jbuffer > 0)
			{
				if (grace > 0)
				{
					// normal jump
					psfx(1);
					jbuffer = 0;
					grace = 0;
					spd.y = -2;
					init_object<smoke>(x, y + 4);
				}
				else
				{
					// wall jump
					auto wall_dir = (is_solid(-3, 0) ? -1 : (is_solid(3, 0) ? 1 : 0));
					if (wall_dir != 0)
					{
						psfx(2);
						jbuffer = 0;
						spd.y = -2;
						spd.x = -wall_dir * (maxrun + 1);
						if (is_ice(wall_dir * 3, 0))
							init_object<smoke>(x + wall_dir * 6, y);
					}
				}
			}

			// dash
			auto d_full = 5;
			auto d_half = d_full * 0.70710678118f;

			if (djump > 0 && dash)
			{
				init_object<smoke>(x, y);
				djump -= 1;
				dash_time = 4;
				has_dashed = true;
				dash_effect_time = 10;

				auto dash_x_input = (pico8::btn(k_left) ? -1 : (pico8::btn(k_right) ? 1 : 0)); // todo check again
				auto dash_y_input = (pico8::btn(k_up) ? -1 : (pico8::btn(k_down) ? 1 : 0));		 // todo check again

				if (dash_x_input != 0 && dash_y_input != 0)
				{
					spd.x = dash_x_input * d_half;
					spd.y = dash_y_input * d_half;
				}
				else if (dash_x_input != 0)
				{
					spd.x = dash_x_input * d_full;
					spd.y = 0;
				}
				else
				{
					spd.x = 0;
					spd.y = dash_y_input * d_full;
				}

				psfx(3);
				freeze = 2;
				shake = 6;
				dash_target.x = 2 * sign(spd.x);
				dash_target.y = 2 * sign(spd.y);
				dash_accel.x = 1.5f;
				dash_accel.y = 1.5f;

				if (spd.y < 0)
					dash_target.y *= 0.75f;
				if (spd.y != 0)
					dash_accel.x *= 0.70710678118f;
				if (spd.x != 0)
					dash_accel.y *= 0.70710678118f;
			}
			else if (dash && djump <= 0)
			{
				psfx(9);
				init_object<smoke>(x, y);
			}
		}

		// animation
		spr_off += 0.25f;
		if (!on_ground)
		{
			if (is_solid(input, 0))
				spr = 5;
			else
				spr = 3;
		}
		else if (pico8::btn(k_down))
			spr = 6;
		else if (pico8::btn(k_up))
			spr = 7;
		else if (spd.x == 0 || (!pico8::btn(k_left) && !pico8::btn(k_right)))
			spr = 1;
		else
			spr = 1 + mod((int)spr_off, 4);

		// next level
		if (y < -4 && level_index() < 30)
			next_room();

		// was on the ground
		was_on_ground = on_ground;
	}
	void draw() override
	{
		// clamp in screen
		if (x < -1 || x > 121)
		{
			x = clamp(x, (number)-1, (number)121);
			spd.x = 0;
		}
		playerx = this->x; // save current player position for Celeste additions; ADDITION

		draw_hair(hair, shared_from_this(), flipX ? -1 : 1, djump);
		draw_player(shared_from_this(), djump);
	}
};

class player_hair
{
public:
	struct node
	{
	public:
		number x;
		number y;
		number size;
		node()
		{
			x = 0;
			y = 0;
			size = 0;
		}
		node(number x, number y, number size)
		{
			x = x;
			y = y;
			size = size;
		}
	};

	node hair[5];

	player_hair(shared_ptr<ClassicObject> obj)
	{
		for (auto i = 0; i <= 4; i++)
			hair[i] = node(obj->x, obj->y, picomath::max(1, picomath::min(2, 3 - i)));
	};
};

// moved outside class player_hair because of circular definitions
void draw_hair(shared_ptr<player_hair> hair, shared_ptr<player> obj, int facing, int djump)
{
	auto c = (djump == 1 ? 8 : (djump == 2 ? (7 + floor(mod((frames / 3), 2)) * 4) : 12));
	auto last = Vect(obj->x + 4 - facing * 2, obj->y + (pico8::btn(k_down) ? 4 : 3));
	for (auto &h : hair->hair)
	{
		h.x += (last.x - h.x) / 1.5f;
		h.y += (last.y + 0.5f - h.y) / 1.5f;
		pico8::circfill(h.x, h.y, h.size, c);
		last = Vect(h.x, h.y);
	}
}

void kill_player(shared_ptr<player> obj)
{
	sfx_timer = 12;
	pico8::sfx(0);
	deaths += 1;
	shake = 10;
	markfordelete(obj);
	// Stats.Increment(Stat.PICO_DEATHS);

	dead_particles.clear();
	for (auto dir = 0; dir <= 7; dir++)
	{
		auto angle = (dir / 8.0);
		dead_particles.emplace(objectmasterindex + 1, make_shared<DeadParticle>(
																											obj->x + 4,
																											obj->y + 4,
																											10,
																											Vect(picomath::cos(angle) * 3, picomath::sin(angle + 0.5f) * 3)));
	}

	restart_room();
}

void draw_player(shared_ptr<ClassicObject> obj, int djump)
{
	auto spritePush = 0; // make global for 60FPS // todo 60FPS
	if (djump == 2)
	{
		if (floor(mod((frames / 3), 2)) == 0) // check here we could do slowmo // todo
			spritePush = 10 * 16;
		else
			spritePush = 9 * 16;
	}
	else if (djump == 0)
	{
		spritePush = 8 * 16;
	}

	pico8::spr(obj->spr + spritePush, obj->x, obj->y, 1, 1, obj->flipX, obj->flipY);
}

class player_spawn : public ClassicObject, enable_shared_from_this<player>
{
private:
	Vect target;
	int state;
	int delay;
	shared_ptr<player_hair> hair;

public:
	player_spawn()
	{
		type = ObjType::Player_Spawn;
	}
	void init() override
	{
		spr = 3;
		target = Vect(x, y);
		y = 128;
		spd.y = -4;
		state = 0;
		delay = 0;
		solids = false;
		hair = make_shared<player_hair>(shared_from_this());
		pico8::sfx(4);
	}
	void update() override
	{
		// jumping up
		if (state == 0)
		{
			if (y < target.y + 16)
			{
				state = 1;
				delay = 3;
			}
		}
		// falling
		else if (state == 1)
		{
			spd.y += 0.5f;
			if (spd.y > 0 && delay > 0)
			{
				spd.y = 0;
				delay -= 1;
			}
			if (spd.y > 0 && y > target.y)
			{
				y = target.y;
				spd = Vect(0, 0);
				state = 2;
				delay = 5;
				shake = 5;
				init_object<smoke>(x, y + 4);
				pico8::sfx(5);
			}
		}
		// landing
		else if (state == 2)
		{
			delay -= 1;
			spr = 6;
			if (delay < 0)
			{
				markfordelete(shared_from_this());
				auto player1 = init_object<player>(x, y);
				player1.hair = hair; // todo: wird hair kopiert oder habe ich hier ein leck? // todo: hair check & copy constructor für hair
			}
		}
	}
	void draw() override
	{
		draw_hair(hair, shared_from_this(), 1, max_djump);
		draw_player(shared_from_this(), max_djump);
	}
};

class fall_floor : public ClassicObject, enable_shared_from_this<fall_floor>
{
public:
	fall_floor()
	{
		type = ObjType::Fall_Floor;
	}
	int state = 0;
	bool solid = true;
	int delay = 0;
	void update() override
	{
		if (state == 0)
		{
			if (check<player>(0, -1) || check<player>(-1, 0) || check<player>(1, 0))
				break_fall_floor(shared_from_this());
		}
		else if (state == 1)
		{
			delay -= 1;
			if (delay <= 0)
			{
				state = 2;
				delay = 60; // how long it hides for
				collideable = false;
			}
		}
		else if (state == 2)
		{
			delay -= 1;
			if (delay <= 0 && !check<player>(0, 0))
			{
				psfx(7);
				state = 0;
				collideable = true;
				init_object<smoke>(x, y);
			}
		}
	}
	void draw() override
	{
		if (state != 2)
		{
			if (state != 1)
				pico8::spr((number)23, x, y);
			else
				pico8::spr((number)(23 + (15 - delay) / 5), x, y);
		}
	}
};

// todo alle markfordelete prüfen

class spring : public ClassicObject
{
public:
	int hide_in = 0;

private:
	int hide_for = 0;
	int delay = 0;

public:
	spring()
	{
		type = ObjType::Spring;
	}
	void update() override
	{
		if (hide_for > 0)
		{
			hide_for -= 1;
			if (hide_for <= 0)
			{
				spr = 18;
				delay = 0;
			}
		}
		else if (spr == 18)
		{
			auto hit = collide<player>(0, 0);
			if (hit.has_value() && hit.value().spd.y >= 0)
			{
				spr = 19;
				hit.value().y = y - 4;
				hit.value().spd.x *= 0.2f;
				hit.value().spd.y = -3;
				hit.value().djump = max_djump;
				delay = 10;
				init_object<smoke>(x, y);

				// breakable below us
				auto below = collide<fall_floor>(0, 1);
				if (below.has_value())
					break_fall_floor(make_shared<fall_floor>(below.value()));

				psfx(8);
			}
		}
		else if (delay > 0)
		{
			delay -= 1;
			if (delay <= 0)
				spr = 18;
		}

		// begin hiding
		if (hide_in > 0)
		{
			hide_in -= 1;
			if (hide_in <= 0)
			{
				hide_for = 60;
				spr = 0;
			}
		}
	}
};

void break_spring(spring obj)
{
	obj.hide_in = 15;
}

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
		offset = rnd(1.0);
		start = y;
		hitbox = Rectangle(-1, -1, 10, 10);
	}
	void update() override
	{
		if (spr == 22)
		{
			offset += 0.01;
			y = start + picomath::sin(offset) * 2;
			auto hit = collide<player>(0, 0);
			if (hit.has_value() && hit.value().djump < max_djump)
			{
				psfx(6);
				init_object<smoke>(x, y);
				hit.value().djump = max_djump;
				spr = 0;
				timer = 60;
			}
		}
		else if (timer > 0)
			timer -= 1;
		else
		{
			psfx(7);
			init_object<smoke>(x, y);
			spr = 22;
		}
	}
	void draw() override
	{
		if (spr == 22)
		{
			pico8::spr(13 + (int)mod((offset * 8), 3), x, y + 6);
			pico8::spr(spr, x, y);
		}
	}
};

void break_fall_floor(shared_ptr<fall_floor> obj)
{
	if (obj->state == 0)
	{
		psfx(15);
		obj->state = 1;
		obj->delay = 15; // how long until it falls
		init_object<smoke>(obj->x, obj->y);
		auto hit = obj->collide<spring>(0, -1);
		if (hit.has_value())
			break_spring(hit.value());
	}
}

class fruit : public ClassicObject, enable_shared_from_this<fruit>
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
		spr = 26;
		start = y;
		off = 0;
	}

	// todo enable stats

	void update() override
	{
		auto hit = collide<player>(0, 0);
		if (hit.has_value())
		{
			hit.value().djump = max_djump;
			sfx_timer = 20;
			pico8::sfx(13);
			got_fruit.insert(1 + level_index());
			init_object<lifeup>(x, y);
			markfordelete(shared_from_this());
			// Stats.Increment(Stat.PICO_BERRIES);
		}
		off += 1;
		y = start + picomath::sin(off / 40.0f) * 2.5f;
	}
};

class fly_fruit : public ClassicObject, enable_shared_from_this<fly_fruit>
{
	number start;
	bool fly = false;
	number step = 0.5f;
	number sfx_delay = 8;

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
		// fly away
		if (fly)
		{
			if (sfx_delay > 0)
			{
				sfx_delay -= 1;
				if (sfx_delay <= 0)
				{
					sfx_timer = 20;
					pico8::sfx(14);
				}
			}
			spd.y = appr(spd.y, -3.5f, 0.25f);
			if (y < -16)
				markfordelete(shared_from_this());
		}
		// wait
		else
		{
			if (has_dashed)
				fly = true;
			step += 0.05f;
			spd.y = picomath::sin(step) * 0.5f;
		}
		// collect
		auto hit = collide<player>(0, 0);
		if (hit.has_value())
		{
			hit.value().djump = max_djump;
			sfx_timer = 20;
			pico8::sfx(13);
			got_fruit.insert(1 + level_index());
			init_object<lifeup>(x, y);
			markfordelete(shared_from_this());
			// Stats.Increment(Stat.PICO_BERRIES);
		}
	}
	void draw() override
	{
		number off = 0.0f;
		if (!fly)
		{
			auto dir = picomath::sin(step);
			if (dir < 0)
				off = 1 + picomath::max(0, sign(y - start));
		}
		else
			off = mod((off + 0.25), 3.0);
		pico8::spr(45 + off, x - 6, y - 2, 1, 1, true, false);
		pico8::spr(spr, x, y);
		pico8::spr(45 + off, x + 6, y - 2);
	}
};

// todo: check if all floats are actually the pico8::number type

class fake_wall : public ClassicObject, enable_shared_from_this<fake_wall>
{
public:
	fake_wall()
	{
		type = ObjType::Fake_Wall;
	}
	void update() override
	{
		hitbox = Rectangle(-1, -1, 18, 18);
		auto hit = collide<player>(0, 0);
		if (hit.has_value() && hit.value().dash_effect_time > 0)
		{
			hit.value().spd.x = -sign(hit.value().spd.x) * 1.5f;
			hit.value().spd.y = -1.5f;
			hit.value().dash_time = -1;
			sfx_timer = 20;
			pico8::sfx(16);
			markfordelete(shared_from_this());
			init_object<smoke>(x, y);
			init_object<smoke>(x + 8, y);
			init_object<smoke>(x, y + 8);
			init_object<smoke>(x + 8, y + 8);
			init_object<fruit>(x + 4, y + 4);
		}
		hitbox = Rectangle(0, 0, 16, 16);
	}
	void draw() override
	{
		pico8::spr(64, x, y);
		pico8::spr(65, x + 8, y);
		pico8::spr(80, x, y + 8);
		pico8::spr(81, x + 8, y + 8);
	}
};

class key : public ClassicObject, enable_shared_from_this<key>
{
public:
	key()
	{
		type = ObjType::Key;
	}
	void update() override
	{
		auto was = floor(spr);
		spr = 9 + (picomath::sin(frames / 30.0f) + 0.5f) * 1;
		auto current = floor(spr);
		if (current == 10 && current != was)
			flipX = !flipX;
		if (check<player>(0, 0))
		{
			pico8::sfx(23);
			sfx_timer = 20;
			markfordelete(shared_from_this());
			has_key = true;
		}
	}
};

class chest : public ClassicObject, enable_shared_from_this<chest>
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
		x -= 4;
		start = x;
		timer = 20;
	}
	void update() override
	{
		if (has_key)
		{
			timer -= 1;
			x = start - 1 + rnd(3);
			if (timer <= 0)
			{
				sfx_timer = 20;
				pico8::sfx(16);
				init_object<fruit>(x, y - 4);
				markfordelete(shared_from_this());
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
		x -= 4;
		solids = false;
		hitbox.Width = 16;
		last = x;
	}
	void update() override
	{
		spd.x = dir * 0.65f;
		if (x < -16)
			x = 128;
		if (x > 128)
			x = -16;
		if (!check<player>(0, 0))
		{
			auto hit = collide<player>(0, -1);
			if (hit.has_value())
				hit.value().move_x((int)(x - last), 1);
		}
		last = x;
	}
	void draw() override
	{
		pico8::spr(11, x, y - 1);
		pico8::spr(12, x + 8, y - 1);
	}
};

class message : public ClassicObject
{
	number last = 0;
	number index = 0;

public:
	message()
	{
		type = ObjType::Message;
	}
	void draw() override
	{
		// todo check why text in level is unreadable
		string text = "-- celeste mountain --#this memorial to those# perished on the climb";
		if (check<player>(4, 0))
		{
			if (index < text.length())
			{
				index += 0.5f;
				if (index >= last + 1)
				{
					last += 1;
					pico8::sfx(35);
				}
			}

			auto off = Vect(8, 96);
			for (auto i = 0; i < index; i++)
			{
				if (text[i] != '#')
				{
					pico8::rectfill(off.x - 2, off.y - 2, off.x + 7, off.y + 6, 7);
					pico8::print("" + text[i], off.x, off.y, 0);
					off.x += 5;
				}
				else
				{
					off.x = 8;
					off.y += 7;
				}
			}
		}
		else
		{
			index = 0;
			last = 0;
		}
	}
};

class orb : public ClassicObject, enable_shared_from_this<orb>
{
public:
	orb()
	{
		type = ObjType::Orb;
	}
	void init() override
	{
		spd.y = -4;
		solids = false;
	}
	void draw() override
	{
		spd.y = appr(spd.y, 0, 0.5f);
		auto hit = collide<player>(0, 0);
		if (spd.y == 0 && hit.has_value())
		{
			music_timer = 45;
			pico8::sfx(51);
			freeze = 10;
			shake = 10;
			markfordelete(shared_from_this());
			max_djump = 2;
			hit.value().djump = 2;
		}

		pico8::spr(102, x, y);
		auto off = frames / 30.0;
		for (auto i = 0; i <= 7; i++)
			pico8::circfill(x + 4 + picomath::cos(off + i / 8.0f) * 8, y + 4 + picomath::sin(off + i / 8.0f) * 8, 1, 7);
	}
};

class big_chest : public ClassicObject
{
	int state = 0;
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
			x = x;
			y = y;
			h = h;
			spd = spd;
		}
	};

	vector<shared_ptr<particle>> particles;

public:
	big_chest()
	{
		type = ObjType::Big_Chest;
	}
	void init()
	{
		hitbox.Width = 16;
	}
	void draw() override
	{
		if (state == 0)
		{
			auto hit = collide<player>(0, 8);
			if (hit.has_value() && hit.value().is_solid(0, 1))
			{
				pico8::music(-1, 500, 7);
				pico8::sfx(37);
				pause_player = true;
				hit.value().spd.x = 0;
				hit.value().spd.y = 0;
				state = 1;
				init_object<smoke>(x, y);
				init_object<smoke>(x + 8, y);
				timer = 60;
				particles.clear();
			}
			pico8::spr(96, x, y);
			pico8::spr(97, x + 8, y);
		}
		else if (state == 1)
		{
			timer -= 1;
			shake = 5;
			flash_bg = true;
			if (timer <= 45 && particles.size() < 50)
			{
				particles.push_back(make_shared<particle>(
						1 + rnd(14),
						0,
						32 + rnd(32),
						8 + rnd(8)));
			}
			if (timer < 0)
			{
				state = 2;
				particles.clear();
				flash_bg = false;
				new_bg = true;
				init_object<orb>(x + 4, y + 4);
				pause_player = false;
			}
			for (auto &p : particles)
			{
				p->y += p->spd;
				pico8::rectfill(x + p->x, y + 8 - p->y, x + p->x + 1, picomath::min(y + 8 - p->y + p->h, y + 8), 7);
			}
		}

		pico8::spr(112, x, y + 8);
		pico8::spr(113, x + 8, y + 8);
	}
};

class flag : public ClassicObject
{
	number score = 0;
	bool show = false;

public:
	flag()
	{
		type = ObjType::Flag;
	}
	void init()
	{
		x += 5;
		score = got_fruit.size();

		// Stats.Increment(Stat.PICO_COMPLETES);
		// Achievements.Register(Achievement.PICO8);
	}
	void draw() override
	{
		spr = 118 + mod((frames / 5.0f), 3);
		pico8::spr(spr, x, y);
		if (show)
		{
			pico8::rectfill(32, 2, 96, 31, 0);
			pico8::spr(26, 55, 6);
			pico8::print("x" + to_string((int)score), 64, 9, 7);
			draw_time(49, 16);
			pico8::print("deaths:" + to_string((int)deaths), 48, 24, 7);
		}
		else if (check<player>(0, 0))
		{
			pico8::sfx(55);
			sfx_timer = 30;
			show = true;
		}
	}
};

class room_title : public ClassicObject, enable_shared_from_this<room_title>
{
	number delay = 5;

public:
	room_title()
	{
		type = ObjType::Room_Title;
	}
	void draw() override
	{
		delay -= 1;
		if (delay < -30)
			markfordelete(shared_from_this());
		else if (delay < 0)
		{
			pico8::rectfill(24, 58, 104, 70, 0);
			if (room.x == 3 && room.y == 1)
				pico8::print("old site", 48, 62, 7);
			else if (level_index() == 30)
				pico8::print("summit", 52, 62, 7);
			else
			{
				auto level = (1 + level_index()) * 100;
				pico8::print(level + "m", 52 + (level < 1000 ? 2 : 0), 62, 7);
			}

			draw_time(4, 4);
		}
	}
};

// ***

// *** room functions

void restart_room()
{
	will_restart = true;
	delay_restart = 15;
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
	has_dashed = false;
	has_key = false;

	// remove existing objects
	objects.clear();

	// current room
	room.x = x;
	room.y = y;

	// entities
	for (int tx = 0; tx <= 15; tx++)
	{
		for (int ty = 0; ty <= 15; ty++)
		{
			auto tile = pico8::mget(room.x * 16 + tx, room.y * 16 + ty);
			if (tile == 11)
				init_object<platform>(tx * 8, ty * 8).dir = -1;
			else if (tile == 12)
				init_object<platform>(tx * 8, ty * 8).dir = 1;
			else
			{
				if (tile == 1)
					init_object<player_spawn>(tx * 8, ty * 8, tile);
				else if (tile == 18)
					init_object<spring>(tx * 8, ty * 8, tile);
				else if (tile == 22)
					init_object<balloon>(tx * 8, ty * 8, tile);
				else if (tile == 23)
					init_object<fall_floor>(tx * 8, ty * 8, tile);
				else if (tile == 86)
					init_object<message>(tx * 8, ty * 8, tile);
				else if (tile == 96)
					init_object<big_chest>(tx * 8, ty * 8, tile);
				else if (tile == 118)
					init_object<flag>(tx * 8, ty * 8, tile);
				else if (got_fruit.find(1 + level_index()) != got_fruit.end()) // todo check if this find really works
				{
					if (tile == 26)
						init_object<fruit>(tx * 8, ty * 8, tile);
					else if (tile == 28)
						init_object<fly_fruit>(tx * 8, ty * 8, tile);
					else if (tile == 64)
						init_object<fake_wall>(tx * 8, ty * 8, tile);
					else if (tile == 8)
						init_object<key>(tx * 8, ty * 8, tile);
					else if (tile == 20)
						init_object<chest>(tx * 8, ty * 8, tile);
				}
			}
		}
	}

	if (!is_title())
		init_object<room_title>(0, 0);

	// ADDITION
	movetarget.target1 = leveloffsets[level_index()][0];
	// END ADDITION
}

// *** update

void ClassicUpdate()
{
	frames = ((frames + 1) % 30);
	if (frames == 0 && level_index() < 30)
	{
		seconds = ((seconds + 1) % 60);
		if (seconds == 0)
			minutes++;
	}

	if (music_timer > 0)
	{
		music_timer -= 1;
		if (music_timer <= 0)
			pico8::music(10, 0, 7);
	}

	if (sfx_timer > 0)
		sfx_timer -= 1;

	// cancel if freeze
	if (freeze > 0)
	{
		freeze -= 1;
		return;
	}

	// screenshake
	if (shake > 0)
	{
		shake -= 1;
		pico8::camera();
		if (shake > 0)
			pico8::camera(-2 + rnd(5), -2 + rnd(5));
	}

	// restart(soon)
	if (will_restart && delay_restart > 0)
	{
		delay_restart -= 1;
		if (delay_restart <= 0)
		{
			will_restart = true;
			load_room(room.x, room.y);
		}
	}

	// clear deleted objects
	destroy_objects();
	// update each object
	for (auto &obj : objects)
	{
		obj.second->move(obj.second->spd.x, obj.second->spd.y);
		obj.second->update();
	}

	// start game
	if (is_title())
	{
		if (!start_game && (pico8::btnp(k_jump) || pico8::btnp(k_dash) || pressed(X) || pressed(Y))) // addition change was pico8::btn(k_jump) || pico8::btn(k_dash)
		{
			pico8::music(-1, 0, 0);
			start_game_flash = 50;
			start_game = true;
			pico8::sfx(38);
		}
		if (start_game)
		{
			start_game_flash -= 1;
			if (start_game_flash <= -30)
				begin_game();
		}
	}
}

// *** drawing

void draw_time(int x, int y)
{
	auto s = seconds;
	auto m = mod(minutes, 60);
	auto h = floor(minutes / 60);

	x += viewportx - 4; // addition
	y += viewporty - 4; // addition

	// pico8::rectfill(x, y, x + 32, y + 6, 0);
	// pico8::print((h < 10 ? "0" : "") + to_string(h) + ":" + (m < 10 ? "0" : "") + to_string(m) + ":" + (s < 10 ? "0" : "") + to_string(s), x + 1, y + 1, 7);

	// begin addition / change
	char str[27];
	snprintf(str, sizeof(str), "%.2i:%.2i:%.2i", h, m, s);
	pico8::rectfill(x, y, x + pico8::gettextwidth(str) - 6, y + 8, 0);
	pico8::print(str, x + 1, y + 1, 7);
	// end addition / change
}

void ClassicDraw()
{
	// reset all palette values
	pico8::pal();

	// start game flash
	if (start_game)
	{
		auto c = 10;
		if (start_game_flash > 10)
		{
			if (mod(frames, 10) < 5)
				c = 7;
		}
		else if (start_game_flash > 5)
			c = 2;
		else if (start_game_flash > 0)
			c = 1;
		else
			c = 0;

		if (c < 10)
		{
			pico8::pal(6, c);
			pico8::pal(12, c);
			pico8::pal(13, c);
			pico8::pal(5, c);
			pico8::pal(1, c);
			pico8::pal(7, c);
		}
	}

	// clear screen
	auto bg_col = 0;
	if (flash_bg)
		bg_col = frames / 5;
	else if (new_bg)
		bg_col = 2;
	pico8::cls(bg_col); // todo for 60fps 2. draw: reuse bg_col

	// clouds
	if (!is_title())
	{
		for (auto &c : clouds) // todo: check all (auto &item : objects) loops
		{
			c->x += c->spd;
			pico8::rectfill(c->x, c->y, c->x + c->w, c->y + 4 + (1 - c->w / 64) * 12, new_bg ? 14 : 1);
			if (c->x > 128)
			{
				c->x = -c->w;
				c->y = rnd(128 - 8);
			}
		}
	}

	// draw bg terrain
	pico8::map(room.x * 16, room.y * 16, 0, 0, 16, 16, 2);

	destroy_objects();
	// platforms / big chest
	for (auto &obj : objects)
	{
		if (obj.second->type == ObjType::Platform || obj.second->type == ObjType::Big_Chest) // todo class hierarchy issue
			obj.second->draw();
	}

	// draw terrain
	auto off = is_title() ? -4 : 0;
	pico8::map(room.x * 16, room.y * 16, off, 0, 16, 16, 1);

	// draw objects
	destroy_objects();
	for (auto &obj : objects)
	{
		if (obj.second->type == ObjType::Platform && !(obj.second->type == ObjType::Big_Chest)) // todo class hierarchry issue
			obj.second->draw();
	}

	// draw fg terrain
	pico8::map(room.x * 16, room.y * 16, 0, 0, 16, 16, 3);

	// particles
	for (auto &p : particles)
	{
		p->x += p->spd;
		p->y += picomath::sin(p->off);
		p->off += picomath::min(0.05f, p->spd / 32);
		pico8::rectfill(p->x, p->y, p->x + p->s, p->y + p->s, p->c);
		if (p->x > 128 + 4)
		{
			p->x = -4;
			p->y = rnd(128);
		}
	}

	// dead particles
	for (auto &p : dead_particles)
	{
		p.second->x += p.second->spd.x;
		p.second->y += p.second->spd.y;
		p.second->t -= 1;
		if (p.second->t <= 0)
		{
			markfordelete(p.second); // todo: check not sure if should be drawn anyway
		}
	}
	destroy_particles();
	for (auto &p : dead_particles)
	{
		pico8::rectfill(p.second->x - p.second->t / 5, p.second->y - p.second->t / 5,
										p.second->x + p.second->t / 5, p.second->y + p.second->t / 5, 14 + mod(p.second->t, 2));
	}

	// draw outside of the screen for screenshake
	pico8::rectfill(-5, -5, -1, 133, 0);
	pico8::rectfill(-5, -5, 133, -1, 0);
	pico8::rectfill(-5, 128, 133, 133, 0);
	pico8::rectfill(128, -5, 133, 133, 0);

	// C# Change: "press button" instead to fit consoles
	// no need for credits here
	if (is_title())
	{
		// pico8::print("press button", 42, 96, 5);
		// pico8::print("matt thorson", 42, 96, 5);
		// pico8::print("noel berry", 46, 102, 5);
		// pico8::center("A+B", 80, 5);
		pico8::center("press button", 80, 5);
		pico8::center("Maddy Thorson", 96, 5);
		pico8::center("Noel Berry", 104, 5);
	}

	if (level_index() == 30)
	{
		destroy_objects();
		for (auto &obj : objects)
		{
			if (obj.second->type == ObjType::Player)
			{
				auto diff = picomath::min(24, 40 - picomath::abs(obj.second->x + 4 - 64));
				pico8::rectfill(0, 0, diff, 128, 0);
				pico8::rectfill(128 - diff, 0, 128, 128, 0);
				break;
			}
		}
	}
	// draw_time(4,4); // debugging and 30 fps test // ADDITION
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
	return rnd(1) < 0.5f;
}

bool solid_at(number x, number y, number w, number h)
{
	return tile_flag_at(x, y, w, h, 0);
}

bool ice_at(number x, number y, number w, number h)
{
	return tile_flag_at(x, y, w, h, 4);
}

bool tile_flag_at(number x, number y, number w, number h, int flag)
{
	for (auto i = (int)picomath::max(0, floor(x / 8.0f)); i <= picomath::min(15, (x + w - 1) / 8); i++)
		for (auto j = (int)picomath::max(0, floor(y / 8.0f)); j <= picomath::min(15, (y + h - 1) / 8); j++)
			if (pico8::fget(tile_at(i, j), flag))
				return true;
	return false;
}

int tile_at(int x, int y)
{
	return pico8::mget(room.x * 16 + x, room.y * 16 + y);
}

bool spikes_at(number x, number y, int w, int h, number xspd, number yspd)
{
	for (auto i = (int)picomath::max(0, floor(x / 8.0f)); i <= picomath::min(15, (x + w - 1) / 8); i++)
		for (auto j = (int)picomath::max(0, floor(y / 8.0f)); j <= picomath::min(15, (y + h - 1) / 8); j++)
		{
			auto tile = tile_at(i, j);
			if (tile == 17 && (mod((y + h - 1), 8) >= 6 || y + h == j * 8 + 8) && yspd >= 0)
				return true;
			else if (tile == 27 && mod(y, 8) <= 2 && yspd <= 0)
				return true;
			else if (tile == 43 && mod(x, 8) <= 2 && xspd <= 0)
				return true;
			else if (tile == 59 && (mod((x + w - 1), 8) >= 6 || x + w == i * 8 + 8) && xspd >= 0)
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
	auto currentoffsets = leveloffsets[level_index()];
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
	pico8::init(true);
	restoresettings();
	currentgamestate = game;
	ClassicInit();
}

void update(uint32_t tick)
{
	if (currentgamestate == game)
	{
		ClassicUpdate();
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
}

void draw(uint32_t tick)
{
	if (currentgamestate == game)
	{
		blend(pico8::PALETTE);
		target(pico8::PICO8SCREEN);
		ClassicDraw();
		target();
		blend(pico8::CONVERT);
		viewportx = secondaryCamera();
		viewporty = leveloffsets[level_index()][1];
		blit(pico8::PICO8SCREEN, viewportx, viewporty, 120, 120, 0, 0);
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
