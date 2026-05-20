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

#if defined(__APPLE__)
#import "MacMenu.h"
#elif defined(_WIN64)
#include "WinMenu.h"
#endif

#include "main.h"

#include "AppWindows.h"
#include "FontSet.h"
#include "PopupState.h"
#include "Graphics.h"
#include "Budget.h"
#include "gameOptions.h"

#include "BudgetWindow.h"
#include "Evaluation.h"
#include "EvaluationWindow.h"
#include "GraphWindow.h"
#include "MiniMapWindow.h"
#include "QueryWindow.h"
#include "SavedGamesWindow.h"
#include "MessagesWindow.h"
#include "StatusWindow.h"

#include "CityProperties.h"
#include "Colors.h"
#include "Connection.h"
#include "Font.h"
#include "Graph.h"
#include "Map.h"
#include "Tool.h"

#include "g_ani.h"

#include "s_alloc.h"
#include "s_disast.h"
#include "s_fileio.h"
#include "s_gen.h"
#include "s_msg.h"
#include "s_sim.h"

#include "Scan.h"
#include "Sprite.h"

#include "w_sound.h"
#include "w_tk.h"
#include "w_update.h"
#include "w_util.h"

#include "Texture.h"
#include "ToolPalette.h"

#include "WindowGroup.h"
#include "WindowStack.h"
#include "AboutWindow.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "SDL_include.h"
#include SDL_INCLUDE_IMAGE
#include SDL_INCLUDE_TTF

#if defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#endif

const std::string MicropolisVersion{"4.0"};

// Simulation flags
bool Exit;
bool SimulationStep;
bool AnimationStep;
bool RedrawMinimap;
bool RightButtonDrag;

// ── SDL resource owners (RAII) ────────────────────────────────────────────────
// The raw-pointer externs (MainWindow, mainWindowRenderer) remain for
// compatibility with the rest of the codebase.  The unique_ptr owners live
// here and are reset explicitly in cleanUp() before SDL_Quit().
struct SdlWindowDeleter   { void operator()(SDL_Window*   w) const { SDL_DestroyWindow(w);   } };
struct SdlRendererDeleter { void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); } };

static std::unique_ptr<SDL_Window,   SdlWindowDeleter>   mainWindowOwner;
static std::unique_ptr<SDL_Renderer, SdlRendererDeleter> mainRendererOwner;
// ─────────────────────────────────────────────────────────────────────────────

SDL_Window*    MainWindow{nullptr};
SDL_Renderer*  mainWindowRenderer{nullptr};

uint32_t mainWindowId{};

Texture MainMapTexture{};

Texture BigTileset{};
Texture RCI_Indicator{};

int InitSimLoad;
int ScenarioID;

std::string resourcesPath;
std::string documentsPath;

#if defined(__APPLE__)
MacMenu* macMenu;
#elif defined(_WIN64)
std::unique_ptr<WinMenu> winMenu;
#endif

// Popup / notification state now lives in appWindows.popup (see PopupWindow.h).

constexpr auto RciValveHeight{20};
const int TileSize{16};
int renderTileSize{16};
int miniTileSize{3};

// Window creation dimensions — used by initRenderer() to avoid magic numbers
int defaultWindowWidth{1280};
int defaultWindowHeight{720};

float scale = -1.0f;
float ToolPaletteScale{1.0f};

// View state — grouped struct (see ViewState.h)
ViewState viewState{};

// UI layout rects — grouped struct (see UILayout.h)
UILayout uiLayout{};

std::string currentBudget{};

std::vector<const SDL_Rect*> UiRects{};

Budget budget{};
CityProperties cityProperties{};

// savedGamesWindow kept standalone: referenced by name in s_fileio.cpp
std::unique_ptr<SavedGamesWindow> savedGamesWindow = nullptr;

// All other windows owned here; destruction order is well-defined
AppWindows appWindows{};
// AppWindows destructor must see MiniMapWindow as a complete type
AppWindows::~AppWindows() = default;

// Font resources and shared string renderer
FontSet fonts{};

WindowStack guiWindowStack;
WindowGroup guiModalWindows;

// ── Platform helpers ──────────────────────────────────────────────────────────

#if defined(__APPLE__)
static void initPlatformPaths()
{
    macMenu = [[MacMenu alloc] init];

    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSError* error;

    NSString* docFolderPath = [[fileManager URLsForDirectory:NSApplicationDirectory inDomains:NSUserDomainMask] firstObject].path;
    NSLog(@"NSDocumentDirectory %@", docFolderPath);

    NSString* dstPath = [docFolderPath stringByAppendingPathComponent:@"cities"];
    NSLog(@"Cities path %@", dstPath);
    if ([fileManager fileExistsAtPath:dstPath] == NO)
    {
        NSString* srcPath = [[NSBundle mainBundle] pathForResource:@"cities" ofType:nil];
        [fileManager copyItemAtPath:srcPath toPath:dstPath error:&error];
    }

    resourcesPath = std::string([[[NSBundle mainBundle] resourcePath] UTF8String]);
    documentsPath = std::string([dstPath UTF8String]);
}
#else
static void initPlatformPaths()
{
    resourcesPath = ".";
    documentsPath = "cities";
}
#endif

#if defined(_WIN64)
static void initPlatformMenu() { winMenu = std::make_unique<WinMenu>(); }
#else
static void initPlatformMenu() {}
#endif

// ─────────────────────────────────────────────────────────────────────────────


