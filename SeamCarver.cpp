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

int SeamCarver::FindMin(int v1, int v2, int v3)
{
	int min = v1;

	if (v2 < min)
		min = v2;
	if (v3 < min)
		min = v3;

	return min;
}

Point* SeamCarver::FindVerticalSeam()
{
	int height = pic->GetHeight();
	int width = pic->GetWidth();
	Point* seam = new Point[height];
	Pixel** image = pic->GetImage();

	//Holds the "new" energy values used to find a path.
	int** energy = new int*[width];
	for (int i = 0; i < width; ++i)
	{
		energy[i] = new int[height];
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			//If pixel is on the border it keeps its energy.
			if ((x == 0) || (y == 0) || (x == width - 1) || (y == height - 1))
			{
				energy[x][y] = pic->GetPixel(x, y).energy;
			}
			//If the pixel is not on the border it has pixels in the row above and the new energy can be calculated
			else
			{
				int min = FindMin(image[x - 1][y - 1].energy, image[x][y - 1].energy, image[x + 1][y - 1].energy);
				energy[x][y] = image[x][y].energy + min;
			}
		}
	}

	int xStart = 0;
	int value = INF;

	for (int i = 0; i < width; i++)
	{
		if (energy[i][height - 1] < value)
		{
			value = energy[i][height - 1];
			xStart = i;
		}
	}

	for (int y = height; y >= 0; y--)
	{
		//RETRACE
	}

	return seam;
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

	pic->AutoResize();
}
void SeamCarver::RemoveRows(int rowCount)
{
	for (int i = 0; i < rowCount; i++)
	{
		Point* positions = FindHorizontalSeam();
		pic->DeleteRow(positions);
		delete[] positions;
	}

	pic->AutoResize();
}
void SeamCarver::RemoveColums(int columnCount)
{
	for (int i = 0; i < columnCount; i++)
	{
		Point* positions = FindVerticalSeam();
		pic->DeleteColumn(positions);
		delete[] positions;
	}

	pic->AutoResize();
}
