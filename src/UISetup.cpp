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
#include "Colors.h"
#include "Connection.h"
#include "Evaluation.h"
#include "gameOptions.h"
#include "g_ani.h"
#include "Map.h"
#include "MiniMapWindow.h"
#include "s_fileio.h"
#include "Scan.h"
#include "Tool.h"

#include "w_sound.h"
#include "w_util.h"
#include "s_msg.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#include "SDL_include.h"
#include SDL_INCLUDE_IMAGE
#include SDL_INCLUDE_TTF

#if defined(_WIN64)
#define strdup _strdup
#endif


static int MiniMapTileMultiplier()
{
    return (renderTileSize + miniTileSize - 1) / miniTileSize;
}

Vector<int> MiniMapSize()
{
    return Vector<int> {
        SimWidth * miniTileSize,
        SimHeight * miniTileSize
    };
}

SDL_Rect FullMapViewportRect()
{
    Vector<int> miniMapSize = MiniMapSize();

    SDL_Rect rect = {
        miniMapSize.x + (int)(118.0f * ToolPaletteScale),
        0,
        viewState.windowSize.x - miniMapSize.x - (int)(118.0f * ToolPaletteScale),
        viewState.windowSize.y
    };

    return rect;
}


void buildBigTileset()
{
    SDL_Surface* srcSurface{IMG_Load((resourcesPath + "/images/tiles.png").c_str())};
    SDL_Surface* dstSurface{SDL_CreateRGBSurface(srcSurface->flags,
        512, 512, 24,
        srcSurface->format->Rmask,
        srcSurface->format->Gmask,
        srcSurface->format->Bmask,
        srcSurface->format->Amask)};

    SDL_Rect srcRect{0, 0, TileSize, TileSize};
    SDL_Rect dstRect{0, 0, TileSize, TileSize};

    for (int i{0}; i < TILE_COUNT; ++i)
    {
        srcRect.y = i * TileSize;
        dstRect = {(i % (TileSize * 2)) * TileSize, (i / (TileSize * 2)) * TileSize, TileSize, TileSize};
        SDL_BlitSurface(srcSurface, &srcRect, dstSurface, &dstRect);
    }

    SDL_Texture* texture{SDL_CreateTextureFromSurface(mainWindowRenderer, dstSurface)};

    SDL_FreeSurface(srcSurface);
    SDL_FreeSurface(dstSurface);

    if (!texture)
    {
        const std::string message(std::string("buildBigTileset(): ") + SDL_GetError());
        std::cout << message << std::endl;
        throw std::runtime_error(message);
    }

    Vector<int> size{};
    SDL_QueryTexture(texture, nullptr, nullptr, &size.x, &size.y);

    BigTileset = { texture, SDL_Rect{ 0, 0, size.x, size.y }, { size.x, size.y } };
}


void loadGraphics()
{
    buildBigTileset();
    RCI_Indicator = loadTexture(mainWindowRenderer, "images/demandg.png");
}

void updateMapDrawParameters()
{
    viewState.fullMapViewRect =
    {
        viewState.mapOffset.x * 16 / renderTileSize,
        viewState.mapOffset.y * 16 / renderTileSize,
        //viewState.windowSize.x,
        //viewState.windowSize.y,
        FullMapViewportRect().w * 16 / renderTileSize,
        FullMapViewportRect().h * 16 / renderTileSize
    };

    SDL_Rect MiniMapViewRect =
    {
        viewState.mapOffset.x / renderTileSize,
        viewState.mapOffset.y / renderTileSize,
        //viewState.windowSize.x,
        //viewState.windowSize.y,
        FullMapViewportRect().w / renderTileSize + 1,
        FullMapViewportRect().h / renderTileSize + 1
    };


    appWindows.gameMiniMap->updateMapViewPosition(MiniMapViewRect);
}


