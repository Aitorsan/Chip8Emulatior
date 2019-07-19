#pragma once
#include "stack.hpp"
#include "olcGameEngine/olcPixelGameEngine.hpp"
#include <memory>
#include <map>
constexpr auto RAM_SIZE = 4096;
const unsigned int WIDTH = 65 ;
const unsigned int HEIGHT = 35;

class Chip8
{
public:
	using Instruction_func = void(Chip8::*)(unsigned short);
private:
	struct Instruction
	{
		unsigned short opcode;
		Instruction_func function;
	};
public:
	
	Chip8();
	~Chip8();
	void CopyFontSetInMemory();
	void AskForRoom();
	void DoCycle(float fElapsedTime);
	unsigned short Fetch();
	Instruction Decode(unsigned short opcode);
	void Execute(Instruction& operation);
	void CheckTimers();
	bool NeedsRedraw();
	void RestoreRedraw();
	unsigned char* GetScreenBuffer();

	//operations
	void OpSysAddress(unsigned short opcode);
	void OpDraw(unsigned short opcode);
	void OpClear(unsigned short opcode);
	void OpReturn(unsigned short opcode);
	void OpJump(unsigned short opcode);
	void OpCall(unsigned short opcode);
	void OpSkipNextIfValueEqVx(unsigned short opcode);
	void OpSkipNextIfValueNotEqVx(unsigned short opcode);
	void OpSkipNextIfVxEqVy(unsigned short opcode);
	void OpLoadValueIntoVx(unsigned short opcode);
	void OpAddValueToVx(unsigned short opcode);
	void OpVxRegOperations(unsigned short opcode);
	void OpSkipNextIfVxNotEqVy(unsigned short opcode);
	void OpLoadValueIn_I(unsigned short opcode);
	void OpJumpOffsetFromV0(unsigned short opcode);
	void OpSetVxRandomByte(unsigned short opcode);
	void OpVxSkipOperations(unsigned short opcode);
	void OpLoadOperations(unsigned short opcode);
protected:
	bool loadGameInMemory(const char* location);	
	
private:
	
	//iterator to the end of the instructions for convinience
	std::map <unsigned short, Instruction_func>::const_iterator m_EndIterator;
	//ram
	unsigned char cRam[RAM_SIZE];
	//stack of 16 , of 16 bits values
	stack<unsigned short, 16> uStack;
	// V registers ( 8 bit)
	unsigned char cV[0xF];
	//PC register ( 16 bit)
	unsigned short uPc;
	// I register (16 bit )
	unsigned short uI;
	//Sound timer count at 60 hertz
	unsigned char ucSound_timer;
	//Delay timer ( 8 bit) count at 60 hertz
	unsigned char ucDelay_timer;
	//Display buffer
	unsigned char cScreenBuffer[WIDTH*HEIGHT];
	//needs redraw
	bool needRedraw; 
	
public:
	//keys
	std::pair<olc::Key, bool> keys_mapped[16]
	{
		{olc::Key::X,false},{olc::Key::K1,false},{olc::Key::K2,false},
		{olc::Key::K3,false},{olc::Key::Q,false},{olc::Key::W,false},
		{olc::Key::E,false},{olc::Key::A,false},{olc::Key::S,false},
		{olc::Key::D,false},{olc::Key::Z,false},{olc::Key::C,false},
		{olc::Key::K4,false},{olc::Key::R,false},{olc::Key::F,false},
		{olc::Key::V,false}
	};
	
};


