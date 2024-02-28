#include <iostream>
#include <complex>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <functional>
#include "Mandel.h"

#define DEBUG false
#define DETAIL false

using namespace std;

std::mutex pixelM;

class Calc
{
	private:

	public:
	/* Calculation is f(z)0 - n->z = z ^ 2 + c, c = x + yi
	* int max	Max number of iterations
	*/
	Mandel::clParams testPoint (double c_re, double c_im, Mandel::clParams data)
	{
//		Mandel::clParams calcData = data;
		Uint32 count = 0;
		bool extra = false;
		double Z_re2;
		double Z_im2;
		double zReold = 0;
		double zImold = 0;
		int period = 20;
		double Z_re = c_re, Z_im = c_im;

		data.n = 0;

		for (; count < data.max; ++count)
		{
			Z_re2 = Z_re * Z_re;
			Z_im2 = Z_im * Z_im;

			if (Z_re2 + Z_im2 > 4)
			{ // Here if not in the set
				extra = true;
				break;
			}

			Z_im = 2 * Z_re * Z_im + c_im;
			Z_re = Z_re2 - Z_im2 + c_re;

			// Check for periodicity
			if (Z_im == zImold && Z_re == zReold)
			{
				data.n = data.n4 = data.max;  /* Set to max for the color plotting */
				break;        /* We are inside the Mandelbrot set, leave the while loop */
			}

			period++;

			if (period > 20)
			{
				period = 0;
				zImold = Z_im;
				zReold = Z_re;
			}
			// else continue
			// End periodicity check
		}

		if (data.n != data.max) // periodicity check
		{						// periodicity check
			data.n = data.n4 = count;

			if (extra)
			{
				for (count = 0; count < 4; ++count)
				{
					Z_re2 = Z_re * Z_re;
					Z_im2 = Z_im * Z_im;

					Z_im = 2 * Z_re * Z_im + c_im;
					Z_re = Z_re2 - Z_im2 + c_re;

					data.n4++;
				}

				data.z = abs (Z_im + Z_re);
			}
		}						// periodicity check

		return data;
	}

	Mandel::clParams simpleColor (Mandel::clParams params)
	{
		std::string msg = {};
		Mandel::clParams newColor = params;
		int index = 0;
		double f;
		double i;

		if (params.n < params.max)
		{
			/*
			* m = max iterations
			* n = number of iterations
			* y = percent max iterations for pixel
			* c = colors per palette
			* d = palettes per max iterations
			* t = total colors for pixel
			* p = total d for the pixel
			* i = index
			*
			* d = m/c
			* y = n/m
			* p = y * d		= (n/m) * (m/c)
			* t = p * c     = c((n/m) * (m/c))
			* i = mod (t/c) = mod ((c((n/m) * (m/c)))/c)
			*
			* modf (((m/c) * (n/m)) * c, &index)
			*/

			//index = static_cast<int> (floor ((fmod (params.n, params.cycles) / params.palette->size()) * (params.palette->size() - 1)));
			f = modf ((double)((params.max / params.palette->size ()) * (params.n / params.max)), &i);
			f = modf (f * params.palette->size (), &i);
			index = (int)i;

#if DEBUG
			std::cout << "Colors: " << params.palette->size () << "\n";
			std::cout << "Cycles: " << params.cycles << "\n";
			std::cout << "   Max: " << params.max << "\n";
			std::cout << "  Iter: " << params.n << "\n";
			std::cout << "     f: " << f << "\n";
			std::cout << "     i: " << i << "\n";
			std::cout << " index: " << index << std::endl;
#endif

			pixelM.lock ();
			newColor.red = newColor.palette->at (index).r;
			newColor.green = newColor.palette->at (index).g;
			newColor.blue = newColor.palette->at (index).b;
			pixelM.unlock ();
#if DEBUG
			printf ("RBG: 0x%x, 0x%x, 0x%x\n\n", newColor.red, newColor.green, newColor.blue);
#endif
		}
		else
		{ // In the set, color black
			pixelM.lock ();
			newColor.red = COL_BLACK;
			newColor.green = COL_BLACK;
			newColor.blue = COL_BLACK;
			pixelM.unlock ();
		}

		return newColor;
	}

