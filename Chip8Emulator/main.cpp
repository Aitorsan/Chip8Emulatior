#define OLC_PGE_APPLICATION
#include "olcGameEngine/olcPixelGameEngine.hpp"
#include "Chip8.h"


class EmuWindow : public olc::PixelGameEngine
{
public:
	EmuWindow()
		:mcpu{ nullptr }
	{
		mcpu = new Chip8();
		mcpu->AskForRoom();
	}
	~EmuWindow()
	{
		delete mcpu;
		mcpu = nullptr;
	}
	virtual bool OnUserCreate() override{

		sAppName = "Chip8 emulator";
		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)override {
		Sleep(10);
		HandleInput();
		while (!mcpu->NeedsRedraw())
		{
			mcpu->DoCycle(fElapsedTime);

		}
		
		DrawBuffer();
		mcpu->RestoreRedraw();
		return true;
	}

	void DrawBuffer() {
		for (int y = 0; y < ScreenHeight(); ++y)
		{
			for (int x = 0; x < ScreenWidth(); ++x)
			{
				if (mcpu->GetScreenBuffer()[y* ScreenWidth() + x] == 1)
					Draw(x, y);
				else
					Draw(x, y, olc::Pixel(0,0,0));
			}
		}
	}
	void HandleInput() {
		for (int i = 0; i < 16; ++i)
			{
			if (GetKey(mcpu->keys_mapped[i].first).bPressed|| GetKey(mcpu->keys_mapped[i].first).bHeld) 
			{
			    if(GetKey(mcpu->keys_mapped[i].first).bPressed)
					std::cout << "Pressed\n";
				if (GetKey(mcpu->keys_mapped[i].first).bHeld)
					std::cout << "held\n";

			}
			mcpu->keys_mapped[i].second = GetKey((olc::Key)mcpu->keys_mapped[i].first).bPressed;

		}
	
	
	}
private:
	Chip8* mcpu;
};



int main()
{

	EmuWindow window;
	window.Construct(65, 32, 8, 8);
	window.Start();
	system("pause");
	return 0;
}