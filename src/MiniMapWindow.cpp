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
#include "MiniMapWindow.h"
#include "FontSet.h"

#include "BindFunction.h"
#include "Colors.h"
#include "EffectMap.h"
#include "Graphics.h"
#include "Map.h"

#include "w_util.h"

#include "SDL_include.h"
#include SDL_INCLUDE_IMAGE

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

float uiScale;

    constexpr auto VAL_NONE = 0;
    constexpr auto VAL_LOW = 1;
    constexpr auto VAL_MEDIUM = 2;
    constexpr auto VAL_HIGH = 3;
    constexpr auto VAL_VERYHIGH = 4;
    constexpr auto VAL_PLUS = 5;
    constexpr auto VAL_VERYPLUS = 6;
    constexpr auto VAL_MINUS = 7;
    constexpr auto VAL_VERYMINUS = 8;

    constexpr auto UNPOWERED = 0;
    constexpr auto POWERED = 1;
    constexpr auto CONDUCTIVE = 2;

    constexpr SDL_Color BackgroundColor{ 228, 228, 228, 255 };

    std::array<SDL_Color, 3> PowerColorTable =
    { {
        Colors::LightGrey,
        Colors::Red,
        Colors::LightGrey
    } };


    std::array<SDL_Color, 9> OverlayColorTable =
    { {
        Colors::Clear,
        Colors::LightGrey,
        Colors::Yellow,
        Colors::Orange,
        Colors::Red,
        Colors::DarkGreen,
        Colors::LightGreen,
        Colors::Orange,
        Colors::Yellow
    } };


    int rateOfGrowthColorIndex(int value)
    {
        if (value > 100)
        {
            return VAL_VERYPLUS;
        }

        if (value > 20)
        {
            return VAL_PLUS;
        }

        if (value < -100)
        {
            return VAL_VERYMINUS;
        }

        if (value < -20)
        {
            return VAL_MINUS;
        }

        return VAL_NONE;
    }


    int GetColorIndex(int x)
    {
        if (x < 50)
        {
            return VAL_NONE;
        }
        if (x < 100)
        {
            return VAL_LOW;
        }
        if (x < 150)
        {
            return VAL_MEDIUM;
        }
        if (x < 200)
        {
            return VAL_HIGH;
        }

        return VAL_VERYHIGH;
    }

    void MiniMapWindow::clearOverlayTexture(SDL_Renderer& renderer)
    {
        SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
        SDL_Rect bgRect = { area().x, area().y, miniTileSize * SimWidth, miniTileSize * SimHeight };
        SDL_RenderFillRect(&renderer, &bgRect);
    }

    void MiniMapWindow::drawOverlayPoints(SDL_Renderer& renderer, Texture& overlay, const EffectMap& map)
    {
        //turnOffBlending(renderer, overlay);
        //clearOverlayTexture(renderer);

        for (int x = 0; x < map.dimensions().x; x++)
        {
            for (int y = 0; y < map.dimensions().y; y++)
            {
                const auto& color = OverlayColorTable[GetColorIndex(map.value({ x, y }))];
                
                SDL_Rect pointRect { position().x + x * miniTileSize * SimWidth / map.mDimensions.x, position().y + y * miniTileSize * SimHeight / map.mDimensions.y, miniTileSize * SimWidth / map.mDimensions.x, miniTileSize * SimHeight / map.mDimensions.y };
                SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(&renderer, &pointRect);
            }
        }

        //turnOnBlending(renderer, overlay);
    }

void MiniMapWindow::setSize()
{
    size(MiniMapSize() + Vector<int>{0, mTitleBarArea.height + (int)(28.0f * uiScale)});

    uiScale = (float)miniTileSize / 3.0f;
    
    mMinimapArea = { position().x, position().y + mTitleBarArea.height, area().width, area().height - mTitleBarArea.height - mButtonArea.h };
    mButtonArea = { area().x, area().y + mMinimapArea.h + mTitleBarArea.height, mMinimapArea.w, (int)(28.0f * uiScale) };
    
    setButtonPositions();
}
/**
 * Constructs a minimap window
 *
 * \param position  Position to open the window at
 * \param size      Width/Height of the map in tiles
 */
