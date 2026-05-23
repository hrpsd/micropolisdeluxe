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
#include "HomeWindow.h"

#include "main.h"
#include "Map.h"
#include "MiniMapWindow.h"
#include "s_fileio.h"
#include "SDL_include.h"

#include <string>


// ── File-local constants and stateless helpers ────────────────────────────────
namespace
{
    constexpr SDL_Rect BgRect{ 0, 0, 1200, 900 };

    // Source rect within the background image that contains the checkbox tick mark
    constexpr SDL_Rect CheckedBox{ 261, 13, 14, 12 };

    template <typename BaseType>
    SDL_Point pointToSdlPoint(const MPoint<BaseType>& point)
    {
        return { point.x, point.y };
    }

    template <typename BaseType>
    SDL_Rect adjustedRect(const SDL_Rect& source, const MPoint<BaseType>& offset)
    {
        return
        {
            (int)(source.x * scale) + offset.x,
            (int)(source.y * scale) + offset.y,
            (int)(source.w * scale),
            (int)(source.h * scale)
        };
    }

    void postQuit()
    {
        SDL_Event event{};
        event.type = SDL_QUIT;
        SDL_PushEvent(&event);
    }

    // map an SDL_Keycode to a printable ASCII character, honouring the shift modifier.
    // Returns 0 if the key is not printable.
    char keyToPrintable(int32_t key, bool shift)
    {
        if (key >= SDLK_a && key <= SDLK_z)
        {
            return static_cast<char>((shift ? 'A' : 'a') + (key - SDLK_a));
        }
        if (key >= SDLK_0 && key <= SDLK_9)
        {
            static const char shifted[] = ")!@#$%^&*(";
            return shift ? shifted[key - SDLK_0] : static_cast<char>('0' + (key - SDLK_0));
        }
        // Numeric keypad — digits and arithmetic punctuation. These keycodes are
        // independent of NumLock state in SDL2 (SDL emits SDLK_KP_N only when
        // NumLock is on; otherwise it emits the navigation-key variants which
        // we ignore here).
        if (key >= SDLK_KP_1 && key <= SDLK_KP_9)
        {
            return static_cast<char>('1' + (key - SDLK_KP_1));
        }
        switch (key)
        {
        case SDLK_SPACE:        return ' ';
        case SDLK_MINUS:        return shift ? '_' : '-';
        case SDLK_EQUALS:       return shift ? '+' : '=';
        case SDLK_PERIOD:       return shift ? '>' : '.';
        case SDLK_COMMA:        return shift ? '<' : ',';
        case SDLK_SLASH:        return shift ? '?' : '/';
        case SDLK_SEMICOLON:    return shift ? ':' : ';';
        case SDLK_QUOTE:        return shift ? '"' : '\'';
        case SDLK_KP_0:         return '0';
        case SDLK_KP_PERIOD:    return '.';
        case SDLK_KP_DIVIDE:    return '/';
        case SDLK_KP_MULTIPLY:  return '*';
        case SDLK_KP_MINUS:     return '-';
        case SDLK_KP_PLUS:      return '+';
        case SDLK_KP_EQUALS:    return '=';
        default:                return 0;
        }
    }
}
// ─────────────────────────────────────────────────────────────────────────────


