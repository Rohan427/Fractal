#pragma once

#include <vector>
#include <iostream>
#include "Constants.h"
#include "Window.h"
#include "MouseManager.h"
#include "MandelGPU.h"

class Mandel
{
	public:
		struct palettePoint
		{
			Uint8 r;
			Uint8 g;
			Uint8 b;
		};

		bool USEGPU = true;

		double* hostBuffer = nullptr;

		typedef std::vector<palettePoint>  colors;
		std::vector<colors> palettes;

		struct pixelValue
		{
			Uint32 x = 0;		// x coordinate
			Uint32 y = 0;		// y coordinate
			Uint32 n = 0;		// Iteration count
		};

		typedef std::vector<pixelValue> row;

		struct clParams
		{
			double x = 0;			// x coordinate
			double y = 0;			// y coordinate
			double n = 0;			// Iteration count
			double n4 = 0;			// Extra iteration count
			double max = 0.0;		// Max number of iterations
			double z = 0;			// |z|
			Uint32 red = 0x00;		// Red color seed
			Uint32 green = 0x77;	// Green color seed
			Uint32 blue = 0x77;		// Blue color seed
			Uint32 color = 0;		// Calculated color
			ErrorHandler status;	// Status
			colors* palette;
			std::vector<pixelValue>* pixels;
			std::vector<row>* rows;
			int cycles;
			Window* renderer;
		};

		struct gpuPixel
		{
			double x;
			double y;
			double n;
			double c_im;
			double c_re;
		};

		std::vector<pixelValue> pixels;
		std::vector<row> rows;
		std::vector<gpuPixel>* gpuPixels;

		// Iterator variables
		int iteration;
		Window imageWindow;
		MouseManager* mgr;
		int height;
		int width;

	private:
		// Current fractal bounds
		struct bounds
		{
			double MinRe = MINRE;
			double MaxIm = MAXIM;
			double MaxRe = MAXRE;
			double Re_factor = (MAXRE - MINRE) / (WIN_WIDTH - 1.0);
			double Im_factor = (MAXIM - MINIM) / (WIN_HEIGHT - 1.0);
			double MinIm = MINIM;
		} curBounds;

		// Previous fractal bounds
		bounds preBounds;

		Uint32 iterMax = 0;
		Uint32 iterMin = INITIALITERATION;
		Uint32 iterLast = 0;
		ErrorHandler calcStatus;

		std::vector<palettePoint>* curPalette;

		int paletteIndex = 0;
		clParams lastImageData;
		unsigned int hostBuffersz;

		// Methods
		void debug (std::string msg);
		void showPalette (int index);
		void genColors();
		void genColorsrd();
		void genColorsb();
		void genColorsg();
		void genColorsbw();

	public:

		// Methods
		void iterate (Uint32 ImageHeight, Uint32 ImageWidth, double max, Window renderer, MouseManager* mgr);
		void iterate2();
		void initPixelData (Uint32 ImageHeight, Uint32 ImageWidth, double max, Window renderer, MouseManager* mgr);
		void initPixelDataGPU (Uint32 ImageHeight, Uint32 ImageWidth, double max, Window renderer, MouseManager* mgr);
		void calcLoop (Uint32 ImageWidth, double Re_factor, double Im_factor, double MaxIm, double MinRe, Mandel::gpuPixel curPixel);
		void createPalettes();
		double getReFactor();
		double getImFactor();
		void setFractalParams (double minRe, double maxRe, double minIm);
		void calcFractalBounds (double x, double y, double length, double width);
		bounds getBounds();
		void initBounds();
		void loadPalette (int index);
		ErrorHandler drawImage();
		void cylePaletteUp();
		void cylePaletteDn();
		ErrorHandler replotImage (MouseManager* mgr, Window renderer);

		void testPoint (double max, Mandel::gpuPixel* data);
		void testPointGPU (double max, double* hostBuffer, int numPixels);
		clParams simpleColor (Mandel::clParams* params);
		void iterate3 (double max, Window renderer, MouseManager* mgr);
		void iterate4 (double max, Uint32 ImageHeight, Uint32 ImageWidth, Window renderer, MouseManager* mgr);
};
