#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include "hardware/flash.h"
#include "pico8.cpp"
#include "celeste.hpp"
using namespace picosystem;
using namespace picomath;

// #include <cstdint>
// #include <climits>

uint8_t layers[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 255};
auto layer = 0;
// #define FLASH_TARGET_OFFSET (256 * 1024 * 1024)
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES / 4 * 3 - FLASH_SECTOR_SIZE * 2)

const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

void init()
{
	pico8::init();
	// Your init code here
}

auto i = 0;
auto offsetx = 0;
auto offsety = 0;
uint8_t save_data[FLASH_PAGE_SIZE] = {1, 2, 3, 4};
bool saved = false;

void update(uint32_t tick)
{
	if (pressed(A))
	{
		layer = layers[i];
		i++;
		if (i > 9)
		{
			i = 0;
		}
	}
	if (pressed(LEFT))
	{
		offsetx -= 8;
	}
	if (pressed(RIGHT))
	{
		offsetx += 8;
	}
	if (pressed(UP))
	{
		offsety -= 8;
	}
	if (pressed(DOWN))
	{
		offsety += 8;
	}
	if (pressed(X))
	{
		uint32_t ints = save_and_disable_interrupts();
		flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE * 1);
		flash_range_program(FLASH_TARGET_OFFSET, save_data, FLASH_PAGE_SIZE * 1);
		restore_interrupts(ints);
	}
}

void draw(uint32_t tick)
{
	target(pico8::PICO8SCREEN);

	pico8::cls(12);
	picosystem::camera(offsetx, offsety);

	pico8::circfill(64, 64, 32, 11);
	pico8::map(0, 0, 0, 0, 128, 64, 0);
	pico8::rect(4, 4, 124, 124, 8);
	// pico8::map(0,0,4,4, 128, 64, layer); // kein garbage
	// pico8::map(0,0,0,0, 128, 64, layer); // garbage
	// auto i = 0;
	// for (auto y = 0; y < 128; y += 8)
	// {
	// 	for (auto x = 0; x < 128; x += 8)
	// 	{
	// 		pico8::spr(i, x, y, 1, 1, false, false);
	// 		i++;
	// 	}
	// }
	if (saved)
	{
		text("saved", 40, 30);
	}

	text(to_string(flash_target_contents[0]), 40, 40);
	text(to_string(flash_target_contents[1]), 40, 50);
	text(to_string(flash_target_contents[2]), 40, 60);
	target();
	// picosystem::clip(8,8,50,50);
	blit(pico8::PICO8SCREEN, 4, 4, 120, 120, 0, 0);
}