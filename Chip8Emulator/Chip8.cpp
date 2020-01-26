#include "Chip8.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

    #define EXECUTE(function_pointer,arg) ((this->*function_pointer)(arg)) //OMG dude thats crazy stuff
    #define DEBUG(operation,instruction) \
     [&]() {\
	std::stringstream stream;\
	stream << std::hex << operation;\
	std::cout << stream.str()<< " " <<instruction<<std::endl;Sleep(10);}();

	std::map < const unsigned short, typename Chip8::Instruction_func> instruction_set 
	{
		 {0x0000, &Chip8::OpSysAddress}	
		,{0x00E0, &Chip8::OpClear}
		,{0x00EE, &Chip8::OpReturn}
		,{0x1000, &Chip8::OpJump}
		,{0x2000, &Chip8::OpCall}
		,{0x3000, &Chip8::OpSkipNextIfValueEqVx}
		,{0x4000, &Chip8::OpSkipNextIfValueNotEqVx}
		,{0x5000, &Chip8::OpSkipNextIfVxEqVy}
		,{0x6000, &Chip8::OpLoadValueIntoVx}
		,{0x7000, &Chip8::OpAddValueToVx}
		,{0x8000, &Chip8::OpVxRegOperations}
		,{0x9000, &Chip8::OpSkipNextIfVxNotEqVy}
		,{0xA000, &Chip8::OpLoadValueIn_I}
		,{0xB000, &Chip8::OpJumpOffsetFromV0}
		,{0xC000, &Chip8::OpSetVxRandomByte}
		,{0xD000, &Chip8::OpDraw }
		,{0xE000, &Chip8::OpVxSkipOperations}
		,{0xF000, &Chip8::OpLoadOperations}
	};

	constexpr unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	
Chip8::Chip8()
	: m_EndIterator{ instruction_set.cend()}
	, cRam{ }
	, uStack{ }
	, cV{0x00}
	, uPc{0x200}
	, uI{0x000}
	, ucSound_timer{0x00}
    , ucDelay_timer{0x00}
	, cScreenBuffer{}
	, needRedraw{false}
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	memset(cRam, 0xFF,RAM_SIZE);
	memset(cScreenBuffer, 0, WIDTH*HEIGHT);
	CopyFontSetInMemory();
}

Chip8::~Chip8()
{
}

void Chip8::CopyFontSetInMemory()
{
	for (int i = 0; i < 80 ; ++i)
		cRam[i] =chip8_fontset[i];
}

