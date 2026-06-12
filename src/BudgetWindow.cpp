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
#include "BudgetWindow.h"
#include "FontSet.h"
#include "main.h"

#include "w_util.h"

#include <algorithm>
#include <map>

namespace
{
    const SDL_Rect bgRect{ 0, 0, 400, 300 };
    const SDL_Rect mainButtonDown{ 2, 426, 434, 36 };

    enum class TextPanelId
    {
        TaxRate,
        TaxesCollected,
        TransportNeeded,
        TransportAllocated,
        TransportRate,
        PoliceNeeded,
        PoliceAllocated,
        PoliceRate,
        FireNeeded,
        FireAllocated,
        FireRate,
        CashFlow,
        PreviousFunds,
        CurrentFunds
    };


    const TextPanelId panels[]
    {
        TextPanelId::TaxRate,
        TextPanelId::TaxesCollected,
        TextPanelId::TransportNeeded,
        TextPanelId::TransportAllocated,
        TextPanelId::TransportRate,
        TextPanelId::PoliceNeeded,
        TextPanelId::PoliceAllocated,
        TextPanelId::PoliceRate,
        TextPanelId::FireNeeded,
        TextPanelId::FireAllocated,
        TextPanelId::FireRate,
        TextPanelId::CashFlow,
        TextPanelId::PreviousFunds,
        TextPanelId::CurrentFunds
    };


    const std::map<TextPanelId, SDL_Rect> TextPanelLayout
    {
        { TextPanelId::TaxRate, {138, 30, 285, 21} },
        { TextPanelId::TaxesCollected, {138, 55, 285, 21} },

        { TextPanelId::TransportNeeded, {153, 115, 100, 31} },
        { TextPanelId::TransportAllocated, {253, 115, 100, 31} },
        { TextPanelId::TransportRate, {364, 115, 41, 31} },

        { TextPanelId::PoliceNeeded, {153, 145, 100, 31} },
        { TextPanelId::PoliceAllocated, {253, 145, 100, 31} },
        { TextPanelId::PoliceRate, {364, 145, 45, 31} },

        { TextPanelId::FireNeeded, {153, 175, 100, 31} },
        { TextPanelId::FireAllocated, {253, 175, 100, 31} },
        { TextPanelId::FireRate, {364, 175, 45, 31} },

        { TextPanelId::CashFlow, {138, 230, 285, 21} },
        { TextPanelId::PreviousFunds, {138, 250, 285, 21} },
        { TextPanelId::CurrentFunds, {138, 270, 285, 21} }
    };


    std::map<TextPanelId, SDL_Rect> TextPanelRects;
    std::map<TextPanelId, std::string> TextPanelText;
    std::map<TextPanelId, MPoint<int>> TextPanelAdjust;


    const BudgetWindow::ButtonId buttons[]
    {
        BudgetWindow::ButtonId::None,
        BudgetWindow::ButtonId::TaxRateUp,
        BudgetWindow::ButtonId::TaxRateDown,
        BudgetWindow::ButtonId::TransportUp,
        BudgetWindow::ButtonId::TransportDown,
        BudgetWindow::ButtonId::PoliceUp,
        BudgetWindow::ButtonId::PoliceDown,
        BudgetWindow::ButtonId::FireUp,
        BudgetWindow::ButtonId::FireDown,
        //BudgetWindow::ButtonId::Accept
    };

    //SDL_Rect upArrowButtonDown{ 470, 7, 13, 13 };
    //SDL_Rect downArrowButtonDown{ 485, 7, 13, 13 };
    SDL_Rect upArrowButtonDown{ 371, 471, 16, 16 };
    SDL_Rect downArrowButtonDown{ 371, 486, 16, 16 };

    const std::map<BudgetWindow::ButtonId, SDL_Rect> ButtonDownTable
    {
        { BudgetWindow::ButtonId::None, {} },
        { BudgetWindow::ButtonId::TaxRateUp, upArrowButtonDown },
        { BudgetWindow::ButtonId::TaxRateDown, downArrowButtonDown },
        { BudgetWindow::ButtonId::TransportUp, upArrowButtonDown },
        { BudgetWindow::ButtonId::TransportDown, downArrowButtonDown },
        { BudgetWindow::ButtonId::PoliceUp, upArrowButtonDown },
        { BudgetWindow::ButtonId::PoliceDown, downArrowButtonDown },
        { BudgetWindow::ButtonId::FireUp, upArrowButtonDown },
        { BudgetWindow::ButtonId::FireDown, downArrowButtonDown },
        //{ BudgetWindow::ButtonId::Accept, mainButtonDown }
    };

