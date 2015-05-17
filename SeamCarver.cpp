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

int counter = 0;

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
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//Följ: http://blogs.techsmith.com/inside-techsmith/seam-carving/
			//Följa nivå för nivå innan, alltså Y = 0, gå igenom ALLA X innan y++


			//If pixel is on the top border it keeps its energy
			if (y == 0)
			{
				energy[x][y] = pic->GetPixel(x, y).energy;
				//cout << energy[x][y] << " ";
			}

			//If the pixel is not on the top border it has pixels in the row above and the new energy can be calculated
			else
			{
				int min;

				//cout << x << "," << y << endl;

				if (x == 0)
				{
					min = FindMin(INF, image[x][y - 1].energy, image[x + 1][y - 1].energy);
					//cout << "Min: " << min << " " << INF << " " << image[x][y - 1].energy << " " << image[x + 1][y - 1].energy << endl;
				}
				else if (x == width - 1)
				{
					min = FindMin(image[x - 1][y - 1].energy, image[x][y - 1].energy, INF);
					//cout << "Min: " << min << " " << image[x - 1][y - 1].energy << " " << image[x][y - 1].energy << " " << INF << endl;
				}
				else
				{
					min = FindMin(image[x - 1][y - 1].energy, image[x][y - 1].energy, image[x + 1][y - 1].energy);
					//cout << "Min: " << min << " " << image[x - 1][y - 1].energy << " " << image[x][y - 1].energy << " " << image[x + 1][y - 1].energy << endl;
				}
					
				energy[x][y] = image[x][y].energy + min;
			}
		}
	}

	int xPos = 0;
	int minValue = INF;

	//Find x-coordinate in the last row with the lowest energy
	for (int i = 0; i < width; i++)
	{
		int e = energy[i][height - 1];
		if (e <= minValue)
		{
			minValue = e;
			xPos = i;
		}
	}


	//KOLLA ORDNINGEN HÄR!!! xPos fås ovan, ändras direkt nedan innan den från ovan används?

	//Retrace
	seam[height - 1] = Point(xPos, height - 1);

	for (int yPos = height - 2; yPos >= 0; yPos--)
	{
		int min;
		int index;
		//cout << xPos << "," << yPos << endl;

		if (yPos != 0)
		{
			if (xPos == 0)
			{
				min = FindMin(INF, energy[xPos][yPos - 1], energy[xPos + 1][yPos - 1], index);
				//cout << "Min: " << min << " Index: " << index << " " << INF << " " << energy[xPos][yPos - 1] << " " << energy[xPos + 1][yPos - 1] << endl;
			}
			else if (xPos == width - 1)
			{
				min = FindMin(energy[xPos - 1][yPos - 1], energy[xPos][yPos - 1], INF, index);
				//cout << "Min: " << min << " Index: " << index << " " << energy[xPos - 1][yPos - 1] << " " << energy[xPos][yPos - 1] << " " << INF << endl;
			}
			else
			{
				min = FindMin(energy[xPos - 1][yPos - 1], energy[xPos][yPos - 1], energy[xPos + 1][yPos - 1], index);
				//cout << "Min: " << min << " Index: " << index << " " << energy[xPos - 1][yPos - 1] << " " << energy[xPos][yPos - 1] << " " << energy[xPos + 1][yPos - 1] << endl;
			}

			if (index == 1)
				xPos = xPos - 1;
			if (index == 3)
				xPos = xPos + 1;
			else
				xPos = xPos;

			if (xPos < 0)
				xPos = 0;
			if (xPos > width - 1)
				xPos = width - 1;
		}

		seam[yPos] = Point(xPos, yPos);

		//cout << seam[yPos].x << " " << seam[yPos].y << endl;
	}

	//Point p = Point(xPos, 0);
	//seam[0] = p;
//
	//for (int i = height - 1; i >= 0; i--)
	//{
	//	cout << seam[i].x << " " << seam[i].y << endl;
	//}

	cout << endl;

	ofstream myfile;
	myfile.open("Pictures/Debug/energySEAM" + to_string(counter) + ".txt");
	counter++;

	for (int y = 0; y < pic->GetHeight(); y++)
	{
		for (int x = 0; x < pic->GetWidth(); x++)
		{
			myfile << energy[x][y];
			for (int i = 0; i < 12 - GetNumberOfDigits(energy[x][y]); i++)
			{
				myfile << " ";
			}
		}
		myfile << endl << endl << endl;
	}

	myfile << xPos;

	myfile.close();


	for (int i = 0; i < width; i++)
	{
		delete[] energy[i];
	}
	delete[] energy;

	return seam;
}

Point* SeamCarver::FindHorizontalSeam()
{
	//Broken, check vertical when done

	int height = pic->GetHeight();
	int width = pic->GetWidth();
	Point* seam = new Point[width];
	Pixel** image = pic->GetImage();

	////Holds the "new" energy values used to find a path.
	//int** energy = new int*[width];
	//for (int i = 0; i < width; ++i)
	//{
	//	energy[i] = new int[height];
	//}

	//for (int y = 0; y < height; y++)
	//{
	//	for (int x = 0; x < width; x++)
	//	{
	//		//If pixel is on the border it keeps its energy.
	//		if ((x == 0) || (y == 0) || (x == width - 1) || (y == height - 1))
	//		{
	//			energy[x][y] = pic->GetPixel(x, y).energy;
	//		}
	//		//If the pixel is not on the border it has pixels in the row above and the new energy can be calculated
	//		else
	//		{
	//			int min = FindMin(image[x - 1][y - 1].energy, image[x - 1][y].energy, image[x - 1][y + 1].energy);
	//			energy[x][y] = image[x][y].energy + min;
	//		}
	//	}
	//}

	//int y = 0;
	//int minValue = INF;

	////Find y-coordinate in the last row with the lowest energy
	//for (int i = 1; i < height; i++)
	//{
	//	if (energy[width - 1][i] < minValue)
	//	{
	//		minValue = energy[width - 1][i];
	//		y = i;
	//	}
	//}

	//Point p(width - 1, y);
	//seam[width - 1] = p;

	//for (int x = width - 2; x > 0; x--)
	//{
	//	int min;
	//	int index;

	//	if (y == 0)
	//		min = FindMin(INF, image[x - 1][y].energy, image[x - 1][y + 1].energy, index);
	//	else if (y == height - 1)
	//		min = FindMin(image[x - 1][y - 1].energy, image[x - 1][y].energy, INF, index);
	//	else
	//		min = FindMin(image[x - 1][y - 1].energy, image[x - 1][y].energy, image[x - 1][y + 1].energy, index);

	//	if (index == 1)
	//		y = y - 1;
	//	if (index == 3)
	//		y = y + 1;
	//	else
	//		y = y;

	//	Point p(x, y);
	//	seam[y] = p;

	//	if (y == 0)
	//		y++;
	//	if (y == height - 1)
	//		y--;
	//}

	//p = Point(0, y);
	//seam[0] = p;

	//for (int i = 0; i < width; i++)
	//{
	//	delete[] energy[i];
	//}
	//delete[] energy;

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
