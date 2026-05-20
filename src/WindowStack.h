#pragma once

#include "Point.h"

#include <list>


class WindowBase;


class WindowStack final
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(WindowBase* window);
	void removeWindow(WindowBase* window);

	WindowBase* front();

	bool windowVisible() const;

	bool pointInWindow(const MPoint<int>& point) const;

	void updateStack(const MPoint<int>& point);

	void bringToFront(WindowBase* window);

	void hide();

	void draw();
	void update();

	void injectMouseUp();
	void injectMouseMotion(const MPoint<int>&);

	void reposition(float prevScale, float scale);

private:

	std::list<WindowBase*> mWindowList;
};
