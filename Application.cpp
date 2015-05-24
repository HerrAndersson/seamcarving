#include "Application.h"
#include <iostream>
#include <fstream>
#include <math.h>
//#include "vld.h"

using namespace std;

int removeColumns = 1;
int removeRows = 1;
bool showV = true;
bool showH = true;

string png = ".png";
string jpg = ".jpg";

//PNG&JPG
string towerMedium = "towerMedium";

//PNG
string tree = "tree";
string ocean = "ocean";
string towerLarge = "towerLarge";
string beach = "beach";

string currentIn = "Pictures/Input/" + towerMedium + png;
string currentOut = "Pictures/Output/" + towerMedium + "_carved" + jpg;

bool saved = false;

Application::Application(ToScreen* scr)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	Screen = scr;

	picture = new Picture(currentIn, PNG);
	original = new Picture(currentIn, PNG);

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

bool Application::Update(float deltaTime)
{
	Clear();

	if (removeColumns > 0)
	{
		if (showV)
		{
			delete[] vertical;
			vertical = carver->FindVerticalSeam();
			picture->ShowSeam(vertical, false);

			ShowPicture(picture, 0, 0);
			//ShowPicture(picture, original->GetWidth(), 0);
			//ShowPicture(original, 0, 0);
			Screen->Update(deltaTime);
			showV = false;
		}
		else
		{
			if (vertical)
			{
				picture->DeleteColumn(vertical);

				ShowPicture(picture, 0, 0);
				//ShowPicture(picture, original->GetWidth(), 0);
				//ShowPicture(original, 0, 0);
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
			delete[] horizontal;
			horizontal = carver->FindHorizontalSeam();
			picture->ShowSeam(horizontal, true);

			ShowPicture(picture, 0, 0);
			//ShowPicture(picture, original->GetWidth(), 0);
			//ShowPicture(original, 0, 0);
			Screen->Update(deltaTime);
			showH = false;
		}
		else
		{
			if (horizontal)
			{
				picture->DeleteRow(horizontal);

				ShowPicture(picture, 0, 0);
				//ShowPicture(original, 0, 0);
				Screen->Update(deltaTime);
				removeRows--;
			}
			showH = true;
		}
	}
	else
	{
		if (!saved)
		{
			Save();
			return false;
		}
	}

	if (GetAsyncKeyState(VK_ESCAPE) != 0)
	{
		//Save();
		return false;
	}

	return true;
}

void Application::Save()
{
	picture->AutoResize();
	picture->Save(currentOut, JPG);
	saved = true;
}

void Application::Clear()
{
	for (int x = 0; x < SCREEN_WIDTH; x++)
		for (int y = 0; y < SCREEN_HEIGHT; y++)
			Screen->SetPixelColor(x, y, 0, 0, 0);
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
