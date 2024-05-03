#include <iostream>
#include <chrono>
#include <cstring>
#include <math.h>
#include <float.h>

#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

struct Z_Color 
{
	int r;
	int g;
	int b;
};

#define FPS 60

//SDL_Surface *surf;
//SDL_Texture *tex;
//SDL_Rect rect;

//TTF_Font *font;
//SDL_Color font_color = { 128, 0, 0 };

class z 
{	//This class defines complex numbers
			//Ta klasa definiuje liczby zespolone

	double re, im;

public:

	z() 
    {
		re = 0;
		im = 0;
	}
	z(double _re, double _im) 
    {
		re = _re;
		im = _im;
	}

	//Returns the RE value of Z
	//Zwraca wartość RE liczby Z
	double r() 
    {
		return this->re;
	}
	//Returns the IM value of Z
	//Zwraca wartość IM liczby Z
	double i() 
    {
		return this->im;
	}

	//Addition
	//Dodawanie
	z operator+ (const double _d) 
    {
		return z(this->re + _d, this->im);
	}
	z operator+ (const z _z) 
    {
		return z(this->re + _z.re, this->im + _z.im);
	}

	//Substracion
	//Odejmowanie
	z operator- (const double _d) 
    {
		return z(this->re - _d, this->im);
	}
	z operator- (const z _z) 
    {
		return z(this->re - _z.re, this->im - _z.im);
	}

	//Multiplication
	//Mnożenie
	z operator* (const double _d) 
    {
		return z(this->re * _d, this->im * _d);
	}
	z operator* (const z _z) 
    {
		return z((this->re * _z.re - this->im * _z.im),	//(ac - bd)
			(this->re * _z.im + this->im * _z.re));	//(ad + bc)i
	}

	//Division
	//Dzielenie
	z operator/ (const double _d) 
    {
		return z(this->re / _d, this->im / _d);
	}
	z operator/ (const z _z) 
    {
		return z((this->re * _z.re + this->im * _z.im) / (_z.re * _z.re + _z.im * _z.im),	//(ac + bd) / (c2 + d2)
			(this->im * _z.re - this->re * _z.im) / (_z.re * _z.re + _z.im * _z.im));	//(bc - ad)i / (c2 + d2)
	}

	//Print number Z using ostream
	//Drukowanie liczby Z przy użyciu ostream
	friend std::ostream& operator<<(std::ostream& os, const z& _z) 
    {
		char str_re[256] = { 0 };
		char str_im[256] = { 0 };

		sprintf(str_re, "%.15g", _z.re);
		sprintf(str_im, "%.15g", _z.im);

		os << str_re;
		if (!(_z.re < 0)) os << '+';
		os << str_im << 'i';

		return os;
	}
};

//This returns number Z as a string
//Ta funkcja zwraca łańcuch znaków przedstawiający liczbę zespoloną
/*
char* zdisp(z _z) {
	static char temp[256] = { 0 };
	sprintf_s(temp, sizeof(temp), "(%.15g, %.15g)", _z.r(), _z.i() );
	return temp;
}
*/

//Conjecture
//Sprzężenie zespolone
z conj(z _z) 
{
	return z(_z.r(), -_z.i());
}

//Absolute value
//Moduł (wartość bezwzględna)
double abs(z _z) 
{
	return sqrt(_z.r() * _z.r() + _z.i() * _z.i());
}

/*===========================================================================================*/

#define WIDTH 860
#define HEIGHT WIDTH

#define JULIA2 0
#define JULIA3 1
#define MANDELBROT2 2
#define MANDELBROT3 3
#define MANDELBROT4 4
#define INVMANDELBROT 5
#define LAMBDAMANDELBROT 6
#define TRICORN 7
#define BURNINGSHIP 8

static Z_Color matrix[WIDTH][HEIGHT];

