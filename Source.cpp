#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <freeglut.h>
#include <FreeImage.h>
#include <cmath>
#include <queue>    
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <cstring>
using namespace std;

int x, y;
queue<int> my_queue;
int window_w = 1000;
int window_h = 1000;
typedef struct {
	float color_r, color_b, color_g;
	int size;
	bool random_mode;
	bool smooth_color;
}paint_brush;
paint_brush pb;
//the pixel structure
typedef struct {
	GLubyte r, g, b;
} pixel;

//the global structure
typedef struct {
	pixel* data;
	int w, h;
	int maxColor, minColor;
} glob;
glob global;
int brush_style = 0;
enum { MENU_SAVE, MENU_QUIT, MENU_RAND, MENU_RESET, MENU_FREEZE, MENU_SMOOTH, MENU_RAND_FREEZE, MENU_BRUSH_BIGGER, MENU_BRUSH_SMALLER, MENU_BRUSH_DOT, 
	MENU_BRUSH_SQUARE, MENU_BRUSH_SNOWFLAKE, MENU_BRUSH_DIAMOND, MENU_BRUSH_TRIANGLE};

//function activates when button on the mouse is down and mouse is moving
void mouse(int mousex, int mousey)
{
	x = mousex;
	y = window_h - mousey;
	glutPostRedisplay();
}
//Clear the screen
void reset(void)
{
	glColor3f(0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glBegin(GL_POLYGON);
	glVertex2f(window_w, window_h);
	glVertex2f(-window_w, window_h);
	glVertex2f(-window_w, -window_h);
	glVertex2f(window_w, -window_h);
	glEnd();
	glFlush();
}
//write_img
void write_img(char* name) {
	FIBITMAP* image;
	RGBQUAD aPixel;
	int i, j;
	global.data = (pixel*)malloc((window_h) * (window_w) * sizeof(pixel*));
	glReadPixels(0, 0, window_w, window_h, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)global.data);

	image = FreeImage_Allocate(window_w, window_h, 24, 0, 0, 0);
	if (!image) {
		perror("FreeImage_Allocate");
		return;
	}
	for (i = 0; i < window_h; i++) {
		for (j = 0; j < window_w; j++) {
			aPixel.rgbRed = global.data[i * window_w + j].r;
			aPixel.rgbGreen = global.data[i * window_w + j].g;
			aPixel.rgbBlue = global.data[i * window_w + j].b;

			FreeImage_SetPixelColor(image, j, i, &aPixel);
		}
	}
	if (!FreeImage_Save(FIF_TIFF, image, name, 0)) {
		perror("FreeImage_Save");
	}
	FreeImage_Unload(image);
}//write_img