void windowSize()
{
    SDL_GetWindowSize(MainWindow, &viewState.windowSize.x, &viewState.windowSize.y); // fills viewState.windowSize

    float prevScale{ scale };
    scale = std::min((float)viewState.windowSize.x / (float)defaultWindowWidth, (float)viewState.windowSize.y / (float)defaultWindowHeight);

    miniTileSize = (int)roundf(3.0f * scale);

    if (appWindows.home)
    {
        appWindows.home->position({ 0, 0 });
        appWindows.home->size(viewState.windowSize);

    }


    uiLayout.headerRect.x = (int)roundf(10 * scale);
    uiLayout.headerRect.y = (int)roundf(10 * scale);
    uiLayout.headerRect.w = MiniMapSize().x - (int)roundf(20 * scale);
    uiLayout.headerRect.h = (int)roundf(100 * scale);

    uiLayout.statusRect.x = (int)roundf(10 * scale);
    uiLayout.statusRect.y = uiLayout.headerRect.y + uiLayout.headerRect.h + (int)roundf(10 * scale);
    uiLayout.statusRect.w = MiniMapSize().x - (int)roundf(20 * scale);
    uiLayout.statusRect.h = (int)roundf(150 * scale);

    if (scale != prevScale)
    {
        fonts.body = std::make_unique<Font>("res/ZenKakuGothicAntique-Regular.ttf", (int)roundf(14 * scale));
        fonts.large = std::make_unique<Font>("res/ZenKakuGothicAntique-Regular.ttf", (int)roundf(24 * scale));
        fonts.caption = std::make_unique<Font>("res/ZenKakuGothicAntique-Medium.ttf", (int)roundf(12 * scale));
        fonts.title = std::make_unique<Font>("res/ZenKakuGothicAntique-Medium.ttf", (int)roundf(14 * scale));
        fonts.largeTitle = std::make_unique<Font>("res/ZenKakuGothicAntique-Medium.ttf", (int)roundf(24 * scale));
    }

    SDL_SetTextureColorMod(fonts.caption->texture(), 0, 0, 0);

    if (appWindows.gameMiniMap)
    {
        appWindows.gameMiniMap->position(MPoint{ 0, appWindows.messages->position().y + appWindows.messages->size().y });
        appWindows.gameMiniMap->setSize();
    }

    if (appWindows.popup && appWindows.popup->messageId() != 0)
    {
        appWindows.popup->showMessage(appWindows.popup->messageId(), true);
    }

    if (!(appWindows.home && appWindows.home->visible()) && prevScale > 0.0f)
    {
        guiWindowStack.reposition(prevScale, scale);
    }

    if (appWindows.toolPalette)
    {
        ToolPaletteScale = roundf(scale);
    }
}


void clampViewOffset()
{
    viewState.mapOffset =
    {
        std::clamp(viewState.mapOffset.x, 0, std::max(0, MainMapTexture.dimensions.x * renderTileSize / 16 - FullMapViewportRect().w)),
        std::clamp(viewState.mapOffset.y, 0, std::max(0, MainMapTexture.dimensions.y * renderTileSize / 16 - FullMapViewportRect().h))
    };
}


static void minimapViewUpdated(const MPoint<int>& newOffset)
{
    viewState.mapOffset = newOffset.skewBy({ renderTileSize, renderTileSize });
    clampViewOffset();
    updateMapDrawParameters();
}


void centerWindow(WindowBase& window)
{
    window.position({ viewState.windowSize.x / 2 - window.area().width / 2, viewState.windowSize.y / 2 - window.area().height / 2 });
}


void windowResized(const Vector<int>& size)
{
    windowSize();
    clampViewOffset();

    appWindows.gameMiniMap->updateViewportSize(Vector<int>{FullMapViewportRect().w, FullMapViewportRect().h});

    updateMapDrawParameters();
}


void initViewParameters()
{
    windowSize();

    uiLayout.headerRect.w = MiniMapSize().x - 20;
    uiLayout.headerRect.h = 100;

    uiLayout.statusRect.y = uiLayout.headerRect.y + uiLayout.headerRect.h + 10;
    uiLayout.statusRect.w = MiniMapSize().x - 20;
    uiLayout.statusRect.h = 150;

    uiLayout.rciDestination = { uiLayout.headerRect.x + 5, uiLayout.headerRect.y + fonts.body->height() + 10, RCI_Indicator.dimensions.x, RCI_Indicator.dimensions.y };

    uiLayout.residentialValve = { uiLayout.rciDestination.x + 9, uiLayout.rciDestination.y + 24, 4, 0 };
    uiLayout.commercialValve = { uiLayout.rciDestination.x + 18, uiLayout.rciDestination.y + 24, 4, 0 };
    uiLayout.industrialValve = { uiLayout.rciDestination.x + 27, uiLayout.rciDestination.y + 24, 4, 0 };
}

void drawPendingTool(const ToolPalette& palette)
{
    if (palette.tool() == Tool::None
        || (pendingToolProperties().draggable && EventHandling::MouseLeftDown)
        || !pointInRect(EventHandling::MousePosition, FullMapViewportRect()))
    {
        return;
    }

    const SDL_Rect toolRect
    {
        viewState.tileHighlight.x + MiniMapSize().x + (int)(118.0f * ToolPaletteScale) - (pendingToolProperties().offset * renderTileSize),
        viewState.tileHighlight.y  - (pendingToolProperties().offset * renderTileSize),
        pendingToolProperties().size * renderTileSize,
        pendingToolProperties().size * renderTileSize
    };

    if (palette.toolGost().texture)
    {
        SDL_RenderCopy(mainWindowRenderer, palette.toolGost().texture, &palette.toolGost().area, &toolRect);
        return;
    }

    SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 100);
    SDL_RenderFillRect(mainWindowRenderer, &toolRect);

    SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(mainWindowRenderer, &toolRect);
}


