#pragma once

#include <iostream>
#include <SDL.h>
#include <chrono>
#include <thread>
#include "Mandel.h"

class EventHandler
{
	private:
		SDL_Event ev = {};
		bool isRunning = true;

		Mandel fractal;
		ErrorHandler errHdlr;

		Window imageWindow;
		Window menu;
		MouseManager mgr;

		void plotImage (MouseManager *mgr, int iteration);
		void plotImageGPU (MouseManager* mgr, int iteration);
		void replotImage (MouseManager *mgr);
		void handleWindowEvent ();
		void setMode (int mode);
		void setMode2 (MouseManager *mgr, int mode);

	public:
		ErrorHandler run();
		void rectangle();
};