	Mandel::clParams smoothColor (Mandel::clParams params)
	{
		Mandel::clParams newColor = params;
		double fract;
		unsigned N = newColor.palette->size ();
		double i = params.n;
		double max = params.max;
		double s = 2;
		double i_max;
		double i_max2;
		double i_maxN;
		double i_maxN1_5;
		double mod;

		if (newColor.n == newColor.max)
		{ // In the set, color black
			newColor.red = COL_BLACK;
			newColor.green = COL_BLACK;
			newColor.blue = COL_BLACK;
		}
		else
		{
			i_max = i / max;
			i_max2 = pow (i_max, 2);
			i_maxN = i_max2 * N;
			i_maxN1_5 = pow (i_maxN, 1.5);
			fract = fmod (i_maxN1_5, N);

			double fractM = (fract);
			int fractR = ceil (fractM);
#if DEBUG
			if (i > 10)
			{
				cout << "     i: " << i << std::endl;
				cout << "   max: " << max << std::endl;
				cout << "     s: " << s << std::endl;
				cout << "     N: " << N << std::endl;
				cout << " mod N: " << fract << std::endl;
				cout << " Index: " << fractM << std::endl << std::endl;
				cout << "IndexR: " << fractR << std::endl << std::endl;
			}
#endif
			newColor.red = newColor.palette->at (fractR).r;
			newColor.green = newColor.palette->at (fractR).g;
			newColor.blue = newColor.palette->at (fractR).b;
#if DEBUG
			printf ("RBG: 0x%x, 0x%x, 0x%x\n\n", newColor.red, newColor.green, newColor.blue);
#endif
		}

		return newColor;
	}

	ErrorHandler calcLoop (Uint32 ImageWidth, double Re_factor, double Im_factor, double MaxIm, double MinRe, Mandel::clParams calcdata, Window renderer)
	{
		Mandel::clParams data = calcdata;
		ErrorHandler status;
		Calc calc;
		double c_im = MaxIm - data.y * Im_factor;

		for (data.x = 0; data.x < ImageWidth; ++data.x)
		{
			double c_re = MinRe + data.x * Re_factor;
			Mandel::pixelValue pixel;

			data = calc.testPoint (c_re, c_im, data);

			pixel.n = data.n;
			pixel.x = data.x;
			pixel.y = data.y;

			pixelM.lock ();
			data.pixels->push_back (pixel);
			pixelM.unlock ();

			data = simpleColor (data);
#if DEBUG
			printf ("Plotting RGBA: 0x%x 0c%x 0x%x 0x%x\n", data.red, data.green, data.blue, MAX_COL_VALUE);
#endif
			displayMutex.lock ();
			status = renderer.plotPoint (data.x, data.y, data.red, data.green, data.blue, MAX_COL_VALUE);
			displayMutex.unlock ();

			if (status.getStatus () != ERR_SUCCESS)
			{
				return status;
			}
			// else continue
		}

		return status;
	}

	ErrorHandler replotImage (Mandel::clParams data, int i, Window renderer)
	{
		ErrorHandler status;

		//cout << "Begin repolot" << endl;

		data.x = data.pixels->at (i).x;
		data.y = data.pixels->at (i).y;
		data.n = data.pixels->at (i).n;

		data = simpleColor (data);

		displayMutex.lock ();
		status = renderer.plotPoint (data.x, data.y, data.red, data.green, data.blue, MAX_COL_VALUE);
		displayMutex.unlock ();

		//cout << "End replot" << endl;

		return status;
	}
};





