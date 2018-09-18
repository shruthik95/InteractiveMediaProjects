#include <windows.h>
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <sstream>
#include <stdio.h>

#include <glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace std;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;

struct Color{
    GLfloat channel[3];
};

struct Vertex{
	double x;
	double y;
	double z;
	double matrix[4];
	Color c;
	  float uni_x;
    float uni_y;
    float uni_z;
	Vertex(double x,double y,double z)
	{
		this->x=x;
		this->y=y;
		this->z=z;
  }

	Vertex() {}

	 void normalize()
    {
        float len = x * x + y * y + z * z;
        uni_x = x / sqrt(len);
        uni_y = y / sqrt(len);
        uni_z = z / sqrt(len);
    }

    //const Vertex operator * (const Vertex right) const
    //{
    //    Vertex result;
    //    result.x = x*right.x;
    //    result.y = y*right.y;
    //    result.z = z*right.z;
    //    return result;
    //}

    const Vertex operator * (const float right) const
    {
        Vertex result;
        result.x = x*right;
        result.y = y*right;
        result.z = z*right;
        return result;
    }

    const Vertex operator - (const Vertex& right) const
    {
        Vertex result;

        result.x = x - right.x;
        result.y = y - right.y;
        result.z = z - right.z;

        return result;
    }

   /* const Vertex operator - (const float& right) const
    {
        Vertex result;
        result.x = x - right;
        result.y = y - right;
        result.z = z - right;

        return result;
    }*/

    const Vertex operator + (const Vertex& right) const
    {
        Vertex result;
        result.x = x + right.x;
        result.y = y + right.y;
        result.z = z + right.z;
        return result;
    }

    float dotProduct(const Vertex& right) const
    {
        float r;
        r = 1.0*x*right.x + 1.0*y*right.y + 1.0*z*right.z;

        return r;
    }

    Vertex cross(const Vertex& right)
    {
        Vertex result;
        result.x = y*right.z - z*right.y;
        result.y = z*right.x - x*right.z;
        result.z = x*right.y - y*right.x;

        return result;
    }

	}; //end of struct vertex


struct Triangle{ 
	int v1;
	int v2;
	int v3;
	Color c[3];

	Triangle(int v1,int v2,int v3)
	{
		this->v1=v1;
		this->v2=v2;
		this->v3=v3;
	}

	Triangle(){}

}; //end of struct triangle

vector<Vertex> vertices_list;
vector<Triangle> triangles_list;
Vertex light;
Vertex pos;
Color ambient_color;
Color teddy_color;
Color specular_color;
int p = 100;

void loadObj(char *filename, vector<Vertex> &vertices_list, vector<Triangle> &triangles_list)
{
	ifstream file(filename);
	string str;
	string valuex, valuey, valuez, v;
	string vt0, vt1, vt2, f;

	while (!file.eof())
	{
		getline(file, str);
		if (str[0] == 'v')
		{
			std::istringstream iss(str);
			iss >> v >> valuex >> valuey >> valuez;
			
			double x=atof(valuex.c_str());
			double y=atof(valuey.c_str());
			double z=atof(valuez.c_str());

			vertices_list.push_back(Vertex(x,y,z));
		}
		if (str[0] == 'f')
		{
			std::istringstream iss(str);
			iss >> f >> vt0 >> vt1 >> vt2;
			
			int vt_0=atoi(vt0.c_str());
			int vt_1=atoi(vt1.c_str());
			int vt_2=atoi(vt2.c_str());
			
			triangles_list.push_back(Triangle(vt_0-1,vt_1-1,vt_2-1));
		}
	}

} // end of loadobj

int mesh_only = 0; 

void drawTriangles()
{
	for (int i = 0; i < triangles_list.size(); i++)
	{
		if (mesh_only)
		{
			glBegin(GL_LINE_LOOP);
		}
		else
		{
			glBegin(GL_TRIANGLES);
		}

		 glColor3f(triangles_list[i].c[0].channel[0], triangles_list[i].c[0].channel[1], triangles_list[i].c[0].channel[2]);
		glVertex3d(vertices_list[triangles_list[i].v1].x, vertices_list[triangles_list[i].v1].y, vertices_list[triangles_list[i].v1].z);

		 glColor3f(triangles_list[i].c[1].channel[0], triangles_list[i].c[1].channel[1], triangles_list[i].c[1].channel[2]);
		glVertex3d(vertices_list[triangles_list[i].v2].x, vertices_list[triangles_list[i].v2].y, vertices_list[triangles_list[i].v2].z);

		 glColor3f(triangles_list[i].c[2].channel[0], triangles_list[i].c[2].channel[1], triangles_list[i].c[2].channel[2]);
        glVertex3d(vertices_list[triangles_list[i].v3].x, vertices_list[triangles_list[i].v3].y, vertices_list[triangles_list[i].v3].z);

		glEnd();
	}
} // end of draw triangles