HomeWindow::HomeWindow(SDL_Renderer* renderer) :
    mRenderer{ renderer },
    mBackground(loadTexture(renderer, "images/background-micropolis.png"))
{
    size({ BgRect.w, BgRect.h });
    closeButtonActive(false);
    anchor();

    initHighlightButtons();

    mDifficultyEntries =
    {
        { 0, { 982, 106, 190, 70 } },
        { 1, { 982, 176, 190, 70 } },
        { 2, { 982, 246, 190, 70 } },
    };

    mScenarioEntries =
    {
        { 0, { 310, 451, 209, 188 }, "Dullsville" },
        { 1, { 519, 451, 209, 188 }, "San Francisco" },
        { 2, { 727, 450, 209, 188 }, "Hamburg" },
        { 3, { 936, 450, 209, 188 }, "Bern" },
        { 4, { 310, 639, 209, 188 }, "Tokyo" },
        { 5, { 937, 638, 209, 188 }, "Rio de Janeiro" },
        { 6, { 728, 638, 209, 188 }, "Boston" },
        { 7, { 519, 639, 209, 188 }, "Detroit" },
    };

    // Each entry links a rect to the corresponding bool in mOptions via pointer-to-member.
    // Order matches the original CheckBoxes array (alphabetical within column groups).
    mCheckBoxEntries =
    {
        { { 145, 172, 12, 12 }, &Options::mAutoBudget       },
        { {  13, 172, 12, 12 }, &Options::mAutoBulldoze     },
        { {  13, 188, 12, 12 }, &Options::mAutoGoto         },
        { {  13, 204, 12, 12 }, &Options::mDisastersEnabled },
        { { 145, 204, 12, 12 }, &Options::mPlayMusic        },
        { { 145, 188, 12, 12 }, &Options::mPlaySound        },
    };
}


void HomeWindow::initHighlightButtons()
{
    // Indices 4 and 5 are the left/right map-navigation arrows — draw() and
    // onMouseDown() reference these by index for the disabled-arrow logic.
    mHighlightButtons =
    {
        { "button1hilite", {  70, 238, 157, 90 }, false, [this] { showWindowAndBringToFront(*savedGamesWindow); }},

        { "button2hilite", {  62, 392, 157, 90 }, false, [this] { generateNewTerrain(); mCurrentMap = static_cast<int>(maps.size()) - 1; }},

        { "button3hilite", {  68, 544, 157, 90 }, false, [] { exit(0); }},

		{ "button4hilite", { 101, 705, 157, 90 }, false, [] {
			_load_file(documentsPath + pathSeparator + "about.cty");
		}},

        { "lefthilite",    { 540, 375,  50, 50 }, false,  // index 4 — left arrow
            [this]
            {
                if (mCurrentMap > 0)
                {
                    --mCurrentMap;
                    map = maps[mCurrentMap];
                }
            }},

        { "righthilite",   { 841, 375,  50, 50 }, false,  // index 5 — right arrow
            [this]
            {
                if (mCurrentMap < static_cast<int>(maps.size()) - 1)
                {
                    ++mCurrentMap;
                    map = maps[mCurrentMap];
                }
            }},

        { "playhilite",    { 625, 376, 180, 50 }, false, [this] { cityProperties.CityName(mTextFieldContent); drawBigMap(); playThisMap(mDifficulty); showPopup(lastPitch, true); } },
    };
}


void HomeWindow::optionsChangedConnect(CallbackOptionsChanged callback)
{
    mOptionsChangedCallback = callback;
}


void HomeWindow::newGameCallbackConnect(CallbackSignal callback)
{
    mNewGameCallback = callback;
}


void HomeWindow::openGameCallbackConnect(CallbackSignal callback)
{
    mOpenGameCallback = callback;
}


void HomeWindow::saveGameCallbackConnect(CallbackSignal callback)
{
    mSaveGameCallback = callback;
}


void HomeWindow::setOptions(const Options& options)
{
    // mOptions is the single source of truth; draw() derives checkbox state from it directly.
    mOptions = options;
}


Texture* HomeWindow::getTexture(const std::string& name)
{
    auto [it, inserted] = mTextureCache.try_emplace(name);
    if (inserted)
    {
        it->second = loadTexture(mRenderer, "images/" + name + ".png");
        SDL_SetTextureBlendMode(it->second.texture, SDL_BLENDMODE_BLEND);
    }
    return &it->second;
}

