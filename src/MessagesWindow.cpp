#include "MessagesWindow.h"
#include "main.h"
#include "FontSet.h"
#include "s_msg.h"

#include "SDL_include.h"

namespace {
    SDL_Rect bgRect = { 0, 0, 360, 216 };
}

MessagesWindow::MessagesWindow()
{
    size({ 360, 216 });
    mTitle = "Messages";
    closeButtonActive(true);
}

void MessagesWindow::draw()
{
    size({ (int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale) });
    WindowBase::draw();

    int stringsSpacing = (int)roundf(16.0f * scale);
    
    int i = 0;
    for (std::string s : LastMessages())
    {
        fonts.renderer->drawString(*fonts.body, s, { area().x + (int)roundf(10.0f * scale), area().y + (int)roundf(bgRect.h * scale) - (int)roundf(20.0f * scale) - stringsSpacing * i++ });
    }
}

void MessagesWindow::update()
{
}
