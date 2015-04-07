#include "SeamCarver.h"


SeamCarver::SeamCarver(Picture* pic)
{
	this->pic = pic;
}
SeamCarver::~SeamCarver()
{
	//Gets deleted from outside of this class
	pic = nullptr;
}

Point* SeamCarver::FindVerticalSeam()
{
	//Söka med ACTUAL HEIGHT, inte height
	return nullptr;
}
Point* SeamCarver::FindHorizontalSeam()
{
	return nullptr;
}

void SeamCarver::ChangePicture(Picture* newPic)
{
	this->pic = newPic;
}

void SeamCarver::RemoveRowsAndColumns(int rowCount, int columnCount)
{
	//Remove one row, one column, one row, one column etc.
	int length = rowCount + columnCount;
	for (int i = 0; i < length; i++)
	{
		if (i % 2 == 0 && rowCount > 0)
		{
			Point* positions = FindHorizontalSeam();
			pic->DeleteRow(positions);
			delete[] positions;
			rowCount--;
		}
		else
		{
			Point* positions = FindVerticalSeam();
			pic->DeleteColumn(positions);
			delete[] positions;
			columnCount--;
		}
	}
}
void SeamCarver::RemoveRows(int rowCount)
{
	for (int i = 0; i < rowCount; i++)
	{
		Point* positions = FindHorizontalSeam();
		pic->DeleteRow(positions);
		delete[] positions;
	}
}
void SeamCarver::RemoveColums(int columnCount)
{
	for (int i = 0; i < columnCount; i++)
	{
		Point* positions = FindVerticalSeam();
		pic->DeleteColumn(positions);
		delete[] positions;
	}
}