SDL_Rect HomeWindow::homeScaleRect(SDL_Rect srcRect)
{
    float bgWidth = fminf(area().width, (float)BgRect.w / (float)BgRect.h * area().height);
    float bgHeight = bgWidth * (float)BgRect.h / (float)BgRect.w;

    // Background
    const SDL_Rect bgDstRect{
        (int)roundf(area().x + (area().width - bgWidth) / 2.0f),
        (int)roundf(area().y + (area().height - bgHeight) / 2.0f),
		(int)roundf(bgWidth),
		(int)roundf(bgHeight)
    };

    SDL_Rect scaledRect = srcRect;
    scaledRect.x *= bgWidth / (float)BgRect.w;
    scaledRect.x += bgDstRect.x;
    scaledRect.y *= bgHeight / (float)BgRect.h;
    scaledRect.y += bgDstRect.y;
    scaledRect.w *= bgWidth / (float)BgRect.w;
    scaledRect.h *= bgHeight / (float)BgRect.h;

    return scaledRect;
}

void HomeWindow::showScenarioPitch(int id)
{
    struct msgpr msg;
    int i{ -1 };
    do {
        msg = popMsgs[++i];
    } while (msg.messageId != 0 && msg.messageId != id);

    if (msg.messageId != 0)
    {
		#if defined(_WIN64)
        pitch.title = _strdup(popMsgs[i].text);  // We own str's memory now.
        *strstr(pitch.title, "- ") = 0;
        char* msgText = _strdup(popMsgs[i].text);  // We own str's memory now.
		#else
		pitch.title = strdup(popMsgs[i].text);  // We own str's memory now.
		*strstr(pitch.title, "- ") = 0;
		char* msgText = strdup(popMsgs[i].text);  // We own str's memory now.
		#endif
        msgText = strstr(msgText, "- ") + 2;  // Skip the "- " part.
        auto font = fonts.body.get();
        int textWidth = (int)roundf(viewState.windowSize.x / 4.0f);
            
        SDL_Surface* pitchSurface = TTF_RenderText_Blended_Wrapped(font->mFontInfo.font, msgText, SDL_Color{ 0, 0, 0 }, textWidth);
        pitch.rect.w = pitchSurface->w;
        pitch.rect.h = pitchSurface->h;
        pitch.texture = SDL_CreateTextureFromSurface(mainWindowRenderer, pitchSurface);

        pitch.lastId = id;
    }   
}