MiniMapWindow::MiniMapWindow(SDL_Renderer* renderer, const MPoint<int>& pos, const Vector<int>& mapSize):
    mRenderer{ renderer },
    mStringRender(mRenderer)
{
    closeButtonActive(true);
    mTitle = "Mini map";
    size(MiniMapSize());
    position(pos);
    
    setSize();
        
    /*if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        std::cout << "MiniMapWindow::c'tor: SDL Video subsystem was not initialized. Initializing now." << std::endl;
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            throw std::runtime_error(std::string("MiniMapWindow(): Unable to initialize video subsystem: ") + SDL_GetError());
        }
    }

    mWindow = SDL_CreateWindow("Mini map",
        position.x, position.y,
        size.x * miniTileSize, size.y * miniTileSize + ButtonAreaHeight,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR);

    if (!mWindow)
    {
        throw std::runtime_error("MiniMapWindow(): Unable to create primary window: " + std::string(SDL_GetError()));
    }

    #if defined(__APPLE__)
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_SOFTWARE);
    #else
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    #endif
    
    if (!mRenderer)
    {
        throw std::runtime_error("MiniMapWindow(): Unable to create renderer: " + std::string(SDL_GetError()));
    }

    mWindowID = SDL_GetWindowID(mWindow);*/

    mTiles = loadTexture(mRenderer, "images/tilessm.png");
    mTexture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_TARGET, mapSize.x * miniTileSize, mapSize.y * miniTileSize);
    mButtonTextures = loadTexture(mRenderer, "icons/minimap.png");

    setButtonValues();
    setButtonTextureUv();
    setButtonPositions();
    resetOverlayButtons();

    initOverlayTextures();
}


/*MiniMapWindow::~MiniMapWindow()
{
    ///SDL_DestroyRenderer(mRenderer);
    //SDL_DestroyWindow(mWindow);
}*/


/*Uint32 MiniMapWindow::id() const
{
    return mWindowID;
}*/


void MiniMapWindow::focusOnMapCoordBind(fnPointIntParam fn)
{
    BindFuncPtr<std::vector<fnPointIntParam>, fnPointIntParam>(mFocusOnTileCallbacks, fn);
}


void MiniMapWindow::focusOnMapCoordUnbind(fnPointIntParam fn)
{
    UnbindFuncPtr<std::vector<fnPointIntParam>, fnPointIntParam>(mFocusOnTileCallbacks, fn);
}


void MiniMapWindow::updateMapViewPosition(SDL_Rect rect)
{
    mSelector = rect;
}


void MiniMapWindow::updateViewportSize(const Vector<int>& viewportSize)
{
    //mSelector.w = static_cast<int>(std::ceil(viewportSize.x / static_cast<float>(renderTileSize)) * miniTileSize);
    //mSelector.h = static_cast<int>(std::ceil(viewportSize.y / static_cast<float>(renderTileSize)) * miniTileSize);
}


void MiniMapWindow::updateTilePointedAt(const MPoint<int>& tilePointedAt)
{
    mTileHighlight.x = tilePointedAt.x * miniTileSize;
    mTileHighlight.y = tilePointedAt.y * miniTileSize;
}


void MiniMapWindow::linkEffectMap(ButtonId id, const EffectMap& map)
{
    mEffectMaps[id] = &map;
}


void MiniMapWindow::initTexture(Texture& texture, const Vector<int>& dimensions)
{
    texture.texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dimensions.x, dimensions.y);
    SDL_QueryTexture(texture.texture, nullptr, nullptr, &texture.dimensions.x, &texture.dimensions.y);
    texture.area = { 0, 0, texture.dimensions.x, texture.dimensions.y };
}


