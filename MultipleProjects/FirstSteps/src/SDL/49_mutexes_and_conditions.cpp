/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL Threads, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

#include "helper.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;

//Texture wrapper class
class LTexture_49
{
public:
	//Initializes variables
	LTexture_49();

	//Deallocates memory
	~LTexture_49();

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

//Our worker functions
static int producer( void* data );
static int consumer( void* data );
static void produce();
static void consume();

//The window we'll be rendering to
static SDL_Window* gWindow = NULL;

//The window renderer
static SDL_Renderer* gRenderer = NULL;

//Scene textures
static LTexture_49 gSplashTexture;

//The protective mutex
static SDL_mutex* gBufferLock = NULL;

//The conditions
static SDL_cond* gCanProduce = NULL;
static SDL_cond* gCanConsume = NULL;

//The "data buffer"
static int gData = -1;

LTexture_49::LTexture_49()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;

	mSurfacePixels = NULL;
	mRawPixels = NULL;
	mRawPitch = 0;
}

LTexture_49::~LTexture_49()
{
	//Deallocate
	free();
}

bool LTexture_49::loadFromFile( std::string path )
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

bool LTexture_49::loadPixelsFromFile( std::string path )
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

bool LTexture_49::loadFromPixels()
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
bool LTexture_49::loadFromRenderedText( std::string textureText, SDL_Color textColor )
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

bool LTexture_49::createBlank( int width, int height, SDL_TextureAccess access )
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

void LTexture_49::free()
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

void LTexture_49::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_49::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture_49::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_49::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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

void LTexture_49::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget( gRenderer, mTexture );
}

int LTexture_49::getWidth()
{
	return mWidth;
}

int LTexture_49::getHeight()
{
	return mHeight;
}

Uint32* LTexture_49::getPixels32()
{
	Uint32* pixels = NULL;

	if( mSurfacePixels != NULL )
	{
		pixels =  static_cast<Uint32*>( mSurfacePixels->pixels );
	}

	return pixels;
}

Uint32 LTexture_49::getPixel32( Uint32 x, Uint32 y )
{
	//Convert the pixels to 32 bit
	Uint32* pixels = static_cast<Uint32*>( mSurfacePixels->pixels );

	//Get the pixel requested
	return pixels[ ( y * getPitch32() ) + x ];
}

Uint32 LTexture_49::getPitch32()
{
	Uint32 pitch = 0;

	if( mSurfacePixels != NULL )
	{
		pitch = mSurfacePixels->pitch / 4;
	}

	return pitch;
}

Uint32 LTexture_49::mapRGBA( Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
	Uint32 pixel = 0;

	if( mSurfacePixels != NULL )
	{
		pixel = SDL_MapRGBA( mSurfacePixels->format, r, g, b, a );
	}

	return pixel;
}

bool LTexture_49::lockTexture()
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

bool LTexture_49::unlockTexture()
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

void LTexture_49::copyRawPixels32( void* pixels )
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
					printf( "SDL_image could not initialize! %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Create the mutex
	gBufferLock = SDL_CreateMutex();
			
	//Create conditions
	gCanProduce = SDL_CreateCond();
	gCanConsume = SDL_CreateCond();

	//Loading success flag
	bool success = true;
	
	//Load splash texture
	if( !gSplashTexture.loadFromFile( getFilePath("splash_49.png").c_str() ) )
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

	//Destroy the mutex
	SDL_DestroyMutex( gBufferLock );
	gBufferLock = NULL;
			
	//Destroy conditions
	SDL_DestroyCond( gCanProduce );
	SDL_DestroyCond( gCanConsume );
	gCanProduce = NULL;
	gCanConsume = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int producer( void *data )
{
	printf( "\nProducer started...\n" );

	//Seed thread random
	srand( SDL_GetTicks() );
	
	//Produce
	for( int i = 0; i < 5; ++i )
	{
		//Wait
		SDL_Delay( rand() % 1000 );
		
		//Produce
		produce();
	}

	printf( "\nProducer finished!\n" );
	
	return 0;

}

int consumer( void *data )
{
	printf( "\nConsumer started...\n" );

	//Seed thread random
	srand( SDL_GetTicks() );

	for( int i = 0; i < 5; ++i )
	{
		//Wait
		SDL_Delay( rand() % 1000 );
		
		//Consume
		consume();
	}
	
	printf( "\nConsumer finished!\n" );

	return 0;
}

void produce()
{
	//Lock
	SDL_LockMutex( gBufferLock );
	
	//If the buffer is full
	if( gData != -1 )
	{
		//Wait for buffer to be cleared
		printf( "\nProducer encountered full buffer, waiting for consumer to empty buffer...\n" );
		SDL_CondWait( gCanProduce, gBufferLock );
	}

	//Fill and show buffer
	gData = rand() % 255;
	printf( "\nProduced %d\n", gData );
	
	//Unlock
	SDL_UnlockMutex( gBufferLock );
	
	//Signal consumer
	SDL_CondSignal( gCanConsume );
}

void consume()
{
	//Lock
	SDL_LockMutex( gBufferLock );
	
	//If the buffer is empty
	if( gData == -1 )
	{
		//Wait for buffer to be filled
		printf( "\nConsumer encountered empty buffer, waiting for producer to fill buffer...\n" );
		SDL_CondWait( gCanConsume, gBufferLock );
	}

	//Show and empty buffer
	printf( "\nConsumed %d\n", gData );
	gData = -1;
	
	//Unlock
	SDL_UnlockMutex( gBufferLock );
	
	//Signal producer
	SDL_CondSignal( gCanProduce );
}

int _49_mutexes_and_conditions()
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

			//Run the threads
			SDL_Thread* producerThread = SDL_CreateThread( producer, "Producer", NULL );
			SDL_Thread* consumerThread = SDL_CreateThread( consumer, "Consumer", NULL );
			
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

			//Wait for producer and consumer to finish
			SDL_WaitThread( consumerThread, NULL );
			SDL_WaitThread( producerThread, NULL );
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
