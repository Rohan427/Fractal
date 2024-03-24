#include "EventHandler.h"
#include "MouseManager.h"
#include <algorithm>
#include <functional>

using namespace std;

std::mutex displayMutex;

class Plot
{
	private:
		std::mutex renderMutex;
		std::condition_variable runWait;

		void iterateThread()
		{
			while (!stop)
			{
				std::unique_lock<std::mutex> lock (renderMutex);
				runWait.wait (lock, [this] { return run || !stop; });

				if (run)
				{
					fractal.iterate2();
					run = false;
					lock.unlock();
					runWait.notify_all();
				}
			}
		}

	public:
		Window imageWindow;
		Mandel fractal;
		int width;
		int height;
		MouseManager* mgr;
		int iteration = INITIALITERATION;
		bool run = false;
		bool stop = false;


	void plotImage()
	{
		mgr->setMode (MOUSE_RENDER);
		std::cout << "Rendering image..." << endl;

		fractal.calcFractalBounds (mgr->getRectangle().startX,
								   mgr->getRectangle().startY,
								   mgr->getRectangle().length,
								   mgr->getRectangle().width
		);

		// Plot the image
		displayMutex.lock();
		imageWindow.clear (IMAGERESET);
		displayMutex.unlock();

		// initialize iterator
		fractal.imageWindow = imageWindow;
		fractal.width = width;
		fractal.height = height;
		fractal.mgr = mgr;
		fractal.iteration = iteration;

		// Starts rendering
		run = true;

		std::cout << "Image render complete" << endl;

		mgr->resetSize();

		cout << "Rectangle: " << endl;
		cout << "  start x: " << mgr->getRectangle().startX << endl;
		cout << "  start y: " << mgr->getRectangle().startY << endl;
		cout << "    end x: " << mgr->getRectangle().endX << endl;
		cout << "    end y: " << mgr->getRectangle().endY << endl;
	}

	void startThread()
	{
		run = false;
		std::vector<std::thread> threads;

		threads.push_back (thread (&Plot::iterateThread, this));

		std::for_each (threads.begin(), threads.end(), std::mem_fn (&thread::join));
	}
};