void MiniMapWindow::initOverlayTextures()
{
    const auto overlayHalfSize = MiniMapSize().skewInverseBy({ 2, 2 });
    initTexture(mOverlayTextures[ButtonId::Crime], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::LandValue], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::PopulationDensity], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::Pollution], overlayHalfSize);
    initTexture(mOverlayTextures[ButtonId::TrafficDensity], overlayHalfSize);
    
    // Necessary because of integer math dropping fractional component.
    const Vector<int> overlayEigthSize{ MiniMapSize().x / 8, (MiniMapSize().y / 8) + 1 };
    initTexture(mOverlayTextures[ButtonId::PoliceProtection], overlayEigthSize);
    initTexture(mOverlayTextures[ButtonId::FireProtection], overlayEigthSize);
    initTexture(mOverlayTextures[ButtonId::PopulationGrowth], overlayEigthSize);
    
    // Full size textures
    initTexture(mOverlayTextures[ButtonId::TransportationNetwork], MiniMapSize().skewBy({miniTileSize, miniTileSize}));
    initTexture(mOverlayTextures[ButtonId::PowerGrid], MiniMapSize().skewBy({miniTileSize, miniTileSize}));
    initTexture(mOverlayTextures[ButtonId::Residential], MiniMapSize().skewBy({ miniTileSize, miniTileSize }));
    initTexture(mOverlayTextures[ButtonId::Commercial], MiniMapSize().skewBy({ miniTileSize, miniTileSize }));
    initTexture(mOverlayTextures[ButtonId::Industrial], MiniMapSize().skewBy({ miniTileSize, miniTileSize }));

}


void MiniMapWindow::drawCurrentOverlay()
{
    if (mButtonDownId != ButtonId::Normal)
    {
        auto map = mEffectMaps[mButtonDownId];
        if (map)
        {
            drawOverlayPoints(*mRenderer, mOverlayTextures[mButtonDownId], *map);
        }
    }
}


void MiniMapWindow::resetOverlayButtons()
{
    for (auto& button : mButtons)
    {
        button.state = ButtonStateNormal;
    }

    mButtons[0].state = ButtonStatePressed;
}


/*void MiniMapWindow::hide()
{
    //SDL_HideWindow(mWindow);
    mHidden = true;
}


void MiniMapWindow::show()
{
    //SDL_RestoreWindow(mWindow);
    //SDL_ShowWindow(mWindow);
    mHidden = false;
}*/


/*bool MiniMapWindow::hidden() const
{
    return mHidden;
}*/

void MiniMapWindow::drawPlainMap()
{
    //SDL_SetRenderTarget(mRenderer, mTexture.texture);
        
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            SDL_Rect miniMapDrawRect = { area().x + row * miniTileSize, area().y + mTitleBarArea.height + col * miniTileSize, miniTileSize, miniTileSize };
            mTileRect.y = maskedTileValue(tileValue(row, col)) * 3;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }

    //SDL_RenderPresent(mRenderer);
    //SDL_SetRenderTarget(mRenderer, nullptr);

}


void MiniMapWindow::drawResidential()
{
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            SDL_Rect miniMapDrawRect = { area().x + row * miniTileSize, area().y + mTitleBarArea.height + col * miniTileSize, miniTileSize, miniTileSize };

            unsigned int tile = maskedTileValue(row, col);

            if (tile > 422)
            {
                tile = 0;
            }

            mTileRect.y = maskedTileValue(tile) * 3;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }
}


void MiniMapWindow::drawCommercial()
{
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            SDL_Rect miniMapDrawRect = { area().x + row * miniTileSize, area().y + mTitleBarArea.height + col * miniTileSize, miniTileSize, miniTileSize };

            unsigned int tile = maskedTileValue(row, col);

            if ((tile > 609) || ((tile >= 232) && (tile < 423)))
            {
                tile = 0;
            }

            mTileRect.y = maskedTileValue(tile) * 3;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }
}


void MiniMapWindow::drawIndustrial()
{
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            SDL_Rect miniMapDrawRect = { area().x + row * miniTileSize, area().y + mTitleBarArea.height + col * miniTileSize, miniTileSize, miniTileSize };

            unsigned int tile = maskedTileValue(row, col);

            if (((tile >= 240) && (tile <= 611)) ||
                ((tile >= 693) && (tile <= 851)) ||
                ((tile >= 860) && (tile <= 883)) ||
                (tile >= 932))
            {
                tile = 0;
            }

            mTileRect.y = maskedTileValue(tile) * 3;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }
}


