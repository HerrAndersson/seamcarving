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
	//Funkar inte, pixlarna uppdateras vid funktionens slut
	//ShowSeam(positions, false);
	//Sleep(1000);

	for (int i = 0; i < actualWidth; i++)
	{
		for (int j = positions[i].y; j < actualHeight - 1; j++)
		{
			image[i][j] = image[i][j + 1];
		}
	}
	actualHeight--;

	CalculateFullEnergy();
}

void Picture::DeleteColumn(Point* positions)
{
	//Funkar inte, pixlarna uppdateras vid funktionens slut
	//ShowSeam(positions, false);
	//Sleep(1000);

	for (int i = 0; i < actualHeight; i++)
	{
		for (int j = positions[i].x; j < actualWidth - 1; j++)
		{
			image[j][i] = image[j + 1][i];
		}
	}
	actualWidth--;

	CalculateFullEnergy();
}

void Picture::ShowSeam(Point* positions, bool rowOrColumn)
{
	int length = height;
	if (rowOrColumn)
		length = width;

	for (int i = 0; i < length; i++)
	{
		image[positions[i].x][positions[i].y].r = 255;
		image[positions[i].x][positions[i].y].g = 0;
		image[positions[i].x][positions[i].y].b = 0;
	}
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
		for (int j = 0; j < actualHeight; j++)
		{
			Pixel p = Pixel(image[i][j].r, image[i][j].g, image[i][j].b);
			p.energy = image[i][j].energy;
			newImage[i][j] = p;
		}
	}

	for (int i = 0; i < width; i++)
	{
		delete[] image[i];
	}
	delete[] image;

	image = newImage;

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

		//OMVÄND ORDNING ENLIGT http://www.cs.princeton.edu/courses/archive/spr13/cos226/assignments/seamCarving.html
		//Implementerat, utkommenterat = gammalt
		//yr = image[x][yt + 1].r - image[x][actualHeight - 1].r;
		//yg = image[x][yt + 1].g - image[x][actualHeight - 1].g;
		//yb = image[x][yt + 1].b - image[x][actualHeight - 1].b;

		yr = image[x][actualHeight - 1].r - image[x][yt + 1].r;
		yg = image[x][actualHeight - 1].g - image[x][yt + 1].g;
		yb = image[x][actualHeight - 1].b - image[x][yt + 1].b;
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

		//Old, wrong
		//yr = image[x][yt].r - image[x][actualHeight - 2].r;
		//yg = image[x][yt].g - image[x][actualHeight - 2].g;
		//yb = image[x][yt].b - image[x][actualHeight - 2].b;

		yr = image[x][actualHeight - 2].r - image[x][0].r;
		yg = image[x][actualHeight - 2].g - image[x][0].g;
		yb = image[x][actualHeight - 2].b - image[x][0].b;
		yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

		totalEnergy = xEnergy + yEnergy;
		image[x][yt].energy = totalEnergy;
	}

	//Boundary pixels. 4
	xt = actualWidth - 1;
	for (int y = 1; y < actualHeight - 1; y++)
	{
		//Old
		//xr = image[0][y].r - image[xt - 1][y].r;
		//xg = image[0][y].g - image[xt - 1][y].g;
		//xb = image[0][y].b - image[xt - 1][y].b;

		xr = image[xt - 1][y].r - image[0][y].r;
		xg = image[xt - 1][y].g - image[0][y].g;
		xb = image[xt - 1][y].b - image[0][y].b;
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
		//xr = image[xt + 1][y].r - image[actualWidth - 1][y].r;
		//xg = image[xt + 1][y].g - image[actualWidth - 1][y].g;
		//xb = image[xt + 1][y].b - image[actualWidth - 1][y].b;

		xr = image[actualWidth - 1][y].r - image[xt + 1][y].r;
		xg = image[actualWidth - 1][y].g - image[xt + 1][y].g;
		xb = image[actualWidth - 1][y].b - image[xt + 1][y].b;
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
	//xr = image[1][0].r - image[actualWidth - 1][0].r;
	//xg = image[1][0].g - image[actualWidth - 1][0].g;
	//xb = image[1][0].b - image[actualWidth - 1][0].b;

	xr = image[actualWidth - 1][0].r - image[1][0].r;
	xg = image[actualWidth - 1][0].g - image[1][0].g;
	xb = image[actualWidth - 1][0].b - image[1][0].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	//yr = image[0][1].r - image[0][actualHeight - 1].r;
	//yg = image[0][1].g - image[0][actualHeight - 1].g;
	//yb = image[0][1].b - image[0][actualHeight - 1].b;

	yr = image[0][actualHeight - 1].r - image[0][1].r;
	yg = image[0][actualHeight - 1].g - image[0][1].g;
	yb = image[0][actualHeight - 1].b - image[0][1].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[0][0].energy = totalEnergy;

	//Top right. 7
	//xr = image[0][0].r - image[actualWidth - 2][0].r;
	//xg = image[0][0].g - image[actualWidth - 2][0].g;
	//xb = image[0][0].b - image[actualWidth - 2][0].b;

	xr = image[actualWidth - 2][0].r - image[0][0].r;
	xg = image[actualWidth - 2][0].g - image[0][0].g;
	xb = image[actualWidth - 2][0].b - image[0][0].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	//yr = image[actualWidth - 1][1].r - image[actualWidth - 1][actualHeight - 1].r;
	//yg = image[actualWidth - 1][1].g - image[actualWidth - 1][actualHeight - 1].g;
	//yb = image[actualWidth - 1][1].b - image[actualWidth - 1][actualHeight - 1].b;

	yr = image[actualWidth - 1][actualHeight - 1].r - image[actualWidth - 1][1].r;
	yg = image[actualWidth - 1][actualHeight - 1].g - image[actualWidth - 1][1].g;
	yb = image[actualWidth - 1][actualHeight - 1].b - image[actualWidth - 1][1].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[actualWidth - 1][0].energy = totalEnergy;

	//Bottom left. 8
	//xr = image[1][actualHeight - 1].r - image[actualWidth - 1][actualHeight - 1].r;
	//xg = image[1][actualHeight - 1].g - image[actualWidth - 1][actualHeight - 1].g;
	//xb = image[1][actualHeight - 1].b - image[actualWidth - 1][actualHeight - 1].b;

	xr = image[actualWidth - 1][actualHeight - 1].r - image[1][actualHeight - 1].r;
	xg = image[actualWidth - 1][actualHeight - 1].g - image[1][actualHeight - 1].g;
	xb = image[actualWidth - 1][actualHeight - 1].b - image[1][actualHeight - 1].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	//yr = image[0][0].r - image[0][actualHeight - 2].r;
	//yg = image[0][0].g - image[0][actualHeight - 2].g;
	//yb = image[0][0].b - image[0][actualHeight - 2].b;

	yr = image[0][actualHeight - 2].r - image[0][0].r;
	yg = image[0][actualHeight - 2].g - image[0][0].g;
	yb = image[0][actualHeight - 2].b - image[0][0].b;
	yEnergy = (int)(pow(yr, 2) + pow(yg, 2) + pow(yb, 2));

	totalEnergy = xEnergy + yEnergy;
	image[0][actualHeight - 1].energy = totalEnergy;

	//Bottom right. 9
	//xr = image[0][actualHeight - 1].r - image[actualWidth - 2][actualHeight - 1].r;
	//xg = image[0][actualHeight - 1].g - image[actualWidth - 2][actualHeight - 1].g;
	//xb = image[0][actualHeight - 1].b - image[actualWidth - 2][actualHeight - 1].b;

	xr = image[actualWidth - 2][actualHeight - 1].r - image[0][actualHeight - 1].r;
	xg = image[actualWidth - 2][actualHeight - 1].g - image[0][actualHeight - 1].g;
	xb = image[actualWidth - 2][actualHeight - 1].b - image[0][actualHeight - 1].b;
	xEnergy = (int)(pow(xr, 2) + pow(xg, 2) + pow(xb, 2));

	//yr = image[actualWidth - 1][0].r - image[actualWidth - 1][actualHeight - 2].r;
	//yg = image[actualWidth - 1][0].g - image[actualWidth - 1][actualHeight - 2].g;
	//yb = image[actualWidth - 1][0].b - image[actualWidth - 1][actualHeight - 2].b;

	yr = image[actualWidth - 1][actualHeight - 2].r - image[actualWidth - 1][0].r;
	yg = image[actualWidth - 1][actualHeight - 2].g - image[actualWidth - 1][0].g;
	yb = image[actualWidth - 1][actualHeight - 2].b - image[actualWidth - 1][0].b;
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

void Picture::Save(string path, int type)
{
	cout << "Saving: " << path << endl;
	if (type == PNG)
	{
		SavePNG(path);
	}
	else if (type == JPG || type == JPEG)
	{
		SaveJPEG(path);
	}
	cout << "Saving DONE" << endl;
}

void Picture::SavePNG(string path)
{
	//When AutoResize works all actual* should be changed
	vector<unsigned char> temp;
	temp.resize(actualWidth * actualHeight * 4);

	cout << "Working..." << endl;

	for (int y = 0; y < actualHeight; y++)
	{
		for (int x = 0; x < actualWidth; x++)
		{
			temp[4 * actualWidth * y + 4 * x + 0] = image[x][y].r;
			temp[4 * actualWidth * y + 4 * x + 1] = image[x][y].g;
			temp[4 * actualWidth * y + 4 * x + 2] = image[x][y].b;
			temp[4 * actualWidth * y + 4 * x + 3] = 255;

			if (y == actualHeight / 2 && x == actualWidth / 2)
				cout << "Halfway done" << endl;
		}
	}

	cout << "Almost done" << endl;

	unsigned error = encode(path, temp, actualWidth, actualHeight);

	if (error)
	{
		cout << "Encoder error " << error << ": " << lodepng_error_text(error) << endl;
	}
}
void Picture::SaveJPEG(string path)
{

}