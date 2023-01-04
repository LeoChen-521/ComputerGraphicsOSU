#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

//#include "glew.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include "glut.h"



//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

const char *WINDOWTITLE = { "OpenGL / Final Project -- Xiaoru Chen" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b

// initial window size:

const int INIT_WINDOW_SIZE = { 600 };

// size of the 3d box:

const float BOXSIZE = { 2.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = { 3 };
const int SCROLL_WHEEL_DOWN = { 4 };

// equivalent mouse movement when we click a the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = { 5. };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

enum Viewmodes
{
	INSIDEVIEWMODE,
	OUTSIDEVIEWMODE,
	MARSVIEWMODE

};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White",
	(char*)"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };



// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

// Global variables for Final Project
GLuint  Tex0, Tex1, Tex2, Tex3;
GLuint  EarthList;
GLuint 	SunList;
GLuint 	MoonList;
GLuint 	MarsList;

GLuint 	EarthOrbit;
GLuint 	MarsOrbit;
GLuint 	MoonOrbit;

float 	EarthOrbitalPeriod = 365; // Earth Orbital Period = 365day
float 	MarsOrbitalPeriod = 687; // Mars orbital Period = 687 days
float 	MoonOrbitalPeriod = 27; // Moon orbital Period = 27days

float 	EarthOrbitalRadius = pow(EarthOrbitalPeriod, 2./3.) / 2.; //Orbital Period is proportional to OrbitalRadius ^(3/2)
float 	MarsOrbitalRadius  = pow(MarsOrbitalPeriod, 2./3.)  / 2.; //Orbital Period is proportional to OrbitalRadius ^(3/2)
float 	MoonOrbitalRadius  = pow(MoonOrbitalPeriod, 2./3.)  / 2.; //Orbital Period is proportional to OrbitalRadius ^(3/2)

float   SunSize   = 8.;
float 	EarthSize = SunSize   / 2.; //Should be 109 times. Scaled by 1/50.
float 	MoonSize  = EarthSize / 3.;
float 	MarsSize  = EarthSize / 2.;


int 	TextureEnv;				// != 0 means to draw the texture
int 	TextureFiltering;		// != 0 means linear, otherwise nearest
int 	TextureWrapping;		// != 0 means Clamp, otherwise Repeat
float	Time;				// a number from 0 to 1.
bool 	Frozen;
bool 	SunLightOn;
int 	whichViewMode;
float 	Xrot_INSIDE, Yrot_INSIDE, Scale_INSIDE;


float 	White[] = {1., 1., 1., 1.};


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );

//Project 3 --------------------------------
void 	DoTextureEnvMenu( int );
void 	DoTextureFilteringMenu( int );

//------------------------------------------
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoShadowMenu();
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void			Axes( float );
unsigned char *	BmpToTexture( char *, int *, int * );
void			HsvRgb( float[3], float [3] );
int				ReadInt( FILE * );
short			ReadShort( FILE * );

void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);

// Sphere
struct point * PtsPointer( int, int );
void   DrawPoint( struct point *);
void   OsuSphere( float, int, int );
void   DrawCircle(float, float, float, float);



// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never returns
	// this line is here to make the compiler happy:

	return 0;
}

// utility to create an array from 3 separate values:
float *
Array3( float a, float b, float c)
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

float *
MulArray3( float factor, float array0[3] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

// A Material-setting Shortcut

void
SetMaterial( float r, float g, float b, float shininess)
{
	glMaterialfv( GL_BACK, GL_EMISSION, Array3(0., 0., 0.) );
	glMaterialfv( GL_BACK, GL_AMBIENT, MulArray3(.4f, White) );
	glMaterialfv( GL_BACK, GL_DIFFUSE, MulArray3(1., White) );
	glMaterialfv( GL_BACK, GL_SPECULAR, Array3(0., 0., 0.) );
	glMaterialf ( GL_BACK, GL_SHININESS, 2.f );

	glMaterialfv( GL_FRONT, GL_EMISSION, Array3(0., 0., 0.) );
	glMaterialfv( GL_FRONT, GL_AMBIENT, Array3(r, g, b) );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, Array3(r, g, b) );
	glMaterialfv( GL_FRONT, GL_SPECULAR, MulArray3(.8f, White) );
	glMaterialf ( GL_FRONT, GL_SHININESS, shininess );
}

