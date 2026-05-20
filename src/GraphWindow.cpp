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
#include "GraphWindow.h"
#include "FontSet.h"

#include "Colors.h"
#include "Graphics.h"

#include <array>
#include <map>

namespace
{
    const SDL_Rect ButtonUp{ 285, 0, 36, 36 };
    const SDL_Rect ButtonDown{ 326, 0, 36, 36 };
    const SDL_Rect Bg{ 0, 0, 400, 300 };

    const SDL_Rect GraphLayout{ 15, 150, 370, 135 };
    SDL_Rect GraphPosition = GraphLayout;

    enum class ButtonId
    {
        Residential,
        Commercial,
        Industrial,
        Pollution,
        Crime,
        Money,
        None
    };

    ButtonId hoveredButton = ButtonId::None;

    MPoint<int> tooltipPosition;

    std::map<ButtonId, Graph> HistoryGraphTable
    {
        { ButtonId::Residential, { ResHis, "Residential", Colors::LightGreen, { 0 } } },
        { ButtonId::Commercial, { ComHis, "Commercial", Colors::DarkBlue, { 0 } } },
        { ButtonId::Industrial, { IndHis, "Industrial", Colors::Gold, { 0 } } },
        { ButtonId::Money, { MoneyHis, "Cash Flow", Colors::Turquoise, { 0 } } },
        { ButtonId::Crime, { CrimeHis, "Crime", Colors::Red, { 0 } } },
        { ButtonId::Pollution, { PollutionHis, "Pollution", Colors::Olive, { 0 } } }
    };


    struct ButtonMeta
    {
        ButtonId id{ ButtonId::Residential };
        SDL_Rect area{ 0 };
        SDL_Rect iconPosition{ 0 };
        bool toggled{ false };
    };


    const std::map<ButtonId, SDL_Rect> ButtonLayout
    {
        { ButtonId::Residential, { 15, 32, 50, 50 } },
        { ButtonId::Commercial, { 79, 32, 50, 50 } },
        { ButtonId::Industrial, { 143, 32, 50, 50 } },
        { ButtonId::Pollution, { 207, 32, 50, 50 } },
        { ButtonId::Crime, { 271, 32, 50, 50 } },
        { ButtonId::Money, { 335, 32, 50, 50 } },
    };


    std::array<ButtonMeta, 6> Buttons
    {
        ButtonMeta{ ButtonId::Residential, ButtonLayout.at(ButtonId::Residential), { 0 }, true },
        ButtonMeta{ ButtonId::Commercial, ButtonLayout.at(ButtonId::Commercial), { 0 }, true },
        ButtonMeta{ ButtonId::Industrial, ButtonLayout.at(ButtonId::Industrial), { 0 }, true },
        ButtonMeta{ ButtonId::Pollution, ButtonLayout.at(ButtonId::Pollution), { 0 }, true },
        ButtonMeta{ ButtonId::Crime, ButtonLayout.at(ButtonId::Crime), { 0 }, true },
        ButtonMeta{ ButtonId::Money, ButtonLayout.at(ButtonId::Money), { 0 }, true }
    };


    const std::map<ButtonId, SDL_Rect> IconRects
    {
        { ButtonId::Residential, { 285, 59, 22, 22 } },
        { ButtonId::Commercial, { 285, 82, 22, 22 } },
        { ButtonId::Industrial, { 285, 105, 22, 22 } },
        { ButtonId::Pollution, { 285, 128, 22, 22 } },
        { ButtonId::Crime, { 285, 151, 22, 22 } },
        { ButtonId::Money, { 285, 174, 22, 22 } }
    };


    bool ButtonToggled(const ButtonId id)
    {
        for (auto& button : Buttons)
        {
            if (button.id == id)
            {
                return button.toggled;
            }
        }

        return false;
    }
};


GraphWindow::GraphWindow(SDL_Renderer* renderer) :
    mRenderer(*renderer),
    mTexture(loadTexture(renderer, "images/graph.png"))
{
    mTitle = "Graph";
    closeButtonActive(true);
    size({264, 287});
    //initTexture(*mainWindowRenderer, mGraphTexture, { GraphLayout.w, GraphLayout.h });
}


void GraphWindow::onMoved(const Vector<int>& movement)
{
    
}


void GraphWindow::onPositionChanged(const MPoint<int>& position)
{
    
}