Mandel::clParams Mandel::simpleColor (Mandel::clParams* params)
{
	std::string msg = {};
	Mandel::clParams newColor = *params;
	int index = 0;
	double f;
	double i;

	if (params->n < params->max)
	{
		/*
		* m = max iterations
		* n = number of iterations
		* y = percent max iterations for pixel
		* c = colors per palette
		* d = palettes per max iterations
		* t = total colors for pixel
		* p = total d for the pixel
		* i = index
		*
		* d = m/c
		* y = n/m
		* p = y * d		= (n/m) * (m/c)
		* t = p * c     = c((n/m) * (m/c))
		* i = mod (t/c) = mod ((c((n/m) * (m/c)))/c)
		*
		* modf (((m/c) * (n/m)) * c, &index)
		*/

		//index = static_cast<int> (floor ((fmod (params.n, params.cycles) / params.palette->size()) * (params.palette->size() - 1)));
		f = modf ((double)((params->max / params->palette->size ()) * (params->n / params->max)), &i);
		f = modf (f * params->palette->size(), &i);
		index = (int)i;

#if DEBUG
		std::cout << "Colors: " << params->palette->size() << "\n";
		std::cout << "Cycles: " << params->cycles << "\n";
		std::cout << "   Max: " << params->max << "\n";
		std::cout << "  Iter: " << params->n << "\n";
		std::cout << "     f: " << f << "\n";
		std::cout << "     i: " << i << "\n";
		std::cout << " index: " << index << std::endl;
#endif

		pixelM.lock();
		newColor.red = newColor.palette->at (index).r;
		newColor.green = newColor.palette->at (index).g;
		newColor.blue = newColor.palette->at (index).b;
		pixelM.unlock();
#if DEBUG
		printf ("RBG: 0x%x, 0x%x, 0x%x\n\n", newColor.red, newColor.green, newColor.blue);
#endif
	}
	else
	{ // In the set, color black
		pixelM.lock();
		newColor.red = COL_BLACK;
		newColor.green = COL_BLACK;
		newColor.blue = COL_BLACK;
		pixelM.unlock();
	}

	return newColor;
}

/* Calculation is f(z)0 - n->z = z ^ 2 + c, c = x + yi
	* int max	Max number of iterations
	*/
void Mandel::testPoint (double max, Mandel::gpuPixel* data)
{

	//cout << "Mandel::testpoint: x " << data->x << ", y " << data->y <<  " begin" << endl;

	//		Mandel::clParams calcData = data;
	Uint32 count = 0;
	bool extra = false;
	double Z_re2;
	double Z_im2;
	double zReold = 0;
	double zImold = 0;
	int period = 20;
	double Z_re = data->c_re, Z_im = data->c_im;

	double z;
	double n4;

	data->n = 0;

	// TODO: Refactor to remove breaks?
	for (; count < max; ++count)
	{
		Z_re2 = Z_re * Z_re;
		Z_im2 = Z_im * Z_im;

		if (Z_re2 + Z_im2 > 4)
		{ // Here if not in the set
			extra = true;
			break;
		}

		Z_im = 2 * Z_re * Z_im + data->c_im;
		Z_re = Z_re2 - Z_im2 + data->c_re;

		// Check for periodicity
		if (Z_im == zImold && Z_re == zReold)
		{
			data->n = n4 = max;  /* Set to max for the color plotting */
			break;				 /* We are inside the Mandelbrot set, leave the while loop */
		}

		period++;

		if (period > 20)
		{
			period = 0;
			zImold = Z_im;
			zReold = Z_re;
		}
		// else continue
		// End periodicity check
	}

	if (data->n != max) // periodicity check
	{
		data->n = n4 = count;

		if (extra)
		{
			for (count = 0; count < 4; ++count)
			{
				Z_re2 = Z_re * Z_re;
				Z_im2 = Z_im * Z_im;

				Z_im = 2 * Z_re * Z_im + data->c_im;
				Z_re = Z_re2 - Z_im2 + data->c_re;

				n4++;
			}

			z = abs (Z_im + Z_re);
		}
	}// periodicity check

	//cout << "Mandel::testpoint: x " << data->x << ", y " << data->y << " END" << endl;
}

