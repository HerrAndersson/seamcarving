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

	void Clear();
	void ShowPicture(Picture* picture, int offsetX, int offsetY);

	void PerformSeamCarving();
	void DebugEnergy();
	int GetNumberOfDigits(int i);

public:

	Application(ToScreen* scr);
	~Application();

	void Update(float frameTime);

};