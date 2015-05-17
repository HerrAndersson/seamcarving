#pragma once
#include "FileTypes.h"
#include "Picture.h"
#include "ToScreen.h"

using namespace std;

class SeamCarver
{
private:

	Picture* pic;

	int FindMin(int v1, int v2, int v3, int& index);
	int FindMin(int v1, int v2, int v3);

public:

	SeamCarver(Picture* pic);
	virtual ~SeamCarver();

	Point* FindVerticalSeam();
	Point* FindHorizontalSeam();

	void RemoveRowsAndColumns(int rowCount, int columnCount);
	void RemoveRows(int rowCount);
	void RemoveColumns(int columnCount);

	void ChangePicture(Picture* newPic);
};