void Mandel::iterate (Uint32 ImageHeight, Uint32 ImageWidth, double max, Window renderer, MouseManager* mgr)//, EventHandler callback)
{
	clParams calcdata;
	calcdata.red = 0;
	calcdata.green = 0x00;
	calcdata.blue = 0x00;
	Calc calc;
	Uint32 y = 0;

	pixels.clear ();

	calcdata.max = max;
	calcdata.palette = curPalette;
	calcdata.pixels = &pixels;
	calcdata.rows = &rows;
	calcdata.cycles = floor (max / (calcdata.palette->size() - 1));
	calcdata.renderer = &renderer;
	int maxThreads = (availableThreads <= 0) ? 1 : availableThreads - 2;

	cout << std::setprecision (16);

#if DEBUG
#if DETAIL
	std::cout << "Image bounds:" << endl;
	std::cout << "       MinRe: " << curBounds.MinRe << endl;
	std::cout << "       MaxRe: " << curBounds.MaxRe << endl;
	std::cout << "       MinIm: " << curBounds.MinIm << endl;
	std::cout << "       MaxIm: " << curBounds.MaxIm << endl;
	std::cout << "   Re_factor: " << curBounds.Re_factor << endl;
	std::cout << "   Im_factor: " << curBounds.Im_factor << endl;
#endif
#endif
	while ((y < ImageHeight))
	{
		std::vector<std::thread> threads;

		if (y < (ImageHeight - maxThreads))
		{
			for (unsigned i = 0; i < maxThreads; ++i)
			{
				++y;
				calcdata.y = y;

				threads.push_back (std::thread (&Calc::calcLoop, &calc, ImageWidth, curBounds.Re_factor, curBounds.Im_factor, curBounds.MaxIm, curBounds.MinRe, calcdata, renderer));
			}
		}
		else
		{
			for (unsigned i = 0; i < (ImageHeight - y); ++i)
			{
				++y;
				calcdata.y = y;

				threads.push_back (std::thread (&Calc::calcLoop, &calc, ImageWidth, curBounds.Re_factor, curBounds.Im_factor, curBounds.MaxIm, curBounds.MinRe, calcdata, renderer));
			}
		}

		std::for_each (threads.begin(), threads.end(), std::mem_fn (&std::thread::join));
	}

	int i = 0;

	do
	{
		if (calcdata.pixels->at (i).n != INITIALITERATION)
		{
			if (iterMax <= calcdata.pixels->at (i).n)
			{
				iterMax = calcdata.pixels->at (i).n;
			}

			if (iterMin >= calcdata.pixels->at (i).n)
			{
				iterMin = calcdata.pixels->at (i).n;
			}
		}

		i++;
	} while (calcdata.pixels->size() > i);

	lastImageData = calcdata;
	mgr->setMode (MOUSE_NORM);
}

void Mandel::initPixelData (Uint32 ImageHeight, Uint32 ImageWidth, double max, Window renderer, MouseManager* mgr)//, EventHandler callback)
{
	cout << "In Mandel::initPixelData" << endl;

	pixelM.lock();
	gpuPixels.clear();
	pixelM.unlock();

	gpuPixel curPixel;
	int maxThreads = (availableThreads <= 0) ? 1 : availableThreads - 2;
	int pixVectSize = ImageHeight * ImageWidth;

	curPixel.y = 0;

	std::cout << std::setprecision (16);

	for (int j = 0; j < pixVectSize; j += ImageHeight)
	{
		while ((curPixel.y < ImageHeight))
		{
			std::vector<std::thread> threads;

			if (curPixel.y < (ImageHeight - maxThreads))
			{
				for (unsigned i = 0; i < maxThreads; ++i)
				{
					++curPixel.y;
					threads.push_back (std::thread (&Mandel::calcLoop, this, ImageWidth, curBounds.Re_factor, curBounds.Im_factor, curBounds.MaxIm, curBounds.MinRe, curPixel));
				}
			}
			else
			{
				for (unsigned i = 0; i < (ImageHeight - curPixel.y); ++i)
				{
					++curPixel.y;
					threads.push_back (std::thread (&Mandel::calcLoop, this, ImageWidth, curBounds.Re_factor, curBounds.Im_factor, curBounds.MaxIm, curBounds.MinRe, curPixel));
				}
			}

			std::for_each (threads.begin(), threads.end(), std::mem_fn (&std::thread::join));
		}
	}

	std::cout << "Total Pixels: " << gpuPixels.size() << std::endl;
}