void drawDraggableToolVector()
{
    if (!EventHandling::MouseLeftDown) { return; }
    if (guiWindowStack.pointInWindow(EventHandling::MouseDownPosition))
    {
        return;
    }

    SDL_Rect toolRect
    {
        (toolStart().x * TileSize) - viewState.mapOffset.x,
        (toolStart().y * TileSize) - viewState.mapOffset.y,
        TileSize, TileSize
    };

    const int axis = longestAxis(viewState.draggableToolVector);
    const int size = (std::abs(axis) * TileSize) + TileSize;

    const bool xAxisLarger = std::abs(viewState.draggableToolVector.x) > std::abs(viewState.draggableToolVector.y);
    xAxisLarger ? toolRect.w = size : toolRect.h = size;

    if (axis < 0)
    {
        const int startValue = size - TileSize;
        xAxisLarger ? toolRect.x -= startValue : toolRect.y -= startValue;
    }

    SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 100);
    SDL_RenderFillRect(mainWindowRenderer, &toolRect);

    SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(mainWindowRenderer, &toolRect);
}


void optionsChanged()
{
    userSoundOn(gameOptions.mPlaySounds);
    if (gameOptions.mPlayMusic && !musicIsPlaying)
    {
        PlayMusic();
    }
    else if (!gameOptions.mPlayMusic && musicIsPlaying) {
        StopMusic();
    }
}


void initUI()
{
    MPoint<int> mainWindowPosition{};
    SDL_GetWindowPosition(MainWindow, &mainWindowPosition.x, &mainWindowPosition.y);

    SDL_DisplayMode mode{};
    SDL_GetDesktopDisplayMode(0, &mode);

    const MPoint<int> miniMapWindowPosition
    {
        //std::clamp(mainWindowPosition.x - (SimWidth * miniTileSize) - 10, 10, mode.w),
        //std::clamp(mainWindowPosition.y, 10, mode.h)
        533, 47
    };

    appWindows.homeMiniMap = std::make_unique<MiniMapWindow>(mainWindowRenderer, MPoint<int>{ 533, 47 }, Vector<int>{ SimWidth, SimHeight });

    appWindows.gameMiniMap = std::make_unique<MiniMapWindow>(mainWindowRenderer, MPoint<int>{ 0, 350 }, Vector<int>{ SimWidth, SimHeight });
    appWindows.gameMiniMap->updateViewportSize(Vector<int>{ FullMapViewportRect().w, FullMapViewportRect().h });
    appWindows.gameMiniMap->focusOnMapCoordBind(&minimapViewUpdated);

    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::Crime,             CrimeMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::FireProtection,    FireProtectionMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::LandValue,         LandValueMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::PoliceProtection,  PoliceProtectionMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::Pollution,         PollutionMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::PopulationDensity, PopulationDensityMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::PopulationGrowth,  RateOfGrowthMap);
    appWindows.gameMiniMap->linkEffectMap(MiniMapWindow::ButtonId::TrafficDensity,    TrafficDensityMap);

    fonts.renderer  = std::make_unique<StringRender>(mainWindowRenderer);

    appWindows.toolPalette = std::make_unique<ToolPalette>(mainWindowRenderer);
    appWindows.toolPalette->position({ MiniMapSize().x, 0 });

    appWindows.home = std::make_unique<HomeWindow>(mainWindowRenderer);
    centerWindow(*appWindows.home);
    appWindows.home->anchor();

    appWindows.budget = std::make_unique<BudgetWindow>(mainWindowRenderer, *fonts.renderer, budget);
    centerWindow(*appWindows.budget);

    appWindows.graph = std::make_unique<GraphWindow>(mainWindowRenderer);
    centerWindow(*appWindows.graph);

    appWindows.evaluation = std::make_unique<EvaluationWindow>(mainWindowRenderer);
    centerWindow(*appWindows.evaluation);

    appWindows.query = std::make_unique<QueryWindow>(mainWindowRenderer);
    centerWindow(*appWindows.query);

    savedGamesWindow = std::make_unique<SavedGamesWindow>(mainWindowRenderer);
    centerWindow(*savedGamesWindow);

    appWindows.about = std::make_unique<AboutWindow>();
    centerWindow(*appWindows.about);

    appWindows.saveAs = std::make_unique<SaveAsWindow>(mainWindowRenderer);
    centerWindow(*appWindows.saveAs);

    appWindows.popup = std::make_unique<PopupWindow>(mainWindowRenderer);
    appWindows.popup->hide();

    appWindows.status = std::make_unique<StatusWindow>();
    appWindows.status->position({ 0, 0 });
    
    appWindows.messages = std::make_unique<MessagesWindow>();
    appWindows.messages->position({ 0, appWindows.status->size().y });

    guiWindowStack.addWindow(appWindows.home.get());
    guiWindowStack.addWindow(appWindows.homeMiniMap.get());
    guiWindowStack.addWindow(appWindows.budget.get());
    guiWindowStack.addWindow(appWindows.evaluation.get());
    guiWindowStack.addWindow(appWindows.graph.get());
    guiWindowStack.addWindow(appWindows.toolPalette.get());
    guiWindowStack.addWindow(appWindows.query.get());
    guiWindowStack.addWindow(appWindows.gameMiniMap.get());
    guiWindowStack.addWindow(savedGamesWindow.get());
    guiWindowStack.addWindow(appWindows.about.get());
    guiWindowStack.addWindow(appWindows.saveAs.get());
    guiWindowStack.addWindow(appWindows.popup.get());
    guiWindowStack.addWindow(appWindows.messages.get());
    guiWindowStack.addWindow(appWindows.status.get());

    showWindowAndBringToFront(*appWindows.home.get());
    showWindowAndBringToFront(*appWindows.messages.get());
    showWindowAndBringToFront(*appWindows.status.get());

    UiRects.push_back(&uiLayout.headerRect);
}


