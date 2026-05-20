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

#include "SDL_include.h"


#include "Graph.h"
#include "Texture.h"
#include "WindowBase.h"


class GraphWindow : public WindowBase
{
public:
	GraphWindow() = delete;
	GraphWindow(const GraphWindow&) = delete;
	const GraphWindow& operator=(const GraphWindow&) = delete;

	GraphWindow(SDL_Renderer*);
	~GraphWindow() override = default;

	void draw();
	void update() override;

    void onMouseMotion(const MPoint<int>& position);
private:
	void fillGraphPoints(Graph::PointsList&, const GraphHistory&);

	void onMouseDown(const MPoint<int>& position) override;
	void onMoved(const Vector<int>&) override;
	void onPositionChanged(const MPoint<int>&) override;


	SDL_Renderer& mRenderer;
	const Texture mTexture;
	Texture mGraphTexture;
};