struct Mat{
	double matrix[4][4];


	//Normalizing the matrix 
	void normalize()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == j)
					matrix[i][j] = 1.0;
				else
					matrix[i][j] = 0.0;
			}
		}
	}
	const Mat operator * (const Mat& right) const
	{
		Mat result;
       
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.matrix[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					result.matrix[i][j] += matrix[i][k] * right.matrix[k][j];
				}
			}
		}

		return result;
	}

	const Vertex operator * (const Vertex& vec) const
	{
		Vertex result;

		for (int i = 0; i < 4; i++)
		{

			result.matrix[i] = 0;
			for (int k = 0; k < 4; k++)
			{
				result.matrix[i] += matrix[i][k] * vec.matrix[k];
			}

		}

		return result;
	}

}; // end of mat

  
    double theta_x = 0.0;
    double theta_y = 0.0;
    double theta_z = 0.0;
   //double theta=0;
    double translate_x = 0.0;
    double translate_y = 0.0;
    double translate_z = 0.0;


void transformTriangles()
{

	Mat R_x, R_y, R_z;
	Mat T;
	
	R_x.normalize();
	R_y.normalize();
	R_z.normalize();
	T.normalize();

	/*rotation matrix on x axis */
	R_x.matrix[1][1] = cos(theta_x);
	R_x.matrix[1][2] = -sin(theta_x);
	R_x.matrix[2][1] = sin(theta_x);
	R_x.matrix[2][2] = cos(theta_x);


	/*rotation matrix on y axis */
	R_y.matrix[0][0] = cos(theta_y);
	R_y.matrix[0][2] = sin(theta_y);
	R_y.matrix[2][0] = -sin(theta_y);
	R_y.matrix[2][2] = cos(theta_y);

	//rotation matrix on z axis 
	//R_z.matrix[0][0] = cos(theta_z);
	//R_z.matrix[0][1] = -sin(theta_z);
	//R_z.matrix[1][0] = sin(theta_z);
	//R_z.matrix[1][1] = cos(theta_z);

	/* transaltion matrix*/
	T.matrix[0][3] = translate_x;
	T.matrix[1][3] = translate_y;
	//T.matrix[2][3]= translate_z;

    //Mat P=T * R_x * R_y * R_z;
	Mat P = T * R_x * R_y;

	for (int i = 0; i < vertices_list.size(); i++)
	{
		Vertex old_v, new_v;
		old_v.matrix[0] = vertices_list[i].x;
		old_v.matrix[1] = vertices_list[i].y;
		old_v.matrix[2] = vertices_list[i].z;
		old_v.matrix[3] = 1.0;

		//old_v.matrix[4]= {vertices_list[i].x, vertices_list[i].y, vertices_list[i].z, 1.0 };
		new_v = P * old_v;

		vertices_list[i].x = new_v.matrix[0] / new_v.matrix[3];
		vertices_list[i].y = new_v.matrix[1] / new_v.matrix[3];
		vertices_list[i].z = new_v.matrix[2] / new_v.matrix[3];
		vertices_list[i].normalize();
	}

	theta_x = 0;
	theta_y = 0;
	translate_x = 0;
	translate_y = 0;
	
}

//For the mouse control use*/
// Define data */
int mouse_down = 0; //Represent the left mouse key is clicked down
int change_mode = 0; //0 means rotation; 1 means translation
int current_x = 0, current_y = 0;
int shading_type = 0;
static GLdouble ex = 0.0, ey = 0.0, ez = 5.0, upx = 0.0, upy = 0.2, upz = 0.0, ox = 0.0, oy = 0.1, oz = 0.0;

Vertex uniformvector(Vertex v)
{
    Vertex output;
    output = v;
    float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    output.x = output.x / r;
    output.y = output.y / r;
    output.z = output.z / r;
    return output;
}

