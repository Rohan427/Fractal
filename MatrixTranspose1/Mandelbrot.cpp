#include "Mandelbrot.h"
#include <iostream>
#include <SDL.h>
#include <chrono>
#include "Mandel.h"
#include "MandelGPU.h"

using namespace std;

int availableThreads = std::thread::hardware_concurrency ();

int main (int argc, char* argv[])
{
	ErrorHandler errHdlr;
	EventHandler runner = EventHandler();
	MandelGPU mandelGPU;

	mandelGPU.mandelGPU();

	cout << "Event loop started" << std::endl;
	errHdlr = runner.run();

	return errHdlr.getStatus();
}