void initRenderer()
{
    MainWindow = SDL_CreateWindow(
        "Micropolis Deluxe",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        defaultWindowWidth,
        defaultWindowHeight + PlatformHeightAdjust,
        SDL_WINDOW_RESIZABLE);

    if (!MainWindow)
    {
        throw std::runtime_error("initRenderer(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    mainWindowOwner.reset(MainWindow);

    SDL_SetWindowMinimumSize(MainWindow, defaultWindowWidth, defaultWindowHeight);

#if defined(__APPLE__)
    mainWindowRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#else
    mainWindowRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED);
#endif

    if (!mainWindowRenderer)
    {
        throw std::runtime_error("initRenderer(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    mainRendererOwner.reset(mainWindowRenderer);

    SDL_SetRenderDrawBlendMode(mainWindowRenderer, SDL_BLENDMODE_BLEND);

    mainWindowId = SDL_GetWindowID(MainWindow);

    MainMapTexture.texture = SDL_CreateTexture(mainWindowRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, SimWidth * 16, SimHeight * 16);
    MainMapTexture.dimensions = { SimWidth * 16, SimHeight * 16 };

}


void cleanUp()
{
    deinitTimers();

    SDL_DestroyTexture(BigTileset.texture);
    SDL_DestroyTexture(RCI_Indicator.texture);

    // Window and renderer are destroyed through their RAII owners
    mainRendererOwner.reset();
    mainWindowOwner.reset();
    mainWindowRenderer = nullptr;
    MainWindow = nullptr;
}


// ── GameLoop helpers ──────────────────────────────────────────────────────────

static void renderHomeScreen()
{
    guiWindowStack.bringToFront(appWindows.homeMiniMap.get());
    guiWindowStack.draw();
}

static void renderGameScreen()
{
    // Draw animated sprites onto the map texture
    SDL_SetRenderTarget(mainWindowRenderer, MainMapTexture.texture);
    drawSprites();
    SDL_SetRenderTarget(mainWindowRenderer, nullptr);

    // Optional earthquake shake
    SDL_Rect viewRect = viewState.fullMapViewRect;
    if (ShakeNow-- > 0)
    {
        viewRect.x += RandomRange(0, renderTileSize * 3) - renderTileSize * 3 / 2;
        viewRect.y += RandomRange(0, renderTileSize * 3) - renderTileSize * 3 / 2;
    }

    SDL_Rect viewport = FullMapViewportRect();
    SDL_RenderCopy(mainWindowRenderer, MainMapTexture.texture, &viewRect, &viewport);

    // Popup is drawn through the standard window stack — see PopupWindow.cpp.

    currentBudget = NumberToDollarDecimal(budget.CurrentFunds());
    pendingTool(appWindows.toolPalette->tool());

    if (!guiModalWindows.windowVisible())
    {
        if (!guiWindowStack.pointInWindow(EventHandling::MousePosition))
        {
            drawPendingTool(*appWindows.toolPalette);
            drawDraggableToolVector();
        }

        if (currentEvaluation().needsAttention)
        {
            appWindows.evaluation->setEvaluation(currentEvaluation());
            currentEvaluationSeen();
        }

        guiWindowStack.draw();

        // Scenario popups (id >= -8) pause the simulation; bottom-bar
        // notifications (id < -8) keep it running.
        if (!appWindows.popup || !appWindows.popup->suspendsSimulation())
        {
            simLoop(SimulationStep);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────


void GameLoop()
{
    drawBigMap();

    while (!Exit)
    {
        pumpEvents();

        SDL_SetRenderDrawColor(mainWindowRenderer, 223, 223, 223, 255);
        SDL_RenderClear(mainWindowRenderer);

        if (appWindows.home->visible())
            renderHomeScreen();
        else
            renderGameScreen();

        SDL_RenderPresent(mainWindowRenderer);

        newMap(false);
    }

    SDL_DestroyTexture(MainMapTexture.texture);
}


static void SDL_log_function(void* /*userdata*/, int category, SDL_LogPriority priority, const char* message)
{
    printf("[SDLLog %d %d] %s\n", category, priority, message);
}


int main(int argc, char* argv[])
{
    std::cout << "Starting Micropolis-SDL2 version " << MicropolisVersion << " originally by Will Wright and Don Hopkins." << std::endl;
    std::cout << "Original code Copyright (C) 2002 by Electronic Arts, Maxis. Released under the GPL v3" << std::endl;
    std::cout << "Modifications Copyright (C) 2022 - 2024 by Leeor Dicker. Available under the terms of the GPL v3" << std::endl << std::endl;
    std::cout << "Micropolis-SDL2 is not afiliated with Electronic Arts." << std::endl << std::endl;

    try
    {
        initPlatformPaths();

        SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
        if (SDL_Init(SDL_INIT_EVERYTHING))
        {
            throw std::runtime_error(std::string("Unable to initialize SDL: ") + SDL_GetError());
        }

#ifdef DEBUG
        SDL_LogSetOutputFunction(&SDL_log_function, nullptr);
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#else
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);
#endif

        initRenderer();
        loadGraphics();

        initViewParameters();
        initUI();
        // initViewParameters is called once: initUI() may reposition windows but
        // does not change tile size, so a second call is not needed.

        Pause();

        initPlatformMenu();

        gameInit();

        showWindowAndBringToFront(*appWindows.home.get());
        showWindowAndBringToFront(*appWindows.homeMiniMap.get());

        GameLoop();

        cleanUp();

        SDL_Quit();
    }
    catch (const std::exception& e)
    {
        std::string message(std::string(e.what()) + "\n\nMicropolis-SDL2PP will now close.");

#if defined(WIN32)
        MessageBoxA(nullptr, message.c_str(), "Micropolis-SDL2PP", MB_ICONERROR | MB_OK);
#else
        std::cout << message << std::endl;
#endif
    }

    return 0;
}
