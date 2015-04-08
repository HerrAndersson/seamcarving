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
	Picture*		pic;
	SeamCarver*		carver;

	void Clear();

public:

	Application(ToScreen* scr);
	~Application();

	void Update(float frameTime);

};