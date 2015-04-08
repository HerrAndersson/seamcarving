#include "Picture.h"
#include "lodepng.h"
#include "JpegLoader.h"
#include "StbImageWrite/stb_image_write.h"

#include <iostream>
using namespace std;

using namespace lodepng;

Picture::Picture(string filename, int filetype)
{
	this->filename = filename;

	if (filetype == PNG)
	{
		LoadPNG(filename);
	}
	else if (filetype == JPEG || filetype == JPG)
	{
		LoadJPEG(filename);
	}

	actualHeight = height;
	actualWidth = width;

	CalculateFullEnergy();

	//for (int i = 0; i < height; i++)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		cout << image[j][i].energy << " ";
	//	}
	//	cout << endl;
	//}
}

Picture::~Picture()
{
	for (int i = 0; i < width; i++)
	{
		delete[] image[i];
	}
	delete[] image;
}

void Picture::LoadPNG(string filename)
{
	vector<unsigned char> temp;
	unsigned int h, w = 0;

	decode(temp, w, h, filename);
	height = h;
	width = w;

	CreateImageArray(width, height);

	int pos = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Pixel p(temp[pos], temp[pos + 1], temp[pos + 2]);
			image[x][y] = p;
			pos += 4;
		}
	}

}

void Picture::LoadJPEG(string filename)
{
	JpegLoader jpegLoader;
	const JpegLoader::ImageInfo* info = jpegLoader.Load(filename.c_str());

	height = info->nHeight;
	width = info->nWidth;

	CreateImageArray(width, height);

	uint8_t* d = info->pData;

	int pos = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Pixel p((int)d[pos], (int)d[pos + 1], (int)d[pos + 2]);
			image[x][y] = p;
			pos += 3;
		}
	}
}

void Picture::DeleteRow(Point* positions)
{
	for (int i = 0; i < actualWidth; i++)
	{
		for (int j = positions[i].y; j < actualHeight - 1; j++)
		{
			image[i][j] = image[i][j + 1];
		}
	}
	actualHeight--;

	ShowSeam(positions, true);

	CalculateFullEnergy();
}

void Picture::DeleteColumn(Point* positions)
{
	for (int i = 0; i < actualHeight; i++)
	{
		for (int j = positions[i].x; j < actualWidth - 1; j++)
		{
			image[j][i] = image[j + 1][i];
		}
	}
	actualWidth--;

	ShowSeam(positions, false);

	CalculateFullEnergy();
}

void Picture::ShowSeam(Point* positions, bool rowOrColumn)
{
	int length = height;
	if (rowOrColumn)
		length = width;

	for (int i = 0; i < length; i++)
	{
		Pixel current = image[positions->x][positions->y];
		Pixel temp(255, 0, 0);
		image[positions->x][positions->y] = temp;
	}
	
	Sleep(10000);
}

void Picture::AutoResize()
{
	Pixel** newImage = new Pixel*[actualWidth];
	for (int i = 0; i < actualWidth; ++i)
	{
		newImage[i] = new Pixel[actualHeight];
	}

	for (int i = 0; i < actualWidth; i++)
	{
		for (int j = 0; j < actualHeight; i++)
		{
			newImage[i][j] = image[i][j];
		}
	}

	Pixel** temp;
	temp = image;
	image = newImage;

	for (int i = 0; i < width; i++)
	{
		delete[] temp[i];
	}
	delete[] temp;

	width = actualWidth;
	height = actualHeight;
}