void HomeWindow::drawScenarioPitch()
{
    if (pitch.lastId != 0)
    {
        SDL_Rect pitchBgRect;
        int titleHeight;

        float x = pitch.lastId == -1 || pitch.lastId == -2 || pitch.lastId == -5 || pitch.lastId == -8 ? 0.58f : 0.2935f;

        if (pitch.lastId >= -8)
        {
            pitchBgRect.x = (int)roundf(viewState.windowSize.x * x);
            pitchBgRect.y = (int)roundf(viewState.windowSize.y * 0.62f);
            pitchBgRect.w = (int)roundf(pitch.rect.w * 1.15f);
            pitchBgRect.h = (int)roundf(pitch.rect.h * 1.15f);

            pitch.rect.x = (int)roundf(viewState.windowSize.x * (x + 0.02f));
            pitch.rect.y = (int)roundf(viewState.windowSize.y * 0.62f);

            titleHeight = 30;


            SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 255);
            SDL_RenderFillRect(mainWindowRenderer, &pitchBgRect);

            SDL_SetRenderDrawColor(mainWindowRenderer, 127, 127, 127, 255);
            SDL_RenderDrawRect(mainWindowRenderer, &pitchBgRect);

            SDL_Rect pitchBgInnerRect = { pitchBgRect.x + 1, pitchBgRect.y + 1, pitchBgRect.w - 2, pitchBgRect.h - 2 };
            SDL_RenderDrawRect(mainWindowRenderer, &pitchBgInnerRect);

            SDL_Rect pitchTitleRect = pitchBgRect;
            pitchTitleRect.y = (int)roundf(pitchBgRect.y - titleHeight * scale);
            pitchTitleRect.h = (int)roundf(titleHeight * scale);

            SDL_SetRenderDrawColor(mainWindowRenderer, 192, 192, 192, 255);
            SDL_RenderFillRect(mainWindowRenderer, &pitchTitleRect);

            SDL_SetRenderDrawColor(mainWindowRenderer, 127, 127, 127, 255);
            SDL_Rect pitchTitleOuterRect = { pitchTitleRect.x, pitchTitleRect.y, pitchTitleRect.w, pitchTitleRect.h + 2 };
            SDL_RenderDrawRect(mainWindowRenderer, &pitchTitleOuterRect);
            SDL_Rect pitchTitleInnerRect = { pitchTitleRect.x + 1, pitchTitleRect.y + 1, pitchTitleRect.w - 2, pitchTitleRect.h };
            SDL_RenderDrawRect(mainWindowRenderer, &pitchTitleInnerRect);

            Font* pTitleFont = fonts.title.get();
            SDL_SetTextureColorMod(pTitleFont->texture(), 0, 0, 0);
            fonts.renderer->drawString(*pTitleFont, pitch.title, { (int)roundf(pitchTitleRect.x + pitchTitleRect.w / 2.0f - pTitleFont->size(pitch.title).x / 2.0f), (int)roundf(pitchTitleRect.y + titleHeight * 0.5f * scale - pTitleFont->size(pitch.title).y / 2.0f) });

            SDL_RenderCopy(mainWindowRenderer, pitch.texture, nullptr, &pitch.rect);
        }
    }
}
void HomeWindow::draw()
{
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    const SDL_Rect windowRect{ 0, 0, viewState.windowSize.x, viewState.windowSize.y };
    SDL_RenderFillRect(mRenderer, &windowRect);

    float bgWidth = fminf(area().width, (float)BgRect.w / (float)BgRect.h * area().height);
    float bgHeight = bgWidth * (float)BgRect.h / (float)BgRect.w;

    // Background
    const SDL_Rect bgDstRect { 
        (int)roundf(area().x + (area().width - bgWidth) / 2.0f),
		(int)roundf(area().y + (area().height - bgHeight) / 2.0f),
		(int)roundf(bgWidth),
		(int)roundf(bgHeight)
    };

    SDL_RenderCopy(mRenderer, mBackground.texture, &BgRect, &bgDstRect);

    appWindows.homeMiniMap->position(MPoint<int> {
        (int)roundf(bgDstRect.x - 0.5f * miniTileSize * SimWidth + roundf(720.0f) * bgWidth / (float)BgRect.w),
        (int)roundf(bgDstRect.y - 0.5f * miniTileSize * SimHeight + roundf((178.0f - PlatformHeightAdjust)) * bgHeight / (float)BgRect.h)
    });

    // Main button hover highlights
    for (const auto& btn : mHighlightButtons)
    {
        if (btn.hovered)
        {
            SDL_Rect scaledRect = homeScaleRect(btn.rect);
            SDL_RenderCopy(mRenderer, getTexture(btn.textureName)->texture, nullptr, &scaledRect);
        }
    }

    // Scenario thumbnail hover highlights
    for (const auto& entry : mScenarioEntries)
    {
        if (entry.hovered)
        {
            const std::string texKey = "scenario" + std::to_string(entry.scenarioIndex + 1) + "hilite";
            SDL_Rect scaledRect = homeScaleRect(entry.rect);
            SDL_RenderCopy(mRenderer, getTexture(texKey)->texture, nullptr, &scaledRect);
        }
    }

    // Difficulty radio buttons — hover and checked states compose the texture name
    for (const auto& entry : mDifficultyEntries)
    {
        const bool checked = (entry.index == mDifficulty);
        if (!entry.hovered && !checked) { continue; }

        std::string texKey = "checkbox" + std::to_string(entry.index + 1);
        if (entry.hovered) { texKey += "hilite";  }
        if (checked)       { texKey += "checked"; }

        SDL_Rect scaledRect = homeScaleRect(entry.rect);
        SDL_RenderCopy(mRenderer, getTexture(texKey)->texture, nullptr, &scaledRect);
    }

    // Disabled navigation arrows when at map boundaries (indices 4 and 5 are left/right)
    if (mCurrentMap <= 0)
    {
        SDL_Rect scaledRect = homeScaleRect(mHighlightButtons[4].rect);
        SDL_RenderCopy(mRenderer, getTexture("leftdisabled")->texture, nullptr, &scaledRect);
    }
    if (mCurrentMap >= static_cast<int>(maps.size()) - 1)
    {
        SDL_Rect scaledRect = homeScaleRect(mHighlightButtons[5].rect);
        SDL_RenderCopy(mRenderer, getTexture("rightdisabled")->texture, nullptr, &scaledRect);
    }

    drawScenarioPitch();

    drawTextField();
}


