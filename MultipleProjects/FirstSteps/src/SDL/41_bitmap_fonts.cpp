/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, SDL_ttf, standard IO, math, strings, and string streams
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#include "helper.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_41
{
public:
	//Initializes variables
	LTexture_41();

	//Deallocates memory
	~LTexture_41();

	//Loads image at specified path
	bool loadFromFile( std::string path );

	//Loads image into pixel buffer
	bool loadPixelsFromFile( std::string path );

	//Creates image from preloaded pixels
	bool loadFromPixels();

#if defined(SDL_TTF_MAJOR_VERSION)
	//Creates image from font string
	bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor( Uint8 red, Uint8 green, Uint8 blue );

	//Set blending
	void setBlendMode( SDL_BlendMode blending );

	//Set alpha modulation
	void setAlpha( Uint8 alpha );

	//Renders texture at given point
	void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

	//Gets image dimensions
	int getWidth();
	int getHeight();

	//Pixel accessors
	Uint32* getPixels32();
	Uint32 getPixel32( Uint32 x, Uint32 y );
	Uint32 getPitch32();
	Uint32 mapRGBA( Uint8 r, Uint8 g, Uint8 b, Uint8 a );

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Surface pixels
	SDL_Surface* mSurfacePixels;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Our bitmap font
class LBitmapFont
{
    public:
		//The default constructor
		LBitmapFont();

		//Generates the font
		bool buildFont( std::string path );

		//Deallocates font
		void free();

		//Shows the text
		void renderText( int x, int y, std::string text );

    private:
		//The font texture
		LTexture_41 mFontTexture;

		//The individual characters in the surface
		SDL_Rect mChars[ 256 ];

		//Spacing Variables
		int mNewLine, mSpace;
};

//Starts up SDL and creates window
static bool init();

//Loads media
static bool loadMedia();

//Frees media and shuts down SDL
static void close();

//The window we'll be rendering to
static SDL_Window* gWindow = NULL;

//The window renderer
static SDL_Renderer* gRenderer = NULL;

//Scene textures
static LBitmapFont gBitmapFont;

LTexture_41::LTexture_41()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;

	mSurfacePixels = NULL;
}

LTexture_41::~LTexture_41()
{
	//Deallocate
	free();
}

bool LTexture_41::loadFromFile( std::string path )
{
	//Load pixels
	if( !loadPixelsFromFile( path ) )
	{
		printf( "Failed to load pixels for %s!\n", path.c_str() );
	}
	else
	{
		//Load texture from pixels
		if( !loadFromPixels() )
		{
			printf( "Failed to texture from pixels from %s!\n", path.c_str() );
		}
	}

	//Return success
	return mTexture != NULL;
}


