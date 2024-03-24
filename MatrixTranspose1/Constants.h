#pragma once

#include <string>
#include <mutex>
#include <iomanip>
#include <thread>

extern std::mutex displayMutex;

struct point
{
	int x;
	int y;
};

// Display constants
const double WIN_WIDTH = 2048;
const double WIN_HEIGHT = 1024;
const unsigned MAX_COL_VALUE = 0xFF;
const unsigned COL_RED = 0XFF0000FF;
const unsigned COL_GREEN = 0X00FF00FF;
const unsigned COL_BLUE = 0X0000FFFF;
const unsigned COL_WHITE = 0XFFFFFFFF;
const unsigned COL_BLACK = 0X00;
const unsigned RECT_LENGTH = 20;
const unsigned RECT_HEIGHT = 20;
const int REFRESHRATE = 1000000;

const int MEN_WIDTH = 400;
const int MEN_HEIGHT = 300;

const int PALETTE_RED = 0;
const int PALETTE_DK_RED = 1;
const int PALETTE_BLUE = 2;
const int PALETTE_GREEN = 3;
const int PALETTE_BW = 4;

const std::string paletteName[] = {"Red", "Medium Red", "Blue", "Green", "Gray"};

// Initial Mandelbrot Fractal constants
static const double MINRE = -2.1;
static const double MAXIM = 1.2;
//static const double MAXRE =		1.0;

//static const double REFACTOR = (MAXRE - MINRE) / (WIN_WIDTH - 1.0);

static const double MINIM = -1.2;// MAXIM - (MAXRE - MINRE) * WIN_HEIGHT / WIN_WIDTH;// -0.862757;// -1.2;
static const double IMFACTOR = (MAXIM - MINIM) / (WIN_HEIGHT - 1.0);
static const double REFACTOR = IMFACTOR;
static const double MAXRE = MINRE + (REFACTOR * WIN_WIDTH);

static const int INITIALITERATION = 500000;
static const int MAX_ITER = 500000;

// Mouse constants
const int MOUSE_NORM = 0;
const int MOUSE_SIZE = 1;
const int MOUSE_POINT1 = 2;
const int MOUSE_POINT2 = 3;
const int MOUSE_COLOR = 4;
const int MOUSE_RENDER = 5;
const int MOUSE_PALETTE = 6;
const int MOUSE_QUIT = -1;

const int DEF_RECT_COL = 0;

static const char *mouseModes[] = {"NORMAL", "RESIZE", "POINT 1", "POINT 2", "COLOR", "RENDER", "PALETTE", "QUIT"};

// Global integers for errors
const int ERR_SUCCESS = 0;
const int ERR_VIDERR = 1;
const int ERR_WININIT = 2;
const int ERR_IMGERR = 3;
const int ERR_RENDINIT = 4;
const int ERR_PLOT = 5;

// Globals for renderer management
const int REND_OK = 0;
const int REND_ON = 1;
const int REND_ERR = 2;

// Global strings for errors
const std::string ERR_RUNNING = {"Running"};
const std::string ERR_OK = {"OK"};
const std::string ERR_VIDINIT = {"Video Initialization error"};
const std::string ERR_WINCREATE = {"Window creation error"};
const std::string ERR_IMGLOAD = {"Image loading error"};
const std::string ERR_RENDFAIL = {"Failed to initialize renderer"};
const std::string ERR_PLOTFAIL = {"Failed to plot"};
const std::string ERR_SETCOLOR = {"Failed to set color"};
const std::string ERR_SETCLR = {"Failed to clear renderer"};
const std::string ERR_SETRSTCLR = {"Failed to reset color"};
const std::string ERR_NONE = {"/n"};

extern int availableThreads;

struct color_t
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

const color_t DEFAULTCOLOR = {0x00, 0x00, 0x00, 0xff};
const color_t IMAGERESET = {0x00, 0x77, 0x00, 0xff};
