#include "Picture.h"
#include "lodepng.h"
#include "JpegLoader.h"
#include "StbImageWrite/stb_image_write.h"

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
	//Scan through the image and update the energy values. Ignore boundary pixels.
	for (int i = 1; i < actualHeight - 1; ++i)
	{
		int prev = i - 1;		//Pointer to previous row
		int curr = i;			//Pointer to current row
		int next = i + 1;		//Pointer to next row

		for (int j = 1; j < actualWidth - 1; ++j)
		{

		}
	}
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