void menuFunc(int value)
{
	switch (value)
	{
		//Quit
	case MENU_QUIT:
		exit(0);
		break;
		//Random color
	case MENU_RAND:
		pb.smooth_color = false;
		pb.random_mode = true;
		break;
		//Reset
	case MENU_RESET:
		reset();
		break;
		//Freeze
	case MENU_FREEZE:
		pb.smooth_color = false;
		pb.random_mode = false;
		break;
		//Smooth => Extra feature makes small random changes to color to look more nicer
	case MENU_SMOOTH:
		pb.random_mode = false;
		pb.smooth_color = true;
		break;
		//Random Freeze
	case MENU_RAND_FREEZE:
		pb.smooth_color = false;
		pb.color_r = rand() % 256;
		pb.color_g = rand() % 256;
		pb.color_b = rand() % 256;
		pb.random_mode = false;
		break;
		//Smaller paint brush
	case MENU_BRUSH_BIGGER:
		if (pb.size > 5) {
			pb.size = pb.size + 3;
		}
		break;
		//Bigger paint brush
	case MENU_BRUSH_SMALLER:
		if (pb.size < 100) {
			pb.size = pb.size - 3;
		}
		break;
		//Save
	case MENU_SAVE:
		printf("SAVING IMAGE: picaso.tif\n");
		write_img((char*)"picaso.tif");
		break;
	case MENU_BRUSH_DOT:
		brush_style = 0;
		break;
	case MENU_BRUSH_SQUARE:
		brush_style = 1;
		break;
	case MENU_BRUSH_SNOWFLAKE:
		brush_style = 2;
		break;
	case MENU_BRUSH_DIAMOND:
		brush_style = 3;
		break;
	case MENU_BRUSH_TRIANGLE:
		brush_style = 4;
		break;
	}

}//menuFunc
//Glut menu set up
void init_menu()
{
	int sub_menu1 = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Increase brush size", MENU_BRUSH_BIGGER);
	glutAddMenuEntry("Decrease brush size", MENU_BRUSH_SMALLER);

	int sub_menu2 = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Switch random colors", MENU_RAND);
	glutAddMenuEntry("Freeze current color", MENU_FREEZE);
	glutAddMenuEntry("Use smooth gradient colors", MENU_SMOOTH);
	glutAddMenuEntry("Pick random color and freeze", MENU_RAND_FREEZE);

	int sub_menu3 = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Circle", MENU_BRUSH_DOT);
	glutAddMenuEntry("Square", MENU_BRUSH_SQUARE);
	glutAddMenuEntry("Snowflake", MENU_BRUSH_SNOWFLAKE);
	glutAddMenuEntry("Diamond", MENU_BRUSH_DIAMOND);
	glutAddMenuEntry("Triangle", MENU_BRUSH_TRIANGLE);


	int main_menu = glutCreateMenu(&menuFunc);
	glutAddSubMenu("Brush", sub_menu1);
	glutAddSubMenu("Paint", sub_menu2);
	glutAddSubMenu("Shape", sub_menu3);
	glutAddMenuEntry("Quit", MENU_QUIT);
	glutAddMenuEntry("Save", MENU_SAVE);
	glutAddMenuEntry("Clear", MENU_RESET);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//print menu options
void show_keys()
{
	printf("Q:quit\nN:random color mode\nR:reset\nF:freeze color\nH:randomize color and freeze\np:make paint brush smaller\nP:make paintbrush bigger\nS:save image\nM:apply smoother color");
	printf("More options on right click");

}

//glut keyboard function
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		//Quit
	case 0x1B:
	case'q':
	case 'Q':
		exit(0);
		break;
		//Random color
	case'n':
	case 'N':
		pb.smooth_color = false;
		pb.random_mode = true;
		break;
		//Reset
	case'r':
	case 'R':
		reset();
		break;
		//Freeze
	case'f':
	case 'F':
		pb.smooth_color = false;
		pb.random_mode = false;
		break;
		//Smooth => Extra feature makes small random changes to color to look more nicer
	case'm':
	case 'M':
		pb.random_mode = false;
		pb.smooth_color = true;
		break;
		//Random Freeze
	case'h':
	case 'H':
		pb.smooth_color = false;
		pb.color_r = rand() % 256;
		pb.color_g = rand() % 256;
		pb.color_b = rand() % 256;
		pb.random_mode = false;

		break;
		//Smaller paint brush
	case'p':
		if (pb.size > 3) {
			pb.size = pb.size - 1;
		}
		break;
		//Bigger paint brush
	case 'P':
		if (pb.size < 100) {
			pb.size = pb.size + 1;
		}
		break;
		//Save
	case's':
	case 'S':
		printf("SAVING IMAGE: picaso.tif\n");
		write_img((char*)"picaso.tif");
		break;
	}
}//keyboard