void Picture::CalculateFullEnergy()
{
	/*

	6/////////////////2//////////////////7
	//								    //
	 5			      1				    4
	//								    //
	//								    //
	8/////////////////3//////////////////9

	*/

	int yt, xt;
	int xr, xg, xb;
	int yr, yg, yb;
	int xEnergy, yEnergy, totalEnergy;

	//Scan through the image and update the energy values. Ignore boundary pixels. 1
	for (int y = 1; y < actualHeight - 1; ++y)
	{
		for (int x = 1; x < actualWidth - 1; ++x)
		{
			xr = image[x + 1][y].r - image[x - 1][y].r;
			xg = image[x + 1][y].g - image[x - 1][y].g;
			xb = image[x + 1][y].b - image[x - 1][y].b;
			xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

			yr = image[x][y + 1].r - image[x][y - 1].r;
			yg = image[x][y + 1].g - image[x][y - 1].g;
			yb = image[x][y + 1].b - image[x][y - 1].b;
			yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

			totalEnergy = xEnergy + yEnergy;
			image[x][y].energy = totalEnergy;
		}
	}

	//Boundary pixels. 2
	yt = 0;
	for (int x = 1; x < actualWidth - 1; x++)
	{
		xr = image[x + 1][yt].r - image[x - 1][yt].r;
		xg = image[x + 1][yt].g - image[x - 1][yt].g;
		xb = image[x + 1][yt].b - image[x - 1][yt].b;
		xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

		yr = image[x][yt + 1].r - image[x][actualHeight - 1].r;
		yg = image[x][yt + 1].g - image[x][actualHeight - 1].g;
		yb = image[x][yt + 1].b - image[x][actualHeight - 1].b;
		yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

		totalEnergy = xEnergy + yEnergy;
		image[x][yt].energy = totalEnergy;
	}

	//Boundary pixels. 3
	yt = actualHeight - 1;
	for (int x = 1; x < actualWidth - 1; x++)
	{
		xr = image[x + 1][yt].r - image[x - 1][yt].r;
		xg = image[x + 1][yt].g - image[x - 1][yt].g;
		xb = image[x + 1][yt].b - image[x - 1][yt].b;
		xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

		yr = image[x][yt].r - image[x][actualHeight - 2].r;
		yg = image[x][yt].g - image[x][actualHeight - 2].g;
		yb = image[x][yt].b - image[x][actualHeight - 2].b;
		yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

		totalEnergy = xEnergy + yEnergy;
		image[x][yt].energy = totalEnergy;
	}

	//Boundary pixels. 4
	xt = actualWidth - 1;
	for (int y = 1; y < actualHeight - 1; y++)
	{
		xr = image[0][y].r - image[xt - 1][y].r;
		xg = image[0][y].g - image[xt - 1][y].g;
		xb = image[0][y].b - image[xt - 1][y].b;
		xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

		yr = image[xt][y + 1].r - image[xt][y - 1].r;
		yg = image[xt][y + 1].g - image[xt][y - 1].g;
		yb = image[xt][y + 1].b - image[xt][y - 1].b;
		yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

		totalEnergy = xEnergy + yEnergy;
		image[xt][y].energy = totalEnergy;
	}

	//Boundary pixels. 5
	xt = 0;
	for (int y = 1; y < actualHeight - 1; y++)
	{
		xr = image[xt + 1][y].r - image[actualWidth - 1][y].r;
		xg = image[xt + 1][y].g - image[actualWidth - 1][y].g;
		xb = image[xt + 1][y].b - image[actualWidth - 1][y].b;
		xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

		yr = image[xt][y + 1].r - image[xt][y - 1].r;
		yg = image[xt][y + 1].g - image[xt][y - 1].g;
		yb = image[xt][y + 1].b - image[xt][y - 1].b;
		yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

		totalEnergy = xEnergy + yEnergy;
		image[xt][y].energy = totalEnergy;
	}

	//CORNERS
	//Top left. 6
	xr = image[1][0].r - image[actualWidth - 1][0].r;
	xg = image[1][0].g - image[actualWidth - 1][0].g;
	xb = image[1][0].b - image[actualWidth - 1][0].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	yr = image[0][1].r - image[0][actualHeight - 1].r;
	yg = image[0][1].g - image[0][actualHeight - 1].g;
	yb = image[0][1].b - image[0][actualHeight - 1].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[0][0].energy = totalEnergy;

	//Top right. 7
	xr = image[0][0].r - image[actualWidth - 2][0].r;
	xg = image[0][0].g - image[actualWidth - 2][0].g;
	xb = image[0][0].b - image[actualWidth - 2][0].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	yr = image[actualWidth - 1][1].r - image[actualWidth - 1][actualHeight - 1].r;
	yg = image[actualWidth - 1][1].g - image[actualWidth - 1][actualHeight - 1].g;
	yb = image[actualWidth - 1][1].b - image[actualWidth - 1][actualHeight - 1].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[actualWidth - 1][0].energy = totalEnergy;

	//Bottom left. 8
	xr = image[1][actualHeight - 1].r - image[actualWidth - 1][actualHeight - 1].r;
	xg = image[1][actualHeight - 1].g - image[actualWidth - 1][actualHeight - 1].g;
	xb = image[1][actualHeight - 1].b - image[actualWidth - 1][actualHeight - 1].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	yr = image[0][0].r - image[0][actualHeight - 2].r;
	yg = image[0][0].g - image[0][actualHeight - 2].g;
	yb = image[0][0].b - image[0][actualHeight - 2].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[0][actualHeight - 1].energy = totalEnergy;

	//Bottom right. 9
	xr = image[0][actualHeight - 1].r - image[actualWidth - 2][actualHeight - 1].r;
	xg = image[0][actualHeight - 1].g - image[actualWidth - 2][actualHeight - 1].g;
	xb = image[0][actualHeight - 1].b - image[actualWidth - 2][actualHeight - 1].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	yr = image[actualWidth - 1][0].r - image[actualWidth - 1][actualHeight - 2].r;
	yg = image[actualWidth - 1][0].g - image[actualWidth - 1][actualHeight - 2].g;
	yb = image[actualWidth - 1][0].b - image[actualWidth - 1][actualHeight - 2].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[actualWidth - 1][actualHeight - 1].energy = totalEnergy;
}

void Picture::CreateImageArray(int dimX, int dimY)
{
	image = new Pixel*[dimX];
	for (int i = 0; i < dimX; ++i)
	{
		image[i] = new Pixel[dimY];
	}
}

Pixel Picture::GetPixel(int x, int y)
{
	return image[x][y];
}

int Picture::GetWidth()
{
	return actualWidth;
}
int Picture::GetHeight()
{
	return actualHeight;
}
Pixel** Picture::GetImage()
{
	return image;
}