void HomeWindow::drawTextField()
{
    const SDL_Rect fieldRect = homeScaleRect(mTextFieldRect);

    // "City Name:" label, drawn just above the field. Position is computed in
    // design space so it scales with the rest of the home-screen UI.
    Font* labelFont = fonts.title.get();
    SDL_SetTextureColorMod(labelFont->texture(), 0, 0, 0);
    const SDL_Rect labelAnchor = homeScaleRect({ mTextFieldRect.x, mTextFieldRect.y - 22, 0, 0 });
    fonts.renderer->drawString(*labelFont, "City Name:", { labelAnchor.x, labelAnchor.y });

    // White interior
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(mRenderer, &fieldRect);

    // Border (darker when focused so the user sees which field receives keystrokes)
    if (mTextFieldFocused) { SDL_SetRenderDrawColor(mRenderer,  60,  90, 200, 255); }
    else                   { SDL_SetRenderDrawColor(mRenderer, 127, 127, 127, 255); }
    SDL_RenderDrawRect(mRenderer, &fieldRect);
    const SDL_Rect innerBorder{ fieldRect.x + 1, fieldRect.y + 1, fieldRect.w - 2, fieldRect.h - 2 };
    SDL_RenderDrawRect(mRenderer, &innerBorder);

    // Text — left-aligned, vertically centred, padded inside the border.
    Font* bodyFont = fonts.body.get();
    SDL_SetTextureColorMod(bodyFont->texture(), 0, 0, 0);

    const int padX     = (int)roundf(6.0f * (fieldRect.w / (float)mTextFieldRect.w));
    const int textY    = fieldRect.y + fieldRect.h / 2 - bodyFont->height() / 2;
    const int textX    = fieldRect.x + padX;

    if (!mTextFieldContent.empty())
    {
        fonts.renderer->drawString(*bodyFont, mTextFieldContent, { textX, textY });
    }

    // Caret — drawn only when focused; positioned at the end of the existing text.
    // Blinks at ~1Hz (500ms on, 500ms off). The phase is reset on each keystroke
    // by handleTextFieldKey() so the caret reappears immediately while typing.
    if (mTextFieldFocused)
    {
        const Uint32 elapsed = SDL_GetTicks() - mCaretBlinkStart;
        const bool caretVisible = ((elapsed / 500u) % 2u) == 0u;
        if (caretVisible)
        {
            const int caretX = textX + (mTextFieldContent.empty() ? 0 : bodyFont->width(mTextFieldContent)) + 1;
            const int caretY0 = fieldRect.y + 4;
            const int caretY1 = fieldRect.y + fieldRect.h - 5;
            SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
            SDL_RenderDrawLine(mRenderer, caretX, caretY0, caretX, caretY1);
        }
    }
}


void HomeWindow::handleTextFieldKey(int32_t key)
{
    if (!mTextFieldFocused) { return; }

    if (key == SDLK_BACKSPACE)
    {
        if (!mTextFieldContent.empty()) { mTextFieldContent.pop_back(); }
        mCaretBlinkStart = SDL_GetTicks();
        return;
    }

    const bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;
    const char ch = keyToPrintable(key, shift);
    if (ch != 0)
    {
        mTextFieldContent.push_back(ch);
        mCaretBlinkStart = SDL_GetTicks();
    }
}


