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

	errHdlr.setErrorData (mandelGPU.mandelGPU2(), "", "", REND_OK);

	if (errHdlr.getStatus() > 0)
	{
		cout << "Test failed" << std::endl;
		return errHdlr.getStatus();
	}
	// else start application

	cout << "Event loop starting..." << std::endl;
	errHdlr = runner.run();

	return errHdlr.getStatus();
}