bool LTexture_41::loadPixelsFromFile( std::string path )
{
	//Free preexisting assets
	free();

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Convert surface to display format
		mSurfacePixels = SDL_ConvertSurfaceFormat( loadedSurface, SDL_GetWindowPixelFormat( gWindow ), 0 );
		if( mSurfacePixels == NULL )
		{
			printf( "Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = mSurfacePixels->w;
			mHeight = mSurfacePixels->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return mSurfacePixels != NULL;
}

bool LTexture_41::loadFromPixels()
{
	//Only load if pixels exist
	if( mSurfacePixels == NULL )
	{
		printf( "No pixels loaded!" );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( mSurfacePixels, SDL_TRUE, SDL_MapRGB( mSurfacePixels->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface( gRenderer, mSurfacePixels );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = mSurfacePixels->w;
			mHeight = mSurfacePixels->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( mSurfacePixels );
		mSurfacePixels = NULL;
	}

	//Return success
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture_41::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
	}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture_41::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

	//Free surface if it exists
	if( mSurfacePixels != NULL )
	{
		SDL_FreeSurface( mSurfacePixels );
		mSurfacePixels = NULL;
	}
}

void LTexture_41::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_41::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture_41::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_41::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture_41::getWidth()
{
	return mWidth;
}

int LTexture_41::getHeight()
{
	return mHeight;
}

Uint32* LTexture_41::getPixels32()
{
	Uint32* pixels = NULL;

	if( mSurfacePixels != NULL )
	{
		pixels =  static_cast<Uint32*>( mSurfacePixels->pixels );
	}

	return pixels;
}

Uint32 LTexture_41::getPixel32( Uint32 x, Uint32 y )
{
	//Convert the pixels to 32 bit
	Uint32* pixels = static_cast<Uint32*>( mSurfacePixels->pixels );

	//Get the pixel requested
	return pixels[ ( y * getPitch32() ) + x ];
}

Uint32 LTexture_41::getPitch32()
{
	Uint32 pitch = 0;

	if( mSurfacePixels != NULL )
	{
		pitch = mSurfacePixels->pitch / 4;
	}

	return pitch;
}

Uint32 LTexture_41::mapRGBA( Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
	Uint32 pixel = 0;

	if( mSurfacePixels != NULL )
	{
		pixel = SDL_MapRGBA( mSurfacePixels->format, r, g, b, a );
	}

	return pixel;
}


LBitmapFont::LBitmapFont()
{
    //Initialize variables
    mNewLine = 0;
    mSpace = 0;
}

bool LBitmapFont::buildFont( std::string path )
{
	//Get rid of preexisting texture
	free();

	//Load bitmap image
	bool success = true;
	if( !mFontTexture.loadPixelsFromFile( path ) )
	{
		printf( "Unable to load bitmap font surface!\n" );
		success = false;
	}
	else
	{
		//Get the background color
		Uint32 bgColor = mFontTexture.getPixel32( 0, 0 );

		//Set the cell dimensions
		int cellW = mFontTexture.getWidth() / 16;
		int cellH = mFontTexture.getHeight() / 16;

		//New line variables
		int top = cellH;
		int baseA = cellH;

		//The current character we're setting
		int currentChar = 0;

		//Go through the cell rows
		for( int rows = 0; rows < 16; ++rows )
		{
			//Go through the cell columns
			for( int cols = 0; cols < 16; ++cols )
			{
				//Set the character offset
				mChars[ currentChar ].x = cellW * cols;
				mChars[ currentChar ].y = cellH * rows;

				//Set the dimensions of the character
				mChars[ currentChar ].w = cellW;
				mChars[ currentChar ].h = cellH;

				//Find Left Side
				//Go through pixel columns
				for( int pCol = 0; pCol < cellW; ++pCol )
				{
					//Go through pixel rows
					for( int pRow = 0; pRow < cellH; ++pRow )
					{
						//Get the pixel offsets
						int pX = ( cellW * cols ) + pCol;
						int pY = ( cellH * rows ) + pRow;

						//If a non colorkey pixel is found
						if( mFontTexture.getPixel32( pX, pY ) != bgColor )
						{
							//Set the x offset
							mChars[ currentChar ].x = pX;

							//Break the loops
							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				//Find Right Side
				//Go through pixel columns
				for( int pColW = cellW - 1; pColW >= 0; --pColW )
				{
					//Go through pixel rows
					for( int pRowW = 0; pRowW < cellH; ++pRowW )
					{
						//Get the pixel offsets
						int pX = ( cellW * cols ) + pColW;
						int pY = ( cellH * rows ) + pRowW;

						//If a non colorkey pixel is found
						if( mFontTexture.getPixel32( pX, pY ) != bgColor )
						{
							//Set the width
							mChars[ currentChar ].w = ( pX - mChars[ currentChar ].x ) + 1;

							//Break the loops
							pColW = -1;
							pRowW = cellH;
						}
					}
				}

				//Find Top
				//Go through pixel rows
				for( int pRow = 0; pRow < cellH; ++pRow )
				{
					//Go through pixel columns
					for( int pCol = 0; pCol < cellW; ++pCol )
					{
						//Get the pixel offsets
						int pX = ( cellW * cols ) + pCol;
						int pY = ( cellH * rows ) + pRow;

						//If a non colorkey pixel is found
						if( mFontTexture.getPixel32( pX, pY ) != bgColor )
						{
							//If new top is found
							if( pRow < top )
							{
								top = pRow;
							}

							//Break the loops
							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				//Find Bottom of A
				if( currentChar == 'A' )
				{
					//Go through pixel rows
					for( int pRow = cellH - 1; pRow >= 0; --pRow )
					{
						//Go through pixel columns
						for( int pCol = 0; pCol < cellW; ++pCol )
						{
							//Get the pixel offsets
							int pX = ( cellW * cols ) + pCol;
							int pY = ( cellH * rows ) + pRow;

							//If a non colorkey pixel is found
							if( mFontTexture.getPixel32( pX, pY ) != bgColor )
							{
								//Bottom of a is found
								baseA = pRow;

								//Break the loops
								pCol = cellW;
								pRow = -1;
							}
						}
					}
				}

				//Go to the next character
				++currentChar;
			}
		}

		//Calculate space
		mSpace = cellW / 2;

		//Calculate new line
		mNewLine = baseA - top;

		//Lop off excess top pixels
		for( int i = 0; i < 256; ++i )
		{
			mChars[ i ].y += top;
			mChars[ i ].h -= top;
		}

		//Create final texture
		if( !mFontTexture.loadFromPixels() )
		{
			printf( "Unable to create font texture!\n" );
			success = false;
		}
	}

	return success;
}

void LBitmapFont::free()
{
	mFontTexture.free();
}

void LBitmapFont::renderText( int x, int y, std::string text )
{
    //If the font has been built
    if( mFontTexture.getWidth() > 0 )
    {
		//Temp offsets
		int curX = x, curY = y;

        //Go through the text
        for( int i = 0; i < text.length(); ++i )
        {
            //If the current character is a space
            if( text[ i ] == ' ' )
            {
                //Move over
                curX += mSpace;
            }
            //If the current character is a newline
            else if( text[ i ] == '\n' )
            {
                //Move down
                curY += mNewLine;

                //Move back
                curX = x;
            }
            else
            {
                //Get the ASCII value of the character
                int ascii = (unsigned char)text[ i ];

                //Show the character
				mFontTexture.render( curX, curY, &mChars[ ascii ] );

                //Move over the width of the character with one pixel of padding
                curX += mChars[ ascii ].w + 1;
            }
        }
    }
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Seed random
		srand( SDL_GetTicks() );

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load font texture
	if( !gBitmapFont.buildFont( getFilePath("lazyfont.png").c_str() ) )
	{
		printf( "Failed to load bitmap font!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gBitmapFont.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int _41_bitmap_fonts()
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render test text
				gBitmapFont.renderText( 0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789" );

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