void Mandel::iterate3 (double max, Window renderer, MouseManager* mgr)
{
	cout << "In Mandel::iterate3" << endl;

	unsigned i = 0;

	clParams calcdata;
	calcdata.red = 0;
	calcdata.green = 0x00;
	calcdata.blue = 0x00;

	pixels.clear();

	calcdata.max = max;
	calcdata.palette = curPalette;
	calcdata.pixels = &pixels;
	calcdata.rows = &rows;
	calcdata.cycles = floor (max / (calcdata.palette->size() - 1));
	calcdata.renderer = &renderer;
	int pixVectSize = gpuPixels.size();
	int j = 0;
	Mandel::pixelValue pixel;
	int maxThreads = (availableThreads <= 0) ? 1 : availableThreads - 2;

	std::cout << std::setprecision (16);

	cout << "Test points..." << endl;

	do
	{
		std::vector<std::thread> threads;

		if (j < (pixVectSize - maxThreads))
		{
			for (i = 0; i < maxThreads; ++i)
			{
				//cout << "testpoint " << j << endl;
				threads.push_back (std::thread (&Mandel::testPoint, this, max, &(gpuPixels.at (j))));
				++j;
			}
		}
		else
		{
			for (i = 0; i < (pixVectSize - j); ++i)
			{
				//cout << "testpoint " << j << endl;
				threads.push_back (std::thread (&Mandel::testPoint, this, max, &(gpuPixels.at (j))));
				++j;
			}
		}

		std::for_each (threads.begin(), threads.end(), std::mem_fn (&std::thread::join));
	} while (j < pixVectSize);

	cout << "Render points..." << endl;

	i = 0;

	do
	{
		calcdata.x = (double)gpuPixels.at (i).x;
		calcdata.y = (double)gpuPixels.at (i).y;
		calcdata.n = gpuPixels.at (i).n;
		calcdata.n4 = gpuPixels.at (i).n;

		pixel.n = calcdata.n;
		pixel.x = calcdata.x;
		pixel.y = calcdata.y;

		pixelM.lock();
		calcdata.pixels->push_back (pixel);
		pixelM.unlock();

		calcdata = simpleColor (&calcdata);
	#if DEBUG
		printf ("Plotting RGBA:\n    x: %4.0f\n    y: %4.0f\n    %#x %#x %#x %#x\n\n", calcdata.red, calcdata.green, calcdata.blue, MAX_COL_VALUE);
	#endif
		displayMutex.lock();
		calcdata.status = renderer.plotPoint (calcdata.x, calcdata.y, calcdata.red, calcdata.green, calcdata.blue, MAX_COL_VALUE);
		displayMutex.unlock();

		if (calcdata.status.getStatus() != ERR_SUCCESS)
		{
			std::cout << "ERROR: " << calcdata.status.getMsg() << std::endl;
		}
		// else continue

		i++;
	} while (i < gpuPixels.size());

	lastImageData = calcdata;
	mgr->setMode (MOUSE_NORM);

	std::cout << "Mandel::iterate3 END" << std::endl;
}

