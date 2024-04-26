#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>
#include <gsl/gsl_sf_bessel.h>
#include <conio.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

using namespace std;

// Screen dimentions
int HEIGHT = 50;
int WIDTH = 50;

// Camera position
vector<double> pos = { 0, 0, 0};
// Horizontal rotation
double THETA = 0;
// Vertical rotation
double GAMMA = 0;

// Looking direction vector
vector<double> u;
// Vertical screen vector
vector<double> vertical;
// Horizontal screen vector
vector<double> horizontal;

// Display screen
vector<vector<char>> screen(HEIGHT, vector<char>(WIDTH, ' '));

// Vector parameters
struct eq_params {
	vector<double> from;
	vector<double> dir;
};

// Function equation
double eq(double x, double y, double z) {
	return x * x + y * y - z * z - 9;
}
// Function equation for t
double eq_t(double t, void* params) {
	struct eq_params* p = (struct eq_params*)params;
	return eq(
		p->from[0] + p->dir[0] * t,
		p->from[1] + p->dir[1] * t,
		p->from[2] + p->dir[2] * t
		);
}



void updateVectors() {
	u = {
		cos(THETA) * cos(GAMMA),
		sin(THETA) * cos(GAMMA),
		sin(GAMMA)
	};
	vertical = {
		cos(THETA) * cos(GAMMA - M_PI_2),
		sin(THETA) * cos(GAMMA - M_PI_2),
		sin(GAMMA - M_PI_2)
	};
	horizontal = {
		cos(THETA - M_PI_2),
		sin(THETA - M_PI_2),
		0
	};
}
double solveForT(eq_params* params) {
	int status;
	int iter = 0, max_iter = 100;
	const gsl_root_fsolver_type* T;
	gsl_root_fsolver* s;
	double t = 0;
	double x_low = 0, x_high = 100;
	gsl_function F;

	F.function = &eq_t;
	F.params = params;

	T = gsl_root_fsolver_brent;
	s = gsl_root_fsolver_alloc(T);
	gsl_set_error_handler_off();
	gsl_root_fsolver_set(s, &F, x_low, x_high);

	do {
		iter++;
		status = gsl_root_fsolver_iterate(s);
		t = gsl_root_fsolver_root(s);
		x_low = gsl_root_fsolver_x_lower(s);
		x_high = gsl_root_fsolver_x_upper(s);
		status = gsl_root_test_interval(x_low, x_high, 0, 0.001);
	} while (status == GSL_CONTINUE && iter < max_iter);

	gsl_root_fsolver_free(s);

	return t;
}
void generatePixel(int i, int j) {
	vector<double> curU = {
		10 * u[0] + vertical[0] * (i - HEIGHT / 2) + horizontal[0] * (j - WIDTH / 2),
		10 * u[1] + vertical[1] * (i - HEIGHT / 2) + horizontal[1] * (j - WIDTH / 2),
		10 * u[2] + vertical[2] * (i - HEIGHT / 2) + horizontal[2] * (j - WIDTH / 2)
	};
	struct eq_params params = { pos, curU };

	double t = solveForT(&params);

	screen[i][j] = ' ';
	if (eq_t(t, &params) < 0.1) {
		screen[i][j] = '*';
	}
}
void generateScreen() {

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			generatePixel(i, j);
		}
	}
}
void display() {
	system("CLS");
	cout << "[ " << pos[0] << ", " << pos[1] << ", " << pos[2] << " ] | THETA : " << THETA << " | GAMMA : " << GAMMA << endl;
	cout << "u	: [ " << u[0] << ", " << u[1] << ", " << u[2] << " ]" << endl;
	cout << "ver	: [ " << vertical[0] << ", " << vertical[1] << ", " << vertical[2] << " ]" << endl;
	cout << "hor	: [ " << horizontal[0] << ", " << horizontal[1] << ", " << horizontal[2] << " ]" << endl;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			cout << screen[i][j] << " ";
		}
		cout << endl;
	}
}
void keyAction() {
	bool cont = true;
	while (cont) {
		cont = false;
		char key = _getch();
		switch (key) {
		case 'w':
			pos[0]++;
			break;
		case 's':
			pos[0]--;
			break;
		case 'a':
			pos[1]--;
			break;
		case 'd':
			pos[1]++;
			break;
		case 'r':
			pos[2]++;
			break;
		case 'f':
			pos[2]--;
			break;
		case KEY_UP:
			GAMMA += M_PI / 16;
			break;
		case KEY_DOWN:
			GAMMA -= M_PI / 16;
			break;
		case KEY_LEFT:
			THETA -= M_PI / 16;
			break;
		case KEY_RIGHT:
			THETA += M_PI / 16;
			break;
		default:
			cont = true;
			break;
		}
	}
}

int main()
{
	ios::sync_with_stdio(0), cin.tie(0), cout.tie(0);
	while (true)
	{
		updateVectors();
		generateScreen();
		display();
		keyAction();
	}

	return 0;
}