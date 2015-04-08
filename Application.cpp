#include "Application.h"
#include <iostream>

int removeColumns = 10;
int removeRows = 0;

Application::Application(ToScreen* scr)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	Screen = scr;

	//pic = new Picture("Pictures/Input/tree.png", PNG);
	//pic = new Picture("Pictures/Input/tower.jpg", JPG);
	//pic = new Picture("Pictures/Input/tree.jpg", JPEG);
	//pic = new Picture("Pictures/Input/towerSmall.jpg", JPEG);
	//pic = new Picture("Pictures/Input/towerMedium.jpg", JPEG);
	pic = new Picture("Pictures/Input/towerMedium.png", PNG);

	carver = new SeamCarver(pic);
}

Application::~Application()
{
	Screen = nullptr; //Comes from main.cpp
	delete pic;
	delete carver;
}

void Application::Update(float deltaTime)
{
	Clear();

	PerformSeamCarving();

	for (int x = 0; x < pic->GetWidth(); x++)
	{
		for (int y = 0; y < pic->GetHeight(); y++)
		{
			Pixel p = pic->GetPixel(x, y);
			Screen->SetPixelColor(x, y, p.r, p.g, p.b);
		}
	}

	Screen->Update(deltaTime);
}

void Application::Clear()
{
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			Screen->SetPixelColor(x, y, 0, 0, 0);
		}
	}
}

void Application::PerformSeamCarving()
{
	if (removeColumns > 0)
	{
		carver->RemoveColumns(1);
		removeColumns--;
	}
	else if (removeRows > 0)
	{
		//carver->RemoveRows(1);
		//removeRows--;
	}
	else
	{
		//pic->AutoResize();
		pic->Save("Pictures/Output/towerMedium.png", PNG);
	}
}
