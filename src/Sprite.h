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
#pragma once

#include "Point.h"
#include "Texture.h"
#include "Vector.h"

#include <string>
#include <vector>

struct SimSprite
{
	enum class Type
	{
		Train,
		Helicopter,
		Airplane,
		Ship,
		Monster,
		Tornado,
		Explosion
	};

	std::string name{};

	Type type{ 0 };
	int frame{ 0 };
	
	MPoint<int> position{};
	MPoint<int> origin{};
	MPoint<int> offset{};
	Vector<int> hot{};
	MPoint<int> destination{};

	Vector<int> size{};
	
	int count{ 0 };
	int mSoundCount{ 0 };
	int dir{ 0 };
	int mNewDir{ 0 };
	int step{ 0 };
	int flag{ 0 };
	int turn{ 0 };
	int accel{ 0 };
	int speed{ 0 };

	bool active{ false };

	std::vector<Texture> frames;
};


MPoint<int>& crashPosition();
void crashPosition(const MPoint<int>& position);

SimSprite* getSprite(SimSprite::Type type);
void drawSprites();
void destroyAllSprites();
void updateSprites();

void generateHelicopter(const MPoint<int>& position);
void generateAirplane(const MPoint<int>& position);
void generateShip();
void generateTrain(const MPoint<int>& position);

void makeExplosionAt(const MPoint<int>& position);
void generateExplosion(const MPoint<int>& position);
void generateTornado();
void generateMonster();
