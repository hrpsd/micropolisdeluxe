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

/**
 * KeyboardHandler.cpp
 *
 * Keyboard shortcuts, simulation speed control, and window visibility helpers.
 *
 * Responsibilities:
 *   - setSpeed / TogglePause — simulation speed control
 *   - toggleMiniMapVisibility — minimap toggle
 *   - showBudgetWindow / showGraphWindow / showEvaluationWindow /
 *     showSavedGamesWindow / showAboutWindow — window show/hide
 *   - handleKeyEvent — routes SDL_KEYDOWN to the above
 *
 * Does NOT contain: mouse events, tile mapping.
 * See MouseHandler.cpp for those.
 */

#include "main.h"

#include "AppWindows.h"
#include "MiniMapWindow.h"
#include "Evaluation.h"

#include "w_util.h"


// ── Simulation speed ─────────────────────────────────────────────────────────

void setSpeed(SimulationSpeed speed)
{
    if (Paused())
    {
        Resume();
    }

    SimSpeed(speed);
}

void TogglePause()
{
    Paused() ? Resume() : Pause();
}

// ── Window visibility helpers ────────────────────────────────────────────────
void toggleMiniMapVisibility()
{
    appWindows.gameMiniMap->visible()
        ? appWindows.gameMiniMap->hide()
        : appWindows.gameMiniMap->show();
}

void showBudgetWindow()
{
    appWindows.budget->visible()
        ? appWindows.budget->hide()
        : showWindowAndBringToFront(*appWindows.budget);
}

void showGraphWindow()
{
    appWindows.graph->visible()
        ? appWindows.graph->hide()
        : showWindowAndBringToFront(*appWindows.graph);
}

void showEvaluationWindow()
{
    if (!appWindows.evaluation->visible())
    {
        appWindows.evaluation->setEvaluation(currentEvaluation());
        currentEvaluationSeen();
    }

    appWindows.evaluation->visible()
        ? appWindows.evaluation->hide()
        : showWindowAndBringToFront(*appWindows.evaluation);
}

void showMessagesWindow()
{
    appWindows.messages->visible()
        ? appWindows.messages->hide()
        : showWindowAndBringToFront(*appWindows.messages);
}

void showAboutWindow()
{
    appWindows.about->visible()
        ? appWindows.about->hide()
        : showWindowAndBringToFront(*appWindows.about);
}

// ── Key event handler ────────────────────────────────────────────────────────
void handleKeyEvent(SDL_Event& event)
{
    // Save-As dialog absorbs keystrokes (text field input) while visible.
    if (appWindows.saveAs && appWindows.saveAs->visible())
    {
        appWindows.saveAs->injectKeyDown(event.key.keysym.sym);
        return;
    }

    // Home (start) window absorbs keystrokes while visible so its editable
    // text field can receive input. Speed/menu shortcuts are irrelevant here.
    if (appWindows.home && appWindows.home->visible())
    {
        appWindows.home->injectKeyDown(event.key.keysym.sym);
        return;
    }

    switch (event.key.keysym.sym)
    {
    case SDLK_ESCAPE:
        guiWindowStack.hide();
        guiModalWindows.hide();
        break;

    case SDLK_0:
    case SDLK_p:
    case SDLK_SPACE:
        TogglePause();
        break;

    case SDLK_1:
        setSpeed(SimulationSpeed::Slow);
        break;

    case SDLK_2:
        setSpeed(SimulationSpeed::Normal);
        break;

    case SDLK_3:
        setSpeed(SimulationSpeed::Fast);
        break;

    case SDLK_4:
        setSpeed(SimulationSpeed::AfricanSwallow);
        break;

    case SDLK_F1:
        showEvaluationWindow();
        break;

    case SDLK_F2:
        saveGame();
        break;

    case SDLK_F3:
        // openGame();  — not yet implemented
        break;

    case SDLK_F4:
        toggleMiniMapVisibility();
        break;

    case SDLK_F5:
        // generateMonster() / generateTornado() / MakeFlood() etc.
        break;

    case SDLK_F7:
        // newGame();   — not yet implemented
        break;

    case SDLK_F9:
        showWindowAndBringToFront(*appWindows.graph.get());
        break;

    case SDLK_F10:
        showWindowAndBringToFront(*appWindows.budget.get());
        break;

    case SDLK_F11:
        showWindowAndBringToFront(*appWindows.query.get());
        break;

    default:
        break;
    }
}