void HomeWindow::onMouseDown(const MPoint<int>& point)
{
    const SDL_Point clickPoint = pointToSdlPoint(point);

    // Text field — clicking inside focuses, clicking outside releases focus.
    const SDL_Rect textFieldHit = homeScaleRect(mTextFieldRect);
    const bool nowFocused = SDL_PointInRect(&clickPoint, &textFieldHit);
    if (nowFocused && !mTextFieldFocused) { mCaretBlinkStart = SDL_GetTicks(); }
    mTextFieldFocused = nowFocused;

    // Options checkboxes — toggle via pointer-to-member
    checkCheckboxesForClick(point);

    // Main highlight buttons
    for (const auto& btn : mHighlightButtons)
    {
        SDL_Rect scaledRect = homeScaleRect(btn.rect);
        if (SDL_PointInRect(&clickPoint, &scaledRect) && btn.onClick)
        {
            btn.onClick();
        }
    }

    // Difficulty radio buttons
    for (const auto& entry : mDifficultyEntries)
    {
        SDL_Rect scaledRect = homeScaleRect(entry.rect);
        if (SDL_PointInRect(&clickPoint, &scaledRect))
        {
            mDifficulty = entry.index;
        }
    }

    // Scenario tiles
    for (const auto& entry : mScenarioEntries)
    {
        SDL_Rect scaledRect = homeScaleRect(entry.rect);
        if (SDL_PointInRect(&clickPoint, &scaledRect))
        {
            mTextFieldContent = entry.name;
            simInit();
            LoadScenario((Scenario)entry.scenarioIndex, cityProperties, budget, gameOptions);
            Pause();
            lastPitch = -1 - entry.scenarioIndex;
        }
    }

    optionsChangedTrigger();
}


void HomeWindow::onKeyDown(int32_t key)
{
    // When the text field is focused it absorbs almost everything so the user
    // can type freely; ESC simply releases focus rather than closing the window.
    if (mTextFieldFocused)
    {
        if (key == SDLK_ESCAPE)
        {
            mTextFieldFocused = false;
            return;
        }
        handleTextFieldKey(key);
        return;
    }

    switch (key)
    {
    case SDLK_ESCAPE:
        hide();
        break;

    case SDLK_RETURN:
        optionsChangedTrigger();
        hide();
        break;

    default:
        break;
    }
}


void HomeWindow::checkCheckboxesForClick(const MPoint<int>& point)
{
    const SDL_Point clickPoint = pointToSdlPoint(point);
    for (auto& entry : mCheckBoxEntries)
    {
        const SDL_Rect hitRect = adjustedRect(entry.rect, area().startPoint());
        if (SDL_PointInRect(&clickPoint, &hitRect))
        {
            mOptions.*entry.fieldPtr = !(mOptions.*entry.fieldPtr);
        }
    }
}


void HomeWindow::onMouseMotion(const MPoint<int>& point)
{
    const SDL_Point sdlPoint = pointToSdlPoint(point);

    for (auto& btn : mHighlightButtons)
    {
        SDL_Rect scaledRect = homeScaleRect(btn.rect);
        btn.hovered = SDL_PointInRect(&sdlPoint, &scaledRect);
    }

    pitch.lastId = 0;

    for (auto& entry : mScenarioEntries)
    {
        SDL_Rect scaledRect = homeScaleRect(entry.rect);
        entry.hovered = SDL_PointInRect(&sdlPoint, &scaledRect);

        if (entry.hovered)
        {
            showScenarioPitch(-1 - entry.scenarioIndex);
        }
    }

    for (auto& entry : mDifficultyEntries)
    {
        SDL_Rect scaledRect = homeScaleRect(entry.rect);
        entry.hovered = SDL_PointInRect(&sdlPoint, &scaledRect);
    }
}


void HomeWindow::optionsChangedTrigger()
{
    if (mOptionsChangedCallback)
        mOptionsChangedCallback(mOptions);
}
