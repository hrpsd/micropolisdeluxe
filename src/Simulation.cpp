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

#include "main.h"

#include "AppWindows.h"
#include "PopupState.h"
#include "Evaluation.h"
#include "gameOptions.h"
#include "Graph.h"

#include "g_ani.h"
#include "MiniMapWindow.h"
#include "HomeMiniMapWindow.h"
#include "s_alloc.h"
#include "s_disast.h"
#include "s_fileio.h"
#include "s_gen.h"
#include "s_msg.h"
#include "s_sim.h"
#include "Sprite.h"
#include "Tool.h"
#include "Map.h"

#include "w_sound.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <vector>

std::string openCityFile;

constexpr unsigned int SimStepDefaultTime{ 100 };
constexpr unsigned int AnimationStepDefaultTime{ 150 };

static std::array<unsigned int, 5> SpeedModifierTable{ 0, 0, 50, 75, 95 };
static std::vector<SDL_TimerID> Timers;


static unsigned int speedModifier()
{
    return SpeedModifierTable[static_cast<unsigned int>(SimSpeed())];
}

static unsigned int zonePowerBlinkTick(unsigned int interval, void*)
{
    toggleBlinkFlag();
    return interval;
}

static unsigned int redrawMiniMapTick(unsigned int interval, void*)
{
    RedrawMinimap = true;
    return interval;
}

static unsigned int simulationTick(unsigned int interval, void*)
{
    SimulationStep = true;
    return SimStepDefaultTime - speedModifier();
}

static unsigned int animationTick(unsigned int interval, void*)
{
    AnimationStep = true;
    return interval;
}

void initTimers()
{
    Timers.push_back(SDL_AddTimer(500, zonePowerBlinkTick, nullptr));
    Timers.push_back(SDL_AddTimer(1000, redrawMiniMapTick, nullptr));
    Timers.push_back(SDL_AddTimer(SimStepDefaultTime, simulationTick, nullptr));
    Timers.push_back(SDL_AddTimer(AnimationStepDefaultTime, animationTick, nullptr));
}

void deinitTimers()
{
    for (auto timer : Timers)
    {
        SDL_RemoveTimer(timer);
    }
}

static void showBudgetIfBudgetNeedsAttention()
{
    if (!gameOptions.mAutoBudget && budget.NeedsAttention())
    {
        showWindowAndBringToFront(*appWindows.budget);
    }
}


void simExit()
{
    Exit = true;
}


void simUpdate()
{
    updateDate();

    if (newMonth() && appWindows.graph->visible())
    {
        appWindows.graph->update();

        showBudgetIfBudgetNeedsAttention();
    }

    scoreDoer(cityProperties);

    appWindows.gameMiniMap->update();
}


void simLoop(bool doSim)
{
    if (doSim)
    {
        SimFrame(cityProperties, budget);
        SimulationStep = false;
    }

    if (AnimationStep)
    {
        AnimationStep = false;

        if (!Paused())
        {
            animateTiles();
            updateSprites();
        }

        const MPoint<int> begin{ viewState.mapOffset.x / renderTileSize, viewState.mapOffset.y / renderTileSize };
        const MPoint<int> end
        {
            std::clamp((viewState.mapOffset.x + viewState.windowSize.x) / renderTileSize + 1, 0, SimWidth),
            std::clamp((viewState.mapOffset.y + viewState.windowSize.y) / renderTileSize + 1, 0, SimHeight)
        };

        //drawBigMapSegment(begin, end);
        drawBigMapSegment(MPoint<int>{0, 0}, MPoint<int>{SimWidth, SimHeight});
    }

    if (RedrawMinimap && appWindows.gameMiniMap->visible())

    {
        //gameMiniMapWindow->draw();
        RedrawMinimap = false;
    }

    simUpdate();
}


