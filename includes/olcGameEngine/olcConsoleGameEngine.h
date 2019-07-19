/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9

The Original & Best :P

One Lone Coder License
~~~~~~~~~~~~~~~~~~~~~~
- This software is Copyright (C) 2018 Javidx9
- This is free software
- This software comes with absolutely no warranty
- The copyright holder is not liable or responsible for anything
  this software does or does not
- You use this software at your own risk
- You can distribute this software
- You can modify this software
- Redistribution of this software or a derivative of this software
  must attribute the Copyright holder named above, in a manner
  visible to the end user

License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.

Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE

From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE

Cheers!

Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're 
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.

Author
~~~~~~
Twitter: @javidx9	http://twitter.com/javidx9
Blog:				http://www.onelonecoder.com
YouTube:			http://www.youtube.com/javidx9

Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o

Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
wowLinh, JavaJack59, idkwid, kingtatgi, Return Null, CPP Guy, MaGetzUb

Last Updated: 02/07/2018

Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.

	int main()
	{
		// Use olcConsoleGameEngine derived app
		OneLoneCoder_Example game;

		// Create a console with resolution 160x100 characters
		// Each character occupies 8x8 pixels
		game.ConstructConsole(160, 100, 8, 8);

		// Start the engine!
		game.Start();

		return 0;
	}

Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in. The same applies to mouse! m_mousePosX and Y can be used to get
the current cursor position, and m_mouse[1..5] returns the mouse buttons.

The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.

There may be bugs! 

See my other videos for examples!
http://www.youtube.com/javidx9

Lots of programs to try:
http://www.github.com/OneLoneCoder/videos

Chat on the Discord server:
https://discord.gg/WhwHUMV

Be bored by Twitch:
http://www.twitch.tv/javidx9
*/
#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! - Javidx9
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

 enum COLOUR
{
	FG_BLACK		= 0x0000,
	FG_DARK_BLUE    = 0x0001,	
	FG_DARK_GREEN   = 0x0002,
	FG_DARK_CYAN    = 0x0003,
	FG_DARK_RED     = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW  = 0x0006,
	FG_GREY			= 0x0007, // Thanks MS :-/
	FG_DARK_GREY    = 0x0008,
	FG_BLUE			= 0x0009,
	FG_GREEN		= 0x000A,
	FG_CYAN			= 0x000B,
	FG_RED			= 0x000C,
	FG_MAGENTA		= 0x000D,
	FG_YELLOW		= 0x000E,
	FG_WHITE		= 0x000F,
	BG_BLACK		= 0x0000,
	BG_DARK_BLUE	= 0x0010,
	BG_DARK_GREEN	= 0x0020,
	BG_DARK_CYAN	= 0x0030,
	BG_DARK_RED		= 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW	= 0x0060,
	BG_GREY			= 0x0070,
	BG_DARK_GREY	= 0x0080,
	BG_BLUE			= 0x0090,
	BG_GREEN		= 0x00A0,
	BG_CYAN			= 0x00B0,
	BG_RED			= 0x00C0,
	BG_MAGENTA		= 0x00D0,
	BG_YELLOW		= 0x00E0,
	BG_WHITE		= 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class olcSprite
{
public:
	olcSprite();
	olcSprite(int w, int h);
	olcSprite(std::wstring sFile);
	int nWidth = 0;
	int nHeight = 0;

private:
	short *m_Glyphs = nullptr;
	short *m_Colours = nullptr;
	void Create(int w, int h);

public:
	void SetGlyph(int x, int y, short c);
	void SetColour(int x, int y, short c);
	short GetGlyph(int x, int y);
	short GetColour(int x, int y);
	short SampleGlyph(float x, float y);
	short SampleColour(float x, float y);
	bool Save(std::wstring sFile);
	bool Load(std::wstring sFile);
};

class olcConsoleGameEngine
{
public:
	olcConsoleGameEngine();
	
	void EnableSound();
	int ConstructConsole(int width, int height, int fontw, int fonth);
	virtual void Draw(int x, int y, short character = 0x2588, short color = 0x000F);
	void Fill(int x1, int y1, int x2, int y2, short character = 0x2588, short color = 0x000F);
	void DrawString(int x, int y, std::wstring c, short col = 0x000F);
	void DrawStringAlpha(int x, int y, std::wstring c, short col = 0x000F);
	void Clip(int &x, int &y);
	void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);
	// htts://www.avrfreaks.net/sites/default/files/triangles.c
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);
	void DrawCircle(int xc, int yc, int r, short c = 0x2588, short col = 0x000F);
	void FillCircle(int xc, int yc, int r, short c = 0x2588, short col = 0x000F);
	void DrawSprite(int x, int y, olcSprite *sprite);
	void DrawPartialSprite(int x, int y, olcSprite *sprite, int ox, int oy, int w, int h);
	void DrawWireFrameModel(const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE, short c = PIXEL_SOLID);
	virtual ~olcConsoleGameEngine();
