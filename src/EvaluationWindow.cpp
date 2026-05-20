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
#include "EvaluationWindow.h"
#include "main.h"
#include "FontSet.h"

#include "Evaluation.h"


namespace
{
    const SDL_Rect BgRect{ 0, 0, 400, 300 };

    constexpr SDL_Rect YesNoRect{ 12, 30, 160, 70 };
    constexpr SDL_Rect OpinionRect{ 12, 130, 234, 115 };
    constexpr SDL_Rect StatisticsRect{ 210, 30, 234, 193 };

    Vector<int> ContentPanelPadding{ 2, 2 };

    int DualColumnOffset = 0;

    constexpr auto stringLength = [](std::string a, std::string b)
        {
            return a.length() < b.length();
        };


    struct TextColumnMeta
    {
        StringRender& stringRenderer;
        Font& font;
        const MPoint<int> startPoint;
        int lineSpacing;
        const std::vector<std::string> columns;
    };

    void drawTextColumn(const TextColumnMeta& textColumnMeta)
    {
        int offset = 0;
        for (auto& columnText : textColumnMeta.columns)
        {
            textColumnMeta.stringRenderer.drawString(textColumnMeta.font, columnText, textColumnMeta.startPoint + Vector<int>{ 0, offset });
            offset += textColumnMeta.font.height();
        }
    }
};


EvaluationWindow::EvaluationWindow(SDL_Renderer* renderer):
    mRenderer{ renderer },
    mStringRenderer{ renderer }
{
    mTitle = "City Evaluation";
    closeButtonActive(true);
    size({ BgRect.w, BgRect.h });

    SDL_SetTextureBlendMode(mTextTexture.texture, SDL_BLENDMODE_BLEND);
}


void EvaluationWindow::drawYesNoPanel()
{
    MPoint<int> panelStart =
    scalePoint({
        YesNoRect.x + ContentPanelPadding.x,
        YesNoRect.y + ContentPanelPadding.y
    });

    mStringRenderer.drawString(*fonts.title, "Public Opinion", panelStart);
    
    mStringRenderer.drawString(*fonts.caption, "Is the Mayor doing a good job?", panelStart + Vector<int>{ 0, mTitleSpacing });

    const TextColumnMeta yesNoLabels
    {
        mStringRenderer,
        * fonts.body,
        panelStart + Vector<int>{ (int)roundf(58 * scale), mTitleSpacing + mLineSpacing * 2},
        mLineSpacing,
        { "Yes", "No" }
    };

    drawTextColumn(yesNoLabels);

    const TextColumnMeta yesNoValues
    {
        mStringRenderer,
        *fonts.body,
        panelStart + Vector<int>{ (int)roundf(96 * scale) + (int)roundf(0 * scale), mTitleSpacing + mLineSpacing * 2},
        mLineSpacing,
        { mEvaluation.mGoodYes, mEvaluation.mGoodNo }
    };

    drawTextColumn(yesNoValues);
}


void EvaluationWindow::drawOpinionPanel()
{
    const MPoint<int> panelStart =
    scalePoint({
        OpinionRect.x + ContentPanelPadding.x,
        OpinionRect.y + ContentPanelPadding.y
    });

    mStringRenderer.drawString(*fonts.body, "What are the biggest issues?", panelStart + Vector<int>{ 0, mTitleSpacing });

    const TextColumnMeta opinionLabels
    {
        mStringRenderer,
        *fonts.caption,
        panelStart + Vector<int>{ (int)roundf(20 * scale), mTitleSpacing * 2 },
        mLineSpacing,
        {
            mEvaluation.problemString[0],
            mEvaluation.problemString[1],
            mEvaluation.problemString[2],
            mEvaluation.problemString[3]
        }
    };

    drawTextColumn(opinionLabels);

    const TextColumnMeta opinionValues
    {
        mStringRenderer,
        *fonts.caption,
        panelStart + Vector<int>{ (int)roundf(132 * scale), mTitleSpacing * 2 },
        mLineSpacing,
        {
            mEvaluation.problemVote[0],
            mEvaluation.problemVote[1],
            mEvaluation.problemVote[2],
            mEvaluation.problemVote[3]
        }
    };

    drawTextColumn(opinionValues);
}


void EvaluationWindow::drawStatsPanel()
{
    const MPoint<int> panelStart =
    scalePoint({
        StatisticsRect.x + ContentPanelPadding.x,
        StatisticsRect.y + ContentPanelPadding.y
    });
    
    mStringRenderer.drawString(*fonts.title, "Statistics", panelStart);

    const TextColumnMeta statLabels
    {
        mStringRenderer,
        *fonts.caption,
        panelStart + Vector<int>{ 0, mTitleSpacing },
        mLineSpacing,
        {
            "Population",
            "Net Migration",
            "Assessed Value",
            "City Class",
            "Game Level"
        }
    };

    drawTextColumn(statLabels);

    const auto it = std::max_element(statLabels.columns.begin(), statLabels.columns.end(), stringLength);
    DualColumnOffset = statLabels.font.width(*it) + (int)roundf(20 * scale);

    const TextColumnMeta statValues
    {
        mStringRenderer,
        *fonts.caption,
        {
            panelStart.x + DualColumnOffset,
            globalScaleRect(StatisticsRect).y + ContentPanelPadding.y + mTitleSpacing
        },
        mLineSpacing,
        {
            mEvaluation.pop,
            mEvaluation.delta,
            mEvaluation.mAssessedDollars,
            mEvaluation.mCityClass,
            mEvaluation.mCityLevel
        }
    };

    drawTextColumn(statValues);
}


void EvaluationWindow::drawScorePanel()
{
    
    MPoint<int> scorePanelStart =
    scalePoint({
        StatisticsRect.x + ContentPanelPadding.x,
        StatisticsRect.y + ContentPanelPadding.y + mTitleSpacing + mLineSpacing * 6
    });

    mStringRenderer.drawString(*fonts.title, "Overall City Score", scorePanelStart);
    mStringRenderer.drawString(*fonts.caption, "Range 0 - 1000", scorePanelStart + Vector<int>{ 0, mLineSpacing * 2});

    const TextColumnMeta scoreLabels
    {
        mStringRenderer,
        *fonts.caption,
        scorePanelStart + Vector<int>{ 0, mLineSpacing * 3 },
        mLineSpacing * 2,
        { "Current Score", "Annual Change" }
    };

    drawTextColumn(scoreLabels);

    const TextColumnMeta scoreValues
    {
        mStringRenderer,
        *fonts.caption,
        scorePanelStart + Vector<int>{ DualColumnOffset, mLineSpacing * 3 },
        mLineSpacing * 2,
        { mEvaluation.score, mEvaluation.changed }
    };

    drawTextColumn(scoreValues);
}


void EvaluationWindow::setEvaluation(const Evaluation& evaluation)
{
    mEvaluation = evaluation;

    
    
}

void EvaluationWindow::draw()
{
    size({ (int)roundf(BgRect.w * scale), (int)roundf(BgRect.h * scale) });
    WindowBase::draw();
    
    mLineSpacing = fonts.caption->height() + (int)roundf(1 * scale);
    mTitleSpacing = fonts.title->height() + (int)roundf(2 * scale);
    ContentPanelPadding = { (int)roundf(2 * scale), (int)roundf(2 * scale) };
    
    SDL_SetTextureColorMod(fonts.body->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(fonts.title->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(fonts.largeTitle->texture(), 0, 0, 0);
    
    drawYesNoPanel();
    drawOpinionPanel();
    drawStatsPanel();
    drawScorePanel();
    
}
