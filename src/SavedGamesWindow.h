//
//  SavedGamesWindow.h
//  Micropolis
//
//  Created by Toto on 20/07/2025.
//

#pragma once

#include "WindowBase.h"

#include "Font.h"
#include "StringRender.h"
#include "Texture.h"

#include <memory>
#include <map>

#include "SDL_include.h"


struct ZoneStats;


class SavedGamesWindow : public WindowBase
{
public:
    SavedGamesWindow() = delete;
    SavedGamesWindow(const SavedGamesWindow&) = delete;
    const SavedGamesWindow operator=(const SavedGamesWindow&) = delete;

    SavedGamesWindow(SDL_Renderer* renderer);

    void draw();
    void update() override {}

private:
    void onMouseDown(const MPoint<int>&) override;

private:
    std::unique_ptr<Font> mFont;
    std::unique_ptr<Font> mFontBold;
    std::unique_ptr<Font> mFontBoldItalic;

    SDL_Renderer* mRenderer{ nullptr };

    StringRender mStringRenderer;
    
    Texture mDownButton;
    Texture mUpButton;
    SDL_Rect mDownRect;
    SDL_Rect mUpRect;
    
    int rowOffset;
    int numCities;
    
    std::vector<SDL_Rect> mButtons;
    std::vector<std::string> mPaths;

};