    const std::map<BudgetWindow::ButtonId, const SDL_Rect> ButtonLayout
    {
        { BudgetWindow::ButtonId::None, {} },
        { BudgetWindow::ButtonId::TaxRateUp, { 431, 30, 13, 13 } },
        { BudgetWindow::ButtonId::TaxRateDown, { 431, 44, 13, 13 } },
        { BudgetWindow::ButtonId::TransportUp, { 431, 122, 13, 13 } },
        { BudgetWindow::ButtonId::TransportDown, { 431, 136, 13, 13 } },
        { BudgetWindow::ButtonId::PoliceUp, { 431, 150, 13, 13 } },
        { BudgetWindow::ButtonId::PoliceDown, { 431, 164, 13, 13 } },
        { BudgetWindow::ButtonId::FireUp, { 431, 178, 13, 13 } },
        { BudgetWindow::ButtonId::FireDown, { 431, 192, 13, 13 } },
        //{ BudgetWindow::ButtonId::Accept, { 11, 373, 434, 36 } }
    };

    std::map<BudgetWindow::ButtonId, SDL_Rect> ButtonRects;
};


BudgetWindow::BudgetWindow(SDL_Renderer* renderer, const StringRender& stringRenderer, Budget& budget) :
    mBudget(budget),
    mRenderer(renderer),
    mStringRenderer(stringRenderer),
    mTexture(loadTexture(renderer, "images/budget.png"))
{
    size({ (int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale) });
    closeButtonActive(true);
    //anchor();
    
    

    SDL_SetTextureColorMod(fonts.body->texture(), 0, 0, 0);
}


void BudgetWindow::onPositionChanged(const MPoint<int>& pos)
{
    for (auto id : buttons)
    {
        ButtonRects[id] =
        globalScaleRect({
            ButtonLayout.at(id).x - (int)roundf(50 * scale),// + pos.x,
            ButtonLayout.at(id).y,// + pos.y,
            ButtonLayout.at(id).w,
            ButtonLayout.at(id).h
        });
    }

    for (auto id : panels)
    {
        TextPanelRects[id] =
        globalScaleRect({
            TextPanelLayout.at(id).x - (int)roundf(50 * scale),// + pos.x,
            TextPanelLayout.at(id).y,// + pos.y,
            TextPanelLayout.at(id).w,
            TextPanelLayout.at(id).h
        });
    }
}