void MiniMapWindow::drawPowerMap()
{
    SDL_Color tileColor{};
    
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            SDL_Rect miniMapDrawRect = { area().x + row * miniTileSize, area().y + mTitleBarArea.height + col * miniTileSize, miniTileSize, miniTileSize };

            const unsigned int unmaskedTile = tileValue(row, col);
            unsigned int tile = maskedTileValue(unmaskedTile);

            bool colored{ true };

            if (tile <= LASTFIRE)
            {
                colored = false;
            }
            else if (unmaskedTile & ZONEBIT)
            {
                tileColor = (unmaskedTile & PWRBIT) ? Colors::Red : Colors::LightBlue;
            }
            else
            {
                if (unmaskedTile & CONDBIT)
                {
                    tileColor = Colors::LightGrey;
                }
                else
                {
                    tile = DIRT;
                    colored = false;
                }
            }

            if (colored)
            {
                SDL_SetRenderDrawColor(mRenderer, tileColor.r, tileColor.g, tileColor.b, 255);
                SDL_RenderFillRect(mRenderer, &miniMapDrawRect);
            }
            else
            {
                mTileRect.y = maskedTileValue(tileValue(row, col)) * 3;
                SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
            }
        }
    }
}


void MiniMapWindow::drawLilTransMap()
{
    SDL_Rect miniMapDrawRect{ 0, 0, miniTileSize, miniTileSize };
    
    for (int row = 0; row < SimWidth; row++)
    {
        for (int col = 0; col < SimHeight; col++)
        {
            SDL_Rect miniMapDrawRect = { area().x + row * miniTileSize, area().y + mTitleBarArea.height + col * miniTileSize, miniTileSize, miniTileSize };

            unsigned int tile = maskedTileValue(row, col);

            if ((tile >= ResidentialBase) ||
                ((tile >= BRWXXX7) && tile <= 220) ||
                (tile == UNUSED_TRASH6))
            {
                tile = 0;
            }

            mTileRect.y = maskedTileValue(tile) * 3;
            SDL_RenderCopy(mRenderer, mTiles.texture, &mTileRect, &miniMapDrawRect);
        }
    }
}


void MiniMapWindow::draw()
{
    if (appWindows.home->visible())
    {
        drawPlainMap();
    }
    else
    {
        WindowBase::draw();
        
        size({MiniMapSize().x, MiniMapSize().y + mTitleBarArea.height + mButtonArea.h});
        
        setSize();
        
        setButtonPositions();
        
        switch (mButtonDownId)
        {
            case ButtonId::PowerGrid:
                drawPowerMap();
                break;
                
            case ButtonId::TransportationNetwork:
                drawLilTransMap();
                break;
                
            case ButtonId::Residential:
                drawResidential();
                break;
                
            case ButtonId::Commercial:
                drawCommercial();
                break;
                
            case ButtonId::Industrial:
                drawIndustrial();
                break;
                
            default:
                drawPlainMap();
                drawCurrentOverlay();
        }
        
        drawUI();
    }
}

