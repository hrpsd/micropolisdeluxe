#include "WinMenu.h"
#include "main.h"
#include "w_sound.h"
#include "w_util.h"
#include "s_disast.h"
#include "Sprite.h"
#include "gameOptions.h"
#include "MiniMapWindow.h"
#include "HomeMiniMapWindow.h"
#include "SaveAsWindow.h"
#include "CityProperties.h"
#include <SDL_syswm.h>

#define IDM_FILE_ABOUT 10
#define IDM_FILE_SAVE 11
#define IDM_FILE_SAVEAS 12
#define IDM_FILE_HOME 13
#define IDM_FILE_QUIT 14

#define IDM_OPTIONS_AUTOBUDGET 20
#define IDM_OPTIONS_AUTOBULLDOZE 21
#define IDM_OPTIONS_DISASTERS 22
#define IDM_OPTIONS_SOUNDS 23
#define IDM_OPTIONS_MUSIC 24
#define IDM_OPTIONS_ANIMATION 25
#define IDM_OPTIONS_NOTICES 27

#define IDM_DISASTERS_MONSTER 30
#define IDM_DISASTERS_FIRE 31
#define IDM_DISASTERS_FLOOD 32
#define IDM_DISASTERS_MELTDOWN 33
#define IDM_DISASTERS_TORNADO 34
#define IDM_DISASTERS_EARTHQUAKE 35

#define IDM_PRIORITY_SUPERFAST 40
#define IDM_PRIORITY_FAST 41
#define IDM_PRIORITY_NORMAL 42
#define IDM_PRIORITY_SLOW 43
#define IDM_PRIORITY_PAUSED 44

#define IDM_ZOOM_IN 50
#define IDM_ZOOM_OUT 51

#define IDM_WINDOWS_MINIMAP 60
#define IDM_WINDOWS_MESSAGES 61
#define IDM_WINDOWS_BUDGET 62
#define IDM_WINDOWS_EVALUATION 63
#define IDM_WINDOWS_GRAPH 64


bool minimapItemChecked = true;
bool messagesItemChecked = true;
bool budgetItemChecked = false;
bool evaluationItemChecked = false;
bool graphItemChecked = false;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
WNDPROC btnWndProc;



HWND GetHwnd() {
    SDL_SysWMinfo windowinfo;

    SDL_VERSION(&windowinfo.version)

    if (!SDL_GetWindowWMInfo(MainWindow, &windowinfo))
        return(NULL);
    return (windowinfo.info.win.window);
}

void updateMenuItems()
{
    HWND hwnd = GetHwnd();

    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_AUTOBUDGET, gameOptions.mAutoBudget ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_AUTOBULLDOZE, gameOptions.mAutoBulldoze ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_DISASTERS, gameOptions.mDisasters ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_SOUNDS, gameOptions.mPlaySounds ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_MUSIC, gameOptions.mPlayMusic ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_ANIMATION, gameOptions.mAnimation ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_OPTIONS_NOTICES, gameOptions.mNotices ? MF_CHECKED : MF_UNCHECKED);

    CheckMenuItem(GetMenu(hwnd), IDM_WINDOWS_MINIMAP, minimapItemChecked ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_WINDOWS_MESSAGES, messagesItemChecked ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_WINDOWS_BUDGET, budgetItemChecked ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_WINDOWS_EVALUATION, evaluationItemChecked ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_WINDOWS_GRAPH, graphItemChecked ? MF_CHECKED : MF_UNCHECKED);

    optionsChanged();
}

