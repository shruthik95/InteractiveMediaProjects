#include <stdio.h> //create a file, this is a default library

#include <iostream> //system library


//Below are the three important OpenCV library header files
#include <opencv2/core/core.hpp>  //the very basics too for image processing
#include <opencv2/highgui/highgui.hpp>   //for input/output, e.g. load the image into RAM
                                         //show the image to the screen
#include <opencv2/imgproc/imgproc.hpp>  //some advanced tool for image processing

using namespace cv; //OpenCV namespace
using namespace std;

bool btndown=false;
Point pt_down,pt_up ;
int blur_degree=5;
int A1,B1,A2,B2; 
 Mat image;  
 
void OnMouseEvent(int event, int x, int y, int flags, void* userdata)
{
	if(event== EVENT_LBUTTONDOWN)
	{
		btndown=true;
		pt_down.x=x;
		pt_down.y=y;
		A1=x;
		B1=y;
		printf("the first point is (%d, %d) \n",x,y);
	}

	if(event == EVENT_MOUSEMOVE && btndown==true)
	{
		printf("mouse is dragging(%d,%d)..\n",x,y );
		pt_up.x=x;
		pt_up.y=y;
		A2=x;
		B2=y;
		
	}
	
	if(event == EVENT_LBUTTONUP)
    {
		pt_up.x=x;
		pt_up.y=y;
        A2=x;
		B2=y;
		btndown = false;
		printf("the mouse is released (%d,%d) \n",x,y);
    }

}

void mosaic (Mat image)
{
	double sumb=0, sumg=0, sumr=0; //store the sum of all the pixels values in the block

	int no_pixels_block = blur_degree * blur_degree; 
	
	/*int num_rows = (pt_up.x - pt_down.x)/blur_degree;
	int num_cols = (pt_up.y - pt_down.x)/blur_degree;*/
	int num_rows = (A2 - A1)/blur_degree;
	int num_cols = (B2 - B1)/blur_degree;
	
	//for loops to traverse all the the blocks in image

	for (int rows = 0; rows <= num_rows; rows++)
	{
		for (int cols = 0; cols <= num_cols; cols++)
		{
			/*int x1 = pt_down.x + rows*(blur_degree);
			int y1 = pt_down.y+ cols*(blur_degree);*/
			int x1 = A1 + rows*(blur_degree);
			int y1 = B1 + cols*(blur_degree);

			// for loop to traverse all the pixels in the block 

			for (int row_block = y1;  row_block < y1 + (blur_degree);  row_block++)
			{
				for (int  col_block = x1; col_block < x1 + (blur_degree); col_block++)
				{
					sumb += image.ptr<uchar>(row_block)[3 * col_block ];
					sumg += image.ptr<uchar>(row_block)[3 * col_block + 1];
					sumr += image.ptr<uchar>(row_block)[3 * col_block + 2];
				}
			}

          //calculating average of pixels in one block
			double avg_blue = sumb / no_pixels_block;
			double avg_green = sumg / no_pixels_block;
			double avg_red = sumr / no_pixels_block;
			
			
			 //assigning average value back to the pixels

			for (int row_block = y1; row_block < y1 + (blur_degree) && row_block < B2; row_block++)
			{
				for (int col_block = x1; col_block < x1 + (blur_degree) && col_block < A2; col_block ++)
				{
					image.ptr<uchar>(row_block)[3 * col_block ] = avg_blue;
					image.ptr<uchar>(row_block)[3 * col_block + 1] = avg_green;
					image.ptr<uchar>(row_block)[3 * col_block + 2]= avg_red;
				}
			}
			
			sumb=0, sumg=0, sumr=0;

		} //end of cols for loop
	} // end of rows for loop
} //end of mosaic effect


int main(int argc, char** argv)
{
	VideoCapture camera(0);
  
	namedWindow("test");
	setMouseCallback("test", OnMouseEvent);
	
    for(;;)
    {
		camera>> image;
		
		if(btndown==true)
		{ 
			//rectangle(image,Point(A1,B1),Point(A2,B2),Scalar(0,255,0),2);
			 rectangle(image, Point(pt_down.x, pt_down.y),Point(pt_up.x, pt_up.y), Scalar(0, 0, 255), 2);
			
		}
			mosaic(image); //calling mosaic function 

		imshow("test", image);  //display the image
        
        char c = waitKey(1);   
       
		if(c == 27)   
        {
            break;
        }

  // keyboard events
		switch(c)
		{
		case 'i': 
		case 'I':
				if (blur_degree < 5)
			{
				blur_degree += 1;
			}
			else if (blur_degree >= 5)
			{
				blur_degree += 5;
			}
			mosaic(image);
			printf("i or I is pressed to increase mosaic effect \n");
			break;
		
		case 'd':
		case 'D':
			
			if (blur_degree <= 5 && blur_degree > 1)
			{
				blur_degree -= 1;
			}
			else if (blur_degree > 5)
			{
				blur_degree -= 5;
			}
			
			mosaic(image);
			printf("d or D is pressed to decrease mosaic effect \n");
			break;
		case 'r':
		case 'R':
			A1 = 0, B1 = 0, A2 = 0, B2 = 0; //it removes the mosaic effect
			printf("r or R is pressed to remove mosaic effect \n");
			break;

		case 's':
		case 'S':
			imwrite("p1.png", image);
			printf("s or S is pressed to save the image of mosaic effect to the current project folder \n");// it stores the image pic in the current project folder.
			break;
		/*case '27':
			   break;*/
		default:
			break;
		} //end of switch

       
	}//end for loop
    
    return 1;

}