void Mandel::calcLoop (Uint32 ImageWidth, double Re_factor, double Im_factor, double MaxIm, double MinRe, Mandel::gpuPixel curPixel)
{
	Calc calc;
	curPixel.c_im = MaxIm - curPixel.y * Im_factor;

	//cout << "Thread: " << curPixel.y << endl;

	for (curPixel.x = 0; curPixel.x < ImageWidth; ++curPixel.x)
	{
		curPixel.c_re = MinRe + curPixel.x * Re_factor;
		pixelM.lock();
		gpuPixels.push_back (curPixel);
		pixelM.unlock();

		//std::cout << "Pixel:" << endl;
	    //std::cout << "    x     " << curPixel.x << std::endl;
		//std::cout << "    y     " << curPixel.y << std::endl;
		//std::cout << "    c_re  " << curPixel.c_re << std::endl;
		//std::cout << "    cc_im " << curPixel.c_im << std::endl;
	}
}

void Mandel::iterate2()
{
	clParams calcdata;
	calcdata.red = 0;
	calcdata.green = 0x00;
	calcdata.blue = 0x88;
	Calc calc;
	Uint32 y = 0;
	int maxThreads = std::thread::hardware_concurrency() - 2;

	calcdata.max = iteration;
	maxThreads = (maxThreads <= 0) ? 1 : maxThreads;

	std::setprecision (16);

	std::cout << "Image bounds:" << std::endl;
	std::cout << "       MinRe: " << curBounds.MinRe << std::endl;
	std::cout << "       MaxRe: " << curBounds.MaxRe << std::endl;
	std::cout << "       MinIm: " << curBounds.MinIm << std::endl;
	std::cout << "       MaxIm: " << curBounds.MaxIm << std::endl;
	std::cout << "   Re_factor: " << curBounds.Re_factor << std::endl;
	std::cout << "   Im_factor: " << curBounds.Im_factor << std::endl;

	while ((y < height))
	{
		std::vector<std::thread> threads;

		if (y < (height - maxThreads))
		{
			for (unsigned i = 0; i < maxThreads; ++i)
			{
				++y;
				calcdata.y = y;

				threads.push_back (std::thread (&Calc::calcLoop, &calc, width, curBounds.Re_factor, curBounds.Im_factor, curBounds.MaxIm, curBounds.MinRe, calcdata, imageWindow));
			}
		}
		else
		{
			for (unsigned i = 0; i < (height - y); ++i)
			{
				++y;
				calcdata.y = y;

				threads.push_back (std::thread (&Calc::calcLoop, &calc, width, curBounds.Re_factor, curBounds.Im_factor, curBounds.MaxIm, curBounds.MinRe, calcdata, imageWindow));
			}
		}

		std::for_each (threads.begin (), threads.end (), std::mem_fn (&std::thread::join));
	}

	mgr->setMode (MOUSE_NORM);
}

double Mandel::getReFactor()
{
	return curBounds.Re_factor;
}

double Mandel::getImFactor()
{
	return curBounds.Im_factor;
}

void Mandel::setFractalParams (double minRe, double maxRe, double minIm)
{
	curBounds.MinRe = minRe;
	curBounds.MaxRe = maxRe;
	curBounds.MinIm = minIm;
}


void Mandel::calcFractalBounds (double x, double y, double length, double width)
{
	// Fixed start point for plots is (MinRe, MaxIm) 
	curBounds.MinRe = preBounds.MinRe + (preBounds.Re_factor * x);
	curBounds.MaxIm = preBounds.MaxIm - (preBounds.Im_factor * y);
	curBounds.MaxRe = curBounds.MinRe + (preBounds.Re_factor * length);
	curBounds.MinIm = curBounds.MaxIm - (preBounds.Im_factor * width);// curBounds.MaxIm - (curBounds.MaxRe - curBounds.MinRe) * WIN_HEIGHT / WIN_WIDTH;
	curBounds.Im_factor = (curBounds.MaxIm - curBounds.MinIm) / (WIN_HEIGHT - 1.0);

	curBounds.Re_factor = curBounds.Im_factor;// (curBounds.MaxRe - curBounds.MinRe) / (WIN_WIDTH - 1.0);

	preBounds = curBounds;
}

