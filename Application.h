#include "stdafx.h"
#include "ToScreen.h"
#include "Picture.h"
#include "FileTypes.h"
#include "SeamCarver.h"

using namespace std;

class Application
{
private:

	ToScreen*		Screen;
	Picture*		picture;
	Picture*		original;
	SeamCarver*		carver;

	Point* vertical = nullptr;;
	Point* horizontal = nullptr;

	void Clear();
	void ShowPicture(Picture* picture, int offsetX, int offsetY);

	int GetNumberOfDigits(int i);

public:

	Application(ToScreen* scr);
	~Application();

	bool Update(float frameTime);

};