//
//  SavedGamesWindow.cpp
//  Micropolis
//
//  Created by Toto on 20/07/2025.
//

#include "SavedGamesWindow.h"

#include "main.h"

#include "w_resrc.h"

#include <array>
#include <filesystem>
#include <map>

namespace
{
    constexpr SDL_Rect BgRect{ 0, 0, 900, 600 };
    constexpr SDL_Rect CloseButtonRect{ 61, 221, 142, 20 };
};

const int cols = 6;

SavedGamesWindow::SavedGamesWindow(SDL_Renderer* renderer) :
    mRenderer{ renderer },
    mStringRenderer{ renderer },
    mDownButton(loadTexture(mainWindowRenderer, "images/down.png")),
    mUpButton(loadTexture(mainWindowRenderer, "images/up.png"))
{
    size({ BgRect.w, BgRect.h });

    mTitle = "Load City";
    closeButtonActive(true);
    numCities = 0;
    rowOffset = 0;
}

void SavedGamesWindow::draw()
{
    size({ (int)roundf(BgRect.w * scale), (int)roundf(BgRect.h * scale) });
    WindowBase::draw();
    
    namespace fs = std::filesystem;
    fs::path someDir(documentsPath);
    fs::directory_iterator end_iter;

    typedef std::multimap<fs::file_time_type, fs::path, std::greater<fs::file_time_type>> result_set_t;
    result_set_t result_set;

    if ( fs::exists(someDir) && fs::is_directory(someDir))
    {
      for( fs::directory_iterator dir_iter(someDir) ; dir_iter != end_iter ; ++dir_iter)
      {
        if (fs::is_regular_file(dir_iter->status()) )
        {
          result_set.insert(result_set_t::value_type(fs::last_write_time(dir_iter->path()), *dir_iter));
        }
      }
    }
 
    int i = 0;
    
    int width  = (int)roundf(120 * scale);
    int height = (int)roundf(100 * scale);
    
    int spacing = (int)roundf(24 * scale);
    
    int xOffset = (int)roundf((area().width - cols * (width + spacing) + spacing) / 2.0f);
    int yOffset = mTitleBarArea.height + (int)roundf((area().height - mTitleBarArea.height - 4 * (height + spacing) + spacing) / 2.0f);
    
    numCities = (int)result_set.size();
    
    mButtons.clear();
    mPaths.clear();
    
    for (auto const& [time, path] : result_set)
    {
        if (i >= rowOffset * cols && i < std::min<int>(numCities, (rowOffset + 4) * cols))
        {
            int j = i - rowOffset * cols;
            
            SDL_Rect buttonRect = { area().x + xOffset + (j % cols) * (width + spacing), area().y + yOffset + (j / cols) * (height + spacing), width, height};
            
            mButtons.push_back(buttonRect);
            mPaths.push_back(path.string());
            
            drawThumbCityInRect(path.string(), buttonRect);
        }
        i++;
    }
    
    if (rowOffset > 0) {
        mUpRect = { area().x + area().width - mTitleBarArea.height, area().y + mTitleBarArea.height, mTitleBarArea.height, mTitleBarArea.height };
        SDL_RenderCopy(mainWindowRenderer, mUpButton.texture, &mUpButton.area, &mUpRect);
    }
    
    if (rowOffset < numCities / cols - 3) {
        mDownRect = { area().x + area().width - mTitleBarArea.height, area().y + area().height - mTitleBarArea.height, mTitleBarArea.height, mTitleBarArea.height };
        SDL_RenderCopy(mainWindowRenderer, mDownButton.texture, &mDownButton.area, &mDownRect);
    }
}


void SavedGamesWindow::onMouseDown(const MPoint<int>& position)
{
    const MRectangle<int> closeButtonRect =
    {
        area().x + CloseButtonRect.x,
        area().y + CloseButtonRect.y,
        CloseButtonRect.w,
        CloseButtonRect.h
    };
    
    if (closeButtonRect.contains(position))
    {
        hide();
    }
    
    const SDL_Point& pt{ position.x, position.y };
    
    if (SDL_PointInRect(&pt, &mUpRect))
    {
        if (rowOffset > 0)
        {
            rowOffset--;
        }
    }
    
    if (SDL_PointInRect(&pt, &mDownRect))
    {
        if (rowOffset < numCities / cols - 3)
        {
            rowOffset++;
        }
    }
    
    int i = 0;
    for (auto buttonRect : mButtons)
    {
        if (SDL_PointInRect(&pt, &buttonRect))
        {
            openGame(mPaths[i]);
            appWindows.home->lastPitch = 0;
        }
        i++;
    }
}