Mandel::bounds Mandel::getBounds()
{
	return curBounds;
}

void Mandel::initBounds()
{
	bounds newBounds;

	curBounds = newBounds;
	preBounds = newBounds;
}

void Mandel::createPalettes()
{
	genColors();
	genColorsrd();
	genColorsb();
	genColorsg();
	genColorsbw();
}

/*
* Generate a linear color table from red to blue for fast color selection.
* The sequence is as follows:
* r = max intensity, b = off, g =  off
* b -> max
* r -> 0
* g -> max
* b -> 0
* r -> max
* g -> 1
*/
void Mandel::genColors()
{
	Uint8 max = 255; // 220
	Uint8 increment = 1;
	palettePoint color;
	colors colors;

	// Add first color, red
	color.r = max;
	color.b = 0;
	color.g = 0;
	colors.push_back (color);
	/* r = max intensity, b = off, g =  off
	 * b -> max
	 */
	while (color.b < max)
	{
		color.b += increment;
		colors.push_back (color);
	}

	// r -> 0
	while (color.r > 0)
	{
		color.r -= increment;
		colors.push_back (color);
	}

	// g -> max
	while (color.g < max)
	{
		color.g += increment;
		colors.push_back (color);
	}

	// b -> 0
	while (color.b > 0)
	{
		color.b -= increment;
		colors.push_back (color);
	}

	// r -> max
	while (color.r < max)
	{
		color.r += increment;
		colors.push_back (color);
	}

	// b -> max (increase to white from yellow)
	while (color.b < max)
	{
		color.b += increment;
		colors.push_back (color);
	}

	palettes.push_back (colors);

	cout << "Color RED palette size: " << colors.size();
	cout << std::endl;
}

void Mandel::genColorsrd()
{
	Uint8 max = 168; // 220
	Uint8 increment = 1;
	palettePoint color;
	colors colors;

	// Add first color, red
	color.r = max;
	color.b = 0;
	color.g = 0;
	colors.push_back (color);
	/* r = max intensity, b = off, g =  off
	 * b -> max
	 */
	while (color.b < max)
	{
		color.b += increment;
		colors.push_back (color);
	}

	// r -> 0
	while (color.r > 0)
	{
		color.r -= increment;
		colors.push_back (color);
	}

	// g -> max
	while (color.g < max)
	{
		color.g += increment;
		colors.push_back (color);
	}

	// b -> 0
	while (color.b > 0)
	{
		color.b -= increment;
		colors.push_back (color);
	}

	// r -> max
	while (color.r < max)
	{
		color.r += increment;
		colors.push_back (color);
	}

	palettes.push_back (colors);

	cout << "Color DARK RED palette size: " << colors.size();
	cout << "            Palettes: " << palettes.size () << std::endl;
	cout << std::endl;
}

void Mandel::genColorsb()
{
	Uint8 max = 255; // 220
	Uint8 increment = 1;
	palettePoint color;
	colors colors;

	// Add first color, blue
	color.r = 0;
	color.b = max;
	color.g = 0;
	colors.push_back (color);


	// r -> max
	while (color.r < max)
	{
		color.r += increment;
		colors.push_back (color);
	}

	// g -> max
	while (color.g < max)
	{
		color.g += increment;
		colors.push_back (color);
	}

	// b -> 0
	while (color.b > 0)
	{
		color.b -= increment;
		colors.push_back (color);
	}

	// b -> max
	while (color.b < max)
	{
		color.b += increment;
		colors.push_back (color);
	}

	// r -> 0
	while (color.r > 1)
	{
		color.r -= increment;
		colors.push_back (color);
	}

	palettes.push_back (colors);

	cout << "Color BLUE palette size: " << colors.size();
	cout << "            Palettes: " << palettes.size() << std::endl;
	cout << std::endl;
}

