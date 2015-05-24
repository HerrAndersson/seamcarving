#include "Application.h"
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

int removeColumns = 350;
int removeRows = 150;
bool saved = false;
bool showV = true;
bool showH = true;

Application::Application(ToScreen* scr)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	Screen = scr;

	string png = ".png";
	string jpg = ".jpg";
	string type = "_carved";

	//PNG&JPG
	string towerMedium =	"Pictures/Input/towerMedium";

	//PNG
	string tree =			"Pictures/Input/tree";
	string ocean =			"Pictures/Input/ocean";
	string towerLarge =		"Pictures/Input/towerLarge";
	string beach =			"Pictures/Input/beach";


	picture = new Picture(towerMedium + png, PNG);
	original = new Picture(towerMedium + png, PNG);

	carver = new SeamCarver(picture);
}

Application::~Application()
{
	Screen = nullptr; //Comes from main.cpp

	delete picture;
	delete carver;
	delete original;

	delete[] vertical;
	delete[] horizontal;
}

int Application::GetNumberOfDigits(int i)
{
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}

bool s = true;
void Application::Update(float deltaTime)
{
	Clear();

	if (removeColumns > 0)
	{
		if (showV)
		{
			vertical = carver->FindVerticalSeam();
			picture->ShowSeam(vertical, false);

			ShowPicture(picture, original->GetWidth(), 0);
			ShowPicture(original, 0, 0);
			Screen->Update(deltaTime);
			showV = false;
		}
		else
		{
			if (vertical)
			{
				picture->DeleteColumn(vertical);

				ShowPicture(picture, original->GetWidth(), 0);
				ShowPicture(original, 0, 0);
				Screen->Update(deltaTime);
				removeColumns--;
			}
			showV = true;
		}
	}
	else if (removeRows > 0)
	{
		if (showH)
		{
			horizontal = carver->FindHorizontalSeam();
			picture->ShowSeam(horizontal, true);

			ShowPicture(picture, original->GetWidth(), 0);
			ShowPicture(original, 0, 0);
			Screen->Update(deltaTime);
			showH = false;
		}
		else
		{
			if (horizontal)
			{
				picture->DeleteRow(horizontal);

				ShowPicture(picture, original->GetWidth(), 0);
				ShowPicture(original, 0, 0);
				Screen->Update(deltaTime);
				removeRows--;
			}
			showH = true;
		}
	}
	else
	{
		//picture->AutoResize();
		//picture->Save("Pictures/output/")
	}

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
