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

#include <string>
#include "gameOptions.h"

enum class Scenario
{
    Dullsville = 0,
    SanFransisco,
    Hamburg,
    Bern,
    Tokyo,
    Detroit,
    Boston,
    Rio
};

class Budget;
class CityProperties;

bool LoadCity(const std::string& filename, CityProperties&, Budget&, GameOptionsS&);
void LoadScenario(Scenario, CityProperties&, Budget&, GameOptionsS&);
void SaveCity(const std::string&, const CityProperties&, const Budget&, GameOptionsS&);
bool _load_file(const std::string filename);