void initWillStuff()
{
    RoadEffect = 32;
    PoliceEffect = 1000;
    FireEffect = 1000;
    cityScore(500);
    cityPopulation(-1);
    LastCityTime(-1);
    LastCityYear(1);
    LastCityMonth(0);
    pendingTool(Tool::None);
    MessageId(NotificationId::None, MPoint<int>{-1, -1});
    destroyAllSprites();
    DisasterEvent = 0;
    initMapArrays();
    DoNewGame();
}


void simInit()
{
    userSoundOn(gameOptions.mPlaySounds);

    ScenarioID = 0;
    StartingYear = 1900;
    AutoGotoMessageLocation(true);
    CityTime = 50;
    gameOptions.mDisasters = true;
    gameOptions.mAutoBulldoze = true;
    gameOptions.mAutoBudget= true;
    MessageId(NotificationId::None, MPoint<int>{-1, -1});
    ClearMes();
    SimSpeed(SimulationSpeed::Paused);
    changeEval();
    MessageLocation({-1, -1});

    InitSimLoad = 2;
    Exit = false;

    InitializeSound();
    StopEarthquake();
    ClearMap();
    initWillStuff();
    budget.CurrentFunds(5000);
    SetGameLevelFunds(0, cityProperties, budget);
    SimSpeed(SimulationSpeed::Paused);

    if (appWindows.popup)
    {
        appWindows.popup->gotoCoords(-1, -1);
    }
}

void generateNewTerrain()
{
    appWindows.home->mTextFieldContent = "NowHere";
    ClearMap();
    GenerateNewCity(cityProperties, budget);
}

static void doPlayNewCity(CityProperties& properties, Budget& budget)
{
    GenerateNewCity(properties, budget);
    Resume();
    SimSpeed(SimulationSpeed::Normal);
}

void playThisMap(int level) {
    SetGameLevelFunds(level, cityProperties, budget);
    Resume();
    SimSpeed(SimulationSpeed::Normal);
    appWindows.home->hide();
    appWindows.homeMiniMap->hide();
    appWindows.gameMiniMap->show();
}

static void doStartScenario(int scenario)
{
    Eval("UIStartScenario " + std::to_string(scenario));
}


static void primeGame(const int startFlag)
{
    appWindows.home->hide();
    appWindows.homeMiniMap->hide();
    appWindows.gameMiniMap->show();

    switch (startFlag)
    {
    case -2: // Load a city
      if (LoadCity("filename", cityProperties, budget, gameOptions))
        {
            break;
        }
        // If load fails, simply create a new city
        [[fallthrough]];

    case -1:
        cityProperties.GameLevel(0);
        appWindows.home->mTextFieldContent = "NowHere";
        doPlayNewCity(cityProperties, budget);
        break;

    case 0:
        throw std::runtime_error("Unexpected startup switch: " + std::to_string(startFlag));
        break;

    default: // scenario number
        doStartScenario(startFlag);
        break;
    }
}


void resetGame()
{
    simInit();
    primeGame(-1);
}


void newGame()
{
    resetGame();
    drawBigMap();
}

void openGame(std::string path)
{
    savedGamesWindow->hide();

    LoadCity(path, cityProperties, budget, gameOptions);
    openCityFile = path;
    appWindows.homeMiniMap->show();

    std::string cityName = path;

    size_t pos = cityName.find_last_of(pathSeparator);
    if (pos != std::string::npos) {
        cityName = cityName.substr(pos + 1);
    }

    pos = cityName.find(".");
    if (pos != std::string::npos) {
        cityName = cityName.substr(0, pos);
    }

    cityName[0] = std::toupper(cityName[0]);
    appWindows.home->mTextFieldContent = cityName;
}

void saveGame()
{
    std::string openCityFile = documentsPath + pathSeparator + cityProperties.CityName() + ".cty";
    SaveCity(openCityFile, cityProperties, budget, gameOptions);
}

void saveGameAs()
{
    //saveGame();
}


void gameInit()
{
    simInit();

    primeGame(-1);

    updateMapDrawParameters();
    initTimers();
}