ErrorHandler EventHandler::run()
{
	int show = 0;
	int iteration = INITIALITERATION;
	int status = ERR_SUCCESS;

	//Plot imagePlot;

	fractal.createPalettes();
	fractal.loadPalette (PALETTE_BW);

	// Initialze SDL
	displayMutex.lock();
	status = imageWindow.init (SDL_INIT_VIDEO).getStatus();
	displayMutex.unlock();

	// If init worked, continue, otherwise bail out
	if (status != ERR_SUCCESS)
	{
		std::cout << "Failed to initialize window: "
			<< "Message: "
			<< errHdlr.getMsg()
			<< ", Detail: "
			<< errHdlr.getDetail()
			<< std::endl;
	}
	else if (TTF_Init() != 0)
	{
		std::cout << "Failed to initialize font engine" << std::endl;
	}
	// Create the menu window
	else
	{
		displayMutex.lock();
		errHdlr = menu.createWindow (MEN_WIDTH, MEN_HEIGHT);
		displayMutex.unlock();

		if (status != ERR_SUCCESS)
		{
			std::cout << "Failed to initialize menu window: "
				<< "Message: "
				<< errHdlr.getMsg()
				<< ", Detail: "
				<< errHdlr.getDetail()
				<< std::endl;
		}
		// Create the menu renderer
		else
		{
			displayMutex.lock();
			errHdlr = menu.createRenderer (DEFAULTCOLOR);
			displayMutex.unlock();

			if (errHdlr.getStatus() != ERR_SUCCESS)
			{
				std::cout << "Menu renderer failed: "
					<< "Message: "
					<< errHdlr.getMsg()
					<< ", Detail: "
					<< errHdlr.getDetail()
					<< std::endl;
			}
			// Create the image window
			else
			{
				std::cout << "Menu Window created, ID: " << menu.getWindowID() << std::endl;

				displayMutex.lock();
				//menu.clear (DEFAULTCOLOR);
				menu.initMenu();
				menu.setMinimized (false);
				menu.output (0, "DEFAULT");
				menu.show();
				errHdlr = imageWindow.createWindow (WIN_WIDTH, WIN_HEIGHT);
				displayMutex.unlock();

				if (errHdlr.getStatus() != ERR_SUCCESS)
				{
					std::cout << "Failed to create image window: "
						<< "Message: "
						<< errHdlr.getMsg()
						<< ", Detail: "
						<< errHdlr.getDetail()
						<< std::endl;
				}
				// Create the image renderer
				else
				{
					std::cout << "Image Window created, ID: " << imageWindow.getWindowID () << std::endl;

					displayMutex.lock();
					errHdlr = imageWindow.createRenderer (IMAGERESET);
					displayMutex.unlock();

					if (errHdlr.getStatus() != ERR_SUCCESS)
					{
						std::cout << "Image renderer failed: "
							<< "Message: "
							<< errHdlr.getMsg()
							<< ", Detail: "
							<< errHdlr.getDetail()
							<< std::endl;
					}
					else
					{
						/*std::cout << "Plot test" << std::endl;
						errHdlr = window.plotTest();

						if (errHdlr.getStatus() != ERR_SUCCESS)
						{
							std::cout << "Run failed: "
								<< "Message: "
								<< errHdlr.getMsg()
								<< ", Detail: "
								<< errHdlr.getDetail()
								<< std::endl;
						}
						else*/
						{
							std::cout << "Initial Image..." << std::endl;

							mgr.setMode (MOUSE_RENDER);

							// Plot the image
							displayMutex.lock();
							imageWindow.setPreviousColor (color_t{0x00, 0x77, 0x00, 0xff});
							imageWindow.resetColor();
							displayMutex.unlock();

							cout << "Plot inital image..." << endl;

							if (fractal.USEGPU)
							{
								std::thread initial (&Mandel::initPixelDataGPU, &fractal, (Uint32)WIN_HEIGHT, (Uint32)WIN_WIDTH, (double)iteration, imageWindow, &mgr);

								if (initial.joinable())
								{
									initial.join();
								}

								std::thread plot1 (&Mandel::iterate4, &fractal, (double)iteration, (Uint32)WIN_HEIGHT, (Uint32)WIN_WIDTH, imageWindow, &mgr);

								if (plot1.joinable())
								{
									plot1.join();
								}
							}
							else
							{
								std::thread initial (&Mandel::iterate, &fractal, WIN_HEIGHT, WIN_WIDTH, iteration, imageWindow, &mgr);

								if (initial.joinable())
								{
									initial.detach();
								}
							}

							cout << "Initial image complete" << endl;

							mgr.resetSize();

							while (isRunning && (status == ERR_SUCCESS))
							{
								show++;

								while (SDL_PollEvent (&ev) != 0)
								{
									handleWindowEvent();
								}

								if (show >= REFRESHRATE)
								{
									//cout << "MODE " << mgr.getMode() << endl;

									displayMutex.lock();
									imageWindow.show();
									displayMutex.unlock();

									if (mgr.getMode() == MOUSE_NORM)
									{
										rectangle();
									}
									// else don't redraw rectangle

									if (menu.isWindowUpdate())
									{
										//cout << "render menu" << endl;
										displayMutex.lock();
										menu.show();
										displayMutex.unlock();
									}
									// else continue

									show = 0;
								}
								//else continue
							} // ENDWHILE: while (isRunning && (status == ERR_SUCCESS))
						} //ENDIF: if (errHdlr.getStatus() != ERR_SUCCESS) (plot test - optional)
					} // ENDIF: if (errHdlr.getStatus() != ERR_SUCCESS) (Image renderer)
				} // ENDIF: if (errHdlr.getStatus() != ERR_SUCCESS) (Image creation)
			} // ENDIF: if (status != ERR_SUCCESS) (menu renderer)
		} // ENDIF-ELSE: if (status != ERR_SUCCESS) (menu creation)
	} // ENDIF: if (status != ERR_SUCCESS) (SDL init)

	imageWindow.getWindow().cleanUp();
	menu.getWindow().cleanUp();
	HIP_CHECK (hipHostFree (fractal.hostBuffer));
	return errHdlr;
}