void BudgetWindow::handleMouseDown(const ButtonId id)
{
    switch (id)
    {
    case ButtonId::None:
        break;

    case ButtonId::TaxRateUp:
        mBudget.TaxRate(std::clamp(mBudget.TaxRate() + 1, Budget::MinTaxRate, Budget::MaxTaxRate));
        break;

    case ButtonId::TaxRateDown:
        mBudget.TaxRate(std::clamp(mBudget.TaxRate() - 1, Budget::MinTaxRate, Budget::MaxTaxRate));
        break;

    case ButtonId::TransportUp:
        mBudget.RoadPercent(std::clamp(mBudget.RoadPercent() + Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
        break;

    case ButtonId::TransportDown:
        mBudget.RoadPercent(std::clamp(mBudget.RoadPercent() - Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
        break;

    case ButtonId::PoliceUp:
        mBudget.PolicePercent(std::clamp(mBudget.PolicePercent() + Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
        break;

    case ButtonId::PoliceDown:
        mBudget.PolicePercent(std::clamp(mBudget.PolicePercent() - Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
        break;

    case ButtonId::FireUp:
        mBudget.FirePercent(std::clamp(mBudget.FirePercent() + Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
        break;

    case ButtonId::FireDown:
        mBudget.FirePercent(std::clamp(mBudget.FirePercent() - Budget::FundingRateStep, Budget::MinFundingRate, Budget::MaxFundingRate));
        break;

    case ButtonId::Accept:
        mButtonDownId = ButtonId::None;
        hide();
        break;

    default:
        break;
    }

    update();
}


void BudgetWindow::onMouseDown(const MPoint<int>& pos)
{
    for (auto id : buttons)
    {
        const SDL_Point pt{ pos.x, pos.y };
        if (id != ButtonId::None && SDL_PointInRect(&pt, &ButtonRects[id]))
        {
            mButtonDownId = id;
            handleMouseDown(id);
            return;
        }
    }
}


void BudgetWindow::onMouseUp()
{
    mButtonDownId = ButtonId::None;
}


void BudgetWindow::draw()
{
    onPositionChanged({});
    
    update();
    
    size({(int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale)});
    mTitle = "Annual Fiscal Budget";
    WindowBase::draw();
    
    int leftColumn = area().x + (int)roundf(125 * scale);
    
    mStringRenderer.drawString(*fonts.title, "Tax Rate", { leftColumn - fonts.title->width("Tax Rate"), TextPanelRects[TextPanelId::TaxRate].y + TextPanelAdjust[TextPanelId::TaxRate].y });
    mStringRenderer.drawString(*fonts.title, "Taxes Collected", { leftColumn - fonts.title->width("Taxes Collected"), TextPanelRects[TextPanelId::TaxesCollected].y + TextPanelAdjust[TextPanelId::TaxesCollected].y });
    
    
    mStringRenderer.drawString(*fonts.body, "Requested", { TextPanelRects[TextPanelId::TransportNeeded].x + TextPanelAdjust[TextPanelId::TransportNeeded].x - fonts.body->width("Requested"), TextPanelRects[TextPanelId::TransportNeeded].y - (int)roundf(20 * scale) });
    
    mStringRenderer.drawString(*fonts.body, "Allocated", { TextPanelRects[TextPanelId::TransportAllocated].x + TextPanelAdjust[TextPanelId::TransportAllocated].x - fonts.body->width("Allocated"), TextPanelRects[TextPanelId::TransportAllocated].y - (int)roundf(20 * scale) });
    
    mStringRenderer.drawString(*fonts.body, "Rate", { TextPanelRects[TextPanelId::TransportRate].x + TextPanelAdjust[TextPanelId::TransportRate].x, TextPanelRects[TextPanelId::TransportRate].y - (int)roundf(20 * scale) });
    
    
    mStringRenderer.drawString(*fonts.title, "Transportation", { leftColumn - fonts.title->width("Transportation"), TextPanelRects[TextPanelId::TransportNeeded].y + TextPanelAdjust[TextPanelId::TransportNeeded].y });
    mStringRenderer.drawString(*fonts.title, "Police", { leftColumn - fonts.title->width("Police"), TextPanelRects[TextPanelId::PoliceNeeded].y + TextPanelAdjust[TextPanelId::PoliceNeeded].y});
    mStringRenderer.drawString(*fonts.title, "Fire", { leftColumn - fonts.title->width("Fire"), TextPanelRects[TextPanelId::FireNeeded].y + TextPanelAdjust[TextPanelId::FireNeeded].y });
    
    mStringRenderer.drawString(*fonts.title, "Cash Flow", { leftColumn - fonts.title->width("Cash Flow"), TextPanelRects[TextPanelId::CashFlow].y + TextPanelAdjust[TextPanelId::CashFlow].y });
    mStringRenderer.drawString(*fonts.title, "Previous Funds", { leftColumn - fonts.title->width("Previous Funds"), TextPanelRects[TextPanelId::PreviousFunds].y + TextPanelAdjust[TextPanelId::PreviousFunds].y });
    mStringRenderer.drawString(*fonts.title, "Current Funds", { leftColumn - fonts.title->width("Current FUnds"), TextPanelRects[TextPanelId::CurrentFunds].y + TextPanelAdjust[TextPanelId::CurrentFunds].y });
    
    
    for (auto id : buttons)
    {
        //if (id == mButtonDownId)
        {
            SDL_RenderCopy(mainWindowRenderer, mTexture.texture, &ButtonDownTable.at(id), &ButtonRects[id]);
        }
    }

    for (auto id : panels)
    {
        const std::string str = TextPanelText[id];
        SDL_Rect& rect = TextPanelRects[id];
        mStringRenderer.drawString(*fonts.body, str, { rect.x + TextPanelAdjust[id].x, rect.y + TextPanelAdjust[id].y });
    }
}


void BudgetWindow::update()
{
    TextPanelText[TextPanelId::CashFlow] = NumberToDollarDecimal(mBudget.CashFlow());
    TextPanelText[TextPanelId::CurrentFunds] = NumberToDollarDecimal(mBudget.CurrentFunds());

    TextPanelText[TextPanelId::PreviousFunds] = NumberToDollarDecimal(mBudget.PreviousFunds());
    TextPanelText[TextPanelId::TaxesCollected] = NumberToDollarDecimal(mBudget.TaxIncome());
    TextPanelText[TextPanelId::TaxRate] = std::to_string(mBudget.TaxRate()) + "%";

    TextPanelText[TextPanelId::FireAllocated] = NumberToDollarDecimal(mBudget.FireFundsGranted());
    TextPanelText[TextPanelId::FireNeeded] = NumberToDollarDecimal(mBudget.FireFundsNeeded());
    TextPanelText[TextPanelId::FireRate] = std::to_string(static_cast<int>(mBudget.FirePercent() * 100.0f)) + "%";

    TextPanelText[TextPanelId::PoliceAllocated] = NumberToDollarDecimal(mBudget.PoliceFundsGranted());
    TextPanelText[TextPanelId::PoliceNeeded] = NumberToDollarDecimal(mBudget.PoliceFundsNeeded());
    TextPanelText[TextPanelId::PoliceRate] = std::to_string(static_cast<int>(mBudget.PolicePercent() * 100.0f)) + "%";

    TextPanelText[TextPanelId::TransportAllocated] = NumberToDollarDecimal(mBudget.RoadFundsGranted());
    TextPanelText[TextPanelId::TransportNeeded] = NumberToDollarDecimal(mBudget.RoadFundsNeeded());
    TextPanelText[TextPanelId::TransportRate] = std::to_string(static_cast<int>(mBudget.RoadPercent() * 100.0f)) + "%";

    for (auto id : panels)
    {
        TextPanelAdjust[id] = { (int)roundf(TextPanelLayout.at(id).w * scale) - fonts.body->width(TextPanelText[id]), (int)roundf(TextPanelLayout.at(id).h * scale / 2) - fonts.body->height() / 2 };
    }
}