void Mandel::genColorsg()
{
	Uint8 max = 255; // 220
	Uint8 increment = 1;
	palettePoint color;
	colors colors;

	// Add first color, red
	color.r = 0;
	color.b = 0;
	color.g = 255;
	colors.push_back (color);

	// r -> max
	while (color.r < max)
	{
		color.r += increment;
		colors.push_back (color);
	}

	// g -> 0
	while (color.g > 0)
	{
		color.g -= increment;
		colors.push_back (color);
	}

	// b -> max
	while (color.b < max)
	{
		color.b += increment;
		colors.push_back (color);
	}

	// r -> 0
	while (color.r > 0)
	{
		color.r -= increment;
		colors.push_back (color);
	}

	// g -> max
	while (color.g < max)
	{
		color.g += increment;
		colors.push_back (color);
	}

	// b -> 1
	while (color.b > 1)
	{
		color.b -= increment;
		colors.push_back (color);
	}

	palettes.push_back (colors);

	cout << "Color GREEN palette size: " << colors.size ();
	cout << "            Palettes: " << palettes.size () << std::endl;
	cout << std::endl;
}

void Mandel::genColorsbw()
{
	Uint8 max = 255; // 220
	Uint8 increment = 1;
	palettePoint color;
	colors colors;

	// Add first color, red
	color.r = 0;
	color.b = 0;
	color.g = 0;
	colors.push_back (color);

	while (color.r < max)
	{
		color.r += increment;
		color.g += increment;
		color.b += increment;
		colors.push_back (color);
	}

	while (color.g > 1)
	{
		color.r -= increment;
		color.g -= increment;
		color.b -= increment;
		colors.push_back (color);
	}

	palettes.push_back (colors);

	std::cout << "Color GRAY palette size: " << colors.size();
	std::cout << "            Palettes: " << palettes.size() << std::endl;
	std::cout << std::endl;
}

void Mandel::cylePaletteUp()
{
	if (paletteIndex == palettes.size() - 1)
	{
		paletteIndex = 0;
	}
	else
	{
		paletteIndex++;
	}

	curPalette = &palettes.at (paletteIndex);
	showPalette (paletteIndex);
}

void Mandel::cylePaletteDn()
{
	if (paletteIndex == 0)
	{
		paletteIndex = palettes.size() - 1;
	}
	else
	{
		paletteIndex--;
	}

	curPalette = &palettes.at (paletteIndex);
	showPalette (paletteIndex);
}

ErrorHandler Mandel::replotImage (MouseManager* mgr, Window renderer)
{
	Calc calc;
	lastImageData.palette = curPalette;
	lastImageData.cycles = floor (lastImageData.max / (lastImageData.palette->size() - 1));
	int t = 0;

	while (t < lastImageData.pixels->size())
	{
		calc.replotImage (lastImageData, t, renderer);
		t++;
	}

	displayMutex.lock();
	renderer.resetColor();
	displayMutex.unlock();

	mgr->setMode (MOUSE_NORM);

	return calcStatus;
}

void Mandel::showPalette (int index)
{
	std::cout << "Using Palette " << paletteName[index] << std::endl;
	std::cout << "size: actual, selected " << palettes.at (paletteIndex).size() << ", " << lastImageData.palette->size () << std::endl;
}

ErrorHandler Mandel::drawImage ()
{
	return calcStatus;
}

void Mandel::loadPalette (int index)
{
	if ((index <= palettes.size() - 1)
		&& (index >= 0))
	{
		paletteIndex = index;
		curPalette = &palettes.at (paletteIndex);
	}
	// else ignore - out of range
}

void Mandel::debug (std::string msg)
{
#if DEBUG
	std::cout << msg;
#endif
}