void EventHandler::rectangle()
{
	int length;
	int width;
	int startx = mgr.getRectangle ().startX;
	int starty = mgr.getRectangle ().startY;

	// Adjust length and start based on which X user selected first
	if (mgr.getRectangle().startX > mgr.getRectangle().endX)
	{
		length = mgr.getRectangle().startX - mgr.getRectangle().endX;
		startx = mgr.getRectangle().endX;
	}
	else
	{
		length = mgr.getRectangle().endX - mgr.getRectangle().startX;
	}

	// Adjust width based on which Y user selected first
	if (mgr.getRectangle().startY > mgr.getRectangle().endY)
	{
		width = mgr.getRectangle().startY - mgr.getRectangle().endY;
		starty = mgr.getRectangle().endY;
	}
	else
	{
		width = mgr.getRectangle().endY - mgr.getRectangle().startY;
	}

	displayMutex.lock();
	imageWindow.drawRectangle (startx, starty, length, width, mgr.getRectColor());
	displayMutex.unlock ();
}

void EventHandler::plotImage (MouseManager* mgr, int iteration)
{
	mgr->setMode (MOUSE_RENDER);
	std::cout << "Rendering image with CPU..." << endl;

	fractal.calcFractalBounds (mgr->getRectangle().startX,
							   mgr->getRectangle().startY,
							   mgr->getRectangle().length,
							   mgr->getRectangle().width
	);

	// Plot the image
	displayMutex.lock();
	imageWindow.resetColor();
	displayMutex.unlock();

	std::thread plot (&Mandel::iterate, &fractal, WIN_HEIGHT, WIN_WIDTH, iteration, imageWindow, mgr);

	if (plot.joinable ())
	{
		plot.detach ();
	}
	// else contine

	mgr->resetSize();
}

void EventHandler::plotImageGPU (MouseManager* mgr, int iteration)
{
	mgr->setMode (MOUSE_RENDER);
	std::cout << "Rendering image with GPU..." << endl;

	fractal.calcFractalBounds (mgr->getRectangle().startX,
							   mgr->getRectangle().startY,
							   mgr->getRectangle().length,
							   mgr->getRectangle().width
	);

	// Plot the image
	displayMutex.lock();
	imageWindow.resetColor();
	displayMutex.unlock();

	std::thread initial (&Mandel::initPixelDataGPU, &fractal, (Uint32)WIN_HEIGHT, (Uint32)WIN_WIDTH, (double)iteration, imageWindow, mgr);

	if (initial.joinable())
	{
		initial.join();
	}

	std::thread plot1 (&Mandel::iterate4, &fractal, (Uint32)WIN_HEIGHT, (Uint32)WIN_WIDTH, (double)iteration, imageWindow, mgr);

	if (plot1.joinable())
	{
		plot1.detach();
	}
	// else contine

	mgr->resetSize();
}

void EventHandler::replotImage (MouseManager* mgr)
{
	setMode2 (mgr, MOUSE_RENDER);

	// Plot the image
	displayMutex.lock();
	imageWindow.resetColor();
	displayMutex.unlock();

	std::thread replot (&Mandel::replotImage, &fractal, mgr, imageWindow);

	if (replot.joinable())
	{
		replot.detach();
	}
	// else continue

	mgr->resetSize();
}

