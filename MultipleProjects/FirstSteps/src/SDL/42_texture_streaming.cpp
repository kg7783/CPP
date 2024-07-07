/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include "helper.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class LTexture_42
{
public:
	//Initializes variables
	LTexture_42();

	//Deallocates memory
	~LTexture_42();

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
	bool createBlank( int width, int height );

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

//A test animation stream
class DataStream
{
	public:
		//Initializes internals
		DataStream();

		//Loads initial data
		bool loadMedia();

		//Deallocator
		void free();

		//Gets current frame data
		void* getBuffer();

	private:
		//Internal data
		SDL_Surface* mImages[ 4 ];
		int mCurrentImage;
		int mDelayFrames;
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
static LTexture_42 gStreamingTexture;

//Animation stream
static DataStream gDataStream;

LTexture_42::LTexture_42()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;

	mSurfacePixels = NULL;
	mRawPixels = NULL;
	mRawPitch = 0;
}

LTexture_42::~LTexture_42()
{
	//Deallocate
	free();
}

bool LTexture_42::loadFromFile( std::string path )
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

bool LTexture_42::loadPixelsFromFile( std::string path )
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

bool LTexture_42::loadFromPixels()
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
bool LTexture_42::loadFromRenderedText( std::string textureText, SDL_Color textColor )
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

bool LTexture_42::createBlank( int width, int height )
{
	//Get rid of preexisting texture
	free();

	//Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height );
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

void LTexture_42::free()
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

void LTexture_42::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture_42::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture_42::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture_42::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
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

int LTexture_42::getWidth()
{
	return mWidth;
}

int LTexture_42::getHeight()
{
	return mHeight;
}

Uint32* LTexture_42::getPixels32()
{
	Uint32* pixels = NULL;

	if( mSurfacePixels != NULL )
	{
		pixels =  static_cast<Uint32*>( mSurfacePixels->pixels );
	}

	return pixels;
}

Uint32 LTexture_42::getPixel32( Uint32 x, Uint32 y )
{
	//Convert the pixels to 32 bit
	Uint32* pixels = static_cast<Uint32*>( mSurfacePixels->pixels );

	//Get the pixel requested
	return pixels[ ( y * getPitch32() ) + x ];
}

Uint32 LTexture_42::getPitch32()
{
	Uint32 pitch = 0;

	if( mSurfacePixels != NULL )
	{
		pitch = mSurfacePixels->pitch / 4;
	}

	return pitch;
}

Uint32 LTexture_42::mapRGBA( Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
	Uint32 pixel = 0;

	if( mSurfacePixels != NULL )
	{
		pixel = SDL_MapRGBA( mSurfacePixels->format, r, g, b, a );
	}

	return pixel;
}

bool LTexture_42::lockTexture()
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

bool LTexture_42::unlockTexture()
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

void LTexture_42::copyRawPixels32( void* pixels )
{
	//Texture is locked
	if( mRawPixels != NULL )
	{
		//Copy to locked pixels
		memcpy( mRawPixels, pixels, mRawPitch * mHeight );
	}
}

DataStream::DataStream()
{
	mImages[ 0 ] = NULL;
	mImages[ 1 ] = NULL;
	mImages[ 2 ] = NULL;
	mImages[ 3 ] = NULL;

	mCurrentImage = 0;
	mDelayFrames = 4;
}

bool DataStream::loadMedia()
{
	bool success = true;
	
	for( int i = 0; i < 4; ++i )
	{
		std::stringstream path;

		switch(i)
		{
			case 0:
				path << getFilePath("foo_walk_0.png").c_str();
			break;

			case 1:
				path << getFilePath("foo_walk_1.png").c_str();
			break;

			case 2:
				path << getFilePath("foo_walk_2.png").c_str();
			break;

			case 3:
				path << getFilePath("foo_walk_3.png").c_str();
			break;
		}

		SDL_Surface* loadedSurface = IMG_Load( path.str().c_str() );
		if( loadedSurface == NULL )
		{
			printf( "Unable to load %s! SDL_image error: %s\n", path.str().c_str(), IMG_GetError() );
			success = false;
		}
		else
		{
			mImages[ i ] = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0 );
		}

		SDL_FreeSurface( loadedSurface );
	}

	return success;
}

void DataStream::free()
{
	for( int i = 0; i < 4; ++i )
	{
		SDL_FreeSurface( mImages[ i ] );
		mImages[ i ] = NULL;
	}
}

void* DataStream::getBuffer()
{
	--mDelayFrames;
	if( mDelayFrames == 0 )
	{
		++mCurrentImage;
		mDelayFrames = 4;
	}

	if( mCurrentImage == 4 )
	{
		mCurrentImage = 0;
	}

	return mImages[ mCurrentImage ]->pixels;
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

	//Load blank texture
	if( !gStreamingTexture.createBlank( 64, 205 ) )
	{
		printf( "Failed to create streaming texture!\n" );
		success = false;
	}

	//Load data stream
	if( !gDataStream.loadMedia() )
	{		
		printf( "Unable to load data stream!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gStreamingTexture.free();
	gDataStream.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int _42_texture_streaming()
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

				//Copy frame from buffer
				gStreamingTexture.lockTexture();
				gStreamingTexture.copyRawPixels32( gDataStream.getBuffer() );
				gStreamingTexture.unlockTexture();

				//Render frame
				gStreamingTexture.render( ( SCREEN_WIDTH - gStreamingTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gStreamingTexture.getHeight() ) / 2 );

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