void MiniMapWindow::drawUI()
{
    //SDL_RenderClear(mRenderer);

    //SDL_RenderCopy(mRenderer, mTexture.texture, nullptr, &mMinimapArea);

    if (mButtonDownId != ButtonId::Normal)
    {
        //SDL_RenderCopy(mRenderer, mOverlayTextures[mButtonDownId].texture, nullptr, &mMinimapArea);
    }
    
    SDL_Rect viewRect = { area().x + mSelector.x * miniTileSize, area().y + mTitleBarArea.height + mSelector.y * miniTileSize, mSelector.w * miniTileSize, mSelector.h * miniTileSize };
    
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(mRenderer, &viewRect);
    viewRect.x += 1; viewRect.y += 1; viewRect.w -= 2; viewRect.h -= 2;
    SDL_RenderDrawRect(mRenderer, &viewRect);
    SDL_RenderDrawRect(mRenderer, &mTileHighlight);

    const int arraySize = static_cast<int>(mButtons.size());
    for (int i{ 0 }; i < arraySize; ++i)
    {
        SDL_RenderCopy(mRenderer, mButtonTextures.texture, &mButtonUV[i + (mButtons[i].state * arraySize)], &mButtons[i].rect);
    }

    if (mHoveredButton != ButtonId::None) {
        std::string labels[] = {
            "Normal",
            "Land Value",
            "Crime",
            "Fire Protection",
            "Police Protection",
            "Population Density",
            "Population Growth",
            "Pollution",
            "Traffic Density",
            "Transportation Network",
            "Power Grid",
            "Residential",
            "Commercial",
            "Industrial"
        };
        
        std::string tooltipLabel = labels[(int)mHoveredButton];
        
        SDL_SetRenderDrawColor(mRenderer, 0xff, 0xff, 0xca, 255);
        SDL_Rect tooltipRect = {
            tooltipPosition.x,
            tooltipPosition.y + (int)roundf(15 * scale),
            fonts.body->size(tooltipLabel).x,
            (int)roundf(18 * scale)
        };
        SDL_RenderFillRect(mRenderer, &tooltipRect);
        SDL_SetRenderDrawColor(mRenderer, 0xff, 0x00, 0x00, 255);
        SDL_RenderDrawRect(mRenderer, &tooltipRect);
        
        SDL_SetTextureColorMod(fonts.body->texture(), 0, 0, 0);
        mStringRender.drawString(*fonts.body, tooltipLabel, tooltipPosition + Vector<int>{(int)roundf(4 * scale), (int)roundf(13 * scale)});
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    }
    
    //SDL_RenderPresent(mRenderer);
}

void MiniMapWindow::update()
{
    //size(MiniMapSize());
    mMinimapArea = { position().x, position().y + mTitleBarArea.height, area().width, area().height - mTitleBarArea.height - mButtonArea.h };
}

void MiniMapWindow::injectEvent(const SDL_Event& event)
{
    /*if (event.window.windowID != id())
    {
        return;
    }*/

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        handleMouseEvent(event);
        break;

    case SDL_WINDOWEVENT:
        handleWindowEvent(event);
        break;

    default:
        break;
    }
}


void MiniMapWindow::handleMouseEvent(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            const MPoint<int> point{ event.button.x, event.button.y };
            if (pointInRect(point, mMinimapArea))
            {
                mButtonDownInMinimapArea = true;
                float dx = fabsf((point.x - area().x) / (float)miniTileSize - mSelector.w / 2);
                mSelector.x = (int)roundf(std::clamp(dx, 0.f, fmaxf(0.f, (float)(SimWidth - mSelector.w))));
                
                float dy = fabsf((point.y - area().y - mTitleBarArea.height) / (float)miniTileSize - mSelector.h / 2);
                mSelector.y = (int)roundf(std::clamp(dy, 0.f, fmaxf(0.f, (float)(SimHeight - mSelector.h))));
                focusViewpoint();
            }
            else if (pointInRect(point, mButtonArea))
            {
                for (auto& button : mButtons)
                {
                    if (pointInRect(point, button.rect))
                    {
                        button.state = ButtonStatePressed;
                        mButtonDownId = button.id;

                        // fixme    Find a better way to do this
                        if (button.id == ButtonId::TransportationNetwork)
                        {
                            drawLilTransMap();
                        }
                        else if (button.id == ButtonId::PowerGrid)
                        {
                            drawPowerMap();
                        }
                        else if (button.id == ButtonId::Residential)
                        {
                            drawResidential();
                        }
                        else if (button.id == ButtonId::Commercial)
                        {
                            drawCommercial();
                        }
                        else if (button.id == ButtonId::Industrial)
                        {
                            drawIndustrial();
                        }
                        else
                        {
                            drawCurrentOverlay();
                        }
                    }
                    else
                    {
                        button.state = ButtonStateNormal;
                    }
                }

                if (noButtonsSelected())
                {
                    mButtons[0].state = ButtonStatePressed;
                    mButtonDownId = ButtonId::Normal;
                }
            }
        }
        break;

    case SDL_MOUSEMOTION:
        handleMouseMotion(event);
        break;

    case SDL_MOUSEBUTTONUP:
        mButtonDownInMinimapArea = false;
        break;

    default:
        break;
    }
}


