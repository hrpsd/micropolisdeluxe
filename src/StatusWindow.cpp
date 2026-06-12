#include "StatusWindow.h"
#include "main.h"
#include "FontSet.h"
#include "CityProperties.h"
#include "Colors.h"
#include "w_update.h"
#include "Evaluation.h"

#include "SDL_include.h"

namespace {
    SDL_Rect bgRect = { 0, 0, 360, 130 };
}

StatusWindow::StatusWindow()
{
    size({ (int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale) });
    mTitle = "City Status";
    closeButtonActive(false);
    position({ 0, 0 });
}

void StatusWindow::draw()
{
    size({ (int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale) });
    WindowBase::draw();

    drawValve();

    int stringsX = (int)roundf(100.0f * scale);
    int stringsY = (int)roundf(30.0f * scale);
    int stringsSpacing = (int)roundf(16.0f * scale);
    
    fonts.renderer->drawString(*fonts.body, cityProperties.CityName(), { area().x + stringsX, area().y + stringsY + stringsSpacing * 0 });
    fonts.renderer->drawString(*fonts.body, MonthString(static_cast<Month>(LastCityMonth())) + " " + std::to_string(CurrentYear()), { area().x + stringsX, area().y + stringsY + stringsSpacing * 1 });
    fonts.renderer->drawString(*fonts.body, "Population " + std::to_string(cityPopulation()), { area().x + stringsX, area().y + stringsY + stringsSpacing * 2 });
    fonts.renderer->drawString(*fonts.body, "Budget " + currentBudget, { area().x + stringsX, area().y + stringsY + stringsSpacing * 3 });
    fonts.renderer->drawString(*fonts.body, "Score " + std::to_string(cityScore()), { area().x + stringsX, area().y + stringsY + stringsSpacing * 4 });
}

void StatusWindow::update()
{
}

void StatusWindow::drawValve()
{
    double residentialPercent = static_cast<double>(RValve) / 1500.0;
    double commercialPercent = static_cast<double>(CValve) / 1500.0;
    double industrialPercent = static_cast<double>(IValve) / 1500.0;

    SDL_Rect RciDestination = { area().x + (int)roundf(5 * scale), area().y + fonts.body->height() + (int)roundf(10 * scale), (int)roundf(2 * RCI_Indicator.dimensions.x * scale), (int)roundf(2 * RCI_Indicator.dimensions.y * scale) };
    
    SDL_Rect ResidentialValveRect = { RciDestination.x + (int)roundf(18 * scale), RciDestination.y + (int)roundf(48 * scale), (int)roundf(8 * scale), 0 };
    SDL_Rect CommercialValveRect = { RciDestination.x + (int)roundf(36 * scale), RciDestination.y + (int)roundf(48 * scale), (int)roundf(8 * scale), 0 };
    SDL_Rect IndustrialValveRect = { RciDestination.x + (int)roundf(54 * scale), RciDestination.y + (int)roundf(48 * scale), (int)roundf(8 * scale), 0 };
    
    auto RciValveHeight = (int)roundf(20 * scale);
    
    ResidentialValveRect.h = -static_cast<int>(RciValveHeight * residentialPercent);
    CommercialValveRect.h = -static_cast<int>(RciValveHeight * commercialPercent);
    IndustrialValveRect.h = -static_cast<int>(RciValveHeight * industrialPercent);

    SDL_SetRenderDrawColor(mainWindowRenderer, Colors::Green.r, Colors::Green.g, Colors::Green.b, 255);
    SDL_RenderFillRect(mainWindowRenderer, &ResidentialValveRect);

    SDL_SetRenderDrawColor(mainWindowRenderer, Colors::MediumBlue.r, Colors::MediumBlue.g, Colors::MediumBlue.b, 255);
    SDL_RenderFillRect(mainWindowRenderer, &CommercialValveRect);

    SDL_SetRenderDrawColor(mainWindowRenderer, Colors::Gold.r, Colors::Gold.g, Colors::Gold.b, 255);
    SDL_RenderFillRect(mainWindowRenderer, &IndustrialValveRect);

    // not a huge fan of this
    SDL_Rect rciSrc{ 4, 19, 32, 11 };
    SDL_Rect rciDst{ RciDestination.x + (int)roundf(8 * scale), RciDestination.y + (int)roundf(38 * scale), (int)roundf(64 * scale), (int)roundf(22 * scale) };
    SDL_RenderCopy(mainWindowRenderer, RCI_Indicator.texture, &rciSrc, &rciDst);
}
