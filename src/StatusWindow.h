#pragma once

#include "WindowBase.h"
#include "Texture.h"

class StatusWindow : public WindowBase
{
public:
    StatusWindow();
    StatusWindow(const StatusWindow&) = delete;
    const StatusWindow operator=(const StatusWindow&) = delete;

	void draw();
    void update();

private:
    void drawValve();
};
