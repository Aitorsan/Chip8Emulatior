#pragma once
#include "olcGameEngine/olcPixelGameEngine.hpp"


class Chip8;


class EmuWindow : public olc::PixelGameEngine
{
public:
	EmuWindow();
	virtual ~EmuWindow();
	virtual bool OnUserCreate() override;
	virtual bool OnUserUpdate(float fElapsedTime)override;

	void DrawBuffer();

	void HandleInput();

private:
	Chip8* mcpu;
};