int manage_fractale() 
{

	for (int j = 0; j < HEIGHT; j++) 
    {
		for (int i = 0; i < WIDTH; i++) 
        {
			matrix[i][j].r = 0;
			matrix[i][j].g = 0;
			matrix[i][j].b = 0;
		}
	}

	std::cout << "\033]0;(= OwO =)\007";

	char tempstr[256];

	//Initiate SDL
	//Inicjacja SDL
	std::cout << "Initializing SDL...";
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) 
    {
		std::cout << "Oops, couldn't init SDL :(\n" << SDL_GetError() << std::endl;
		exit(0xF19A20);
	}
	std::cout << " Done!\n";

	//std::cout << "Initializing TTF...";
	//if (TTF_Init() == -1) {
	//	std::cout << "Oops, couldn't init TTF :(\n" << TTF_GetError() << std::endl;
	//	exit(0xF19A20);
	//}
	//std::cout << " Done!\n";

	//Creating the SDL window
	//Tworzymy okno w SDL
	std::cout << "Creating a window...";
	if ((window = SDL_CreateWindow("(= OwO =)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_HIDDEN)) == NULL) 
    {
		std::cout << "Oops, couldn't create a window :(\n" << SDL_GetError() << std::endl;
		exit(0xF19A20);
	}
	std::cout << " Done!\n";

	//Creating the SDL window
	//Tworzymy okno w SDL
	std::cout << "Creating a renderer...";
	if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL) {
		std::cout << "Oops, couldn't create a renderer :(\n" << SDL_GetError() << std::endl;
		exit(0xF19A20);
	}
	std::cout << " Done!\n";

	//Coordinations
	//Współrzędne
	double juliaX = 0, juliaY = 0, X = 0, Y = 0, R = 1;

	//Complex
	//Zespolone
	z Z, C;

	//Iterations
	//Iteracje
	unsigned long long int it = 10;

	//What fractal?
	//Jaki fraktal?
	unsigned short int type = 0, bailout = 2;

_START:

#ifdef __linux__
	system("clear");
#elif _WIN32
	system("cls");
#endif

	std::cout << std::flush;

	std::cout << "Select a fractal\n0 = Julia Z^2\n1 = Julia Z^3\n2 = Mandelbrot Z^2\n3 = Mandelbrot Z^3\n4 = Mandelbrot Z^4\n5 = Inverse Mandelbrot\n6 = Lambda Mandelbrot\n7 = Tricorn\n8 = The Burning Ship\n>>> ";
	std::cin >> type;

	if (type == JULIA2 || type == JULIA3) {
		std::cout << "\nEnter the Julia X parameter (double, from -2 to +2)\n>>> ";
		std::cin >> juliaX;
		if (juliaX < -2) { juliaX = -2; }
		if (juliaX > 2) { juliaX = 2; }

		std::cout << "\nEnter the Julia Y parameter (double, from -2 to +2)\n>>> ";
		std::cin >> juliaY;
		if (juliaY < -2) { juliaY = -2; }
		if (juliaY > 2) { juliaY = 2; }
	}

	std::cout << "\nEnter the X offset (double, from -2 to +2)\n>>> ";
	std::cin >> X;
	if (X < -2) { X = -2; }
	if (X > 2) { X = 2; }

	std::cout << "\nEnter the Y offset (double, from -2 to +2)\n>>> ";
	std::cin >> Y;
	if (Y < -2) { Y = -2; }
	if (Y > 2) { Y = 2; }

	std::cout << "\nEnter the zoom ratio (double, must be not smaller than DBL_MIN and not greater than 2)\n>>> ";
	std::cin >> R;
	if (R < DBL_MIN) { R = DBL_MIN; }
	if (R > 2) { R = 2; }

	std::cout << "\nEnter the iteration count (unsigned long long int, not smaller than 1)\n>>> ";
	std::cin >> it;
	if (it < 1) { it = 1; }

	//Sets the title of the window
	//Ustawia tytuł okna
	switch (type) 
    {
	case JULIA2: 
    {
		strcpy(tempstr, "Julia Z^2");
		break;
	}
	case JULIA3: 
    {
		strcpy(tempstr, "Julia Z^3");
		break;
	}
	case MANDELBROT2: 
    {
		strcpy(tempstr, "Mandelbrot Z^2");
		break;
	}
	case MANDELBROT3: 
    {
		strcpy(tempstr, "Mandelbrot Z^3");
		break;
	}
	case MANDELBROT4: 
    {
		strcpy(tempstr, "Mandelbrot Z^4");
		break;
	}
	case INVMANDELBROT: 
    {
		strcpy(tempstr, "Inverse Mandelbrot");
		break;
	}
	case LAMBDAMANDELBROT: 
    {
		strcpy(tempstr, "Lambda Mandelbrot");
		break;
	}
	case TRICORN: 
    {
		strcpy(tempstr, "Tricorn");
		break;
	}
	case BURNINGSHIP: 
    {
		strcpy(tempstr, "The Burning Ship");
		break;
	}

	}
//	sprintf(tempstr, "%s:   X: %.16g   Y: %.16g   R: %.16g   Iterations: %llu", tempstr, X, Y, R, it);
	SDL_SetWindowTitle(window, tempstr);

	//Shows the window
	//Pokazuje okno
	SDL_ShowWindow(window);

	auto start = std::chrono::steady_clock::now();

	std::cout << "\nRendering..." << std::endl;

	for (int j = 0; j < HEIGHT; j++) 
    {	//Y coord

		for (int i = 0; i < WIDTH; i++) 
        {	//X coord

			switch (type) 
            {
			case JULIA2:
			case JULIA3:
            {
				Z = z(
					R * (-bailout + ((double)i / WIDTH) * bailout * 2) + X,
					R * (-bailout + ((double)j / HEIGHT) * bailout * 2) - Y
				);
				C = z(juliaX, juliaY);
				break;
			}
			case MANDELBROT2:
			case MANDELBROT3:
			case MANDELBROT4:
			case INVMANDELBROT:
			case TRICORN:
			case BURNINGSHIP: 
            {
				Z = z(0, 0);
				C = z(
					R * (-bailout + ((double)i / WIDTH) * bailout * 2) + X,
					R * (-bailout + ((double)j / HEIGHT) * bailout * 2) - Y
				);
				break;
			}
			case LAMBDAMANDELBROT: 
            {
				Z = z(0.25, 0);
				C = z(
					R * (-bailout + ((double)i / WIDTH) * bailout * 2) + X,
					R * (-bailout + ((double)j / HEIGHT) * bailout * 2) - Y
				);
				break;
			}
			}



			//std::cout << "C = " << C;

			for (int k = 0; k <= it; k++) 
            {

				//If Z crosses the border, stop calculating
				//Jeżeli Z przekroczy granicę, nie licz dalej
				if (!(abs(Z) < 2)) 
                {
					//SDL_SetRenderDrawColor(renderer,
					//	128 * (0.5 + sin(0.5*(double)k / M_PI) / 2),
					//	255 * (0.5 + sin(0.5*(double)k / M_PI) / 2),
					//	255,
					//	255
					//);
					//SDL_RenderDrawPoint(renderer, i, j);
					matrix[i][j].r = 128 * (0.5 + sin(0.5*(double)k / M_PI) / 2);
					matrix[i][j].g = 255 * (0.5 + sin(0.5*(double)k / M_PI) / 2);
					matrix[i][j].b = 255;
					//std::cout << " Failed!" << std::endl;
					break;
				}

				//If the point belongs to the set, colour it black
				//Jeżeli punkt należy do zbioru, pokoloruj go na czarno
				if (k == it) 
                {
					//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					//SDL_RenderDrawPoint(renderer, i, j);
					matrix[i][j].r = 0;
					matrix[i][j].g = 0;
					matrix[i][j].b = 0;
					//std::cout << " Passed!" << std::endl;
					break;
				}

				switch (type) 
                {
				case JULIA2:
				case MANDELBROT2: 
                {
					Z = Z * Z;
					Z = Z + C;
					break;
				}
				case JULIA3:
				case MANDELBROT3: 
                {
					Z = Z * Z * Z;
					Z = Z + C;
					break;
				}
				case MANDELBROT4: 
                {
					Z = Z * Z * Z * Z;
					Z = Z + C;
					break;
				}
				case INVMANDELBROT: 
                {
					Z = Z * Z;
					Z = Z + (z(1, 0) / C);
					break;
				}
				case LAMBDAMANDELBROT: 
                {
					Z = C * Z;
					Z = Z * z(-Z.r() + 1, -Z.i());
					break;
				}
				case TRICORN: 
                {
					Z = Z * Z;
					Z = conj(Z) + C;
					break;
				}
				case BURNINGSHIP: 
                {
					Z = z(abs(Z.r()), abs(Z.i())) * z(abs(Z.r()), abs(Z.i()));
					Z = Z + C;
					break;
				}
				}
			}

		}
	}

	auto end = std::chrono::steady_clock::now();
	double diff = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()) / 1000000000;

	//SDL_RenderCopy(renderer, tex, NULL, &rect);

	//TTF_CloseFont(font);
	//SDL_FreeSurface(surf);
	//SDL_DestroyTexture(tex);

	printf("Done! Took %f seconds!\n", diff);
	std::cout << "Close the window to render another fractal or close the console to exit!";

	while (1) 
    {

		while (SDL_PollEvent(&event)) 
        {
			switch (event.type) 
            {

				case SDL_QUIT: 
                {
					//SDL_DestroyWindow(window);
					//SDL_Quit();
					//TTF_Quit();
					//return 0;
					SDL_HideWindow(window);
					goto _START;
				}

			}
		}

		SDL_RenderClear(renderer);

		for (int j = 0; j < HEIGHT; j++) 
        {
			for (int i = 0; i < WIDTH; i++) 
            {
				SDL_SetRenderDrawColor(renderer, matrix[i][j].r, matrix[i][j].g, matrix[i][j].b, 255);
				SDL_RenderDrawPoint(renderer, i, j);
			}
		}
		SDL_RenderPresent(renderer);
	}

	return 0;
}