void showPopup(int messageId, bool force)
{
    if (appWindows.popup)
    {
        appWindows.popup->showMessage(messageId, force);
    }
}

void mustGoto(int x, int y)
{
    printf("autoGoto %d %d\n", x, y);
    if (appWindows.popup)
    {
        appWindows.popup->gotoCoords(x, y);
    }
}

void onWindowHide(WindowBase *window)
{
    if (window == appWindows.about.get())
    {
        appWindows.homeMiniMap->show();
    }    
    if (window == appWindows.gameMiniMap.get())
    {
#if defined(__APPLE__)
        minimapMenuItem.state = NSControlStateValueOff;
#elif defined(_WIN64)
        minimapItemChecked = MF_UNCHECKED;
#endif
    }
    if (window == appWindows.budget.get())
    {
#if defined(__APPLE__)
        budgetMenuItem.state = NSControlStateValueOff;
#elif defined(_WIN64)
        budgetItemChecked = MF_UNCHECKED;
#endif
    }
    if (window == appWindows.evaluation.get())
    {
#if defined(__APPLE__)
        evaluationMenuItem.state = NSControlStateValueOff;
#elif defined(_WIN64)
        evaluationItemChecked = MF_UNCHECKED;
#endif
    }
    if (window == appWindows.graph.get())
    {
#if defined(__APPLE__)
        graphMenuItem.state = NSControlStateValueOff;
#elif defined(_WIN64)
        graphItemChecked = MF_UNCHECKED;
#endif
    }
}

void setRenderTileSize()
{
    initViewParameters();
    clampViewOffset();
    updateMapDrawParameters();
    drawBigMap();
}


void drawThumbCityInRect(std::string path, SDL_Rect dstRect)
{
    appWindows.homeMiniMap->hide();

    LoadCity(path, cityProperties, budget, gameOptions);

    printf("City path: %s name: %s\n", path.c_str(), cityProperties.CityName().c_str());

    int tmpTileSize{renderTileSize};
    renderTileSize = 1;

    drawBigMap();

    SDL_Rect srcRect = SDL_Rect { 0, 0, SimWidth * 16, SimHeight * 16 };
    SDL_RenderCopy(mainWindowRenderer, MainMapTexture.texture, &srcRect, &dstRect);

    dstRect.x--; dstRect.w += 2;
    dstRect.y--; dstRect.h += 2;
    SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(mainWindowRenderer, &dstRect);

    renderTileSize = tmpTileSize;

    //std::size_t pos{path.find_last_of("/")};
    //std::string filename{path.substr(pos + 1)};

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

    int stringWidth{fonts.body->size(cityName).x};
    fonts.renderer->drawString(*fonts.body, cityName, {dstRect.x + dstRect.w / 2 - stringWidth / 2, dstRect.y + dstRect.h});
}

// Popup rendering now lives in PopupWindow::draw() and is invoked via the
// regular window stack — no free-function drawPopup() shim is needed.
