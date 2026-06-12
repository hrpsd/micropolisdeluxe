#include "AboutWindow.h"

#include "FontSet.h"

#include "SDL_include.h"

const char* aboutText =
"Micropolis Deluxe is developed by Sylvain Nowe. It is based on Micropolis. Micropolis is an Open Source project. Everyone can change its source code and contribute.\n\
\n\
You can find the source code of Micropolis Deluxe at https://github.com/hrpsd/Micropolis-SDL2\n\
\n\
Micropolis Deluxe Copyright Sylvain Nowe. Copyright Portions Leeor Dicker. Copyright Portions Don Hopkins. Copyright Portions Will Wright. Copyright Portions Electronic Arts. Note : This game is based on material developed by Electronic Arts. License : GNU GPL v3 with Appendix. Developers and publishers are not liable for software errors and possible damage to your devices and files.\n\
\n\
By using this software you accept the following and linked English license terms. If you do not accept these license terms, stop using the software and uninstall it from all your devices.\n\
\n\
Micropolis Deluxe GPL License Notice\n\
Micropolis Deluxe. A similiar version of this game was released for the Unix platform in or about 1990 and has been modified for inclusion in the One Laptop Per Child program. Copyright(C) 1989 - 2007 Electronic Arts Inc. If you need assistance with the UNIX version of Micropolis, you may contact :\n\
http://wiki.laptop.org/go/Micropolis or email micropolis@laptop.org.\n\
\n\
If you need assistance with Micropolis Deluxe, you may contact : email sylvain.nowe@gmail.com\n\
\n\
This program is free software. You can redistribute the sourcecode and Micropolis graphics and /or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses\n\
\n\
ADDITIONAL TERMS per GNU GPL Section 7\n\
No trademark or publicity rights are granted. This license does NOT give you any right, title or interest in the trademark SimCity or any other Electronic Arts trademark. You may not distribute any modification of this program using the trademark SimCity or claim any affliation or association with Electronic Arts Inc. or its employees.\n\
\n\
Any propagation or conveyance of this program must include this copyright notice and these terms.\n\
\n\
If you convey this program(or any modifications of it) and assume contractual liability for the program to recipients of it, you agree to indemnify Electronic Arts for any liability that those contractual assumptions impose on Electronic Arts.\n\
\n\
You may not misrepresent the origins of this program; modified versions of the program must be marked as such and not identified as the original program.";

SDL_Rect bgRect = { 0, 0, 1100, 700 };

AboutWindow::AboutWindow()
{
    size({ (int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale) });
    SDL_Surface* aboutSurface = TTF_RenderText_Blended_Wrapped(fonts.body->mFontInfo.font, aboutText, SDL_Color{ 0, 0, 0 }, (int)roundf(bgRect.w * 0.95f));
    aboutWidth = aboutSurface->w;
    aboutHeight = aboutSurface->h;
    aboutTexture = SDL_CreateTextureFromSurface(mainWindowRenderer, aboutSurface);
    mTitle = "About Micropolis Deluxe";
    closeButtonActive(true);
}

void AboutWindow::draw()
{
    size({ (int)roundf(bgRect.w * scale), (int)roundf(bgRect.h * scale) });
    WindowBase::draw();

    SDL_Rect rect = { area().x + (int)roundf(bgRect.w * 0.025f * scale), area().y + mTitleBarArea.height + (int)roundf(bgRect.h * 0.025f * scale), (int)roundf(aboutWidth * scale), (int)roundf(aboutHeight * scale) };
    SDL_RenderCopy(mainWindowRenderer, aboutTexture, nullptr, &rect);
}

void AboutWindow::update()
{
}
