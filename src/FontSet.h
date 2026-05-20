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

#include "Font.h"
#include "StringRender.h"

/**
 * Owns all font resources and the shared string renderer.
 *
 * Naming convention (standard typography terms):
 *   title      — bold label font (was: titleFont)
 *   largeTitle — bold heading font (was: largeTitleFont)
 *   body       — regular body text (was: defaultFont)
 *   large      — large body text (was: largeFont)
 *   caption    — small/detail text (was: smallFont)
 *   renderer   — shared SDL string renderer (was: stringRenderer)
 */
struct FontSet
{
    std::unique_ptr<Font>         title;
    std::unique_ptr<Font>         largeTitle;
    std::unique_ptr<Font>         body;
    std::unique_ptr<Font>         large;
    std::unique_ptr<Font>         caption;
    std::unique_ptr<StringRender> renderer;
};

extern FontSet fonts;
