#pragma once

#include "main.h"
#include "WindowBase.h"
#include "Texture.h"

class AboutWindow : public WindowBase
{
public:
    AboutWindow();
    AboutWindow(const AboutWindow&) = delete;
    const AboutWindow operator=(const AboutWindow&) = delete;

	void draw();
    void update();

    SDL_Texture* aboutTexture;
    int aboutWidth, aboutHeight;
};