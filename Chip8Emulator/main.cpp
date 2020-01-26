
#include "EmuWindow.h"
int main()
{
	EmuWindow window;
	window.Construct(65, 32, 8, 8);
	window.Start();
	system("pause");
	return 0;
}