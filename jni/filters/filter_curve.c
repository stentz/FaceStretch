#include "filters.h"
#include <android/log.h>

typedef struct {
	float x;
	float y;
} ControlPoint;

void solveSystem(double *solution, ControlPoint *points) {
	double system[3][3] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double result[3] = { 0, 0, 0 };
	system[0][1] = 1;
	system[2][1] = 1;
	double d6 = 1.0 / 6.0;
	double d3 = 1.0 / 3.0;
    
	int i;
	for (i = 1; i < 2; i++) {
		double deltaPrevX = points[i].x - points[i - 1].x;
		double deltaX = points[i + 1].x - points[i - 1].x;
		double deltaNextX = points[i + 1].x - points[i].x;
		double deltaNextY = points[i + 1].y - points[i].y;
		double deltaPrevY = points[i].y - points[i - 1].y;
        
		system[i][0] = d6 * deltaPrevX;
		system[i][1] = d3 * deltaX;
		system[i][2] = d6 * deltaNextX;
		result[i] = (deltaNextY / deltaNextX) - (deltaPrevY / deltaPrevX);
	}
    
	for (i = 1; i < 3; i++) {
		double m = system[i][0] / system[i - 1][1];
		system[i][1] = system[i][1] - m * system[i - 1][2];
		result[i] = result[i] - m * result[i - 1];
	}
    
	solution[2] = result[2] / system[2][1];
	for (i = 1; i >= 0; --i) {
		solution[i] = (result[i] - system[i][2] * solution[i + 1])
        / system[i][1];
	}
}

void getSpline(float *curve, float control_X, float control_Y) {
	double derivatives[3];
	ControlPoint points[3];
	points[0].x = 0.0f;
	points[0].y = 0.0f;
	points[1].x = control_X;
	points[1].y = control_Y;
	points[2].x = 1.0f;
	points[2].y = 1.0f;
    
	solveSystem(derivatives, points);
	int i;
	int start = 0;
	if (points[0].x != 0) {
		start = (int) (points[0].x * 256);
	}
	for (i = 0; i < start; i++) {
		curve[i] = 1.0f - points[0].y;
	}
	for (i = start; i < 256; i++) {
		ControlPoint cur;
		ControlPoint next;
		double x = i / 256.0;
		int pivot = 0;
		int j;
		for (j = 0; j < 3 - 1; j++) {
			if (x >= points[j].x && x <= points[j + 1].x) {
				pivot = j;
			}
		}
		cur = points[pivot];
		next = points[pivot + 1];
		if (x <= next.x) {
			double x1 = cur.x;
			double x2 = next.x;
			double y1 = cur.y;
			double y2 = next.y;
            
			double delta = (x2 - x1);
			double delta2 = delta * delta;
			double b = (x - x1) / delta;
			double a = 1 - b;
			double ta = a * y1;
			double tb = b * y2;
			double tc = (a * a * a - a) * derivatives[pivot];
			double td = (b * b * b - b) * derivatives[pivot + 1];
			double y = ta + tb + (delta2 / 6) * (tc + td);
			if (y > 1.0f) {
				y = 1.0f;
			}
			if (y < 0) {
				y = 0;
			}
			curve[i] = (float) (1.0f - y);
		} else {
			curve[i] = 1.0f - next.y;
		}
	}
}

