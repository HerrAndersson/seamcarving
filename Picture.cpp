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

	return (int)(pow(r, 2) + pow(g, 2) + pow(b, 2));
}

int counter2 = 0;

void Picture::CalculateFullEnergy()
{
	//http://cs.brown.edu/courses/cs129/results/proj3/taox/
	//http://www.cs.princeton.edu/courses/archive/spr13/cos226/assignments/seamCarving.html

	/*

	6/////////////////2//////////////////7
	//								    //
	 5			      1				    4
	//								    //
	//								    //
	8/////////////////3//////////////////9

	*/

	int xEnergy, yEnergy, totalEnergy;

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

	//Boundary pixels. 2 & 3
	for (int x = 1; x < actualWidth - 1; x++)
	{
		//Originalet, hämtat från princeton
		//xEnergy = CalculatePixelEnergy(&image[x + 1][0], &image[x - 1][0]);
		//yEnergy = CalculatePixelEnergy(&image[x][actualHeight - 1], &image[x][1]);

		//totalEnergy = xEnergy + yEnergy;
		//image[x][0].energy = totalEnergy;

		//xEnergy = CalculatePixelEnergy(&image[x + 1][actualHeight - 1], &image[x - 1][actualHeight - 1]);
		//yEnergy = CalculatePixelEnergy(&image[x][actualHeight - 2], &image[x][0]);

		//totalEnergy = xEnergy + yEnergy;
		//image[x][actualHeight - 1].energy = totalEnergy;


		xEnergy = CalculatePixelEnergy(&image[x + 1][0], &image[x - 1][0]);
		yEnergy = CalculatePixelEnergy(&image[x][0], &image[x][1]);

		totalEnergy = xEnergy + yEnergy;
		image[x][0].energy = totalEnergy;

		xEnergy = CalculatePixelEnergy(&image[x + 1][actualHeight - 1], &image[x - 1][actualHeight - 1]);
		yEnergy = CalculatePixelEnergy(&image[x][actualHeight - 2], &image[x][actualHeight - 1]);

		totalEnergy = xEnergy + yEnergy;
		image[x][actualHeight - 1].energy = totalEnergy;


	}

	//Boundary pixels. 4 & 5
	for (int y = 1; y < actualHeight - 1; y++)
	{
		//Originalet, hämtat från princeton
		//xEnergy = CalculatePixelEnergy(&image[actualWidth - 2][y], &image[0][y]);
		//yEnergy = CalculatePixelEnergy(&image[actualWidth - 1][y + 1], &image[actualWidth - 1][y - 1]);

		//totalEnergy = xEnergy + yEnergy;
		//image[actualWidth - 1][y].energy = totalEnergy;

		//xEnergy = CalculatePixelEnergy(&image[actualWidth - 1][y], &image[1][y]);
		//yEnergy = CalculatePixelEnergy(&image[0][y + 1], &image[0][y - 1]);

		//totalEnergy = xEnergy + yEnergy;
		//image[0][y].energy = totalEnergy;

		xEnergy = CalculatePixelEnergy(&image[actualWidth - 2][y], &image[actualWidth - 1][y]);
		yEnergy = CalculatePixelEnergy(&image[actualWidth - 1][y + 1], &image[actualWidth - 1][y - 1]);

		totalEnergy = xEnergy + yEnergy;
		image[actualWidth - 1][y].energy = totalEnergy;

		xEnergy = CalculatePixelEnergy(&image[0][y], &image[1][y]);
		yEnergy = CalculatePixelEnergy(&image[0][y + 1], &image[0][y - 1]);

		totalEnergy = xEnergy + yEnergy;
		image[0][y].energy = totalEnergy;
	}

	//CORNERS
	//Top left. 6
	//xEnergy = CalculatePixelEnergy(&image[actualWidth - 1][0], &image[1][0]);
	//yEnergy = CalculatePixelEnergy(&image[0][actualHeight - 1], &image[0][1]);

	//totalEnergy = xEnergy + yEnergy;
	//image[0][0].energy = totalEnergy;

	xEnergy = CalculatePixelEnergy(&image[0][0], &image[1][0]);
	yEnergy = CalculatePixelEnergy(&image[0][0], &image[0][1]);

	totalEnergy = xEnergy + yEnergy;
	image[0][0].energy = totalEnergy;

	//Top right. 7
	//xEnergy = CalculatePixelEnergy(&image[actualWidth - 2][0], &image[0][0]);
	//yEnergy = CalculatePixelEnergy(&image[actualWidth - 1][actualHeight - 1], &image[actualWidth - 1][1]);

	//totalEnergy = xEnergy + yEnergy;
	//image[actualWidth - 1][0].energy = totalEnergy;

	xEnergy = CalculatePixelEnergy(&image[actualWidth - 2][0], &image[actualWidth - 1][0]);
	yEnergy = CalculatePixelEnergy(&image[actualWidth - 1][0], &image[actualWidth - 1][1]);

	totalEnergy = xEnergy + yEnergy;
	image[actualWidth - 1][0].energy = totalEnergy;

	//Bottom left. 8
	//xEnergy = CalculatePixelEnergy(&image[actualWidth - 1][actualHeight - 1], &image[1][actualHeight - 1]);
	//yEnergy = CalculatePixelEnergy(&image[0][actualHeight - 2], &image[0][0]);

	//totalEnergy = xEnergy + yEnergy;
	//image[0][actualHeight - 1].energy = totalEnergy;

	xEnergy = CalculatePixelEnergy(&image[0][actualHeight - 1], &image[1][actualHeight - 1]);
	yEnergy = CalculatePixelEnergy(&image[0][actualHeight - 2], &image[0][actualHeight - 1]);

	totalEnergy = xEnergy + yEnergy;
	image[0][actualHeight - 1].energy = totalEnergy;

	//Bottom right. 9
	//xEnergy = CalculatePixelEnergy(&image[actualWidth - 2][actualHeight - 1], &image[0][actualHeight - 1]);
	//yEnergy = CalculatePixelEnergy(&image[actualWidth - 1][actualHeight - 2], &image[actualWidth - 1][0]);

	//totalEnergy = xEnergy + yEnergy;
	//image[actualWidth - 1][actualHeight - 1].energy = totalEnergy;

	xEnergy = CalculatePixelEnergy(&image[actualWidth - 2][actualHeight - 1], &image[actualWidth - 1][actualHeight - 1]);
	yEnergy = CalculatePixelEnergy(&image[actualWidth - 1][actualHeight - 2], &image[actualWidth - 1][actualHeight - 1]);

	totalEnergy = xEnergy + yEnergy;
	image[actualWidth - 1][actualHeight - 1].energy = totalEnergy;

	ofstream myfile;
	myfile.open("Pictures/Debug/energyPIC" + to_string(counter2) + ".txt");
	counter2++;

	for (int y = 0; y < actualHeight; y++)
	{
		for (int x = 0; x < actualWidth; x++)
		{
			myfile << image[x][y].energy;
			for (int i = 0; i < 12 - GetNumberOfDigits(image[x][y].energy); i++)
			{
				myfile << " ";
			}
		}
		myfile << endl << endl << endl;
	}

	//myfile.close();
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
	{
		cout << "Encoder error " << error << ": " << lodepng_error_text(error) << endl;
	}
}
void Picture::SaveJPEG(string path)
{

}