public:
	void Start();
	int ScreenWidth();
	int ScreenHeight();
private:
	void GameThread();
public:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime)= 0;	
	// Optional for clean up 
	virtual bool OnUserDestroy();
protected: // Audio Engine =====================================================================
	class olcAudioSample
	{
	public:
		olcAudioSample();
		olcAudioSample(std::wstring sWavFile);
		WAVEFORMATEX wavHeader;
		float *fSample = nullptr;
		long nSamples = 0;
		int nChannels = 0;
		bool bSampleValid = false;
	};
	// This vector holds all loaded sound samples in memory
	std::vector<olcAudioSample> vecAudioSamples;
	// This structure represents a sound that is currently playing. It only
	// holds the sound ID and where this instance of it is up to for its
	// current playback
	struct sCurrentlyPlayingSample
	{
		int nAudioSampleID = 0;
		long nSamplePosition = 0;
		bool bFinished = false;
		bool bLoop = false;
	};
	std::list<sCurrentlyPlayingSample> listActiveSamples;
	// Load a 16-bit WAVE file @ 44100Hz ONLY into memory. A sample ID
	// number is returned if successful, otherwise -1
	unsigned int LoadAudioSample(std::wstring sWavFile);
	// Add sample 'id' to the mixers sounds to play list
	void PlaySample(int id, bool bLoop = false);
	void StopSample(int id);
	// The audio system uses by default a specific wave format
	bool CreateAudio(unsigned int nSampleRate = 44100, unsigned int nChannels = 1,unsigned int nBlocks = 8, unsigned int nBlockSamples = 512);
	// Stop and clean up audio system
	bool DestroyAudio();
	// Handler for soundcard request for more data
	void waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
	// Static wrapper for sound card handler
	static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	// Audio thread. This loop responds to requests from the soundcard to fill 'blocks'
	// with audio data. If no requests are available it goes dormant until the sound
	// card is ready for more data. The block is fille by the "user" in some manner
	// and then issued to the soundcard.
	void AudioThread();
	// Overridden by user if they want to generate sound in real-time
	virtual float onUserSoundSample(int nChannel, float fGlobalTime, float fTimeStep);
	// Overriden by user if they want to manipulate the sound before it is played
	virtual float onUserSoundFilter(int nChannel, float fGlobalTime, float fSample);

	// Finally, before the sound is issued to the operating system for performing, the
	// user gets one final chance to "filter" the sound, perhaps changing the volume
	// or adding funky effects
	float GetMixerOutput(int nChannel, float fGlobalTime, float fTimeStep);
	unsigned int m_nSampleRate;
	unsigned int m_nChannels;
	unsigned int m_nBlockCount;
	unsigned int m_nBlockSamples;
	unsigned int m_nBlockCurrent;
	short* m_pBlockMemory = nullptr;
	WAVEHDR *m_pWaveHeaders = nullptr;
	HWAVEOUT m_hwDevice = nullptr;
	std::thread m_AudioThread;
	std::atomic<bool> m_bAudioThreadActive = false;
	std::atomic<unsigned int> m_nBlockFree = 0;
	std::condition_variable m_cvBlockNotZero;
	std::mutex m_muxBlockNotZero;
	std::atomic<float> m_fGlobalTime = 0.0f;
	
protected:
	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;
public:
	sKeyState GetKey(int nKeyID);
	int GetMouseX();
	int GetMouseY();
	sKeyState GetMouse(int nMouseButtonID);
	bool IsFocused();
protected:
	int Error(const wchar_t *msg);
	static BOOL CloseHandler(DWORD evt);
protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	CHAR_INFO *m_bufScreen;
	std::wstring m_sAppName;
	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;	
	bool m_bEnableSound = false;
	// These need to be static because of the OnDestroy call the OS may make. The OS
	// spawns a special thread just for that
	static std::atomic<bool> m_bAtomActive;
	static std::condition_variable m_cvGameFinished;
	static std::mutex m_muxGame;
};

