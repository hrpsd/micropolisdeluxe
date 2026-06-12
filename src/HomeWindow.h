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

#include "WindowBase.h"

#include "Font.h"
#include "StringRender.h"
#include "Texture.h"
#include "PopupState.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "SDL_include.h"


class HomeWindow : public WindowBase
{
public:
    struct Options
    {
        bool mAutoBudget{ false };
        bool mAutoBulldoze{ true };
        bool mAutoGoto{ true };
        bool mDisastersEnabled{ true };
        bool mPlayMusic{ true };
        bool mPlaySound{ true };
    };

    using CallbackOptionsChanged = std::function<void(const Options&)>;
    using CallbackSignal = std::function<void(void)>;

public:
    HomeWindow() = delete;
    HomeWindow(const HomeWindow&) = delete;
    const HomeWindow operator=(const HomeWindow&) = delete;

    HomeWindow(SDL_Renderer* renderer);

    void optionsChangedConnect(CallbackOptionsChanged);
    void newGameCallbackConnect(CallbackSignal);
    void openGameCallbackConnect(CallbackSignal);
    void saveGameCallbackConnect(CallbackSignal);

    void setOptions(const Options&);

    void draw() override;
    void update() override {}

    // On-screen rect of the (letterboxed) background art within the home window.
    SDL_Rect backgroundRect() const;

    std::string mTextFieldContent;

    int lastPitch{ 0 };

private:
    // ── UI element types ──────────────────────────────────────────────────────

    /** A button that shows a hover-highlight texture and fires an action on click. */
    struct HighlightButton
    {
        std::string           textureName; ///< Base name without path/extension
        SDL_Rect              rect;        ///< Unscaled design-space rect (1200×900 canvas)
        bool                  hovered{ false };
        std::function<void()> onClick;     ///< nullptr = not yet implemented
    };

    /** One radio-style difficulty level entry. */
    struct DifficultyEntry
    {
        int      index;    ///< 0 = Easy, 1 = Medium, 2 = Hard
        SDL_Rect rect;
        bool     hovered{ false };
    };

    /** One scenario thumbnail tile. */
    struct ScenarioEntry
    {
        int      scenarioIndex; ///< 0-based index passed to openScenario()
        SDL_Rect rect;
        std::string name;
        bool     hovered{ false };
        
    };

    /** One options checkbox, linked to its Options field via pointer-to-member. */
    struct CheckBoxEntry
    {
        SDL_Rect        rect;
        bool Options::* fieldPtr; ///< Pointer to the corresponding bool in Options
    };

    // ─────────────────────────────────────────────────────────────────────────

    void initHighlightButtons();

    void onMouseDown(const MPoint<int>&) override;
    void onKeyDown(int32_t key) override;
    void onMouseMotion(const MPoint<int>&) override;

    void checkCheckboxesForClick(const MPoint<int>& point);

    void drawTextField();
    void handleTextFieldKey(int32_t key);

    Texture* getTexture(const std::string& name);

    void optionsChangedTrigger();

    SDL_Rect homeScaleRect(SDL_Rect srcRect);

    void showScenarioPitch(int id);
    void drawScenarioPitch();


private:
    SDL_Renderer* mRenderer{ nullptr };

    Texture mBackground;

    std::unordered_map<std::string, Texture> mTextureCache;

    // Indices 4 and 5 in mHighlightButtons are the left/right map-navigation arrows
    std::vector<HighlightButton> mHighlightButtons;
    std::vector<DifficultyEntry> mDifficultyEntries;
    std::vector<ScenarioEntry>   mScenarioEntries;
    std::vector<CheckBoxEntry>   mCheckBoxEntries;

    int mDifficulty{ 0 };
    int mCurrentMap{ 0 };

    CallbackOptionsChanged mOptionsChangedCallback;
    CallbackSignal mNewGameCallback;
    CallbackSignal mOpenGameCallback;
    CallbackSignal mSaveGameCallback;

    Options mOptions;
    PopupState pitch;
    
    // Editable text field at top-right of the home window (design-space rect on the 1200×900 canvas).
    SDL_Rect    mTextFieldRect{ 960, 28, 220, 32 };
    bool        mTextFieldFocused{ false };
    Uint32      mCaretBlinkStart{ 0 }; ///< SDL_GetTicks() when the caret's blink cycle began
};
