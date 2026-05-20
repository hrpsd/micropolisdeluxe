#pragma once

#include "WindowBase.h"
#include "Texture.h"

class MessagesWindow : public WindowBase
{
public:
    MessagesWindow();
    MessagesWindow(const MessagesWindow&) = delete;
    const MessagesWindow operator=(const MessagesWindow&) = delete;

	void draw();
    void update();
};
