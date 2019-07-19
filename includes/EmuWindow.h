
#include "olcGameEngine/olcPixelGameEngine.hpp"
#include "Chip8.h"
class Chip8;

class EmuWindow : public olc::PixelGameEngine
{
public:
	EmuWindow();
	~EmuWindow();
	virtual bool OnUserCreate()override;
	virtual bool OnUserUpdate(float fElapsedTime) override;

	void DrawBuffer();
	void HandleInput();
private:
	Chip8* mcpu;
};

EmuWindow::EmuWindow()
	:mcpu{ nullptr }
{
	mcpu = new Chip8();
	mcpu->AskForRoom();
}

EmuWindow::~EmuWindow()
{
	delete mcpu;
	mcpu = nullptr;
}

bool EmuWindow::OnUserCreate()
{
	sAppName = "Chip8 emulator";
	return true;
}
bool EmuWindow::OnUserUpdate(float fElapsedTime)
{
	Sleep(10);

	//while (!mcpu->NeedsRedraw())
	{
		mcpu->DoCycle(fElapsedTime);

	}
	DrawBuffer();
	HandleInput();
	mcpu->RestoreRedraw();
	return true;
}

void EmuWindow::DrawBuffer()
{
	for (int y = 0; y < ScreenHeight(); ++y)
	{
		for (int x = 0; x < ScreenWidth(); ++x)
		{
			if (mcpu->GetScreenBuffer()[y* ScreenWidth() + x] == 1)
				Draw(x, y);
			else
				Draw(x, y, 0);
		}
	}
}

void EmuWindow::HandleInput()
{
	for (int i = 0; i < 16; ++i)
		mcpu->keys_mapped[i].second = GetKey((olc::Key)mcpu->keys_mapped[i].first).bPressed;
}


