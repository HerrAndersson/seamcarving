#include "Application.h"

Application::Application(ToScreen* scr)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	Screen = scr;
	pic = new Picture("tree.png", PNG);
	//pic = new Picture("tree.jpg", JPG);

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