void MiniMapWindow::handleWindowEvent(const SDL_Event& event)
{
    switch (event.window.event)
    {
    case SDL_WINDOWEVENT_MINIMIZED:
        hide();
        mButtonDownInMinimapArea = false;
        break;

    case SDL_WINDOWEVENT_FOCUS_LOST:
    case SDL_WINDOWEVENT_HIDDEN:
    case SDL_WINDOWEVENT_CLOSE:
        mButtonDownInMinimapArea = false;
        break;

    default:
        break;
    }
}


void MiniMapWindow::handleMouseMotion(const SDL_Event& event)
{
    const MPoint<int> point{ event.button.x, event.button.y };
    
    mHoveredButton = ButtonId::None;
    
    for (auto& button : mButtons)
    {
        if (pointInRect(point, button.rect))
        {
            tooltipPosition = point;
            tooltipPosition.y -= (int)roundf(32.0f * scale);
            mHoveredButton = button.id;
        }
    }
            
    if (mButtonDownInMinimapArea && event.motion.state & SDL_BUTTON_LMASK)
    {
        float dx = fmaxf(0, (point.x - area().x) / (float)miniTileSize - mSelector.w / 2);
        mSelector.x = (int)roundf(std::clamp(dx, 0.f, fmax(0.f, (float)((SimWidth + 1) - mSelector.w))));
        
        float dy = fmaxf(0, (point.y - area().y - mTitleBarArea.height) / (float)miniTileSize - mSelector.h / 2);
        mSelector.y = (int)roundf(std::clamp(dy, 0.f, fmax(0.f, (float)((SimHeight + 1) - mSelector.h))));
        
        focusViewpoint();
    }
}


void MiniMapWindow::focusViewpoint()
{
    for (auto callback : mFocusOnTileCallbacks)
    {
        callback({ mSelector.x, mSelector.y });
    }
}


void MiniMapWindow::setButtonValues()
{
    mButtons[0].id = ButtonId::Normal;
    mButtons[1].id = ButtonId::LandValue;
    mButtons[2].id = ButtonId::Crime;
    mButtons[3].id = ButtonId::FireProtection;
    mButtons[4].id = ButtonId::PoliceProtection;
    mButtons[5].id = ButtonId::PopulationDensity;
    mButtons[6].id = ButtonId::PopulationGrowth;
    mButtons[7].id = ButtonId::Pollution;
    mButtons[8].id = ButtonId::TrafficDensity;
    mButtons[9].id = ButtonId::TransportationNetwork;
    mButtons[10].id = ButtonId::PowerGrid;
    mButtons[11].id = ButtonId::Residential;
    mButtons[12].id = ButtonId::Commercial;
    mButtons[13].id = ButtonId::Industrial;
}


void MiniMapWindow::setButtonTextureUv()
{
    const int arraySize = static_cast<int>(mButtons.size());
    for (int i = 0; i < arraySize * 2; ++i)
    {
        mButtonUV[i] = { (i / arraySize) * 24, (i % arraySize) * 24, 24, 24 };
    }
}


void MiniMapWindow::setButtonPositions()
{
    Vector<int> buttonSize{ (int)(24.0f * uiScale), (int)(24.0f * uiScale) };
    Vector<int> buttonTransform { buttonSize.x + (int)(1.0f * uiScale), 0 };
    
    const int arraySize = static_cast<int>(mButtons.size());
    const int startPosition = mButtonArea.x + (mButtonArea.w - (buttonTransform.x * arraySize)) / 2;

    for (int i{ 0 }; i < arraySize; ++i)
    {
        mButtons[i].rect = { startPosition + buttonTransform.x * i, mButtonArea.y + (int)(3.0f * uiScale), buttonSize.x, buttonSize.y };
    }
}


bool MiniMapWindow::noButtonsSelected()
{
    for (auto& button : mButtons)
    {
        if (button.state == ButtonStatePressed)
        {
            return false;
        }
    }
    
    return true;
}
