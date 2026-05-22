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

#include "AppWindows.h"
#include "Budget.h"
#include "CityProperties.h"
#include "FontSet.h"
#include "PopupState.h"
#include "UILayout.h"
#include "ViewState.h"
#include "SavedGamesWindow.h"
#include "WindowGroup.h"
#include "WindowStack.h"
#include "w_util.h"
#include "WindowBase.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "SDL_include.h"

// MiniMapWindow.h includes main.h — use forward declaration to avoid circular include
class MiniMapWindow;
class GraphWindow;

 /* Constants */

constexpr int SimWidth = 120;
constexpr int SimHeight = 100;
constexpr SDL_Rect ValidMapCoordinates{ 0, 0, SimWidth - 1, SimHeight - 1 };

constexpr auto HalfWorldWidth = SimWidth / 2;
constexpr auto HalfWorldHeight = SimHeight / 2;

constexpr auto QuarterWorldWidth = SimWidth / 4;
constexpr auto QuarterWorldHeight = SimHeight / 4;

constexpr auto EighthWorldWidth = SimWidth / 8;
constexpr auto EighthWorldHeight = (SimHeight + 7) / 8;

constexpr auto HistoryLength = 120;
constexpr auto MiscHistoryLength = 240;


/* Status Bits */
#define PWRBIT    0x8000 /* 20 bit 15 */
#define CONDBIT    0x4000 /* 10 bit 14 */
#define BURNBIT    0x2000 /* 8 bit 13 */
#define BULLBIT    0x1000 /* 4 bit 12 */
#define ANIMBIT    0x0800 /* 2 bit 11 */
#define ZONEBIT    0x0400 /* 1 bit 10 */
#define ALLBITS    0xFC00 /* mask for upper 6 bits    */
#define LOMASK    0x03FF /* mask for low 10 bits    */

#define BLBNBIT        (BULLBIT+BURNBIT)
#define BLBNCNBIT    (BULLBIT+BURNBIT+CONDBIT)
#define BNCNBIT        (BURNBIT+CONDBIT)

extern const int TileSize;
extern int renderTileSize;
extern int miniTileSize;

extern int defaultWindowWidth;
extern int defaultWindowHeight;

// On Windows the OS adds ~20px; the window is created taller to compensate
#if defined(_WIN64)
constexpr int PlatformHeightAdjust{ 20 };
#else
constexpr int PlatformHeightAdjust{ 180 };
#endif


extern int CurrentTile; // unmasked tile value
extern int CurrentTileMasked; // masked tile value

extern int RoadTotal, RailTotal, FirePop;
extern int ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
extern int ResZPop, ComZPop, IndZPop, TotalZPop;
extern int HospPop, ChurchPop, StadiumPop;
extern int PolicePop, FireStPop;
extern int CoalPop, NuclearPop, PortPop, APortPop;
extern int NeedHosp, NeedChurch;
extern int CrimeAverage, PolluteAverage, LVAverage;

extern int StartingYear;
extern int CityTime;
extern int ScenarioID;
extern int ShakeNow;

extern int RoadEffect, PoliceEffect, FireEffect;

extern int DisasterEvent;
extern int DisasterWait;

extern int ResCap, ComCap, IndCap;
extern int RValve, CValve, IValve;
extern int PoweredZoneCount;
extern int UnpoweredZoneCount;

extern int ScoreType;
extern int ScoreWait;

extern int InitSimLoad;
extern int DoInitialEval;

extern SDL_Renderer* mainWindowRenderer;
extern SDL_Window* MainWindow;
extern uint32_t mainWindowId;

extern std::string resourcesPath;
extern std::string documentsPath;

// Textures
extern Texture RCI_Indicator;
extern Texture BigTileset;
extern Texture MainMapTexture;

// Game state
extern Budget budget;
extern CityProperties cityProperties;
extern std::string currentBudget;

// View state — grouped in ViewState struct (see ViewState.h / extern ViewState viewState)
// UI layout rects — grouped in UILayout struct (see UILayout.h / extern UILayout uiLayout)

// UiRects: list of UI panel rects used by hit-testing in MouseHandler.cpp
extern std::vector<const SDL_Rect*> UiRects;

// Simulation flags
extern bool Exit;
extern bool SimulationStep;
extern bool AnimationStep;
extern bool RedrawMinimap;
extern bool RightButtonDrag;

// Popup / notification window — see PopupWindow.h, accessed via appWindows.popup

// Scale
extern float ToolPaletteScale;
extern float scale;

// Window stack / group
extern WindowStack guiWindowStack;
extern WindowGroup guiModalWindows;

// savedGamesWindow kept standalone: s_fileio.cpp references it by name
extern std::unique_ptr<SavedGamesWindow> savedGamesWindow;

// All other windows grouped in AppWindows (see AppWindows.h)
// All font resources grouped in FontSet      (see FontSet.h)
// Popup/notification state in PopupState     (see PopupState.h)

// EventHandling namespace (defined in EventHandling.cpp)
namespace EventHandling
{
    extern MPoint<int> MouseDownPosition;
    extern MPoint<int> MouseClickPosition;
    extern MPoint<int> MousePosition;
    extern bool MouseLeftDown;
}

void toggleMiniMapVisibility();
void showBudgetWindow();
void showEvaluationWindow();
void showGraphWindow();
void showMessagesWindow();
void showAboutWindow();

void saveGame();
void saveGameAs();
void generateNewTerrain();
void playThisMap(int level);
void openGame(std::string path);
void mustGoto(int x, int y);

void initWillStuff();

void showWindowAndBringToFront(WindowBase& window);
void setSpeed(SimulationSpeed speed);

void simExit();
const MPoint<int>& viewOffset();


struct msgpr {
    const char* text;
    int messageId;
};

extern struct msgpr popMsgs[];

void showPopup(int messageId, bool force = false);

extern SDL_Rect FullMapViewportRect();

Vector<int> MiniMapSize();

void onWindowHide(WindowBase *window);

void windowSize();

void windowResized(const Vector<int>& size);
void setRenderTileSize();

void drawThumbCityInRect(std::string path, SDL_Rect dest);

// Functions defined in Simulation.cpp
void simLoop(bool doSim);
void deinitTimers();
void gameInit();
void simInit();

// Functions defined in UISetup.cpp
void loadGraphics();
void initViewParameters();
void initUI();
void clampViewOffset();
void updateMapDrawParameters();
void drawPendingTool(const ToolPalette& palette);
void drawDraggableToolVector();
void centerWindow(WindowBase& window);

// Event handling — split across three files:
//   EventDispatcher.cpp  SDL event loop, window events, showWindowAndBringToFront
//   MouseHandler.cpp     mouse motion, button handling, tile mapping
//   KeyboardHandler.cpp  keyboard shortcuts, speed control, window visibility
void pumpEvents();

extern std::string openCityFile;

void optionsChanged();