void AddMenus(HWND hwnd)
{
    HMENU hMenu, fileSubMenu, optionsSubMenu, disastersSubMenu, prioritySubMenu, zoomSubMenu, windowsSubMenu;
    
    hMenu = CreateMenu();
    
    fileSubMenu = CreatePopupMenu();
    AppendMenuW(fileSubMenu, MF_STRING, IDM_FILE_HOME, L"&Home Screen");
    AppendMenuW(fileSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(fileSubMenu, MF_STRING, IDM_FILE_SAVE, L"&Save City");
    AppendMenuW(fileSubMenu, MF_STRING, IDM_FILE_SAVEAS, L"&Save City as...");
    AppendMenuW(fileSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(fileSubMenu, MF_STRING, IDM_FILE_ABOUT, L"&About...");
    AppendMenuW(fileSubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(fileSubMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");    
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)fileSubMenu, L"&Micropolis");
    
    optionsSubMenu = CreatePopupMenu();
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_AUTOBUDGET, L"&Auto Budget");
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_AUTOBULLDOZE, L"&Auto Bulldoze");
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_DISASTERS, L"&Disasters");
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_SOUNDS, L"&Play Sounds");
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_MUSIC, L"&Play Music");
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_ANIMATION, L"&Animation");
    AppendMenuW(optionsSubMenu, MF_STRING, IDM_OPTIONS_NOTICES, L"&Notices");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)optionsSubMenu, L"&Options");

    disastersSubMenu = CreatePopupMenu();
    AppendMenuW(disastersSubMenu, MF_STRING, IDM_DISASTERS_MONSTER, L"&Monster");
    AppendMenuW(disastersSubMenu, MF_STRING, IDM_DISASTERS_FIRE, L"&Fire");
    AppendMenuW(disastersSubMenu, MF_STRING, IDM_DISASTERS_FLOOD, L"&Flood");
    AppendMenuW(disastersSubMenu, MF_STRING, IDM_DISASTERS_MELTDOWN, L"&Meltdown");
    AppendMenuW(disastersSubMenu, MF_STRING, IDM_DISASTERS_TORNADO, L"&Tornado");
    AppendMenuW(disastersSubMenu, MF_STRING, IDM_DISASTERS_EARTHQUAKE, L"&Earthquake");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)disastersSubMenu, L"&Disasters");

    prioritySubMenu = CreatePopupMenu();
    AppendMenuW(prioritySubMenu, MF_STRING, IDM_PRIORITY_SUPERFAST, L"&Super Fast");
    AppendMenuW(prioritySubMenu, MF_STRING, IDM_PRIORITY_FAST, L"&Fast");
    AppendMenuW(prioritySubMenu, MF_STRING, IDM_PRIORITY_NORMAL, L"&Normal");
    AppendMenuW(prioritySubMenu, MF_STRING, IDM_PRIORITY_SLOW, L"&Slow");
    AppendMenuW(prioritySubMenu, MF_STRING, IDM_PRIORITY_PAUSED, L"&Paused");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)prioritySubMenu, L"&Priority");

    zoomSubMenu = CreatePopupMenu();
    AppendMenuW(zoomSubMenu, MF_STRING, IDM_ZOOM_IN, L"&Zoom In");
    AppendMenuW(zoomSubMenu, MF_STRING, IDM_ZOOM_OUT, L"&Zoom Out");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)zoomSubMenu, L"&Zoom");
    
    windowsSubMenu = CreatePopupMenu();
    AppendMenuW(windowsSubMenu, MF_STRING, IDM_WINDOWS_MINIMAP, L"&Mini map");
    AppendMenuW(windowsSubMenu, MF_STRING, IDM_WINDOWS_MESSAGES, L"&Messages");
    AppendMenuW(windowsSubMenu, MF_STRING, IDM_WINDOWS_BUDGET, L"&Budget");
    AppendMenuW(windowsSubMenu, MF_STRING, IDM_WINDOWS_EVALUATION, L"&Evaluation");
    AppendMenuW(windowsSubMenu, MF_STRING, IDM_WINDOWS_GRAPH, L"&Graph");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)windowsSubMenu, L"&Windows");

    SetMenu(hwnd, hMenu);

    updateMenuItems();

    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_NORMAL, MF_CHECKED);
    CheckMenuItem(GetMenu(hwnd), IDM_WINDOWS_MINIMAP, MF_CHECKED);
}


WNDPROC prevWndProc;

