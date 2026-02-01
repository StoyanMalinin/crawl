#define OLC_PGE_APPLICATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include "olcPixelGameEngine.h"
#pragma GCC diagnostic pop

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		for (int x = 0; x < ScreenWidth() / 16; x++)
			for (int y = 0; y < ScreenHeight() / 16; y++)
				FillRect(x * 16, y * 16, 16, 16, olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
		return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(256, 240, 1, 1))
		demo.Start();

	return 0;
}
