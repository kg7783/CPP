/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#include "helper.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_45
{
public:
	//Initializes variables
	LTexture_45();

	//Deallocates memory
	~LTexture_45();

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

	//Creates blank texture
	bool createBlank( int width, int height, SDL_TextureAccess access );

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

	//Set self as render target
	void setAsRenderTarget();

	//Gets image dimensions
	int getWidth();
	int getHeight();

	//Pixel accessors
	Uint32* getPixels32();
	Uint32 getPixel32( Uint32 x, Uint32 y );
	Uint32 getPitch32();
	Uint32 mapRGBA( Uint8 r, Uint8 g, Uint8 b, Uint8 a );
	void copyRawPixels32( void* pixels );
	bool lockTexture();
	bool unlockTexture();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Surface pixels
	SDL_Surface* mSurfacePixels;

	//Raw pixels
	void* mRawPixels;
	int mRawPitch;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Starts up SDL and creates window
static bool init();

//Loads media
static bool loadMedia();

//Frees media and shuts down SDL
static void close();

//Our test callback function
static Uint32 callback( Uint32 interval, void* param );

//The window we'll be rendering to
static SDL_Window* gWindow = NULL;

//The window renderer
static SDL_Renderer* gRenderer = NULL;

//Scene textures
static LTexture_45 gSplashTexture;

LTexture_45::LTexture_45()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;

	mSurfacePixels = NULL;
	mRawPixels = NULL;
	mRawPitch = 0;
}

LTexture_45::~LTexture_45()
{
	//Deallocate
	free();
}

bool LTexture_45::loadFromFile( std::string path )
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

bool LTexture_45::loadPixelsFromFile( std::string path )
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

bool LTexture_45::loadFromPixels()
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
bool LTexture_45::loadFromRenderedText( std::string textureText, SDL_Color textColor )
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

bool LTexture_45::createBlank( int width, int height, SDL_TextureAccess access )
{
	//Get rid of preexisting texture
	free();

	//Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height );
	if( mTexture == NULL )
	{
		printf( "Unable to create streamable blank texture! SDL Error: %s\n", SDL_GetError() );
	}
	else
	{
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}

void LTexture_45::free()
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

void LTexture_45::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_45::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture_45::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_45::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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

void LTexture_45::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget( gRenderer, mTexture );
}

int LTexture_45::getWidth()
{
	return mWidth;
}

int LTexture_45::getHeight()
{
	return mHeight;
}

Uint32* LTexture_45::getPixels32()
{
	Uint32* pixels = NULL;

	if( mSurfacePixels != NULL )
	{
		pixels =  static_cast<Uint32*>( mSurfacePixels->pixels );
	}

	return pixels;
}

Uint32 LTexture_45::getPixel32( Uint32 x, Uint32 y )
{
	//Convert the pixels to 32 bit
	Uint32* pixels = static_cast<Uint32*>( mSurfacePixels->pixels );

	//Get the pixel requested
	return pixels[ ( y * getPitch32() ) + x ];
}

Uint32 LTexture_45::getPitch32()
{
	Uint32 pitch = 0;

	if( mSurfacePixels != NULL )
	{
		pitch = mSurfacePixels->pitch / 4;
	}

	return pitch;
}

Uint32 LTexture_45::mapRGBA( Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
	Uint32 pixel = 0;

	if( mSurfacePixels != NULL )
	{
		pixel = SDL_MapRGBA( mSurfacePixels->format, r, g, b, a );
	}

	return pixel;
}

bool LTexture_45::lockTexture()
{
	bool success = true;

	//Texture is already locked
	if( mRawPixels != NULL )
	{
		printf( "Texture is already locked!\n" );
		success = false;
	}
	//Lock texture
	else
	{
		if( SDL_LockTexture( mTexture, NULL, &mRawPixels, &mRawPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
	}

	return success;
}

bool LTexture_45::unlockTexture()
{
	bool success = true;

	//Texture is not locked
	if( mRawPixels == NULL )
	{
		printf( "Texture is not locked!\n" );
		success = false;
	}
	//Unlock texture
	else
	{
		SDL_UnlockTexture( mTexture );
		mRawPixels = NULL;
		mRawPitch = 0;
	}

	return success;
}

void LTexture_45::copyRawPixels32( void* pixels )
{
	//Texture is locked
	if( mRawPixels != NULL )
	{
		//Copy to locked pixels
		memcpy( mRawPixels, pixels, mRawPitch * mHeight );
	}
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 )
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
	
	//Load splash texture
	if( !gSplashTexture.loadFromFile( getFilePath("splash_45.png").c_str() ) )
	{
		printf( "Failed to load splash texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gSplashTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

Uint32 callback( Uint32 interval, void* param )
{
	//Print callback message
	printf( "Callback called back with message: %s\n", reinterpret_cast<char*>(param) );

	return 0;
}

int _45_timer_callbacks()
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

			//Set callback
			SDL_TimerID timerID = SDL_AddTimer( 3 * 1000, callback, /*reinterpret_cast<void*>*/(void*)( "3 seconds waited!" ) );

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

				//Render splash
				gSplashTexture.render( 0, 0 );

				//Update screen
				SDL_RenderPresent( gRenderer );
			}

			//Remove timer in case the call back was not called
			SDL_RemoveTimer( timerID );
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
