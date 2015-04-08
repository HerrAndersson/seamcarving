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

int SeamCarver::FindMin(int v1, int v2, int v3, int& index)
{
	int min = v1;
	index = 1;

	if (v2 < min)
	{
		min = v2;
		index = 2;
	}
	if (v3 < min)
	{
		min = v3;
		index = 3;
	}

	return min;
}

int SeamCarver::FindMin(int v1, int v2, int v3)
{
	int min = v1;

	if (v2 < min)
	{
		min = v2;
	}
	if (v3 < min)
	{
		min = v3;
	}

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
				int min = FindMin(image[x - 1][y + 1].energy, image[x][y + 1].energy, image[x + 1][y + 1].energy);
				energy[x][y] = image[x][y].energy + min;
			}
		}
	}

	int x = 0;
	int minValue = INF;

	//Find x-coordinate in the last row with the lowest energy
	for (int i = 1; i < width; i++)
	{
		if (energy[i][height - 1] < minValue)
		{
			minValue = energy[i][height - 1];
			x = i;
		}
	}

	Point p(x, height - 1);
	seam[height - 1] = p;

	for (int y = height - 2; y > 0; y--)
	{
		int index;
		int min = FindMin(image[x - 1][y - 1].energy, image[x][y - 1].energy, image[x + 1][y - 1].energy, index);
		
		if (index == 1)
			x = x - 1;
		if (index == 3)
			x = x + 1;
		else
			x = x;

		Point p(x, y);
		seam[y] = p;

		if (x == 0)
			x++;
		if (x == width - 1)
			x--;
	}

	p = Point(x, 0);
	seam[0] = p;

		for (int i = 0; i < width; i++)
	{
		delete[] energy[i];
	}
	delete[] energy;

	return seam;
}

Point* SeamCarver::FindHorizontalSeam()
{
	int height = pic->GetHeight();
	int width = pic->GetWidth();
	Point* seam = new Point[width];
	Pixel** image = pic->GetImage();

	//Holds the "new" energy values used to find a path.
	int** energy = new int*[width];
	for (int i = 0; i < width; ++i)
	{
		energy[i] = new int[height];
	}

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			//If pixel is on the border it keeps its energy.
			if ((x == 0) || (y == 0) || (x == width - 1) || (y == height - 1))
			{
				energy[x][y] = pic->GetPixel(x, y).energy;
			}
			//If the pixel is not on the border it has pixels in the row above and the new energy can be calculated
			else
			{
				int min = FindMin(image[x - 1][y - 1].energy, image[x - 1][y].energy, image[x - 1][y + 1].energy);
				energy[x][y] = image[x][y].energy + min;
			}
		}
	}

	int y = 0;
	int minValue = INF;

	//Find y-coordinate in the last row with the lowest energy
	for (int i = 1; i < height; i++)
	{
		if (energy[width - 1][i] < minValue)
		{
			minValue = energy[width - 1][i];
			y = i;
		}
	}

	Point p(width - 1, y);
	seam[width - 1] = p;

	for (int x = width - 2; x > 0; x--)
	{
		int index;
		int min = FindMin(image[x - 1][y - 1].energy, image[x - 1][y].energy, image[x - 1][y + 1].energy, index);

		if (index == 1)
			y = y - 1;
		if (index == 3)
			y = y + 1;
		else
			y = y;

		Point p(x, y);
		seam[y] = p;

		if (y == 0)
			y++;
		if (y == height - 1)
			y--;
	}

	p = Point(0, y);
	seam[0] = p;

	for (int i = 0; i < width; i++)
	{
		delete[] energy[i];
	}
	delete[] energy;

	return seam;
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
void SeamCarver::RemoveColumns(int columnCount)
{
	for (int i = 0; i < columnCount; i++)
	{
		Point* positions = FindVerticalSeam();
		pic->DeleteColumn(positions);
		delete[] positions;
	}

	/*pic->AutoResize();*/
}
