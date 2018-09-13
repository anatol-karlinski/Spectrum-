#include "pch.h"
#include "libbmp.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread> 
#include <sstream>

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

void ConvertImage(int threadNr, string inputFile, string outputFile) {

	std::stringstream message;

	message << "Thread #" << threadNr << ": Work started." << endl;
	cout << message.str();

	auto startTime = chrono::high_resolution_clock::now();

	// Wczytanie strumiania bitow obrazka o nazwie 'inputFile'
	FILE* imageFile = fopen(inputFile.c_str(), "rb");

	// Zabezpieczenie się na wypadek gdzyby obrazka nie było
	if (imageFile == NULL) {
		message.str("");
		message << "Thread #" << threadNr << ": File " << inputFile << " was not found." << endl;
		cout << message.str();
		return;
	}

	message.str("");
	message << "Thread #" << threadNr << ": Loaded image " << inputFile << "." << endl;
	cout << message.str();

	// Odczytanie pierwszych 54 bitów strumienia. Pierwsze 54 bity to nagłowek z informacjami o nazwie, wymiarach i formacie pliku.
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, imageFile);

	// Wyciągnięcie informacji o wymiarach obrazka z nagłowka
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	// Obliczenie przesuniecia bitow ktore występuje w plikach BMP 24
	int row_padded = (width * 3 + 3) & (~3);
	unsigned char* data = new unsigned char[row_padded];
	unsigned char temp;

	// Stworzenie pustego pliku bmp o odpowiednich rozmiarach. Będziemy go wypelniac odpowiedznimi kolorami pozniej.
	BmpImg bmpImage(width, height);

	for (int i = 0; i < height; i++)
	{
		// Zaczytanie pojedynczego wiersz pixeli ze strumienia bitow
		fread(data, sizeof(unsigned char), row_padded, imageFile);
		for (int j = 0, k = 0; j < width * 3; j += 3, k++)
		{
			// Zamiana pojedyczgo pixela z koloru szarosci na RGB
			RGB rgb = ConvertGrayscaleToRGB((int)data[j]);
			bmpImage.set_pixel(k, height - 1 - i, rgb.r, rgb.g, rgb.b);
		}
	}
	// Zamknięcie strumienia bitów oryginalnego obrazka
	fclose(imageFile);

	// Zapisanie nowego, kolorowego obrazka do pliku 'outputFile'
	bmpImage.write(outputFile.c_str());

	// Zatrzymanie stopera liczącego czas wykonania calej operacji.
	auto finishTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();

	message.str("");
	message << "Thread #" << threadNr << ": File " << outputFile.c_str() << " (Size " << width << " x " << height << ") was created in " << finishTime << " ms" << endl;
	cout << message.str();
}

int main()
{
	string inputFile1 = "D:/a2.bmp";
	string outputFile1 = "D:/o2.bmp";

	thread thread1(ConvertImage, 1, inputFile1, outputFile1);

	string inputFile2 = "D:/a1.bmp";
	string outputFile2 = "D:/o1.bmp";

	thread thread2(ConvertImage, 2, inputFile2, outputFile2);

	thread2.join();
	thread1.join();

	cout << "Press ENTER to exit.";
	cin.get();

	return 0;
}