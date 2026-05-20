#define NOMINMAX
#include <windows.h>

class WinMenu {
public:
	WinMenu();

};

extern bool minimapItemChecked;
extern bool budgetItemChecked;
extern bool evaluationItemChecked;
extern bool graphItemChecked;

void updateMenuItems();