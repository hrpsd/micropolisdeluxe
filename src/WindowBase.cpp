#include "WindowBase.h"
#include "main.h"
#include "FontSet.h"
#include "MiniMapWindow.h"

namespace
{
    constexpr int TitleBarHeight = 24;
    constexpr int ClientAreaPadding = 8;
    constexpr Vector<int> CloseButtonSize{ 13, 13 };
    constexpr Vector<int> CloseButtonPosition{ 4, 4 };
}

bool WindowBase::UILoaded;
Texture WindowBase::mTitleBarTexture;
Texture WindowBase::mCloseButton;

WindowBase::WindowBase()
{
    if (!UILoaded) {
        mTitleBarTexture = loadTexture(mainWindowRenderer, "images/Concave - 512x512.png");
        mCloseButton = loadTexture(mainWindowRenderer, "images/close.png");
        UILoaded = true;
    }
}

bool WindowBase::visible() const
{
    return mVisible;
}


void WindowBase::toggleVisible()
{
    mVisible = !mVisible;
    mVisible ? show() : hide();
}


void WindowBase::show()
{
    mVisible = true;
    onShow();
}


void WindowBase::hide()
{
    mVisible = false;
    onHide();
}


void WindowBase::move(const Vector<int>& delta)
{
    mArea.startPoint(mArea.startPoint() + delta);
    mTitleBarArea.startPoint(mArea.startPoint());
    mClientArea.startPoint(mArea.startPoint() + Vector<int>{ClientAreaPadding, TitleBarHeight});
    mCloseButtonArea.startPoint(mArea.startPoint() + CloseButtonPosition);
    
    onMoved(delta);
}


void WindowBase::position(const MPoint<int>& point)
{
    mArea.startPoint(point);
    mTitleBarArea.startPoint(point);
    mClientArea.startPoint(point + Vector<int>{ClientAreaPadding, TitleBarHeight});
    onPositionChanged(point);
}


MPoint<int> WindowBase::position()
{
    return mArea.startPoint();
}


void WindowBase::size(const Vector<int>& size)
{
    mArea.size(size);
    mTitleBarArea.size({ size.x, (int)roundf(TitleBarHeight * scale) });
    mClientArea.size(size - Vector{ ClientAreaPadding * 2, TitleBarHeight + ClientAreaPadding });
    mCloseButtonArea.size(CloseButtonSize);
}


const MRectangle<int>& WindowBase::area() const
{
    return mArea;
}


const MRectangle<int>& WindowBase::clientArea() const
{
    return mClientArea;
}


void WindowBase::closeButtonActive(bool show)
{
    mCloseButtonActive = show;
}


void WindowBase::anchor()
{
    mAnchored = true;
}


void WindowBase::unanchor()
{
    mAnchored = false;
}


bool WindowBase::anchored() const
{
    return mAnchored;
}


void WindowBase::alwaysVisible(bool always)
{
    mAlwaysVisible = always;
}


bool WindowBase::alwaysVisible() const
{
    return mAlwaysVisible;
}


void WindowBase::injectMouseDown(const MPoint<int>& position)
{
    const SDL_Point& pt{ position.x, position.y };

    if (mCloseButtonActive && SDL_PointInRect(&pt, &mCloseRect))
    {
        hide();
        onWindowHide(this);

        if (this == savedGamesWindow.get())
        {
            appWindows.homeMiniMap->show();
        }
        return;
    }

    const SDL_Rect titlebar{ mTitleBarArea.x, mTitleBarArea.y, mTitleBarArea.width, mTitleBarArea.height };
    if (SDL_PointInRect(&pt, &titlebar) /*&& !anchored()*/)
    {
        lastMousePosition = position;
        mDragging = true;
        return;
    }

    onMouseDown(position);
}


void WindowBase::injectMouseUp()
{
    mDragging = false;
    onMouseUp();
}



void WindowBase::injectMouseMotion(const MPoint<int>& mousePosition)
{
    onMouseMotion(mousePosition);
    
    if (mAnchored || !mDragging) { return; }
    move(mousePosition - lastMousePosition);
    lastMousePosition = mousePosition;
}


void WindowBase::injectKeyDown(int32_t key)
{
    onKeyDown(key);
}

MPoint<int> WindowBase::scalePoint(MPoint<int> srcPoint)
{
    MPoint<int> res;
    
    res.x = area().x + (int)roundf(srcPoint.x * 720 / 900 * scale);
    res.y = area().y + (int)roundf(srcPoint.y * 720 / 900 * scale);
    
    return res;
}

SDL_Rect WindowBase::scaleRect(SDL_Rect srcRect, float scale)
{
    SDL_Rect dstRect = srcRect;
    dstRect.x = (int)roundf(dstRect.x * scale);
    dstRect.x += area().x;
    dstRect.y = (int)roundf(dstRect.y * scale);
    dstRect.y += area().y;
    dstRect.w = (int)roundf(dstRect.w * scale);
    dstRect.h = (int)roundf(dstRect.h * scale);
    
    return dstRect;
}

SDL_Rect WindowBase::globalScaleRect(SDL_Rect srcRect)
{
    return scaleRect(srcRect, scale);
}

void WindowBase::draw()
{
    const SDL_Rect rect = { area().x, area().y, area().width, area().height };
    
    SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(mainWindowRenderer, &rect);
    
    const SDL_Rect titleRect = { mTitleBarArea.x, mTitleBarArea.y, mTitleBarArea.width, mTitleBarArea.height };
    SDL_RenderCopy(mainWindowRenderer, mTitleBarTexture.texture, &mTitleBarTexture.area, &titleRect);
    
    if (mCloseButtonActive)
    {
        mCloseRect = { mTitleBarArea.x + mTitleBarArea.width - mTitleBarArea.height, mTitleBarArea.y, mTitleBarArea.height, mTitleBarArea.height };
        SDL_RenderCopy(mainWindowRenderer, mCloseButton.texture, &mCloseButton.area, &mCloseRect);
    }
    
    SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(mainWindowRenderer, &rect);
    
    SDL_SetTextureColorMod(fonts.body->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(fonts.title->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(fonts.largeTitle->texture(), 0, 0, 0);
    
    const int titleX = area().x + size().x / 2 - fonts.title->width(mTitle) / 2;

    StringRender mStringRenderer(mainWindowRenderer);
    mStringRenderer.drawString(*fonts.title, mTitle, { titleX, area().y + (int)roundf(0 * scale) });
}
