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

#include "SDL_include.h"

class Budget;
class CityProperties;

enum class SimulationSpeed
{
	Paused = 0,
	Slow,
	Normal,
	Fast,
	AfricanSwallow
};

const std::string& SpeedString(SimulationSpeed speed);

void DoNewGame();
std::string NumberToDollarDecimal(int value);

void SimSpeed(SimulationSpeed speed);
SimulationSpeed SimSpeed();

void Pause();
void Resume();

bool Paused();

int CurrentYear();
void SetYear(int year);
void SetGameLevelFunds(int level, CityProperties& properties, Budget&);
bool CoordinatesValid(const MPoint<int>& position);
MPoint<int> PositionToCell(const MPoint<int>& position, const MPoint<int>& offset);
const Vector<int> vectorFromPoints(const MPoint<int>& start, const MPoint<int>& end);
bool pointInRect(const MPoint<int>& point, const SDL_Rect& rect);

int RandomRange(int min, int max);
int Random();
int Rand16();
