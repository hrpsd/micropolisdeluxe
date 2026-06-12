// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include <memory>

#include "AboutWindow.h"
#include "BudgetWindow.h"
#include "EvaluationWindow.h"
#include "GraphWindow.h"
#include "HomeWindow.h"
#include "MessagesWindow.h"
#include "PopupWindow.h"
#include "QueryWindow.h"
#include "SaveAsWindow.h"
#include "StatusWindow.h"
#include "ToolPalette.h"

// MiniMapWindow.h includes main.h — forward-declare to avoid circular include
class MiniMapWindow;
class HomeMiniMapWindow;
class GraphWindow;
class AboutWindow;
class BudgetWindow;

/**
 * Owns all SDL GUI window unique_ptrs.
 *
 * savedGamesWindow is kept as a standalone global because s_fileio.cpp
 * references it by name and cannot be modified.
 *
 * The destructor is defined in main.cpp where MiniMapWindow is a complete type.
 */
struct AppWindows
{
    std::unique_ptr<HomeWindow>       home;
    std::unique_ptr<BudgetWindow>     budget;
    std::unique_ptr<GraphWindow>      graph;
    std::unique_ptr<EvaluationWindow> evaluation;
    std::unique_ptr<HomeMiniMapWindow> homeMiniMap;
    std::unique_ptr<MiniMapWindow>    gameMiniMap;
    std::unique_ptr<ToolPalette>      toolPalette;
    std::unique_ptr<QueryWindow>      query;
    std::unique_ptr<AboutWindow>      about;
    std::unique_ptr<MessagesWindow>   messages;
    std::unique_ptr<StatusWindow>     status;
    std::unique_ptr<SaveAsWindow>     saveAs;
    std::unique_ptr<PopupWindow>      popup;

    ~AppWindows(); // defined in main.cpp
};

extern AppWindows appWindows;