void EventHandler::handleWindowEvent()
{
	int iteration = INITIALITERATION;

//	cout << "process event" << endl;

	if (ev.window.windowID == imageWindow.getWindowID())
	{
		imageWindow.handleWindowEvent (ev);

		switch (ev.type)
		{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym)
				{
					case SDLK_KP_ENTER:
						switch (mgr.getMode())
						{
							case MOUSE_NORM:
								if (fractal.USEGPU)
								{
									plotImageGPU (&mgr, iteration);
								}
								else
								{
									plotImage (&mgr, iteration);
								}

								break;

							case MOUSE_POINT1:
								//cout << "POINT 1 EXIT" << endl;
								setMode (MOUSE_NORM);
								break;

							case MOUSE_POINT2:
								//cout << "POINT 2 EXIT" << endl;
								setMode (MOUSE_NORM);
								break;

							case MOUSE_PALETTE:
								replotImage (&mgr);
								break;

							case MOUSE_COLOR:
								rectangle();
								setMode (MOUSE_NORM);
								break;

							default:
								break;
						}

						break;

					case SDLK_r:
						if (mgr.getMode() == MOUSE_NORM)
						{
							std::cout << "Initial Image..." << std::endl;

							// Plot the image
							displayMutex.lock();
							imageWindow.clear (IMAGERESET);
							displayMutex.unlock ();

							setMode (MOUSE_RENDER);

							mgr.initRect();
							fractal.initBounds();
							iteration = INITIALITERATION;
							std::thread rstImage (&Mandel::iterate3, &fractal, (double)iteration, imageWindow, &mgr);

							if (rstImage.joinable())
							{
								rstImage.detach();
							}
							// else continue

							mgr.resetSize ();
						}

						break;

					case SDLK_g:
						fractal.USEGPU = !fractal.USEGPU;

						cout << "Use GPU: " << fractal.USEGPU << endl;
						break;

					case SDLK_KP_PLUS:
						if ((mgr.getMode() == MOUSE_NORM)
							|| (mgr.getMode() == MOUSE_PALETTE)
							)
						{
							setMode (MOUSE_PALETTE);
							fractal.cylePaletteUp();
						}

						break;

					case SDLK_KP_MINUS:
						if ((mgr.getMode() == MOUSE_NORM)
							|| (mgr.getMode() == MOUSE_PALETTE)
							)
						{
							setMode (MOUSE_PALETTE);
							fractal.cylePaletteDn();
						}

						break;

					case SDLK_KP_PERIOD:
						if ((mgr.getMode() == MOUSE_NORM)
							|| (mgr.getMode() == MOUSE_PALETTE)
							)
						{
							setMode (MOUSE_PALETTE);
							fractal.loadPalette (0);
						}

						break;

					default:
						break;
				}

			case SDL_MOUSEBUTTONDOWN:
				switch (mgr.getMode())
				{
					case MOUSE_NORM:
						switch (ev.button.button)
						{
							case SDL_BUTTON_LEFT:
								cout << "RESIZE" << endl;
								break;

							case SDL_BUTTON_MIDDLE:
								cout << "COLOR" << endl;
								setMode (MOUSE_COLOR);
								break;

							case SDL_BUTTON_RIGHT:
								cout << "SELECT" << endl;
								setMode (MOUSE_POINT1);
								break;

							default:
								break;
						}

						break;

					case MOUSE_POINT1:
					{
						point mousePos;

						displayMutex.lock();
						mousePos = imageWindow.getMousePos ();
						displayMutex.unlock();

						mgr.setRectPoint1 (mousePos.x, mousePos.y);
						setMode (MOUSE_POINT2);
					}

					break;

					case MOUSE_POINT2:
					{
						point mousePos;

						displayMutex.lock();
						mousePos = imageWindow.getMousePos ();
						displayMutex.unlock();

						mgr.setRectPoint2 (mousePos.x, mousePos.y);
						setMode (MOUSE_NORM);
						rectangle();
						mgr.finishRect();
					}

					break;

					case MOUSE_SIZE:
						break;

					case MOUSE_COLOR:
						switch (ev.button.button)
						{
							case SDL_BUTTON_LEFT:
								cout << "COLOR UP" << endl;
								mgr.incColor();
								break;

							case SDL_BUTTON_MIDDLE:
								cout << "COLOR RESET" << endl;
								mgr.resetColor();
								break;

							case SDL_BUTTON_RIGHT:
								cout << "COLOR DN" << endl;
								mgr.decColor();
								break;

							default:
								break;
						}

						break;
				}

				break;

			default:
				break;
		}
	}
	else if (ev.window.windowID == menu.getWindowID())
	{
		menu.handleWindowEvent (ev);

		switch (ev.window.event)
		{
			case SDL_WINDOWEVENT_CLOSE:
				isRunning = false;
				break;

			default:
				break;
		}
	}
	// else continue, no more windows
}

void EventHandler::setMode (int mode)
{
	mgr.setMode (mode);

	if (mode >= MOUSE_NORM)
	{
		menu.output (0, mouseModes[mode]);
	}
	// else we're quitting
}

void EventHandler::setMode2 (MouseManager *mgr, int mode)
{
	mgr->setMode (mode);

	if (mode >= MOUSE_NORM)
	{
		menu.output (0, mouseModes[mode]);
	}
	// else we're quitting
}
