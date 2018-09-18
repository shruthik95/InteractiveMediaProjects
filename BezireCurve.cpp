

#include<stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>


using namespace cv;
using namespace std;

vector<Point> pts;
vector<Point> rect;

Mat painting(500,800, CV_8UC3);

bool clicked = false;
double t = 4;
int idx = -1; 

bool p= false;

bool btndown=false;

int p_idx = NULL;


void onMouse(int event, int x, int y, int flags, void *param)
{
	if(event == EVENT_LBUTTONDOWN) //left button down
	{
		btndown=true;
		if (p)
			clicked = true;
		for (int i = 0; i < pts.size(); i++)
		{
			double distance = pow((double)(x - pts[i].x), 2) + pow((double)(y - pts[i].y), 2);
			distance = pow(distance, 0.5);
			if (distance < t)
			{
				idx = i;
				break;
			}
		}
	
	 if (idx > -1)
		{
			pts[idx].x = x;
			pts[idx].y = y;
		}

	
		else if (!p)
		{
			pts.push_back(Point(x, y));
		}

		printf("The number of points are: %d\n", (int)pts.size());
		
	} //leftbttn
		
	if(event == CV_EVENT_MOUSEMOVE && btndown== true) //mouse dragging
		{

		p= false;

		if (clicked)
		{
			Point np;

			np.x = x - rect[p_idx].x;
			np.y = y - rect[p_idx].y;

			int spline = pts.size() / 4;

			int n = 0;

			for (int i = 0; i < spline; i++)
			{
				if (i == p_idx)
				{
					pts[n] = pts[n] + np;
					pts[n + 1] = pts[n + 1] + np;
					pts[n + 2] = pts[n + 2] + np;
					pts[n + 3] = pts[n + 3] + np;
				}
				n += 4;
			}
			rect[p_idx] = Point(x, y);

		}

		if (idx > -1)
		{
			pts[idx].x = x;
			pts[idx].y = y;
		}
		} // mouse dragging
		
	 if(event == EVENT_RBUTTONDOWN) //button up
		{
		if (pts.size() > 0)
			pts.pop_back(); //remove the latest point

		printf("The number of points are: %d\n", (int)pts.size()); 
		}	

	 if(event == EVENT_LBUTTONUP) //buttonup
		{
		idx = -1;
		clicked = false;
		p= false;
		btndown=false;
		}
	}



void drawCurve()
{
	int spline_num = pts.size() / 4;
	int count = 0;


	for (int i = 0; i < spline_num; i++)
	{

		Mat h_hermite(4, 2, CV_32FC1);

		h_hermite.ptr<float>(0)[0] = pts[count + 0].x;
		h_hermite.ptr<float>(0)[1] = pts[count + 0].y;
		h_hermite.ptr<float>(1)[0] = pts[count + 1].x;
		h_hermite.ptr<float>(1)[1] = pts[count + 1].y;
		h_hermite.ptr<float>(2)[0] = pts[count + 2].x;
		h_hermite.ptr<float>(2)[1] = pts[count + 2].y;
		h_hermite.ptr<float>(3)[0] = pts[count + 3].x;
		h_hermite.ptr<float>(3)[1] = pts[count + 3].y;



		float values[4][4] = { { -1, 3, -3, 1 }, { 3, -6, 3, 0 }, { -3, 3, 0, 0 }, { 1, 0, 0, 0 } };
		Mat m_hermite(4, 4, CV_32FC1, &values);

		Mat matrix = m_hermite*h_hermite;

		for (double t = 0.0; t <= 1.0; t += 0.001)
		{
			int x = matrix.ptr<float>(0)[0] * pow(t, 3) + matrix.ptr<float>(1)[0] * pow(t, 2) + matrix.ptr<float>(2)[0] * t + matrix.ptr<float>(3)[0];
			int y = matrix.ptr<float>(0)[1] * pow(t, 3) + matrix.ptr<float>(1)[1] * pow(t, 2) + matrix.ptr<float>(2)[1] * t + matrix.ptr<float>(3)[1];
			circle(painting, Point(x, y), 1, Scalar(0, 255, 0), 2, 8, 0);

		}

		line(painting, pts[count + 0], pts[count + 1], Scalar(255, 0,255), 1, 8, 0);
		line(painting, pts[count + 2], pts[count + 3], Scalar(255, 0, 255), 1, 8, 0);


		count += 4;
	}

}


int main(int argc, char** argv)
{
	namedWindow("painting");
	setMouseCallback("painting", onMouse);


	while (1)
	{
		painting = Scalar(255, 255, 255); //making empty image to white
		
		//draw points
		for (int i = 0; i < pts.size(); i++)
		{
		circle(painting, pts[i], 1, Scalar(0, 0, 255), 2);
		}
		
		//draw spline
		drawCurve();
		
		imshow("painting", painting);
		
		char c = waitKey(1);
		if (c == 27)
			break;

		else if (c == 's' || c == 'S')
		{
			imwrite("D\:curve.jpg", painting);
		}

		
	}
	return 0;
}