LRESULT CALLBACK myNewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_COMMAND:

            switch (LOWORD(wParam)) {
                case IDM_FILE_ABOUT:
                    appWindows.homeMiniMap->hide();
                    showWindowAndBringToFront(*appWindows.about);
                    break;
                case IDM_FILE_SAVE:
                    saveGame();
                    break;
                case IDM_FILE_SAVEAS:
                    if (appWindows.saveAs)
                    {
                        appWindows.saveAs->setCityName(cityProperties.CityName());
                        // showWindowAndBringToFront() re-centers the window for us
                        // (since saveAs isn't status/messages), so no explicit centerWindow call.
                        showWindowAndBringToFront(*appWindows.saveAs);
                    }
                    break;
                case IDM_FILE_HOME:
                    showWindowAndBringToFront(*appWindows.home);
                    showWindowAndBringToFront(*appWindows.homeMiniMap);
                    break;
                case IDM_FILE_QUIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;

                case IDM_OPTIONS_AUTOBUDGET:
                    gameOptions.mAutoBudget ^= true;
                    break;
                case IDM_OPTIONS_AUTOBULLDOZE:
                    gameOptions.mAutoBulldoze ^= true;
                    break;
                case IDM_OPTIONS_DISASTERS:
                    gameOptions.mDisasters ^= true;
                    break;
                case IDM_OPTIONS_SOUNDS:
                    gameOptions.mPlaySounds ^= true;
                    break;
                case IDM_OPTIONS_MUSIC:
                    gameOptions.mPlayMusic ^= true;
                    break;
                case IDM_OPTIONS_ANIMATION:
                    gameOptions.mAnimation ^= true;
                    break;
                case IDM_OPTIONS_NOTICES:
                    gameOptions.mNotices ^= true;
                    break;

                case IDM_DISASTERS_MONSTER:
                    generateMonster();
                    break;
                case IDM_DISASTERS_FIRE:
                    MakeFire();
                    break;
                case IDM_DISASTERS_FLOOD:
                    MakeFlood();
                    break;
                case IDM_DISASTERS_MELTDOWN:
                    MakeMeltdown();
                    break;
                case IDM_DISASTERS_TORNADO:
                    generateTornado();
                    break;
                case IDM_DISASTERS_EARTHQUAKE:
                    MakeEarthquake();
                    break;

                case IDM_PRIORITY_SUPERFAST:
                    setSpeed((SimulationSpeed)4);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SUPERFAST, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_FAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_NORMAL, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SLOW, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_PAUSED, MF_UNCHECKED);
                    break;
                case IDM_PRIORITY_FAST:
                    setSpeed((SimulationSpeed)3);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SUPERFAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_FAST, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_NORMAL, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SLOW, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_PAUSED, MF_UNCHECKED);
                    break;
                case IDM_PRIORITY_NORMAL:
                    setSpeed((SimulationSpeed)2);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SUPERFAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_FAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_NORMAL, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SLOW, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_PAUSED, MF_UNCHECKED);
                    break;
                case IDM_PRIORITY_SLOW:
                    setSpeed((SimulationSpeed)1);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SUPERFAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_FAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_NORMAL, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SLOW, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_PAUSED, MF_UNCHECKED);
                    break;
                case IDM_PRIORITY_PAUSED:
                    setSpeed((SimulationSpeed)0);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SUPERFAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_FAST, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_NORMAL, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_SLOW, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDM_PRIORITY_PAUSED, MF_CHECKED);
                    break;

                case IDM_ZOOM_IN:
                {
                    int newRenderTileSize = std::min(64, std::max(16, renderTileSize + 1));
                    if (newRenderTileSize != renderTileSize)
                    {
                        renderTileSize = newRenderTileSize;
                        setRenderTileSize();
                    }
                } break;
                case IDM_ZOOM_OUT:
                {
                    int newRenderTileSize = std::min(64, std::max(16, renderTileSize - 1));
                    if (newRenderTileSize != renderTileSize)
                    {
                        renderTileSize = newRenderTileSize;
                        setRenderTileSize();
                    }
                } break;

                case IDM_WINDOWS_MINIMAP:
                    minimapItemChecked = !minimapItemChecked;
                    toggleMiniMapVisibility();
                    break;
                case IDM_WINDOWS_MESSAGES:
                    messagesItemChecked = !messagesItemChecked;
                    showMessagesWindow();
                    break;
                case IDM_WINDOWS_BUDGET:
                    budgetItemChecked = !budgetItemChecked;
                    showBudgetWindow();
                    break;
                case IDM_WINDOWS_EVALUATION:
                    evaluationItemChecked = !evaluationItemChecked;
                    showEvaluationWindow();
                    break;
                case IDM_WINDOWS_GRAPH:
                    graphItemChecked = !graphItemChecked;
                    showGraphWindow();
                    break;
            }

            break;
    }

    updateMenuItems();

    return CallWindowProc(prevWndProc, hwnd, uMsg, wParam, lParam);
}


WinMenu::WinMenu()
{
    prevWndProc = (WNDPROC)SetWindowLongPtr(GetHwnd(), GWLP_WNDPROC, (LONG_PTR)&myNewWndProc);

	AddMenus(GetHwnd());
}