void trianglesShading()
{
    if (shading_type == 0)
    {
        for (unsigned int i = 0; i < triangles_list.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    triangles_list[i].c[j].channel[k] = 1.0;

                }
            }
        }
    }
    else if (shading_type == 1)
    {
        for (int i = 0; i < triangles_list.size(); i++)
        {
            Vertex n;
            Vertex n1, n2;
            n1 = vertices_list[triangles_list[i].v2] - vertices_list[triangles_list[i].v1];
            n2 = vertices_list[triangles_list[i].v3] - vertices_list[triangles_list[i].v1];
            n1 = uniformvector(n1);
            n2 = uniformvector(n2);
            n = n1.cross(n2);
            n = uniformvector(n);
            Vertex center;
            center = vertices_list[triangles_list[i].v2] + vertices_list[triangles_list[i].v1] + vertices_list[triangles_list[i].v3];
            center.x = center.x / 3.0;
            center.y = center.y / 3.0;
            center.z = center.z / 3.0;
            Vertex l;
            Vertex bc;
            bc.x = teddy_color.channel[0];
            bc.y = teddy_color.channel[1];
            bc.z = teddy_color.channel[2];
            l = light-center;
            Vertex c1;
            c1 = bc;
            float cosin = 0.0;
            l = uniformvector(l);

            if (l.dotProduct(n) > 0)
                cosin = l.dotProduct(n);
            c1.x = bc.x * ambient_color.channel[0] + light.c.channel[0] * bc.x * cosin;
            c1.y = bc.y * ambient_color.channel[1] + light.c.channel[1] * bc.y * cosin;
            c1.z = bc.z * ambient_color.channel[2] + light.c.channel[2] * bc.z * cosin;
            glColor3f(c1.x, c1.y, c1.z);
            glBegin(GL_TRIANGLES);
            glVertex3f(vertices_list[triangles_list[i].v1].x, vertices_list[triangles_list[i].v1].y, vertices_list[triangles_list[i].v1].z); //point 1
            glVertex3f(vertices_list[triangles_list[i].v2].x, vertices_list[triangles_list[i].v2].y, vertices_list[triangles_list[i].v2].z); //point 2
            glVertex3f(vertices_list[triangles_list[i].v3].x, vertices_list[triangles_list[i].v3].y, vertices_list[triangles_list[i].v3].z); //point 3
            glEnd();
        }
    }
    else if (shading_type == 2)
    {
        for (int i = 0; i < triangles_list.size(); i++)
        {
            Vertex n;
            Vertex n1, n2;
            Vertex v;
            Vertex r;
           //v.x = 0;
          //  v.y = 2;
            //v.z = 5;
            n1 = vertices_list[triangles_list[i].v2] - vertices_list[triangles_list[i].v1];
            n2 = vertices_list[triangles_list[i].v3] - vertices_list[triangles_list[i].v1];
            n1 = uniformvector(n1);
            n2 = uniformvector(n2);
            n = n1.cross(n2);
            n = uniformvector(n);
            Vertex center;
            center = vertices_list[triangles_list[i].v2] + vertices_list[triangles_list[i].v1] + vertices_list[triangles_list[i].v3];
            center.x = 1.0*center.x / 3.0;
            center.y = 1.0*center.y / 3.0;
            center.z = 1.0*center.z / 3.0;
            Vertex l;
           v = pos- center;
           v = uniformvector(v);
            Vertex bc;
            bc.x = teddy_color.channel[0];
            bc.y = teddy_color.channel[1];
            bc.z = teddy_color.channel[2];
            l =  light-center;
			 l = uniformvector(l);
            Vertex c1;
            c1 = bc;
            float cosin;
            cosin = n.dotProduct(l);
            r = n*cosin * 2 - l;
            float cosin1 = 0.0;
            if (l.dotProduct(n) > 0)
                cosin1 = l.dotProduct(n);
            float cosin2 = 0.0;
            if (v.dotProduct(r) > 0)
                cosin2 = v.dotProduct(r);
            cosin2 = pow(cosin2, p);
            c1.x = bc.x*ambient_color.channel[0] + light.c.channel[0] * bc.x*cosin1 + light.c.channel[0] * specular_color.channel[0] * cosin2;
            c1.y = bc.y*ambient_color.channel[1] + light.c.channel[1] * bc.y*cosin1 + light.c.channel[1] * specular_color.channel[1] * cosin2;
            c1.z = bc.z*ambient_color.channel[2] + light.c.channel[2] * bc.z*cosin1 + light.c.channel[2] * specular_color.channel[2] * cosin2;
            glColor3f(c1.x, c1.y, c1.z);
            glBegin(GL_TRIANGLES);
            glVertex3f(vertices_list[triangles_list[i].v1].x, vertices_list[triangles_list[i].v1].y, vertices_list[triangles_list[i].v1].z); //point 1
            glVertex3f(vertices_list[triangles_list[i].v2].x, vertices_list[triangles_list[i].v2].y, vertices_list[triangles_list[i].v2].z); //point 2
            glVertex3f(vertices_list[triangles_list[i].v3].x, vertices_list[triangles_list[i].v3].y, vertices_list[triangles_list[i].v3].z); //point 3
            glEnd();
        }

    }
}

void initialize()
{
	pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 5.0f;
    light.x = 0;
    light.y = 0;
    light.z = 5;

    light.c.channel[0] = 0.8;
    light.c.channel[1] = 0.8;
    light.c.channel[2] = 0.8;
	light.normalize();

    ambient_color.channel[0] = 0.3;
    ambient_color.channel[1] = 0.3;
    ambient_color.channel[2] = 0.3;

    teddy_color.channel[0] = 0.5;
    teddy_color.channel[1] = 0.5;
    teddy_color.channel[2] = 0.5;

    specular_color.channel[0] = 0.8;
    specular_color.channel[1] = 0.8;
    specular_color.channel[2] = 0.8;
}

void onDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0.5,0.5,0.5);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-0.4, 0.4,-0.4 * .48 / .64, 0.4 * .48 / .64, -100, 100);
	glOrtho(-0.4, 0.4,-0.4 * .48 / .64, 0.4 * .48 / .64, 2, 10);
	//gluLookAt(0, 0, 5, 0, 0.2, 0, 0, 1, 0);
	gluLookAt(ex, ey, ez, ox, oy, ox, upx, upy, upz);
    //ex, ey, ez - camera center position
    //ox, oy, oz - camera orientation, specify the point your k
    // eye is looking at.
    //upx, upy, upz - positive y direction


	if (mouse_down == 1) //only when the mouse is dragging, this function is called
		transformTriangles();
	
	///*if (mesh_only)
	//	{
	//		glBegin(GL_LINE_LOOP);
	//	}
	//	else
	//	{
	//		glBegin(GL_TRIANGLES);
	//	}*/
	//glBegin(GL_TRIANGLES);
	 trianglesShading();
	drawTriangles();
	//glEnd();
   glutSwapBuffers();//display the buffer

}

//****************************************************************************
//Name: onMouse
//Description: The mouse event callback
//Function calls: buttonPushed, glutPostResdisplay
//Preconditions: x,y is a valid point
//Postconditions: The mouse event is handled
//Parameters: GLint x, y, the mouse position
//				button, the mouse buttons used
//				state, the properties of the mouse
//				GLUT_UP, GLUT_DOWN, GLUT_LEFT_BUTTON
//				GLUT_right_BUTTON,
//Returns: Nothing

void onMouse(int button, int state, int x, int y)
{

	GLint specialKey = glutGetModifiers();
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			mouse_down = 1;
			current_x = x;
			current_y = y;
			if (specialKey == GLUT_ACTIVE_SHIFT)
			{
				change_mode = 1;
			}
			else
			{
				change_mode = 0;
			}
		}
		else if (state == GLUT_UP)
		{
			mouse_down = 0;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)

			break;

	default:
		break;

	}

}

void onMouseMotion(int x, int y)
{
	if (mouse_down == 1)
	{
		if (change_mode == 0)
		{
			theta_x += static_cast<float>(x - current_x) / 100.f;
			theta_y += static_cast<float>(y - current_y) / 100.f;
		}
		else{

			translate_x += static_cast<float>(x - current_x) / 100.f;
			translate_y += static_cast<float>(y - current_y) / 100.f;
		}

		current_x = x;
		current_y = y;
	}
	glutPostRedisplay();
}


void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(1);

		break;
	case 'M':
	case 'm':
		mesh_only = (mesh_only == 1) ? 0 : 1;
		printf(" m or M key is pressed");
		glutPostRedisplay();
		break;
	case 'g':
        teddy_color.channel[0] = 0.9;
        teddy_color.channel[1] = 0.7;
        teddy_color.channel[2] = 0.1;
        glutPostRedisplay();
        break;

    case '0':
        shading_type = 0;
        glutPostRedisplay();
        break;
    case '1':
        shading_type = 1;
        glutPostRedisplay();
        break;
    case '2':
        shading_type = 2;
        glutPostRedisplay();
        break;

	default:
		break;
	}
}


int main(int argc, char * argv[]) {

	/************************** Part 1: keep the same for most of your projects ***************/
    //Initialization functions
    glutInit(&argc, argv); //do the basic setup for OpenGL
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //GLUT_DOUBLE is for two buffers that make animation run  smoother
    //GLUT_RGB is to render a colored object
    //GLUT_DEPTH is to enable closer object block farther object
    
    glutInitWindowSize(640, 480); //specify the window size
    glutInitWindowPosition(WINDOW_WIDTH, WINDOW_HEIGHT / 2);
    //When the window is launched initially, where is the position on the screen
    
    glutCreateWindow("bunny demo"); //The window's title
   loadObj("bunny_low.obj", vertices_list, triangles_list);
	 initialize();
	
    /***************  Part 3: define callback functions ****************/
    glutDisplayFunc(onDisplay);  //Your job is to define "onDisplay" function
    glutKeyboardFunc(onKeyboard);//Your job is to define "onKeyboard" function
    glutMouseFunc(onMouse);
    glutMotionFunc(onMouseMotion);
    //glutReshapeFunc(onResize);
   // glutIdleFunc(onIdle); //off-str task
                         //When there are no other events or the openGL has free time, what
	
    
    /**************************** Part 4: start loop **********************/
    //Infinite Loop
    glutMainLoop(); //tell OpenGL to start a loop
    
    
    return 1;
}