void
SetPointLight( int ilight, float x, float y, float z, float r, float g, float b ) 
{
	glLightfv( ilight, GL_POSITION, Array3( x, y, z) );
	glLightfv( ilight, GL_AMBIENT, Array3( 0., 0., 0.) );
	glLightfv( ilight, GL_DIFFUSE, Array3( r, g, b) );
	glLightfv( ilight, GL_SPECULAR, Array3( r, g, b) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable( ilight );

}

void
SetSPOTLight( int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b ) 
{
	glLightfv( ilight, GL_POSITION, Array3( x, y, z) );
	glLightfv( ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir) );
	glLightf ( ilight, GL_SPOT_EXPONENT, 1. );
	glLightf ( ilight, GL_SPOT_CUTOFF, 45.);
	glLightfv( ilight, GL_AMBIENT, Array3( 0., 0., 0.) );
	glLightfv( ilight, GL_DIFFUSE, Array3( r, g, b) );
	glLightfv( ilight, GL_SPECULAR, Array3( r, g, b) );
	glLightf ( ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf ( ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf ( ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable( ilight );

}

void
Animate( )
{

	//float Time;
	#define MS_IN_THE_ANIMATION_CYCLE	10000
	
	int ms = glutGet( GLUT_ELAPSED_TIME );	// milliseconds

	ms  %=  MS_IN_THE_ANIMATION_CYCLE;

	Time = (float)ms  /  (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		// glOrtho( -25., 25.,     -25., 25.,     0.1, 1000. );
		glOrtho( -46.5, 46.5,     -46.5, 46.5,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// set the eye position, look-at position, and up-vector:

	//gluLookAt( 0., 0., 3.,     0., 0., 0.,     0., 1., 0. );
	//gluLookAt( 16., 8., 20.,     0., 0., 0.,     0., 1., 0. );
	//gluLookAt( 64., 32., 80.,     0., 0., 0.,     0., 1., 0. );

	if (whichViewMode == OUTSIDEVIEWMODE)
	{
		gluLookAt( 16., 20., 20.,     0., 0., 0.,     0., 1., 0. );

		// rotate the scene:

		glRotatef( (GLfloat)Yrot, 0., 1., 0. );
		glRotatef( (GLfloat)Xrot, 1., 0., 0. );

		// uniformly scale the scene:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
		glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	}
	// Inside view, view on 
	else if (whichViewMode == INSIDEVIEWMODE)
	{
		// set the eye position, look-at position, and up-vector:
		// This position is a static point on Earth



		// glRotatef( (GLfloat)Yrot, 0., 1., 0. );
		// glRotatef( (GLfloat)Xrot, 1., 0., 0. );
		// if( Scale < MINSCALE )
		// 	Scale = MINSCALE;
		// glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	
		//float rad = (720*Time) * (M_PI/180.f); 
		//float EarthRadians = (360./EarthOrbitalPeriod) * (M_PI/180.f);

		// glDisable();

		// gluLookAt( .*cos(EarthRadians) + 9.* sin(EarthRadians), 0., 9.*cos(EarthRadians) + 9. * sin(EarthRadians),     
		// 			0., 0., 0.,   
		// 			0., 1., 0. );
		// float 	EarthDegree  = 360. * Time * (360./EarthOrbitalPeriod) * 3.;
		// float 	EarthrotationDegree = 360. * Time * 6.;
		// float 	EarthRadians = EarthDegree * (M_PI/180.f);

		// gluLookAt( -EarthOrbitalRadius * cos(EarthRadians) + EarthOrbitalRadius * sin(EarthRadians), 
		// 					0., 
		// 		   EarthOrbitalRadius * sin(EarthRadians) + EarthOrbitalRadius * cos(EarthRadians),     
		// 		0., 
		// 			0., 
		// 				0.,
		// 		0., 1., 0. );


		gluLookAt(0., 0., -8.,	 	0., 0, -30., 	0., 1., 0. );


	}
	else if (whichViewMode == MARSVIEWMODE)
	{

		// follow Mars
		float 	MarsDegree   = 360. * Time * (360./MarsOrbitalPeriod)  * 3.;
		float rad  = MarsDegree * (M_PI/180.f);

		//38.93
		float x = -1. * cos(rad) + 1. * sin(rad);
		float z = 1. * sin(rad) + 1. * cos(rad);

		// Mars central point
		float CentralX = -MarsOrbitalRadius * cos(rad) + MarsOrbitalRadius * sin(rad);
		float CentralZ = MarsOrbitalRadius * sin(rad) + MarsOrbitalRadius * cos(rad);


		gluLookAt(x, 20., z,	 0., 0., 0., 	0., 1., 0. );
	}





	// set the fog parameters:
	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	//possibly draw the axes:

	// if( AxesOn != 0 )
	// {
	// 	glColor3fv( &Colors[WhichColor][0] );
	// 	glCallList( AxesList );
	// }




	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, MulArray3( .3f, White ) );
	glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
	//-------------------------------------------------------------------------------------


	glPushMatrix();
	glutSolidSphere(0.25, 100, 100);
	// Wnite color from the sun, Light1
	SetPointLight(GL_LIGHT1, 	0., 0., 0., 	1., 1., 1.);

	//SetSPOTLight(GL_LIGHT1, 	0., 0., 0., 	1., 1., 1., 	1., 1., 1.);
	glPopMatrix();
	//-------------------------------------------------------------------------------------


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );

	// specify the shading model:
	glShadeModel( GL_SMOOTH );

	if (SunLightOn)
	{
		glEnable(GL_LIGHT1);
	}
	else
	{
		glDisable(GL_LIGHT1);
	}

	glEnable(GL_LIGHTING);



	// draw the current object:

	
	glEnable( GL_TEXTURE_2D);

	float 	EarthDegree  = 360. * Time * (360./EarthOrbitalPeriod) * 3.;
	float 	MarsDegree   = 360. * Time * (360./MarsOrbitalPeriod)  * 3.;
	float 	MoonDegree   = 360. * Time * (360./MoonOrbitalPeriod)  * 3.;

	// float 	EarthRadians = EarthDegree * (M_PI/180.f);
	// float 	MarsRadians  = MarsDegree  * (M_PI/180.f);

	// The earth ---------------------------------------------------------------
	glPushMatrix();
	glBindTexture ( GL_TEXTURE_2D, Tex0 );
	//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//rotate around the sun
	// glTranslatef(-(EarthOrbitalRadius) * cos(EarthRadians) + (EarthOrbitalRadius) * sin(EarthRadians), 
	// 				0, 
	// 			 (EarthOrbitalRadius)  * sin(EarthRadians) + (EarthOrbitalRadius) * cos(EarthRadians));

	//rotate around the sun
	glRotatef(EarthDegree, 0, 1, 0);
	glTranslatef(EarthOrbitalRadius, 0., 0);
	glRotatef(360. * Time * 6., 0, 1, 0); // self rotation
	glCallList(EarthList);
	glPopMatrix();

	// The sun------------------------------------------------------------------
	glPushMatrix();
	glBindTexture ( GL_TEXTURE_2D, Tex1 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	SetMaterial(0., 0., 1., 1.);
	glShadeModel(GL_SMOOTH);
	glCallList(SunList);
	glPopMatrix();

	// The Moon -----------------------------------------------------------------
	glPushMatrix();
	glBindTexture ( GL_TEXTURE_2D, Tex2 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(EarthDegree, 0, 1, 0);// rotate around the sun 
	glTranslatef(EarthOrbitalRadius, 0., 0.);
	glRotatef(MoonDegree, 0, 1, 0); // rotate around the earth
	glTranslatef(MoonOrbitalRadius, 0., 0.);
	glRotatef(360. * Time, 0, 1, 0); // self rotation
	glCallList(MoonList);
	glPopMatrix();


	// The Mars -----------------------------------------------------------------
	glPushMatrix();
	//glEnable( GL_TEXTURE_2D);
	glBindTexture ( GL_TEXTURE_2D, Tex3 );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// glTranslatef(-(MarsOrbitalRadius) * cos(MarsRadians) + (MarsOrbitalRadius) * sin(MarsRadians), 
	// 				0, 
	// 			 (MarsOrbitalRadius)  * sin(MarsRadians) + (MarsOrbitalRadius) * cos(MarsRadians));

	glRotatef(MarsDegree, 0, 1, 0);
	glTranslatef(MarsOrbitalRadius, 0., 0.);
	// Self spin. 0.8 * Earth self rotation
	glRotatef(360. * Time *6*0.8, 0, 1, 0);
	glCallList(MarsList);
	glPopMatrix();

	//-----------------------------------------------------------------------------
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	//----------------------------------------------------------------------------
	glCallList(EarthOrbit);
	glCallList(MarsOrbit);

	glPushMatrix();
	glRotatef(EarthDegree, 0, 1, 0);

	glTranslatef(EarthOrbitalRadius, 0., 0.);

	glCallList(MoonOrbit);
	glPopMatrix();


	// view object for testings
	// glPushMatrix();

	// glColor3f(0., 0., 1.);
	// glTranslatef(-10.*cos(EarthRadians) + 10.* sin(EarthRadians), 0, 10.*cos(EarthRadians) + 10. * sin(EarthRadians));
	// glutSolidSphere(2.5, 100, 100);

	// glPopMatrix();


	//A point on the earth




	

	


	// draw some gratuitous text that just rotates on top of the scene:

	// glDisable( GL_DEPTH_TEST );
	// glColor3f( 0., 1., 1. );
	// DoRasterString( 0., 1., 0., (char *)"Text That Moves" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	//DoRasterString( 5., 5., 0., (char *)"Project 3 - Texture Mapping" );


	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoTextureEnvMenu( int id )
{
	TextureEnv = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void 	
DoTextureFilteringMenu( int id )
{
	TextureFiltering = id;
	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoShadowsMenu(int id)
{
	ShadowsOn = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int shadowsmenu = glutCreateMenu(DoShadowsMenu);
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	// Texture Environment
	int TextureEnvmenu = glutCreateMenu( DoTextureEnvMenu );
	glutAddMenuEntry( "No Texture",  0 );
	glutAddMenuEntry( "Replace",   1 );

	// Texture Filtering
	int TextureFilteringmenu = glutCreateMenu( DoTextureFilteringMenu );
	glutAddMenuEntry( "Nearest",  0 );
	glutAddMenuEntry( "Linear",   1 );

	int mainmenu = glutCreateMenu( DoMainMenu );
	// glutAddSubMenu(   "Axes",          axesmenu);
	// glutAddSubMenu(   "Colors",        colormenu);
	// glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	

	// Project 3 menu
	glutAddSubMenu(   "Texture Environment",          TextureEnvmenu);
	glutAddSubMenu(   "Texture Filtering",          TextureFilteringmenu);

	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);

	
	// glutAddSubMenu(   "Texture Filtering",            ???);
	// glutAddSubMenu(   "Texture Scaling",              ???);
	// glutAddSubMenu(   "Textures Rotation",            ???);
	// glutAddSubMenu(   "Textures Wrapping",          ???);


	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	//glutIdleFunc( NULL );
	glutIdleFunc( Animate );


	int width0, height0, width1, height1, width2, height2, width3, height3;


	unsigned char * TextureArray0 = BmpToTexture("worldtex.bmp", &width0, &height0);
	unsigned char * TextureArray1 = BmpToTexture("Sun.bmp", &width1, &height1);
	unsigned char * TextureArray2 = BmpToTexture("Moon.bmp", &width2, &height2);
	unsigned char * TextureArray3 = BmpToTexture("Mars.bmp", &width3, &height3);
	//unsigned char * TextureArray1 = BmpToTexture("file1.bmp", &width1, &height1);

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

	glGenTextures( 1, &Tex0 );
	glGenTextures( 1, &Tex1 );
	glGenTextures( 1, &Tex2 );
	glGenTextures( 1, &Tex3 );

	glBindTexture( GL_TEXTURE_2D, Tex0 ); // make the Tex0 texture current and set its parameters

	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width0, height0, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureArray0);


	// make the Tex1 texture current and set its parameters
	glBindTexture( GL_TEXTURE_2D, Tex1 ); // make the Tex0 texture current and set its parameters

	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureArray1);

	// make the Tex2 texture current and set its parameters
	glBindTexture( GL_TEXTURE_2D, Tex2 ); // make the Tex0 texture current and set its parameters

	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureArray2);

	// make the Tex3 texture current and set its parameters
	glBindTexture( GL_TEXTURE_2D, Tex3 ); // make the Tex0 texture current and set its parameters

	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureArray3);


// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	// Create the display list for the Earth
	EarthList = glGenLists (1);
	glNewList(EarthList, GL_COMPILE);
	glPushMatrix( );
	OsuSphere(EarthSize, 30, 30);
	glPopMatrix( );
	glEndList();

	// Should be 109 times bigger than Earth
	// Scale it by 1/50.
	// 2 times bigger than the Earth
	SunList = glGenLists (1);
	glNewList(SunList, GL_COMPILE);
	glPushMatrix( );
	OsuSphere(SunSize, 30, 30);
	glPopMatrix( );
	glEndList();

	// 1/2 size of the Earths
	MoonList = glGenLists (1);
	glNewList(MoonList, GL_COMPILE);
	glPushMatrix( );
	OsuSphere(MoonSize, 30, 30);
	glPopMatrix( );
	glEndList();

	// size of mars = half size of Earth
	MarsList = glGenLists (1);
	glNewList(MarsList, GL_COMPILE);
	glPushMatrix( );
	OsuSphere(MarsSize, 30, 30);
	glPopMatrix( );
	glEndList();

	//-----------------------------------------
	// Orbits

	EarthOrbit = glGenLists (1);
	glNewList(EarthOrbit, GL_COMPILE);
	glPushMatrix( );
	DrawCircle(EarthOrbitalRadius, 1., 1., 1.);
	glPopMatrix( );
	glEndList();

	MarsOrbit = glGenLists (1);
	glNewList(MarsOrbit, GL_COMPILE);
	glPushMatrix( );
	DrawCircle(MarsOrbitalRadius, 1., 1., 1.);
	glPopMatrix( );
	glEndList();

	MoonOrbit = glGenLists (1);
	glNewList(MoonOrbit, GL_COMPILE);
	glPushMatrix( );
	DrawCircle(MoonOrbitalRadius, 1., 1., 1.);
	glPopMatrix( );
	glEndList();


	//---------------------------------------------------------------------------------

	//create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 20.5 );
		glLineWidth( 0.5 );
	glEndList( );
}

// A circle-drawing function
void 
DrawCircle(float radius, float r, float g, float b)
{
	glColor3f( r, g, b );
	float dang = 2. *M_PI/(float)(100);
	float ang = 0.;

	glBegin (GL_LINE_LOOP);
		for (int i = 0; i<100; i++)
		{
			glVertex3f (radius*cos(ang), 0., radius*sin(ang));
			ang += dang;
		}
	glEnd();
}

// -------------------------------------------------------------------------------
// A sphere-drawing function

int	NumLngs, NumLats;

struct point *	Pts;

struct point
{
	float x, y, z;		// coordinates
	float nx, ny, nz;	// surface normal
	float s, t;		// texture coords
};

inline
struct point *
PtsPointer( int lat, int lng )
{
	if( lat < 0 )	lat += (NumLats-1);
	if( lng < 0 )	lng += (NumLngs-0);
	if( lat > NumLats-1 )	lat -= (NumLats-1);
	if( lng > NumLngs-1 )	lng -= (NumLngs-0);
	return &Pts[ NumLngs*lat + lng ];
}

inline
void
DrawPoint( struct point *p )
{
	glColor3f(0., 1., 0.);
	glNormal3fv( &p->nx );
	glTexCoord2fv( &p->s );
	glVertex3fv( &p->x );
}

void
OsuSphere( float radius, int slices, int stacks )
{
	// set the globals:

	NumLngs = slices;
	NumLats = stacks;
	if( NumLngs < 3 )
		NumLngs = 3;
	if( NumLats < 3 )
		NumLats = 3;

	// allocate the point data structure:

	Pts = new struct point[ NumLngs * NumLats ];

	// fill the Pts structure:

	for( int ilat = 0; ilat < NumLats; ilat++ )
	{
		float lat = -M_PI/2.  +  M_PI * (float)ilat / (float)(NumLats-1);	// ilat=0/lat=0. is the south pole
											// ilat=NumLats-1, lat=+M_PI/2. is the north pole
		float xz = cosf( lat );
		float  y = sinf( lat );
		for( int ilng = 0; ilng < NumLngs; ilng++ )				// ilng=0, lng=-M_PI and
											// ilng=NumLngs-1, lng=+M_PI are the same meridian
		{
			float lng = -M_PI  +  2. * M_PI * (float)ilng / (float)(NumLngs-1);
			float x =  xz * cosf( lng );
			float z = -xz * sinf( lng );
			struct point* p = PtsPointer( ilat, ilng );
			p->x  = radius * x;
			p->y  = radius * y;
			p->z  = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			
			p->s = ( lng + M_PI    ) / ( 2.*M_PI );
			p->t = ( lat + M_PI/2. ) / M_PI;
		}
	}

	struct point top, bot;		// top, bottom points

	top.x =  0.;		top.y  = radius;	top.z = 0.;
	top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
	top.s  = 0.;		top.t  = 1.;

	bot.x =  0.;		bot.y  = -radius;	bot.z = 0.;
	bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
	bot.s  = 0.;		bot.t  =  0.;

	// connect the north pole to the latitude NumLats-2:

	glBegin(GL_TRIANGLE_STRIP);
	for (int ilng = 0; ilng < NumLngs; ilng++)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
		top.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&top);
		struct point* p = PtsPointer(NumLats - 2, ilng);	// ilat=NumLats-1 is the north pole
		DrawPoint(p);
	}
	glEnd();

	// connect the south pole to the latitude 1:

	glBegin( GL_TRIANGLE_STRIP );
	for (int ilng = NumLngs - 1; ilng >= 0; ilng--)
	{
		float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
		bot.s = (lng + M_PI) / (2. * M_PI);
		DrawPoint(&bot);
		struct point* p = PtsPointer(1, ilng);					// ilat=0 is the south pole
		DrawPoint(p);
	}
	glEnd();

	// connect the horizontal strips:

	for( int ilat = 2; ilat < NumLats-1; ilat++ )
	{
		struct point* p;
		glBegin(GL_TRIANGLE_STRIP);
		for( int ilng = 0; ilng < NumLngs; ilng++ )
		{
			p = PtsPointer( ilat, ilng );
			DrawPoint( p );
			p = PtsPointer( ilat-1, ilng );
			DrawPoint( p );
		}
		glEnd();
	}

	// clean-up:

	delete [ ] Pts;
	Pts = NULL;
}

// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;
		// Frozen
		case 'F':
		case 'f':
			Frozen = ! Frozen;
			if( Frozen )
				glutIdleFunc( NULL );
			else
				glutIdleFunc( Animate );
			break;
		case '0':
			SunLightOn = ! SunLightOn;
			break;

		case 'i':
		case 'I':
			whichViewMode = INSIDEVIEWMODE;
			Xrot_INSIDE = Xrot;
			Yrot_INSIDE = Yrot;
			Scale_INSIDE = Scale;
			break;

		case 'u':
		case 'U':
			whichViewMode = OUTSIDEVIEWMODE;
			Xrot = Xrot_INSIDE;
			Yrot = Yrot_INSIDE;
			Scale = Scale_INSIDE;
			break;

		case 'M':
		case 'm':
			whichViewMode = MARSVIEWMODE;
			break;
		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( true )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	WhichColor = WHITE;
	WhichProjection = ORTHO;
	Xrot = Yrot = 0.;

	// Final Project
	TextureFiltering = 1;
	Frozen = false;
	SunLightOn = true;
	whichViewMode = OUTSIDEVIEWMODE;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );
}