//draw dots following the mouse
void drawDots(int og_x, int og_y) {
	switch (brush_style)
	{
		//dots (line)
	case 0:
		float angle;
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			angle = i * 3.142 / 180;
			glVertex2f(og_x + pb.size * cos(angle), og_y + pb.size * sin(angle));
		}
		glEnd();
		break;
		//square
	case 1: 
		glBegin(GL_POLYGON);
		glVertex2f(og_x - pb.size, og_y - pb.size);
		glVertex2f(og_x + pb.size, og_y - pb.size);
		glVertex2f(og_x + pb.size, og_y + pb.size);
		glVertex2f(og_x - pb.size, og_y + pb.size);
		glEnd();
		break;
		//snowflake
	case 2:
		glBegin(GL_LINES);
		glVertex2f(og_x - pb.size, og_y - pb.size);
		glVertex2f(og_x + pb.size, og_y + pb.size);
		glVertex2f(og_x, og_y + pb.size);
		glVertex2f(og_x , og_y - pb.size);
		glVertex2f(og_x + pb.size, og_y );
		glVertex2f(og_x - pb.size, og_y );
		glVertex2f(og_x + pb.size, og_y - pb.size);
		glVertex2f(og_x - pb.size, og_y + pb.size);
		glEnd();
		break;
		//diamond
	case 3:
		glBegin(GL_LINE_LOOP);
		glVertex2f(og_x - pb.size, og_y);
		glVertex2f(og_x, og_y + pb.size);
		glVertex2f(og_x + pb.size, og_y);
		glVertex2f(og_x, og_y - pb.size);
		glEnd();
		break;
		//triangle
	case 4:
		glBegin(GL_TRIANGLES);
		glVertex2f(og_x, og_y + pb.size);
		glVertex2f(og_x + pb.size, og_y - pb.size);
		glVertex2f(og_x - pb.size, og_y - pb.size);
		glEnd();
		break;
	}

	
}

//draw on dixplay whenever is called to refresh display
void display(void)
{
	//pick color
	if (pb.random_mode) {
		pb.color_r = rand() % 256;
		pb.color_g = rand() % 256;
		pb.color_b = rand() % 256;
	}
	else if (pb.smooth_color) {
		int x = rand() % 4;
		if (x < 1) {
			if (pb.color_r < 256)
				pb.color_r++;
			else
				pb.color_r = 0;
		}
		else if (x < 2) {
			if (pb.color_g < 256)
				pb.color_g++;
			else
				pb.color_g = 0;
		}
		else {
			if (pb.color_b < 256)
				pb.color_b++;
			else
				pb.color_b = 0;
		}
	}
	glColor3ub(pb.color_r, pb.color_b, pb.color_g);

	//start drawing cycle
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Pick 8 coordinates for symmetry
	gluOrtho2D(0.0, window_w, 0.0, window_h);
	x = x - (window_w / 2);
	y = y - (window_h / 2);
	drawDots(x + (window_w / 2), y + (window_h / 2));
	drawDots(x + (window_w / 2), -y + (window_h / 2));
	drawDots(-x + (window_w / 2), y + (window_h / 2));
	drawDots(-x + (window_w / 2), -y + (window_h / 2));
	drawDots(y + (window_w / 2), x + (window_h / 2));
	drawDots(-y + (window_w / 2), x + (window_h / 2));
	drawDots(y + (window_w / 2), -x + (window_h / 2));
	drawDots(-y + (window_w / 2), -x + (window_h / 2));
	//finish drawing
	glFlush();
}


int main(int argc, char** argv)
{
	//set variables once
	pb.random_mode = true;
	pb.size = 10;
	pb.smooth_color = false;
	pb.color_r = rand() % 256;
	pb.color_g = rand() % 256;
	pb.color_b = rand() % 256;
	//set up window
	glutInit(&argc, argv);
	glutInitWindowSize(window_w, window_h);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Fun drawing");
	glClear(GL_COLOR_BUFFER_BIT);
	glutDisplayFunc(display);
	glClearColor(0, 0, 0, 0);
	//add function to activate on active mouse movment
	glutMotionFunc(mouse);
	//add functions for keyboard
	glutKeyboardFunc(keyboard);
	//show menu
	show_keys();
	//display menu
	init_menu();
	glutMainLoop();
}
