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

#include "main.h"
#include "Point.h"
#include "Texture.h"
#include "Vector.h"
#include "WindowBase.h"
#include "Font.h"
#include "StringRender.h"

#include <array>
#include <cstdint>
#include <map>
#include <vector>

#include "SDL_include.h"


class EffectMap;


class MiniMapWindow : public WindowBase
{
public:

	enum class ButtonId
	{
		Normal,
		LandValue,
		Crime,
		FireProtection,
		PoliceProtection,
		PopulationDensity,
		PopulationGrowth,
		Pollution,
		TrafficDensity,
		TransportationNetwork,
		PowerGrid,
		Residential,
		Commercial,
		Industrial,
		None
	};

	static constexpr auto ButtonStateNormal{ 0 };
	static constexpr auto ButtonStatePressed{ 1 };

	using fnPointIntParam = void(*)(const MPoint<int>&);

public:
	MiniMapWindow() = delete;
	MiniMapWindow(const MiniMapWindow&) = delete;
	const MiniMapWindow& operator=(const MiniMapWindow&) = delete;

	~MiniMapWindow() override = default;

	void draw();
	void update() override;
	
	MiniMapWindow(SDL_Renderer* renderer, const MPoint<int>& position, const Vector<int>& size);

	
	uint32_t id() const;

	void focusOnMapCoordBind(fnPointIntParam);
	void focusOnMapCoordUnbind(fnPointIntParam);

	void updateMapViewPosition(SDL_Rect rect);
	void updateViewportSize(const Vector<int>& viewportSize);
	void updateTilePointedAt(const MPoint<int>& tilePointedAt);

	void linkEffectMap(ButtonId id, const EffectMap& map);

	void resetOverlayButtons();

	//void hide();
	//void show();

	//bool hidden() const;

	void drawUI();

	void injectEvent(const SDL_Event& event);
	
	void setSize();

private:
	void handleMouseEvent(const SDL_Event& event);
	void handleWindowEvent(const SDL_Event& event);
	void handleMouseMotion(const SDL_Event& event);

    MPoint<int> lastFocusPoint;
	void focusViewpoint();

	void setButtonValues();
	void setButtonTextureUv();
	void setButtonPositions();

	bool noButtonsSelected();

	void initTexture(Texture& texture, const Vector<int>& dimensions);
	void initOverlayTextures();

	void drawCurrentOverlay();
    void drawPlainMap();
	void drawResidential();
	void drawCommercial();
	void drawIndustrial();
	void drawPowerMap();
    void drawLilTransMap();

private:
	struct ButtonMeta
	{
		SDL_Rect rect{};
		int state{};
		ButtonId id{ ButtonId::Normal };
	};

private:
	//SDL_Window* mWindow{ nullptr };
	SDL_Renderer* mRenderer{ nullptr };

	Texture mTiles{};
	Texture mTexture{};
	Texture mButtonTextures{};

	SDL_Rect mSelector{};
	SDL_Rect mTileHighlight{ 0, 0, miniTileSize, miniTileSize };
	SDL_Rect mTileRect{ 0, 0, 3, 3 };
	SDL_Rect mMinimapArea{};
	SDL_Rect mButtonArea{};

	uint32_t mWindowID{};

	std::array<SDL_Rect, 28> mButtonUV{};
	std::array<ButtonMeta, 14> mButtons{};

	std::map<ButtonId, Texture> mOverlayTextures;
	std::map<ButtonId, const EffectMap*> mEffectMaps;

	std::vector<fnPointIntParam> mFocusOnTileCallbacks;

	ButtonId mButtonDownId{ ButtonId::Normal };
	ButtonId mHoveredButton = ButtonId::None;
	MPoint<int> tooltipPosition;
	std::unique_ptr<Font> mFont;
	StringRender mStringRender;
	
	bool mButtonDownInMinimapArea{ false };
	//bool mHidden{ true };
	
	void clearOverlayTexture(SDL_Renderer& renderer);
	void drawOverlayPoints(SDL_Renderer& renderer, Texture& overlay, const EffectMap& map);
    
    MPoint<int> lastViewPosition;
};
