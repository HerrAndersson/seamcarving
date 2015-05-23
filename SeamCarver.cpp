#include "SeamCarver.h"
#include <iostream>
#include <fstream>
#include "Helper.h"

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
	index = -1;

	if (v2 < min)
	{
		min = v2;
		index = 0;
	}
	if (v3 < min)
	{
		min = v3;
		index = 1;
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

int counter = 0;
Point* SeamCarver::FindVerticalSeam()
{
	int height = pic->GetHeight();
	int width = pic->GetWidth();

	Point* seam = new Point[height];
	Pixel** image = pic->GetImage();

	//Holds the "new" energy values used to find a path.
	int** energy = new int*[width];
	for (int i = 0; i < width; i++)
	{
		energy[i] = new int[height];
	}

	for (int x = 0; x < width; x++)
	{
		energy[x][0] = image[x][0].energy;
	}

	for (int y = 1; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int min;
			if (x == 0)
				min = FindMin(INF, energy[x][y - 1], energy[x + 1][y - 1]);
			else if (x == width - 1)
				min = FindMin(energy[x - 1][y - 1], energy[x][y - 1], INF);
			else
				min = FindMin(energy[x - 1][y - 1], energy[x][y - 1], energy[x + 1][y - 1]);

			energy[x][y] = image[x][y].energy + min;
		}
	}

	int xPos = 0;
	int minValue = INF;

	//Find x-coordinate in the last row with the lowest energy
	for (int i = 0; i < width; i++)
	{
		int e = energy[i][height - 1];
		if (e < minValue)
		{
			minValue = e;
			xPos = i;
		}
	}

	//Retrace
	seam[height - 1] = Point(xPos, height - 1);

	for (int yPos = height - 1; yPos > 0; yPos--)
	{
		int min;
		int index;

		if (xPos == 0)
			min = FindMin(INF, energy[xPos][yPos - 1], energy[xPos + 1][yPos - 1], index);
		else if (xPos == width - 1)
			min = FindMin(energy[xPos - 1][yPos - 1], energy[xPos][yPos - 1], INF, index);
		else
			min = FindMin(energy[xPos - 1][yPos - 1], energy[xPos][yPos - 1], energy[xPos + 1][yPos - 1], index);

		xPos += index;

		if (xPos < 0)
			xPos = 0;
		if (xPos > width - 1)
			xPos = width - 1;

		seam[yPos] = Point(xPos, yPos);
	}

	seam[0] = Point(xPos, 0);

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
	for (int i = 0; i < width; i++)
		energy[i] = new int[height];

	for (int y = 0; y < height; y++)
		energy[0][y] = image[0][y].energy;


	for (int x = 1; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int min;
			if (y == 0)
				min = FindMin(INF, energy[x - 1][y], energy[x - 1][y + 1]);
			else if (y == height - 1)
				min = FindMin(energy[x - 1][y - 1], energy[x - 1][y], INF);
			else
				min = FindMin(energy[x - 1][y - 1], energy[x - 1][y], energy[x - 1][y + 1]);

			energy[x][y] = image[x][y].energy + min;
		}
	}

	int yPos = 0;
	int minValue = INF;

	//Find y-coordinate in the last column with the lowest energy
	for (int i = 0; i < height; i++)
	{
		int e = energy[width - 1][i];
		if (e < minValue)
		{
			minValue = e;
			yPos = i;
		}
	}

	//Retrace
	seam[width - 1] = Point(width - 1, yPos);

	for (int xPos = width - 1; xPos > 0; xPos--)
	{
		int min;
		int index;

		if (yPos == 0)
			min = FindMin(INF, energy[xPos - 1][yPos], energy[xPos - 1][yPos + 1], index);
		else if (yPos == height - 1)
			min = FindMin(energy[xPos - 1][yPos - 1], energy[xPos - 1][yPos], INF, index);
		else
			min = FindMin(energy[xPos - 1][yPos - 1], energy[xPos - 1][yPos], energy[xPos - 1][yPos + 1], index);

		yPos += index;

		if (yPos < 0)
			xPos = 0;
		if (yPos > height - 1)
			yPos = height - 1;

		seam[xPos] = Point(xPos, yPos);
	}

	seam[0] = Point(0, yPos);

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
	////Remove one row, one column, one row, one column etc.
	//int length = rowCount + columnCount;
	//for (int i = 0; i < length; i++)
	//{
	//	if (i % 2 == 0 && rowCount > 0)
	//	{
	//		Point* positions = FindHorizontalSeam();
	//		pic->DeleteRow(positions);
	//		delete[] positions;
	//		rowCount--;
	//	}
	//	else if (columnCount > 0)
	//	{
	//		Point* positions = FindVerticalSeam();
	//		pic->DeleteColumn(positions);
	//		delete[] positions;
	//		columnCount--;
	//	}
	//}

	//pic->AutoResize();
}
void SeamCarver::RemoveRows(int rowCount)
{
	//for (int i = 0; i < rowCount; i++)
	//{
	//	Point* positions = FindHorizontalSeam();
	//	pic->DeleteRow(positions);
	//	delete[] positions;
	//}

	//pic->AutoResize();
}
void SeamCarver::RemoveColumns(int columnCount)
{
	//for (int i = 0; i < columnCount; i++)
	//{
	//	Point* positions = FindVerticalSeam();
	//	pic->DeleteColumn(positions);
	//	delete[] positions;
	//}

	///*pic->AutoResize();*/
}
