///////////////////////////////////////////////////////////////////////////////
// 2D Game Program
// ===============
// This program implements a simple 2D game using OpenGL and GLFW. The game
// involves circular objects (referred to as "balls") that move within a window 
// and interact with static rectangular bricks. The primary features of the game
// include:
// 
// - **Bricks**: Represented as either reflective or destructible. Reflective 
//   bricks change the direction of the balls upon collision, while destructible
//   bricks disappear when hit.
// - **Balls**: Circular objects that move randomly within the window. Their 
//   motion is influenced by collisions with bricks and window boundaries.
// - **Input Handling**: Players can spawn new balls by pressing the spacebar or 
//   close the game window by pressing the ESC key.
//
// Key Features:
// -------------
// - **Brick Types**:
//   - Reflective bricks alter the direction of the balls.
//   - Destructible bricks disappear upon collision.
// - **Ball Motion**:
//   - Balls move in random directions and bounce upon hitting boundaries.
//   - Collisions with bricks are handled dynamically.
// - **User Input**:
//   - Spacebar spawns a new ball at the center with random colors.
//   - ESC closes the application.
// 
// Dependencies:
// -------------
// - GLFW for window creation and input handling.
// - OpenGL for rendering graphics.
// 
// Author: Tashi Anderson
// Date: March 2, 2025
// Assignment: 8-2 Assignment - CS-330 Computational Graphics and Visualization
///////////////////////////////////////////////////////////////////////////////

#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE, PADDLE };
enum ONOFF { ON, OFF };

class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	int health;
	BRICKTYPE brick_type;
	ONOFF onoff;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		onoff = ON;
		health = 10;
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside);
			glVertex2d(x + halfside, y - halfside);
			glVertex2d(x - halfside, y - halfside);
			glVertex2d(x - halfside, y + halfside);

			glEnd();
		}
	}
};

Brick paddle(PADDLE, 0.0, -1.0, 0.3, 1, 1, 1);//creates the paddle


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float speed = 0.03;
	int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left
	ONOFF onoff;

	Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		radius = rr;
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
		onoff = ON;
	}

	void CheckCollision(Brick* brk)
	{
		if (brk->brick_type == REFLECTIVE && onoff == ON) //made sure that the brick has to be in its ON state to reflect the ball
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				direction = GetRandomDirection();
				x = x + 0.03;
				y = y + 0.04;
			}
		}

		else if (brk->brick_type == PADDLE && brk->onoff == ON) { //defines the collision between the paddle and balls
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				direction = GetRandomDirection();
				x = x + 0.03;
				y = y + 0.04;
			}
		}
		else if (brk->brick_type == DESTRUCTABLE && brk->onoff == ON && brk->health > 0 && onoff == ON) //same as the reflective type, but added health
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				brk->health--;
				if (brk->health <= 0) {
					brk->onoff = OFF;
				}
				brk->green = 1.0;
				direction = GetRandomDirection();
				x = x + 0.03;
				y = y + 0.04;

				//brk->onoff = OFF;
			}
		}
	}

	int GetRandomDirection()
	{
		return (rand() % 8) + 1;
	}

	void MoveOneStep()
	{
		if (direction == 1 || direction == 5 || direction == 6)  // up
		{
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 2 || direction == 5 || direction == 7)  // right
		{
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 3 || direction == 7 || direction == 8)  // down
		{
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}

		if (direction == 4 || direction == 6 || direction == 8)  // left
		{
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = GetRandomDirection();
			}
		}
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
};


vector<Circle> world;


int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(480, 480, "8-2 Assignment", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	Brick brick(DESTRUCTABLE, 0.5, -0.33, 0.2, 1, 1, 0);
	Brick brick2(REFLECTIVE, -0.5, 0.33, 0.2, 1, 1, 0);
	Brick brick3(DESTRUCTABLE, -0.5, -0.33, 0.2, 1, 1, 1);
	Brick brick4(DESTRUCTABLE, 0, 0, 0.2, 1, 0.5, 0);
	Brick brick5(DESTRUCTABLE, -0.5, -0.33, 0.2, 1, 1, 0);

	while (!glfwWindowShouldClose(window)) {
		//Setup View
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);

		//Movement
		for (int i = 0; i < world.size(); i++)
		{
			world[i].CheckCollision(&paddle);
			world[i].CheckCollision(&brick);
			world[i].CheckCollision(&brick2);
			world[i].CheckCollision(&brick3);
			world[i].CheckCollision(&brick4);
			world[i].CheckCollision(&brick5);
			world[i].MoveOneStep();
			world[i].DrawCircle();

		}
		paddle.drawBrick();
		brick.drawBrick();
		brick2.drawBrick();
		brick3.drawBrick();
		brick4.drawBrick();
		brick5.drawBrick();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate;
	exit(EXIT_SUCCESS);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		double r, g, b;
		r = rand() / 10000;
		g = rand() / 10000;
		b = rand() / 10000;
		Circle B(0, 0, 02, 2, 0.05, r, g, b);
		world.push_back(B);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {//paddle movements
		paddle.x -= 0.05;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		paddle.x += 0.05;
	}
}