#include "EmuWindow.h"
#include <random>
#include "Chip8.h"
#define OLC_PGE_APPLICATION
#include "olcGameEngine/olcPixelGameEngine.hpp"
int randGen(const int& min, const int& max)
{
	static thread_local std::mt19937 generator(std::hash<std::thread::id>()(std::this_thread::get_id()));
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

EmuWindow ::EmuWindow()
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
	HandleInput();
	while (!mcpu->NeedsRedraw())
    {
		Sleep(9);
		mcpu->DoCycle(fElapsedTime);

	}
	DrawBuffer();
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
				Draw(x, y, olc::Pixel(0, 0, 0));
		}
	}
}

void EmuWindow::HandleInput()
{
	for (int i = 0; i < 16; ++i)
	{
		if (GetKey(mcpu->keys_mapped[i].first).bPressed || GetKey(mcpu->keys_mapped[i].first).bHeld)
		{
			if (GetKey(mcpu->keys_mapped[i].first).bPressed)
				std::cout << "Pressed\n";
			if (GetKey(mcpu->keys_mapped[i].first).bHeld)
				std::cout << "held\n";
			mcpu->keys_mapped[i].second = true;
		}
	}
}
