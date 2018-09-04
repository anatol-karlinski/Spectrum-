#include "pch.h"
#include "libbmp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread> 

using namespace std;

struct RGB {
	int r;
	int g;
	int b;
};

int B(int grayscale)
{
	if (grayscale >= 0 && grayscale <= 102) return 255;
	if (grayscale >= 103 && grayscale <= 152) return B(grayscale - 1) - 5;
	if (grayscale >= 153 && grayscale <= 255) return 0;
	return 0;
}

int G(int grayscale)
{
	if (grayscale <= 0 && grayscale >= 51) return 0;
	if (grayscale >= 52 && grayscale <= 101) return G(grayscale - 1) + 5;
	if (grayscale >= 102 && grayscale <= 204) return 255;
	if (grayscale >= 205 && grayscale <= 255) return G(grayscale - 1) - 5;
	return 0;
}

int R(int grayscale)
{
	if (grayscale == 0) return 255;
	if (grayscale >= 1 && grayscale <= 50) return R(grayscale - 1) - 5;
	if (grayscale >= 51 && grayscale <= 153) return 0;
	if (grayscale >= 154 && grayscale <= 203) return R(grayscale - 1) + 5;
	if (grayscale >= 204 && grayscale <= 255) return 255;
	return 0;
}


RGB ConvertGrayscaleToRGB(int grayscale)
{
	RGB rgb;

	rgb.r = R(grayscale);
	rgb.g = G(grayscale);
	rgb.b = B(grayscale);

	return rgb;
}

void ConvertImage(string inputFile, string outputFile) {

	auto startTime = chrono::high_resolution_clock::now();
	FILE* imageFile = fopen(inputFile.c_str(), "rb");

	if (imageFile == NULL) {
		cout << "File " << inputFile << " was not found." << endl;
		return;
	}
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, imageFile);

	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int row_padded = (width * 3 + 3) & (~3);
	unsigned char* data = new unsigned char[row_padded];
	unsigned char temp;
	BmpImg bmpImage(width, height);

	for (int i = 0; i < height; i++)
	{
		fread(data, sizeof(unsigned char), row_padded, imageFile);
		for (int j = 0, k = 0; j < width * 3; j += 3, k++)
		{
			RGB rgb = ConvertGrayscaleToRGB((int)data[j]);
			bmpImage.set_pixel(k, height - 1 - i, rgb.r, rgb.g, rgb.b);
		}
	}

	fclose(imageFile);
	bmpImage.write(outputFile.c_str());
	auto finishTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();

	cout << "File " << outputFile.c_str() << " (Size " << width << " x " << height << ") was created in " << finishTime << " ms" << endl;
}

int main()
{
	string inputFile1 = "D:/a2.bmp";
	string outputFile1 = "D:/o2.bmp";

	thread thread1(ConvertImage, inputFile1, outputFile1);

	string inputFile2 = "D:/a1.bmp";
	string outputFile2 = "D:/o1.bmp";

	thread thread2(ConvertImage, inputFile2, outputFile2);

	thread2.join();
	thread1.join();

	cout << "Press ENTER to exit.";
	cin.get();

	return 0;
}