void GraphWindow::onMouseDown(const MPoint<int>& position)
{
    const SDL_Point& pt{ position.x, position.y };

    for (auto& button : Buttons)
    {
        if (SDL_PointInRect(&pt, &button.area))
        {
            button.toggled = !button.toggled;
            update();
            return; // assumption: mouse position can only ever be within one button
        }
    }
}


void GraphWindow::draw()
{
    float tmpScale = scale;
    
    size({ (int)roundf(Bg.w * scale), (int)roundf(Bg.h * scale) });
    WindowBase::draw();
    
    for (auto& button : Buttons)
    {
        button.area = globalScaleRect(ButtonLayout.at(button.id));
        button.iconPosition = globalScaleRect({ ButtonLayout.at(button.id).x + 6, ButtonLayout.at(button.id).y + 6, 35, 34 });
        
        const SDL_Rect& buttonTexture = button.toggled ? ButtonDown : ButtonUp;
        const int offset = button.toggled ? 1 : 0;
        SDL_RenderCopy(mainWindowRenderer, mTexture.texture, &buttonTexture, &button.area);
        SDL_RenderCopy(mainWindowRenderer, mTexture.texture, &IconRects.at(button.id), &button.iconPosition);
    }
    
    if (hoveredButton != ButtonId::None) {
        std::string tooltipLabel = HistoryGraphTable.at(hoveredButton).name;
        
        SDL_SetRenderDrawColor(mainWindowRenderer, 0xff, 0xff, 0xca, 255);
        SDL_Rect tooltipRect = {
            tooltipPosition.x,
            tooltipPosition.y + (int)roundf(15 * tmpScale),
            fonts.body->size(tooltipLabel).x + (int)roundf(10 * tmpScale),
            (int)(18 * tmpScale)
        };
        SDL_RenderFillRect(mainWindowRenderer, &tooltipRect);
        SDL_SetRenderDrawColor(mainWindowRenderer, 0xff, 0x00, 0x00, 255);
        SDL_RenderDrawRect(mainWindowRenderer, &tooltipRect);
        
        SDL_SetTextureColorMod(fonts.body->texture(), 0, 0, 0);
        fonts.renderer->drawString(*fonts.body, tooltipLabel, tooltipPosition + Vector<int>{(int)roundf(5 * tmpScale), (int)roundf(16 * tmpScale)});
    }
    
    update();
    
    scale = tmpScale;
}


int lastPoint;

void GraphWindow::update()
{
    
    
    for (auto& [type, graph] : HistoryGraphTable)
    {
        fillGraphPoints(graph.points, graph.history);
    }

    //SDL_SetRenderTarget(mainWindowRenderer, mGraphTexture.texture);
    
    //turnOffBlending(*mainWindowRenderer, mGraphTexture);

    //SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 0);
    //SDL_RenderClear(mainWindowRenderer);

    for (auto& [type, graph] : HistoryGraphTable)
    {
        if (!ButtonToggled(type)) { continue; }
        SDL_SetRenderDrawColor(mainWindowRenderer, graph.color.r, graph.color.g, graph.color.b, 255);
        SDL_RenderDrawLines(mainWindowRenderer, graph.points.data(), lastPoint);
    }

    //turnOnBlending(*mainWindowRenderer, mGraphTexture);
    //SDL_SetRenderTarget(mainWindowRenderer, nullptr);
}

void GraphWindow::fillGraphPoints(Graph::PointsList& points, const GraphHistory& history)
{
    SDL_Rect rect = globalScaleRect(GraphLayout);
    
    float sx = static_cast<float>(rect.w / 120.0f);
    float sy = static_cast<float>(rect.h / 320.0f);

    lastPoint = 0;
    
    for (int i = 0; i < HistoryLength; ++i)
    {
        const int x = rect.x + rect.w - static_cast<int>(i * sx);
        const int y = rect.y + rect.h - static_cast<int>(history[i] * sy);

        points[i] = { x, y };
        
        if (history[i] != 0)
        {
            lastPoint++;
        }
    }
}

void GraphWindow::onMouseMotion(const MPoint<int>& position)
{
    hoveredButton = ButtonId::None;
    
    for (auto& button : Buttons)
    {
        SDL_Point point{ position.x, position.y };
        
        if (SDL_PointInRect(&point, &button.area))
        {
            tooltipPosition = position;
            hoveredButton = button.id;
        }
    }
}
