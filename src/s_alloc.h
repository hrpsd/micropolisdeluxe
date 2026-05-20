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

#include "EffectMap.h"
#include "main.h"
#include "Point.h"

#include <array>

enum class SearchDirection
{
	Up, Right, Down, Left, Undefined
};


extern MPoint<int> SimulationTarget;


// 2X2 maps  60 x 50
extern EffectMap PopulationDensityMap;
extern EffectMap TrafficDensityMap;
extern EffectMap PollutionMap;
extern EffectMap LandValueMap;
extern EffectMap CrimeMap;

// 4X4 maps  30 x 25
extern EffectMap TerrainMem;

/* 8X8 maps  15 x 13 */
extern EffectMap RateOfGrowthMap;
extern EffectMap FireStationMap;
extern EffectMap PoliceStationMap;

extern EffectMap PoliceProtectionMap;
extern EffectMap FireProtectionMap;

extern EffectMap ComRate;

using GraphHistory = std::array<int, 120>;

extern GraphHistory ResHis;
extern GraphHistory ComHis;
extern GraphHistory IndHis;
extern GraphHistory MoneyHis;
extern GraphHistory PollutionHis;
extern GraphHistory CrimeHis;
extern GraphHistory MiscHis;

extern GraphHistory ResHis120Years;
extern GraphHistory ComHis120Years;
extern GraphHistory IndHis120Years;
extern GraphHistory MoneyHis120Years;
extern GraphHistory PollutionHis120Years;
extern GraphHistory CrimeHis120Years;
extern GraphHistory MiscHis120Years;

extern int ResHisMax;
extern int ComHisMax;
extern int IndHisMax;

void initMapArrays();
bool moveSimulationTarget(SearchDirection direction);
