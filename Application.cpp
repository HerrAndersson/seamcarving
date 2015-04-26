#include "Application.h"
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

int removeColumns = 4;
int rc = removeColumns;
int removeRows = 0;
bool saved = false;

Application::Application(ToScreen* scr)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	Screen = scr;

	//pic = new Picture("Pictures/Input/tree.png", PNG);
	//pic = new Picture("Pictures/Input/tower.jpg", JPG);
	//pic = new Picture("Pictures/Input/tree.jpg", JPEG);
	//pic = new Picture("Pictures/Input/towerSmall.jpg", JPEG);
	//pic = new Picture("Pictures/Input/towerMedium.jpg", JPEG);
	//pic = new Picture("Pictures/Input/towerMedium.png", PNG);


	pic = new Picture("Pictures/Input/testsimplesmall.png", PNG);
	//pic = new Picture("Pictures/Input/testsimplebig.png", PNG);

	carver = new SeamCarver(pic);
}

Application::~Application()
{
	Screen = nullptr; //Comes from main.cpp
	delete pic;
	delete carver;
}

int Application::GetNumberOfDigits(int i)
{
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}

void Application::DebugEnergy()
{
	ofstream myfile;
	myfile.open("energy.txt");

	for (int y = 0; y < pic->GetHeight(); y++)
	{
		for (int x = 0; x < pic->GetWidth(); x++)
		{
			myfile << pic->GetPixel(x, y).energy;
			for (int i = 0; i < 12 - GetNumberOfDigits(pic->GetPixel(x, y).energy); i++)
			{
				myfile << " ";
			}
		}
		myfile << endl << endl << endl;
	}

	myfile.close();

	system("pause");
}

void Application::Update(float deltaTime)
{
	Clear();

	//DebugEnergy();

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
		pic->AutoResize();
		pic->Save("Pictures/Output/testsimple" + to_string(rc-removeColumns) + ".png", PNG);
	}
	else
	{
		//if (!saved)
		//{
		//	pic->AutoResize();
		//	pic->Save("Pictures/Output/testsimple.png", PNG);
		//	saved = true;
		//}
	}
}
