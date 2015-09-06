#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct cgsize {
	GLint x;
	GLint y;
} cgsize;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void draw_point(int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


void circle_plot_points(GLint xc, GLint yc, cgsize cz)
{
	draw_point(xc+cz.x, yc+cz.y);
	draw_point(xc-cz.x, yc+cz.y);
	draw_point(xc+cz.x, yc-cz.y);
	draw_point(xc-cz.x, yc-cz.y);
	draw_point(xc+cz.y, yc+cz.x);
	draw_point(xc-cz.y, yc+cz.x);
	draw_point(xc+cz.y, yc-cz.x);
	draw_point(xc-cz.y, yc-cz.x);
}

void circle_mid_point(GLint xc, GLint yc, GLint radius)
{
	cgsize circsz;
	GLint p = 1 - radius;
	circsz.x = 0;
	circsz.y = radius;

	circle_plot_points(xc, yc, circsz);

	while (circsz.x < circsz.y) {
		circsz.x += 1;
		if (p < 0) {
			p += 2*circsz.x+1;
		} else {
			circsz.y -= 1;
			p += 2*(circsz.x-circsz.y)+1;
		}
		circle_plot_points(xc, yc, circsz);
	}
}

void ellipse_plot_points(int xcenter, int ycenter, int x, int y)
{
	draw_point(xcenter+x, ycenter+y);
	draw_point(xcenter-x, ycenter+y);
	draw_point(xcenter+x, ycenter-y);
	draw_point(xcenter-x, ycenter-y);
}

void ellipse_mid_point(int xcenter, int ycenter, int rx, int ry)
{
	int rx2 = rx*rx;
	int ry2 = ry*ry;
	int tworx2 = 2*rx2;
	int twory2 = 2*ry2;
	int p;
	int x = 0;
	int y = ry;
	int px = 0;
	int py = tworx2 * y;

	ellipse_plot_points(xcenter, ycenter, x, y);

	p = round(ry2 - (rx2 * ry) + (0.25 * rx2));
	while(px < py) {
		x++;
		px += twory2;
		if (p < 0) {
			p += ry2 + px;
		} else {
			y--;
			py -= tworx2;
			p += ry2 + px - py;
		}
		ellipse_plot_points(xcenter, ycenter, x, y);
	}

	p = round(ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2);
	while (y > 0) {
		y--;
		py -= tworx2;
		if (p > 0) {
			p += rx2 - py;
		} else {
			x++;
			px += twory2;
			p += rx2 - py + px;
		}
		ellipse_plot_points(xcenter, ycenter, x, y);
	}
}

//inline int round(const float a) { return int (a + 0.5); }
void line_dda(int x0, int y0, int xend, int yend)
{
	int dx = xend - x0;
	int dy = yend - y0;
	int steps, k;
	float xincr, yincr, x = x0, y = y0;

	if (fabs(dx) > fabs(dy))
		steps = fabs(dx);
	else
		steps = fabs(dy);

	xincr = dx * 1.f / steps;
	yincr = dy * 1.f / steps;
	//printf("%f, %f\n", xincr, yincr);
	draw_point(round(x), round(y));
	for (k = 0; k < steps; k++) {
		x += xincr;
		y += yincr;
		draw_point(round(x), round(y));
	}
}

/* Bresenham line-drawing procedurce for |m| < 1.0. */
void line_bres(int x0, int y0, int xend, int yend)
{
	int dx = fabs(xend - x0);
	int dy = fabs(yend - y0);
	int p = 2 * dy - dx;
	int twody = 2 * dy;
	int dydx = 2 * (dy -dx);
	int x, y;

	if (x0 > xend)
	{
		x = xend;
		y = yend;
		xend = x0;
	}
	else
	{
		x = x0;
		y = y0;
	}
	draw_point(x, y);

	while(x < xend)
	{
		x++;
		if (p < 0)
			p += twody;
		else
		{
			y++;
			p += dydx;
		}
		draw_point(x, y);
	}
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	while (!glfwWindowShouldClose(window))
	{
		//float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		//ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.f, width, 0.f, height, 1.f, -1.f);
		glMatrixMode(GL_MODELVIEW);
		//glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
		glBegin(GL_TRIANGLES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(100.f, 100.f, 0.f);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(180.f, 100.f, 0.f);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(140.f, 165.f, 0.f);
		glEnd();
		glColor3f(1.f, 0.f, 0.f);
		line_dda(0, 0, width, height);
		glColor3f(0.f, 1.f, 0.f);
		line_bres(0, 50, width-50, height);
		glColor3f(0.f, 0.f, 1.f);
		circle_mid_point(320, 240, 50);
		glColor3f(1.f, 0.f, 1.f);
		ellipse_mid_point(320, 240, 80, 40);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
