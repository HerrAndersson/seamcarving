#include "Picture.h"
#include "lodepng.h"
#include "JpegLoader.h"
#include "StbImageWrite/stb_image_write.h"
#include <fstream>
#include "Helper.h"
#include <iostream>

using namespace std;
using namespace lodepng;

Picture::Picture(string filename, int filetype)
{
	this->filename = filename;

	if (filetype == PNG)
		LoadPNG(filename);
	else if (filetype == JPEG || filetype == JPG)
		LoadJPEG(filename);

	actualHeight = height;
	actualWidth = width;

	CalculateFullEnergy();
}

Picture::~Picture()
{
	for (int i = 0; i < width; i++)
		delete[] image[i];
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

int Picture::CalculatePixelEnergy(Pixel* p1, Pixel* p2)
{
	int r, g, b;
	r = p1->r - p2->r;
	g = p1->g - p2->g;
	b = p1->b - p2->b;

	return (int)(r*r + g*g + b*b);
}

int counter2 = 0;

void Picture::CalculateFullEnergy()
{
	/*

	6/////////////2/////////////7
	//						   //
	 5			  1			    4
	//						   //
	8/////////////3/////////////9

	*/

	int xEnergy = 0;
	int yEnergy = 0; 
	int totalEnergy = 0;

	//Scan through the image and update the energy values. Ignore boundary pixels. 1
	for (int y = 1; y < actualHeight - 1; y++)
	{
		for (int x = 1; x < actualWidth - 1; x++)
		{
			xEnergy = CalculatePixelEnergy(&image[x + 1][y], &image[x - 1][y]);
			yEnergy = CalculatePixelEnergy(&image[x][y + 1], &image[x][y - 1]);

			totalEnergy = xEnergy + yEnergy;
			image[x][y].energy = totalEnergy;
		}
	}

	//int xEnergy = 0;
	//int yEnergy = 0;
	//int totalEnergy = 0;

	////Scan through the image and update the energy values. Ignore boundary pixels. 1
	//for (int y = 1; y < actualHeight - 1; y++)
	//{
	//	for (int x = 1; x < actualWidth - 1; x++)
	//	{
	//		xEnergy = CalculatePixelEnergy(&image[x + 1][y], &image[x - 1][y]);
	//		if (x > 1 && x < actualWidth - 2)
	//			xEnergy += CalculatePixelEnergy(&image[x + 2][y], &image[x - 2][y]);
	//		if (x > 2 && x < actualWidth - 3)
	//			xEnergy += CalculatePixelEnergy(&image[x + 3][y], &image[x - 3][y]);


	//		yEnergy = CalculatePixelEnergy(&image[x][y + 1], &image[x][y - 1]);
	//		if (y > 1 && x < actualHeight - 2)
	//			yEnergy += CalculatePixelEnergy(&image[x - 1][y + 1], &image[x - 1][y - 1]);
	//		if (y > 2 && x < actualHeight - 2)
	//			yEnergy += CalculatePixelEnergy(&image[x + 1][y + 1], &image[x + 1][y - 1]);

	//		totalEnergy = xEnergy + yEnergy;
	//		image[x][y].energy = totalEnergy;
	//	}

	//	xEnergy = 0;
	//	yEnergy = 0;
	//	totalEnergy = 0;
	//}


	//Boundary pixels. 2 & 3
	for (int x = 1; x < actualWidth - 1; x++)
	{
		image[x][0].energy = image[x][1].energy;
		image[x][actualHeight - 1].energy = image[x][actualHeight - 2].energy;
	}

	//Boundary pixels. 4 & 5
	for (int y = 1; y < actualHeight - 1; y++)
	{
		image[actualWidth - 1][y].energy = image[actualWidth - 2][y].energy;
		image[0][y].energy = image[1][y].energy;
	}

	//Corners. 6,7,8,9
	image[0][0].energy = image[1][1].energy;
	image[actualWidth - 1][0].energy = image[actualWidth - 2][1].energy;
	image[0][actualHeight - 1].energy = image[1][actualHeight - 2].energy;
	image[actualWidth - 1][actualHeight - 1].energy = image[actualWidth - 2][actualHeight - 2].energy;

}

void Picture::CreateImageArray(int dimX, int dimY)
{
	image = new Pixel*[dimX];
	for (int i = 0; i < dimX; ++i)
		image[i] = new Pixel[dimY];
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
		SavePNG(path);
	else if (type == JPG || type == JPEG)
		SaveJPEG(path);

	cout << "Saving DONE" << endl;
}

void Picture::SavePNG(string path)
{
	vector<unsigned char> temp;
	temp.resize(actualWidth * actualHeight * 4);

	for (int y = 0; y < actualHeight; y++)
	{
		for (int x = 0; x < actualWidth; x++)
		{
			temp[4 * actualWidth * y + 4 * x + 0] = image[x][y].r;
			temp[4 * actualWidth * y + 4 * x + 1] = image[x][y].g;
			temp[4 * actualWidth * y + 4 * x + 2] = image[x][y].b;
			temp[4 * actualWidth * y + 4 * x + 3] = 255;
		}
	}

	unsigned error = encode(path, temp, actualWidth, actualHeight);

	if (error)
		cout << "Encoder error " << error << ": " << lodepng_error_text(error) << endl;
}
void Picture::SaveJPEG(string path)
{

}