// read a BMP file into a Texture:

#define VERBOSE		false
#define BMP_MAGIC_NUMBER	0x4d42
#ifndef BI_RGB
#define BI_RGB			0
#define BI_RLE8			1
#define BI_RLE4			2
#endif


// bmp file header:
struct bmfh
{
	short bfType;		// BMP_MAGIC_NUMBER = "BM"
	int bfSize;		// size of this file in bytes
	short bfReserved1;
	short bfReserved2;
	int bfOffBytes;		// # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
	int biSize;		// info header size, should be 40
	int biWidth;		// image width
	int biHeight;		// image height
	short biPlanes;		// #color planes, should be 1
	short biBitCount;	// #bits/pixel, should be 1, 4, 8, 16, 24, 32
	int biCompression;	// BI_RGB, BI_RLE4, BI_RLE8
	int biSizeImage;
	int biXPixelsPerMeter;
	int biYPixelsPerMeter;
	int biClrUsed;		// # colors in the palette
	int biClrImportant;
} InfoHeader;



// read a BMP file into a Texture:

unsigned char *
BmpToTexture( char *filename, int *width, int *height )
{
	FILE *fp;
#ifdef _WIN32
        errno_t err = fopen_s( &fp, filename, "rb" );
        if( err != 0 )
        {
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
        }
#else
	fp = fopen( filename, "rb" );
	if( fp == NULL )
	{
		fprintf( stderr, "Cannot open Bmp file '%s'\n", filename );
		return NULL;
	}
#endif

	FileHeader.bfType = ReadShort( fp );


	// if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

	if( VERBOSE ) fprintf( stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
			FileHeader.bfType, FileHeader.bfType&0xff, (FileHeader.bfType>>8)&0xff );
	if( FileHeader.bfType != BMP_MAGIC_NUMBER )
	{
		fprintf( stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType );
		fclose( fp );
		return NULL;
	}


	FileHeader.bfSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize );

	FileHeader.bfReserved1 = ReadShort( fp );
	FileHeader.bfReserved2 = ReadShort( fp );

	FileHeader.bfOffBytes = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "FileHeader.bfOffBytes = %d\n", FileHeader.bfOffBytes );


	InfoHeader.biSize = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSize = %d\n", InfoHeader.biSize );
	InfoHeader.biWidth = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biWidth = %d\n", InfoHeader.biWidth );
	InfoHeader.biHeight = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biHeight = %d\n", InfoHeader.biHeight );

	const int nums = InfoHeader.biWidth;
	const int numt = InfoHeader.biHeight;

	InfoHeader.biPlanes = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biPlanes = %d\n", InfoHeader.biPlanes );

	InfoHeader.biBitCount = ReadShort( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount );

	InfoHeader.biCompression = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression );

	InfoHeader.biSizeImage = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage );

	InfoHeader.biXPixelsPerMeter = ReadInt( fp );
	InfoHeader.biYPixelsPerMeter = ReadInt( fp );

	InfoHeader.biClrUsed = ReadInt( fp );
	if( VERBOSE )	fprintf( stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed );

	InfoHeader.biClrImportant = ReadInt( fp );


	// fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );


	// pixels will be stored bottom-to-top, left-to-right:
	unsigned char *texture = new unsigned char[ 3 * nums * numt ];
	if( texture == NULL )
	{
		fprintf( stderr, "Cannot allocate the texture array!\n" );
		return NULL;
	}

	// extra padding bytes:

	int requiredRowSizeInBytes = 4 * ( ( InfoHeader.biBitCount*InfoHeader.biWidth + 31 ) / 32 );
	if( VERBOSE )	fprintf( stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes );

	int myRowSizeInBytes = ( InfoHeader.biBitCount*InfoHeader.biWidth + 7 ) / 8;
	if( VERBOSE )	fprintf( stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes );

	int oldNumExtra =  4*(( (3*InfoHeader.biWidth)+3)/4) - 3*InfoHeader.biWidth;
	if( VERBOSE )	fprintf( stderr, "Old NumExtra padding = %d\n", oldNumExtra );

	int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
	if( VERBOSE )	fprintf( stderr, "New NumExtra padding = %d\n", numExtra );


	// this function does not support compression:

	if( InfoHeader.biCompression != 0 )
	{
		fprintf( stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression );
		fclose( fp );
		return NULL;
	}
	

	// we can handle 24 bits of direct color:
	if( InfoHeader.biBitCount == 24 )
	{
		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				*(tp+2) = fgetc( fp );		// b
				*(tp+1) = fgetc( fp );		// g
				*(tp+0) = fgetc( fp );		// r
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}
	}

	// we can also handle 8 bits of indirect color:
	if( InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256 )
	{
		struct rgba32
		{
			unsigned char r, g, b, a;
		};
		struct rgba32 *colorTable = new struct rgba32[ InfoHeader.biClrUsed ];

		rewind( fp );
		fseek( fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET );
		for( int c = 0; c < InfoHeader.biClrUsed; c++ )
		{
			colorTable[c].r = fgetc( fp );
			colorTable[c].g = fgetc( fp );
			colorTable[c].b = fgetc( fp );
			colorTable[c].a = fgetc( fp );
			if( VERBOSE )	fprintf( stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
				c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a );
		}

		rewind( fp );
		fseek( fp, FileHeader.bfOffBytes, SEEK_SET );
		int t;
		unsigned char *tp;
		for( t = 0, tp = texture; t < numt; t++ )
		{
			for( int s = 0; s < nums; s++, tp += 3 )
			{
				int index = fgetc( fp );
				*(tp+0) = colorTable[index].r;	// r
				*(tp+1) = colorTable[index].g;	// g
				*(tp+2) = colorTable[index].b;	// b
			}

			for( int e = 0; e < numExtra; e++ )
			{
				fgetc( fp );
			}
		}

		delete[ ] colorTable;
	}

	fclose( fp );

	*width = nums;
	*height = numt;
	return texture;
}

int
ReadInt( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	const unsigned char b2 = fgetc( fp );
	const unsigned char b3 = fgetc( fp );
	return ( b3 << 24 )  |  ( b2 << 16 )  |  ( b1 << 8 )  |  b0;
}

short
ReadShort( FILE *fp )
{
	const unsigned char b0 = fgetc( fp );
	const unsigned char b1 = fgetc( fp );
	return ( b1 << 8 )  |  b0;
}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