bool Chip8::loadGameInMemory(const char* fileName)
{ 
	 std::ifstream reader(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	 if (!reader.is_open())
		 return false;

	 std::ifstream::pos_type size_f = reader.tellg();

	 char* buffer = new char[(std::size_t)size_f];
	 memset(buffer, 11, (std::size_t)size_f);

	 reader.seekg(0,std::ios::beg);
	 reader.read(buffer,size_f);
	 reader.close();
	 //save in ram starting at location 0x200
	 for (int i = 0; i < size_f; ++i)
	 {
		 cRam[512 + i] = buffer[i];
	 }
	 delete[] buffer;
	 return true;
}

 void Chip8::AskForRoom()
 {
	 std::cout << "Introduce room: \n>";
	 std::string folder = "Rooms/";
	 std::string room;
	 std::getline(std::cin, room);
	 folder += room;
	 loadGameInMemory(folder.c_str());
 }

 void Chip8::DoCycle(float fElapsedTime)
 {
	 //Fetch
	 unsigned short raw_instruction = Fetch();
	 //Decode
     Instruction operation = Decode(raw_instruction);
	 //Execute instruction
	 Execute(operation);
	// checkTimers
	 CheckTimers();
 }
 Chip8::Instruction Chip8::Decode(unsigned short raw_instruction)
 {
	 Instruction instruction{  0, nullptr };
	 std::map<unsigned short, Instruction_func>::const_iterator it = instruction_set.find(raw_instruction);
	 
	 if  (  it != m_EndIterator)
	 {
		 instruction.opcode = raw_instruction;
		 instruction.function = it->second;
	 }
	 else
	 {
		 it = instruction_set.find(raw_instruction & 0xF000);
		 if (it != m_EndIterator)
		 {
			 instruction.opcode = raw_instruction;
			 instruction.function = it->second;
		 }
	 }
	 return instruction;
 }

 unsigned short Chip8::Fetch()
 {    
	 unsigned short opcode = 0;
	 //read from memory
	 if (uPc < RAM_SIZE)
	 {
	    opcode = cRam[uPc];
		opcode <<= 8;
		opcode |= (cRam[uPc+1] );
	 }
	 return opcode;
 }

 void Chip8::Execute(Instruction& instruction)
 {
	 if (instruction.function != nullptr)
		 EXECUTE(instruction.function,instruction.opcode);
	 else
		 std::cerr << "Operation not recognize" << std::endl;
 }

 void Chip8::CheckTimers()
 {
	 if (ucDelay_timer > 0)
		 --ucDelay_timer;
	 
	 if (ucSound_timer > 0) 
	 {
		 if (ucSound_timer == 1) 
		 {
			 //make sound not sure how yet
		 }
		 --ucSound_timer;
	 }
 }

 bool Chip8::NeedsRedraw()
 {
	 return needRedraw;
 }

 void Chip8::RestoreRedraw()
 {
	 needRedraw = false;
 }

 unsigned char* Chip8::GetScreenBuffer()
 {
	 return cScreenBuffer;
 }

 void Chip8::OpSysAddress(unsigned short opcode)
 {	 //TODO:
/*jump to a machine code routine at nnn.
  This instruction is only used on the old computers on which Chip-8 was originally implemented. 
  It is ignored by modern interpreters.
	 */
	 DEBUG(opcode,"SYS addr")
 }

 //Dxyk  
 void Chip8::OpDraw(unsigned short opcode)
 {
	 unsigned short x = cV[(opcode & 0x0F00) >> 8];
	 unsigned short y = cV[(opcode & 0x00F0) >> 4];
	 unsigned short n = (opcode & 0x000F);
	 DEBUG(opcode, std::string("DRW V[") + std::to_string( x ) +std::string("], V[") + std::to_string( y ) +std::string("], ") +std::to_string( n ))
	 cV[0xF] = 0x0;
	 for (unsigned int yr = 0; yr <  n; ++yr)
	 {
		 unsigned char line_byte = cRam[uI +yr ];// 1 byte at the time 
		  for (int xc = 0; xc < 8; ++xc)
		  {
			  unsigned char pixel = line_byte & (0x80 >> xc);
			  if (pixel != 0x0)
			  {
				  unsigned short xpos = (x + xc); // offset from the original x that correspond to this pixel
				  unsigned short ypos = (y + yr); // offset from the original y that correspond to this pixel
				  unsigned short buffIndex = ypos * WIDTH + xpos;

				  if (1 == cScreenBuffer[buffIndex]) 
				  {
					  cV[0xF] = 1;
				  }
				  cScreenBuffer[buffIndex] ^= 1;
			  }
		  }
	 }
	 needRedraw = true;
	 //increment program counter
	 uPc += 2;
 }

 void Chip8::OpClear(unsigned short opcode)
 {
	 //Clear the display
	 DEBUG(opcode,"CLS")
	 for (int y = 0; y < HEIGHT; ++y)
	 {
		 for (int x = 0; x < WIDTH; ++x)
		 {
			 cScreenBuffer[y* WIDTH + x] = 0;
		 }
	 }
	 uPc += 2;
 }

 void Chip8::OpReturn(unsigned short opcode)
 {
	 DEBUG(opcode, "RET")
	 uPc = uStack.pop();
	 uPc += 2; //Important!! Increment program counter to obtain the next instruction. this caused me a headache!! LOL
 }
 //1nnn
 void Chip8::OpJump(unsigned short opcode)
 {
	 DEBUG(opcode, std::string("JP ")+ std::to_string(opcode& 0x0FFF))
	 uPc = (opcode & 0x0FFF);
 }

 //2nnn call
 void Chip8::OpCall(unsigned short opcode)
 {
	 DEBUG(opcode,std::string("CALL ")+std::to_string(opcode & 0x0FFF))
	 uStack.push(uPc); 
	 uPc = (opcode & 0x0FFF);
 }

 // 3xkk
 void Chip8::OpSkipNextIfValueEqVx(unsigned short opcode)
 {
	 unsigned short x = (opcode & 0x0F00)>>8;
	 unsigned char kk = (opcode & 0x00FF);
	 DEBUG(opcode,std::string("SE V[")+std::to_string(x)+std::string ("], ")+ std::to_string(kk))
	 if (cV[x] == kk)
	 {
		 uPc += 2;
	 }
	 uPc += 2;
 }

 //4xkk
 void Chip8::OpSkipNextIfValueNotEqVx(unsigned short opcode)
 {
	 unsigned short x = (opcode & 0x0F00)>>8;
	 unsigned char kk = (opcode & 0x00FF);
	
	 if (cV[x] != kk)
	 {
		 uPc += 2;
	 }
	 uPc += 2;
 }

 //5xy0
 void Chip8::OpSkipNextIfVxEqVy(unsigned short opcode)
 {
	 unsigned short x = (opcode & 0x0F00)>>8;
	 unsigned short y = (opcode & 0x00F0)>>4;
	
	 if (cV[x] == cV[y])
		 uPc += 2;

	 uPc += 2;
 }

 //6xkk Set Vx = kk
 void Chip8::OpLoadValueIntoVx(unsigned short opcode)
 {
	 unsigned char kk = (opcode & 0x00FF);
	 unsigned char x = (opcode & 0x0F00) >> 8;
	 cV[x] = kk;
	 uPc += 2;
 }

 //7xkk
 void Chip8::OpAddValueToVx(unsigned short opcode)
 {
	 unsigned short x = (opcode & 0x0F00)>>8;
	 unsigned char kk = (opcode & 0x00FF);
	 cV[x] += kk;
	 uPc += 2;
 }

 //8xy 0..7 and E
 void Chip8::OpVxRegOperations(unsigned short opcode)
 {
	 unsigned short x = (opcode & 0x0F00) >> 8;
	 unsigned short y = (opcode & 0x00F0) >> 4;
	 switch (opcode & 0x000F)
	 {
	 case 0x0:
		 cV[x] = cV[y]; 
		 DEBUG(opcode,std::string("LD V[")+std::to_string(x) +std::string("], V[")+ std::to_string(y)+std::string("]"))
		 break;
	 case 0x1:
		 cV[x] |= cV[y];
		 break;
	 case 0x2:
		 cV[x] &= cV[y];
		 break;	 
	 case 0x3:
		 cV[x] ^= cV[y];
		 break;
	 case 0x4:
		 //v[f] is set to 1 when there is a carry and 0 when there isn't
		 cV[x]  += cV[y];
		 cV[0xF] = cV[x] >( 0xFF-cV[y])? 1:0 ;
		 break;
	 case 0x5:
		 //VF is set to 0 when there's a borrow, and 1 when there isn't.
		 cV[0xF] = cV[x] > cV[y] ? 1 : 0;
		 cV[x] -= cV[y];
		 break;
	 case 0x6:
		 //If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
		 cV[0xF] = cV[x] & 0x1;
		 cV[x] >>= 0x1;
		 break;
	 case 0x7:
		//if Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
		 cV[0xF] = cV[y] > cV[x] ? 1 : 0;
		 cV[x] -= cV[y];
		 break;
	 case 0xE:
		// If the most - significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
		 cV[0xF] = cV[x] & 0x80;
		 cV[x] <<= 0x1;
		 break;
	 default:
		 break;
	 }
	 uPc += 2;
 }

 //9xy0
 void Chip8::OpSkipNextIfVxNotEqVy(unsigned short opcode)
 {
	 if (cV[(opcode & 0x0F00)>>8] != cV[(opcode & 0x00F0)>>4])
	 {
		 uPc += 2;// 2 bytes
	 }
	 uPc += 2;
 }

 //Akkk
 void Chip8::OpLoadValueIn_I(unsigned short opcode)
 {
	 uI = (opcode & 0x0FFF);
	 uPc += 2;
 }

 //Bkkk
 void Chip8::OpJumpOffsetFromV0(unsigned short opcode)
 {
	 unsigned short kkk = (opcode & 0x0FFF);
	 uPc = cV[0] + kkk;
 }

 //Cxkk random
 void Chip8::OpSetVxRandomByte(unsigned short opcode)
 {
	 unsigned short x = opcode & 0x0F00;
	 cV[x] = (opcode & 0x00FF) & (rand() % 256);
	 uPc += 2;
 }
 //ex
 void Chip8::OpVxSkipOperations(unsigned short opcode)
 {  
	 unsigned short x = opcode & 0x0F00;
	 switch (opcode & 0x00FF)
	 {
	 case 0x9E:
		 if (true == keys_mapped[cV[x]].second )
			 uPc += 2;
		 break;
	 case 0xA1:
		if (false == keys_mapped[cV[x]].second )
			 uPc += 2;
		 break;
	 default:
		 break;
	 }
	 uPc += 2;
 }

 //Fxkk
 void Chip8::OpLoadOperations(unsigned short opcode)
 {
	 unsigned short x = (opcode & 0x0F00) >> 8;
	 unsigned short kk = opcode& 0x00FF;
	 switch (kk)
	 {
	 case 0x07:
		 cV[x] = ucDelay_timer; 
		 break;
	 case 0x0A:
	 {
		 // wait for key pressed,store the value of the key in Vx
		 bool pressed = false;
		 for (int i = 0; i < sizeof(keys_mapped) / sizeof(keys_mapped[0]);++i)
		 {
			 if (keys_mapped[i].second)
			 {
				 cV[x] = i;
				 pressed = true;
			 }
		 }
		 if (!pressed) return;
		 break;
	 }
	 case 0x15:
		 ucDelay_timer = cV[x];
		 break;
	 case 0x18:
		 ucSound_timer = cV[x];
		 break;
	 case 0x1E:
		 cV[0xF] = (uI + cV[x]) > 0xFFF ? 1 : 0;
		 uI += cV[x];
		 break;
	 case 0x29:
		 uI = cV[x]*0x5;
		 break;
	 case 0x33:
	 {
		 unsigned short xval = cV[x];
		 cRam[uI] = xval / 100; // hundreds
		 cRam[uI + 1] = (xval / 10) % 10; // tens
		 cRam[uI + 2] = (xval % 100) % 10; // ones
		 break;
	 }
	 case 0x55:
		 for (int i = 0; i <= x; ++i)
			 cRam[uI + i] = cV[i];
		 break;
	 case 0x65:
		 for (int i = 0; i <= x; ++i)
			 cV[i] = cRam[uI + i];
		 break;
	 default:
		 break;
	 }
	 uPc += 2;
 }