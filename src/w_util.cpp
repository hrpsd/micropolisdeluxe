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
#include "w_util.h"

#include "Budget.h"
#include "CityProperties.h"

#include "main.h"

#include "w_tk.h"
#include "w_update.h"

#include <algorithm>
#include <array>
#include <random>
#include <string>


namespace
{
    SimulationSpeed simulationSpeed;
    SimulationSpeed previousSimulationSpeed;

    std::array<std::string, 5> speedStringTable
    {
        "Paused", "Slow", "Normal", "Fast" , "African Swallow"
    };
};


const Vector<int> vectorFromPoints(const MPoint<int>& start, const MPoint<int>& end)
{
    return { end.x - start.x, end.y - start.y };
}


bool pointInRect(const MPoint<int>& MPoint, const SDL_Rect& rect)
{
    return MPoint.x >= rect.x && MPoint.x <= rect.x + rect.w && MPoint.y >= rect.y && MPoint.y <= rect.y + rect.h;
}


MPoint<int> PositionToCell(const MPoint<int>& position, const MPoint<int>& offset)
{
    return
    {
        (((position.x - MiniMapSize().x - (int)(118.0f * ToolPaletteScale)) + (offset.x % renderTileSize)) / renderTileSize),
        (((position.y) + (offset.y % renderTileSize)) / renderTileSize),
    };
}


bool CoordinatesValid(const MPoint<int>& position)
{
    return pointInRect(position, ValidMapCoordinates);
}


std::string NumberToDollarDecimal(int value)
{
    auto outString = std::to_string(value);

    int n = static_cast<int>(outString.length() - 3);
    const int end = (value >= 0) ? 0 : 1;

    while (n > end)
    {
        outString.insert(n, ",");
        n -= 3;
    }

    outString.insert(0, "$");

    return outString;
}


const std::string& SpeedString(SimulationSpeed speed)
{
    return speedStringTable[static_cast<int>(speed)];
}


void SimSpeed(SimulationSpeed speed)
{
    if (speed == SimulationSpeed::Paused)
    {
        previousSimulationSpeed = simulationSpeed;
        simulationSpeed = speed;
        return;
    }

    simulationSpeed = speed;
}


SimulationSpeed SimSpeed()
{
    return simulationSpeed;
}


void Pause()
{
    previousSimulationSpeed = simulationSpeed;
    SimSpeed(SimulationSpeed::Paused);
}


void Resume()
{
    SimSpeed(previousSimulationSpeed);
}


bool Paused()
{
    return simulationSpeed == SimulationSpeed::Paused;
}


void SetGameLevelFunds(int level, CityProperties& properties, Budget& budget)
{
    switch (level)
    {
    default:
    case 0:
        budget.CurrentFunds(20000);
        properties.GameLevel(0);
        break;

    case 1:
        budget.CurrentFunds(10000);
        properties.GameLevel(1);
        break;

    case 2:
        budget.CurrentFunds(5000);
        properties.GameLevel(2);
        break;
    }
}


void SetYear(int year)
{
    // Must prevent year from going negative, since it screws up the non-floored modulo arithmetic.
    if (year < StartingYear)
    {
        year = StartingYear;
    }

    year = (year - StartingYear) - (CityTime / 48);
    CityTime += year * 48;
    updateDate();
}


int CurrentYear()
{
    return (CityTime / 48 + StartingYear);
}


void DoNewGame()
{
  //Eval("UINewGame");
}


void DoGeneratedCityImage(char *name, int time, int pop, char* _class, int score)
{
  /* XXX: TODO: print city */
}


void DoStartElmd()
{
  /* XXX: TODO: start elm daemon */
}


void DoPopUpMessage(char *msg)
{
  /*
  char buf[1024];

  sprintf(buf, "UIPopUpMessage {%s}", msg);
  Eval(buf);
  */
}


static std::random_device RandomDevice;
static std::mt19937 PseudoRandomNumberGenerator(RandomDevice());


int RandomRange(int min, int max)
{
    std::uniform_int_distribution<std::mt19937::result_type> prngDistribution(min, max);
    return prngDistribution(PseudoRandomNumberGenerator);
}


int Random()
{
    return RandomRange(0, std::mt19937::max());
}


int Rand16()
{
    return RandomRange(0, 32767) * (RandomRange(0, 1) ? -1 : 1);
}
