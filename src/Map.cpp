// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#include "Map.h"

#include "AppWindows.h"
#include "s_alloc.h"

#include "Point.h"

#include "Texture.h"
#include "Sprite.h"

#include "w_util.h"

#include <array>

#include "SDL_include.h"

extern SDL_Renderer* mainWindowRenderer;

extern Texture BigTileset;
extern Texture MainMapTexture;


std::array<std::array<int, SimHeight>, SimWidth> map;
std::vector<std::array<std::array<int, SimHeight>, SimWidth>> maps;

namespace
{
	SDL_Rect tileRect{ 0, 0, 16, 16 };
	bool flagBlink{ false };
};


void toggleBlinkFlag()
{
	flagBlink = !flagBlink;
}


void resetMap()
{
	for (int row = 0; row < SimWidth; ++row)
	{
		for (int col = 0; col < SimHeight; ++col)
		{
			map[row][col] = 0;
		}
	}
}


int& tileValue(const MPoint<int>& location)
{
	return tileValue(location.x, location.y);
}


int& tileValue(const int x, const int y)
{
	return map[x][y];
}


unsigned int maskedTileValue(const MPoint<int>& location)
{
	return maskedTileValue(location.x, location.y);
}


unsigned int maskedTileValue(const int x, const int y)
{
	return tileValue(x, y) & LOMASK;
}


unsigned int maskedTileValue(unsigned int tile)
{
	return tile & LOMASK;
}


bool tilePowered(const unsigned int tile)
{
	return tile & PWRBIT;
}

bool tileIsZoned(const unsigned int tile)
{
	return tile & ZONEBIT;
}

bool tileIsRoad(const MPoint<int> coordinates)
{
	if (!CoordinatesValid(coordinates))
	{
		return false;
	}

	const auto tile = maskedTileValue(coordinates);

	if (tile < ROADBASE || tile > LASTRAIL)
	{
		return false;
	}

	if ((tile >= POWERBASE) && (tile < RAILHPOWERV))
	{
		return false;
	}

	return true;
}

bool blink()
{
	return flagBlink;
}


/**
 * Assumes \c begin and \c end are in a valid range
 */
void drawBigMapSegment(const MPoint<int>& begin, const MPoint<int>& end)
{
	SDL_SetRenderTarget(mainWindowRenderer, MainMapTexture.texture);

	SDL_Rect drawRect{ begin.x, begin.y, 16, 16};
	unsigned int tile = 0;

	for (int row = begin.x; row < end.x; row++)
	{
		for (int col = begin.y; col < end.y; col++)
		{
			tile = tileValue(row, col);
			// Blink lightning bolt in unpowered zone center
			if (!appWindows.home->visible() && blink() && tileIsZoned(tile) && !tilePowered(tile))
			{
				tile = LIGHTNINGBOLT;
			}

			//drawRect = { row * drawRect.w, col * drawRect.h, drawRect.w, drawRect.h };
			const unsigned int masked = maskedTileValue(tile);
			tileRect =
			{
				(static_cast<int>(masked) % 32) * 16,
				(static_cast<int>(masked) / 32) * 16,
				16, 16
			};

			SDL_RenderCopy(mainWindowRenderer, BigTileset.texture, &tileRect, &drawRect);
			drawRect.y += drawRect.h;
		}
		drawRect.y = 0;
		drawRect.x += drawRect.w;
	}
	//SDL_RenderPresent(mainWindowRenderer);
	SDL_SetRenderTarget(mainWindowRenderer, nullptr);
}


void drawBigMap()
{
	drawBigMapSegment(MPoint<int>{0, 0}, MPoint<int>{SimWidth, SimHeight});
}
