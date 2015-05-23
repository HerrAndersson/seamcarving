#include "Application.h"
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

//http://cs.brown.edu/courses/cs129/results/proj3/taox/
//http://www.cs.princeton.edu/courses/archive/spr13/cos226/assignments/seamCarving.html
//http://blogs.techsmith.com/inside-techsmith/seam-carving/

int removeColumns = 150;
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
	picture = new Picture("Pictures/Input/towerMedium.png", PNG);
	original = new Picture("Pictures/Input/towerMedium.png", PNG);
	//pic = new Picture("Pictures/Input/ocean.png", PNG);


	//pic = new Picture("Pictures/Input/testsimplesmall.png", PNG);
	//pic = new Picture("Pictures/Input/testsimplesmall2.png", PNG);
	//pic = new Picture("Pictures/Input/testsimplebig.png", PNG);

	carver = new SeamCarver(picture);
}

Application::~Application()
{
	Screen = nullptr; //Comes from main.cpp
	delete picture;
	delete carver;
	delete original;
}

int Application::GetNumberOfDigits(int i)
{
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}

int i = 0;

void Application::Update(float deltaTime)
{
	Clear();

	//DebugEnergy();
	//PerformSeamCarving();

	if (i < removeColumns)
	{
		Point* positions = carver->FindVerticalSeam();
		if (i % 2 == 0)
		{
			picture->ShowSeam(positions, false);

			ShowPicture(picture, original->GetWidth(), 0);
			ShowPicture(original, 0, 0);
			Screen->Update(deltaTime);
		}
		else
		{
			picture->DeleteColumn(positions);

			ShowPicture(picture, original->GetWidth(), 0);
			ShowPicture(original, 0, 0);
			Screen->Update(deltaTime);
		}

		delete[] positions;
	}
	i++;
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

void Application::ShowPicture(Picture* picture, int offsetX, int offsetY)
{
	for (int x = 0; x < picture->GetWidth(); x++)
	{
		for (int y = 0; y < picture->GetHeight(); y++)
		{
			Pixel p = picture->GetPixel(x, y);
			Screen->SetPixelColor(x + offsetX, y, p.r, p.g, p.b);
			Screen->SetPixelColor(x + offsetX, y + picture->GetHeight(), p.EnergyToColor(), p.EnergyToColor(), p.EnergyToColor());
		}
	}
}

void Application::PerformSeamCarving()
{
	if (removeColumns > 0)
	{
		carver->RemoveColumns(1);
		removeColumns--;

		//pic->AutoResize();
		//pic->Save("Pictures/Output/output" + to_string(rc-removeColumns) + ".png", PNG);
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
