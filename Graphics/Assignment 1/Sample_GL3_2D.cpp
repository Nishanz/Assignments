#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cstring>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <FTGL/ftgl.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

using namespace std;

struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;
	GLuint TextureBuffer;
	GLuint TextureID;

	GLenum PrimitiveMode; // GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
	GLenum FillMode; // GL_FILL, GL_LINE
	int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID; // For use with normal shader
	GLuint TexMatrixID; // For use with texture shader
} Matrices;

struct FTGLFont {
	FTFont* font;
	GLuint fontMatrixID;
	GLuint fontColorID;
} GL3Font;

GLuint programID, fontProgramID, textureProgramID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	cout << "Compiling shader : " <<  vertex_file_path << endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage( max(InfoLogLength, int(1)) );
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	cout << VertexShaderErrorMessage.data() << endl;

	// Compile Fragment Shader
	cout << "Compiling shader : " << fragment_file_path << endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage( max(InfoLogLength, int(1)) );
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	cout << FragmentShaderErrorMessage.data() << endl;

	// Link the program
	cout << "Linking program" << endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	cout << ProgramErrorMessage.data() << endl;

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	cout << "Error: " << description << endl;
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

glm::vec3 getRGBfromHue (int hue)
{
	float intp;
	float fracp = modff(hue/60.0, &intp);
	float x = 1.0 - abs((float)((int)intp%2)+fracp-1.0);

	if (hue < 60)
		return glm::vec3(1,x,0);
	else if (hue < 120)
		return glm::vec3(x,1,0);
	else if (hue < 180)
		return glm::vec3(0,1,x);
	else if (hue < 240)
		return glm::vec3(0,x,1);
	else if (hue < 300)
		return glm::vec3(x,0,1);
	else
		return glm::vec3(1,0,x);
}

/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
						  0,                  // attribute 0. Vertices
						  3,                  // size (x,y,z)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
						  1,                  // attribute 1. Color
						  3,                  // size (r,g,b)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

struct VAO* create3DTexturedObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* texture_buffer_data, GLuint textureID, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;
	vao->TextureID = textureID;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->TextureBuffer));  // VBO - textures

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
						  0,                  // attribute 0. Vertices
						  3,                  // size (x,y,z)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	glBindBuffer (GL_ARRAY_BUFFER, vao->TextureBuffer); // Bind the VBO textures
	glBufferData (GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), texture_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
						  2,                  // attribute 2. Textures
						  2,                  // size (s,t)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	return vao;
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

void draw3DTexturedObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Bind Textures using texture units
	glBindTexture(GL_TEXTURE_2D, vao->TextureID);

	// Enable Vertex Attribute 2 - Texture
	glEnableVertexAttribArray(2);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->TextureBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

	// Unbind Textures to be safe
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* Create an OpenGL Texture from an image */
GLuint createTexture (const char* filename)
{
	GLuint TextureID;
	// Generate Texture Buffer
	glGenTextures(1, &TextureID);
	// All upcoming GL_TEXTURE_2D operations now have effect on our texture buffer
	glBindTexture(GL_TEXTURE_2D, TextureID);
	// Set our texture parameters
	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering (interpolation)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load image and create OpenGL texture
	int twidth, theight;
	unsigned char* image = SOIL_load_image(filename, &twidth, &theight, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D); // Generate MipMaps to use
	SOIL_free_image_data(image); // Free the data read from file after creating opengl texture
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess it up

	return TextureID;
}


/**************************
 * Customizable functions *
 **************************/

float camera_rotation_angle = 90;

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
float bird1_rot_dir = 1;
float bird2_rot_dir = 1;
float bird3_rot_dir = 1;
float canon_rot_dir = 1;

float triangle_rotation = 0;
float rectangle_rotation = 1;
float bird1_rotation = 1;
float bird2_rotation = 1;
float bird3_rotation = 1;
float canon_rotation = 1;

bool triangle_rot_status = false;
bool rectangle_rot_status = false;
bool bird1_rot_status = false;
bool bird2_rot_status = false;
bool bird3_rot_status = false;
bool canon_rot_status = false;

int actscore =0;
float power = 8 ;
float power_meter = 8 ;
bool shoot = false ;
float flying_time = -987 ;
float newx =0 ;
float newy =0 ;
double o=0;
double theta = 0 ;
double collisionx = 0;
double collisiony = 0;
double accelarationx = 0;
double accelarationy =0 ;
double gravity = 10 ;
double water_friction = 0.5;
double ground_friction = 0.2;
double ux =0 ;
double uy =0 ;
double finalx =0 ;
double finaly =0 ;
double direction =9 ;
float wintop=-4.0f,winbottom=4.0f,winleft=-4.0f,winright=4.0f;

void gen_projection(float top,float bottom,float left,float right)
{
	Matrices.projection = glm::ortho(top, bottom, left, right, 0.1f, 500.0f);
}

void zoomin()
{
	wintop*=0.8;
	winbottom*=0.8;
	winleft*=0.8;
	winright*=0.8;
	gen_projection(wintop,winbottom,winleft,winright);
}

void zoomout()
{
	wintop*=1.25;
	winbottom*=1.25;
	winleft*=1.25;
	winright*=1.25;
	gen_projection(wintop,winbottom,winleft,winright);
}

void pan_left()
{
    wintop-=0.1f;
    winbottom-=0.1f;
    gen_projection(wintop,winbottom,winleft,winright);
}

void pan_right()
{
    wintop+=0.1f;
    winbottom+=0.1f;
    gen_projection(wintop,winbottom,winleft,winright);
}
/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.

	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_C:
				rectangle_rot_status = !rectangle_rot_status;
				break;
			case GLFW_KEY_P:
				triangle_rot_status = !triangle_rot_status;
				break;
			case GLFW_KEY_X:
				// do something ..
				break;
			default:
				break;
		}
	}
	else if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			default:
				break;
		}
	}
}

/* Executed when a regular key is released */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	//cout<<key<<endl;
    switch (key) {
        case 'Q':
            quit(window);
            break;
        case 'q':
            quit(window);
            break;
        case 27:
            quit(window);
            break;
        case 'c':
            rectangle_rot_status = !rectangle_rot_status;
            break;
        case 'C':
            rectangle_rot_status = !rectangle_rot_status;
            break;
        case 'a':
            if(canon_rotation >=3)
            canon_rotation += -3;
            break;
        case 'A':
            if(canon_rotation >=3)
            canon_rotation += -3;
            break;
        case 'd':
            if(canon_rotation <=87)
            canon_rotation += 3;
            break;
        case 'D':
            if(canon_rotation <=87)
            canon_rotation += 3;
            break;
        case 'w':
            power_meter +=1;
            break;
        case 'W':
            power_meter +=1;
            break;
        case 's':
            power_meter -=1;
            break;
        case 'S':
            power_meter -=1;
            break;
        case 'm':
        case 'M':
            if(shoot == false)
            {
            shoot = true ;
            flying_time = -987;
            theta = (canon_rotation)*M_PI/180.0f ;
            power = power_meter ;
            collisionx =0 ;
            newy =0 ;
            newx =0 ;
            collisiony =0 ;
            ux = power*cos(theta);
            uy = power*(sin(theta));
            o=0;
        }
            break;
        case 56:
			zoomin();
			break;
		case 50:
			zoomout();
			break;
		case 52:
			pan_left();
			break;
		case 54:
			pan_right();
			break;
        default:
            break;
    }
}


/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE)
            	canon_rotation += -3;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {
            	canon_rotation += 3;
            }
            break;
        default:
            break;
    }
}



/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	 is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	 glLoadIdentity ();
	 gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	// Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	// Ortho projection for 2D views
	gen_projection(wintop,winbottom,winleft,winright);
}

VAO *triangle, *rectangle, *canon, *bird1, *bird2, *bird3, *myrect[100], *pig[100], *coin[100];
int i=0,rectno=0,pigno=0,coinno=0;
float xc,yc;
int flag[100],coinx[100],coiny[100];
static GLfloat vertex_buffer_data [1000] ;
static GLfloat color_buffer_data [1000] ;
static GLfloat texture_buffer_data [1000] ;


string mapchar[30];
string inp;
float vdata [] =
    {
        0,0,0,
        0,0,0,
        0,0,0,
        0,0,0,
        0,0,0,
        0,0,0,
    };

void add3(GLdouble x,GLdouble y, GLdouble z)
{
    vertex_buffer_data[i]=x;
    color_buffer_data[i++]=0.484314  ;
    vertex_buffer_data[i]=y;
    color_buffer_data[i++]=0.609804;
    vertex_buffer_data[i]=z;
    color_buffer_data[i++]=0.6098040;
}

void add(GLdouble x,GLdouble y)
{
    vertex_buffer_data[i]=x;
    color_buffer_data[i++]=0.484314  ;
    vertex_buffer_data[i]=y;
    color_buffer_data[i++]=0.609804;
    vertex_buffer_data[i]=0;
    color_buffer_data[i++]=0.6098040;
}



// Creates the triangle object used in this sample code
void create_circle(GLdouble centrex,GLdouble centrey)
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

    const double TWO_PI = 6.2831853;
    GLdouble hexTheta,x,y,radius,previousx,previousy;
    // adds point to the vertex_buffer array

    hexTheta = 0;
    radius = 0.30;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    //cout<<hexTheta<<" "<<radius<<" "<<x<<" "<<y<<endl;
    add(centrex,centrey);
    add(x,y);
    hexTheta = TWO_PI * 1/20;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    add(x,y);
    previousy = y;
    previousx = x;

    int j;
    // building many triangles to form one circle
    for(j=2;j<=20;j++)
    {
        hexTheta = TWO_PI * j/20;
        // defining the new vertices
        x = centrex + radius * cos(hexTheta);
        y = centrey + radius * sin(hexTheta);
        // assigining vertices to new triangle
        add(centrex,centrey);
        add(previousx,previousy);
        add(x,y);
        previousy = y;
        previousx = x;
    }

  // create3DObject creates and returns a handle to a VAO that can be used later
  bird1 = create3DObject(GL_TRIANGLES, 180, vertex_buffer_data, color_buffer_data, GL_FILL);
  bird2 = create3DObject(GL_TRIANGLES, 180, vertex_buffer_data, color_buffer_data, GL_FILL);
  bird3 = create3DObject(GL_TRIANGLES, 180, vertex_buffer_data, color_buffer_data, GL_LINE);
  i=0;
}

void create_coin(GLdouble centrex,GLdouble centrey)
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

    const double TWO_PI = 6.2831853;
    GLdouble hexTheta,x,y,radius,previousx,previousy;
    // adds point to the vertex_buffer array
    coinx[coinno]=centrex;
    coiny[coinno]=centrey;
    hexTheta = 0;
    radius = 0.15;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    //cout<<hexTheta<<" "<<radius<<" "<<x<<" "<<y<<endl;
    add(centrex,centrey);
    add(x,y);
    hexTheta = TWO_PI * 1/20;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    add(x,y);
    previousy = y;
    previousx = x;

    int j;
    // building many triangles to form one circle
    for(j=2;j<=20;j++)
    {
        hexTheta = TWO_PI * j/20;
        // defining the new vertices
        x = centrex + radius * cos(hexTheta);
        y = centrey + radius * sin(hexTheta);
        // assigining vertices to new triangle
        add(centrex,centrey);
        add(previousx,previousy);
        add(x,y);
        previousy = y;
        previousx = x;
    }
  flag[coinno]=1;
  // create3DObject creates and returns a handle to a VAO that can be used later
  coin[coinno++] = create3DObject(GL_TRIANGLES, 180, vertex_buffer_data, color_buffer_data, GL_FILL);
  i=0;
}

void create_pig(GLdouble centrex,GLdouble centrey)
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

    const double TWO_PI = 6.2831853;
    GLdouble hexTheta,x,y,radius,previousx,previousy;
    // adds point to the vertex_buffer array

    hexTheta = 0;
    radius = 0.30;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    //cout<<hexTheta<<" "<<radius<<" "<<x<<" "<<y<<endl;
    add(centrex,centrey);
    add(x,y);
    hexTheta = TWO_PI * 1/20;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    add(x,y);
    previousy = y;
    previousx = x;

    int j;
    // building many triangles to form one circle
    for(j=2;j<=20;j++)
    {
        hexTheta = TWO_PI * j/20;
        // defining the new vertices
        x = centrex + radius * cos(hexTheta);
        y = centrey + radius * sin(hexTheta);
        // assigining vertices to new triangle
        add(centrex,centrey);
        add(previousx,previousy);
        add(x,y);
        previousy = y;
        previousx = x;
    }

  // create3DObject creates and returns a handle to a VAO that can be used later
  pig[pigno++] = create3DObject(GL_TRIANGLES, 180, vertex_buffer_data, color_buffer_data, GL_FILL);
  i=0;
}

void createcanon (GLdouble centrex,GLdouble centrey)
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */
    const double TWO_PI = 6.2831853;
    GLdouble hexTheta,x,y,radius,previousx,previousy;
    // adds point to the vertex_buffer array

    hexTheta = 0;
    radius = 1.00f;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    add(0,0);
    add(x,y);
    hexTheta = TWO_PI * 1/10;
    x = centrex + radius * cos(hexTheta);
    y = centrey + radius * sin(hexTheta);
    add(x,y);
    previousy = y;
    previousx = x;

    
    // building many triangles to form one circle
    for(int j=2;j<=10;j++)
    {
        hexTheta = TWO_PI * j/10;
        // defining the new vertices
        x = centrex + radius * cos(hexTheta);
        y = centrey + radius * sin(hexTheta);
        // assigining vertices to new triangle
        add(0,0);
        add(previousx,previousy);
        add(x,y);
        previousy = y;
        previousx = x;
    }

  // create3DObject creates and returns a handle to a VAO that can be used later
  canon = create3DObject(GL_TRIANGLES, 90, vertex_buffer_data, color_buffer_data, GL_LINE);
  i=0;
}

// Creates the triangle object used in this sample code
void createTriangle ()
{
	/* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

	/* Define vertex array as used in glBegin (GL_TRIANGLES) */
	static const GLfloat vertex_buffer_data [] = {
		0, 1,0, // vertex 0
		-1,-1,0, // vertex 1
		1,-1,0, // vertex 2
	};

	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 0
		0,1,0, // color 1
		0,0,1, // color 2
	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

// Creates the rectangle object used in this sample code
void createRectangle (GLuint textureID)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data [] = {
		-4,-4,0, // vertex 1
		4,-4,0, // vertex 2
		4, 4,0, // vertex 3

		4, 4,0, // vertex 3
		-4, 4,0, // vertex 4
		-4,-4,0  // vertex 1
	};

	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 1
		0,0,1, // color 2
		0,1,0, // color 3

		0,1,0, // color 3
		0.3,0.3,0.3, // color 4
		1,0,0  // color 1
	};

	// Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
	static const GLfloat texture_buffer_data [] = {
		0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		1,0, // TexCoord 3 - top right

		1,0, // TexCoord 3 - top right
		0,0, // TexCoord 4 - top left
		0,1  // TexCoord 1 - bot left
	};

	// create3DTexturedObject creates and returns a handle to a VAO that can be used later
	rectangle = create3DTexturedObject(GL_TRIANGLES, 6, vertex_buffer_data, texture_buffer_data, textureID, GL_FILL);
}

void accelaration_func(){

    accelarationx = 0;
    accelarationy = gravity ;
  //   need to include code of water and ground friction

}
#include<sstream>
    template <typename T>
    std::string to_string(T value)
    {
      //create an output string stream
      std::ostringstream os ;

      //throw the value into the string stream
      os << value ;

      //convert the string stream into a string and return
      return os.str() ;
    }

void move_func(){
      if(shoot == false)
      return ;

      newx = ux*o - accelarationx*o*o/2;
      newy = uy*o - accelarationy*o*o/2;

      if(newx>10.0f){shoot=false;}
      if(newy>10.0f){shoot=false;}
      if(newx<-10.0f){shoot=false;}
      if(newy<-10.0f){shoot=false;}


}

void calc_score()
{
	int num=0;
	for(int i=0;i<coinno;i++)if(flag[i])num+=1;
	actscore = (coinno-num)*100;
}

void collision_func(){

  //   condition of stopping the ball
    if(ux < 0.09 && uy < 0.09)
    {
        shoot=false;
    }
    else if(uy < 0.09){
        uy =0 ;
    }
    else if(ux < 0.09){
        ux =0 ;
    }
    // REVIEW
  //   condition of collision with ground
    if((collisiony + newy ) <= 0.05f)
    {
        if(uy < 0){
            uy -= gravity*o;
            uy *= -1 ;
            collisiony = 0 ;
            newy =0.05 ;
        }
        ux /=1.5;
        uy /=1.5 ;
        collisionx +=newx;
        collisiony += newy;
        // newy = 0.005f;
        // newx = 0 ;
        o=0;
    }
    if((collisiony + newy ) >= 4.9 && (collisiony + newy ) <= 5.25  && (collisionx + newx -3) >= -1.75 && (collisionx + newx -3) <= -0.45) {
        uy -= gravity*o;
        uy /=4 ;
        uy *= -1 ;
        collisiony += newy - 0.005f ;
        collisionx += newx ;
        // direction = -9 ;
        // newy = -1.5f;
        o=0;

    }
  //   condition of initalising the shooting control
      if(ux<=0.09 && uy<=0.09)
      {
          shoot=false;
      }
}

void collect_coin()
{
	for(int i=0;i<coinno;i++)
	{
		if((0.5*0.5)>=((-3.00f + collisiony+newy-coiny[i]+0.2f)*(-3.00f + collisiony+newy-coiny[i]+0.2f)+(-3.00f + collisionx+newx-coinx[i]-0.6f)*(-3.00f + collisionx+newx-coinx[i]-0.6f)))flag[i]=0;
		  //cout<<"Coins are at: "<<coinx[i]+0.6f<<" "<<coiny[i]-0.2f<<endl;
	}
}

void create_rectangle_coordinates(float y1, float x1, float y3, float x3)
{
    float x2,y2,x4,y4;
    x2=x1;
    y2=y3;
    x4=x3;
    y4=y1;
    x1/=10; y1/=3.76;
    x2/=10; y2/=3.76;
    x3/=10; y3/=3.76;
    x4/=10; y4/=3.76;
    x1-=4; y1-=4;
    x2-=4; y2-=4;
    x3-=4; y3-=4;
    x4-=4; y4-=4;
    float data[18] =
    {
        x1,y1,0,
        x2,y2,0,
        x3,y3,0,
        x3,y3,0,
        x4,y4,0,
        x1,y1,0,
    };


    for(int j=0;j<18;j+=3)
    {
        //cout<<data[j]<<" "<<data[j+1]<<" "<<data[j+2]<<endl;
        add3(GLdouble(data[j]),GLdouble(data[j+1]),GLdouble(data[j+2]));
    }
    myrect[rectno++] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
    i=0;
}

void parse_line()
{
    
    int state=0;
    int x1,x2,y1,y2;
    for(int j=0;j<30;j++)
    {
        //cout<<mapchar[j]<<endl;
    for(int i=0;i<80;i++)
    {
        if(mapchar[j][i]=='X'&&state==0)
        {
            state=1;
            x1=i;
            y1=30-j;
        }
        if((mapchar[j][i]==' '&&state==1) || (i==80-1&&state==1))
        {
            state=0;
            x2=i;
            y2=30-(j+1);
            //cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl; 
            create_rectangle_coordinates(y1,x1,y2,x2);
        }
        if(mapchar[j][i]=='P')
        {
            xc=i/10;
            xc-=4;
            yc=(30-j)/3.76;
            yc-=4;
            //cout<<xc<<" "<<yc<<endl;
            create_pig(xc,yc);
        }
        if(mapchar[j][i]=='C')
        {
            xc=i/10;
            xc-=4;
            yc=(30-j)/3.76;
            yc-=4;
            //cout<<xc<<" "<<yc<<endl;
            create_coin(xc,yc);
        }
    }
    }
}

int readfile()
{
    ifstream myfile;
    myfile.open("map.txt");
    int i=0;
    while(getline (myfile,inp))
    {
        mapchar[i]=inp;
        i++;
    }
    //for(int i=0;i<30;i++)cout<<mapchar[i]<<endl;
    parse_line();
    
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
	// clear the color and depth in the frame buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	// Don't change unless you know what you are doing
	glUseProgram (programID);

	// Eye - Location of camera. Don't change unless you are sure!!
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (0, 1, 0);
    Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
    glm::mat4 VP = Matrices.projection * Matrices.view;
    glm::mat4 MVP;	// MVP = Projection * View * Model

	// Compute Camera matrix (view)
	// Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	//  Don't change unless you are sure!!
	//static float c = 0;
	//c++;
	//Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(sinf(c*M_PI/180.0),3*cosf(c*M_PI/180.0),0)); // Fixed camera for 2D (ortho) in XY plane

	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	//glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	//glm::mat4 MVP;	// MVP = Projection * View * Model

	// Load identity to model matrix
	// Render with texture shaders now
	glUseProgram(textureProgramID);

	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	//Matrices.model = glm::mat4(1.0f);
	//glm::mat4 translateRectangle = glm::translate (glm::vec3(2, 0, 0));        // glTranslatef
	//glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	//Matrices.model *= (translateRectangle * rotateRectangle);
	//MVP = VP * Matrices.model;

	// Copy MVP to texture shaders
	//glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Set the texture sampler to access Texture0 memory
	//glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

	// draw3DObject draws the VAO given to it using current MVP matrix
	//draw3DTexturedObject(rectangle);
  // Render with texture shaders now
	Matrices.model = glm::mat4(1.0f);
	MVP = VP * Matrices.model;

	// Copy MVP to texture shaders
	glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Set the texture sampler to access Texture0 memory
	glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DTexturedObject(rectangle);

	Matrices.model = glm::mat4(1.0f);

	/* Render your scene */
	glm::mat4 translateTriangle = glm::translate (glm::vec3(-2.0f, 0.0f, 0.0f)); // glTranslatef
	glm::mat4 rotateTriangle = glm::rotate((float)(triangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
	glm::mat4 triangleTransform = translateTriangle * rotateTriangle;
	Matrices.model *= triangleTransform;
	MVP = VP * Matrices.model; // MVP = p * V * M

	//  Don't change unless you are sure!!
	// Copy MVP to normal shaders
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	//draw3DObject(triangle);
	
	Matrices.model = glm::mat4(1.0f);


  glm::mat4 translatebird2 = glm::translate (glm::vec3(-3.0f, -3.0f, 0.0f)); // glTranslatef
  glm::mat4 rotatebird2 = glm::rotate((float)(bird2_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0) , vec3 decides the axis about which it have to be rotated
  glm::mat4 bird2Transform = translatebird2 ;
  Matrices.model *= translatebird2 * rotatebird2;
  MVP = VP * Matrices.model; // MVP = p * V * M

  //  Don't change unless you are sure!!
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(bird2);

    // bird3

  Matrices.model = glm::mat4(1.0f);

  o += 0.009;
  accelaration_func();
  move_func();
  collision_func();
  collect_coin();
  calc_score();
  glm::mat4 rotatebird3 = glm::rotate((float)(bird3_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0) , vec3 decides the axis about which it have to be rotated
  glm::mat4 translatebird3 = glm::translate (glm::vec3(-3.00f + collisionx+newx , -3.00f + collisiony+newy , 0.0f)); // glTranslatef
  //cout<<"Co-ordinates are: "<<-3.00f + collisionx+newx <<" "<<-3.00f + collisiony+newy<<endl;
  glm::mat4 bird3Transform = translatebird3 ;
  Matrices.model *= translatebird3 * rotatebird3;
  MVP = VP * Matrices.model; // MVP = p * V * M

  //  Don't change unless you are sure!!
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(bird3);

  // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
  // glPopMatrix ();

  // draw3DObject draws the VAO given to it using current MVP matrix
  //draw3DObject(rectangle);

  //map
  for(int i=0;i<rectno;i++)
{
      Matrices.model = glm::mat4(1.0f);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  draw3DObject(myrect[i]);
}

//coin
  for(int i=0;i<coinno;i++)
{
  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translatecoin = glm::translate(glm::vec3(0.6f, -0.15f, 0.0f)); // glTranslatef
  Matrices.model *= translatecoin;
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  if(flag[i])draw3DObject(coin[i]);
}

  // pig

for(int i=0;i<pigno;i++)
{
      Matrices.model = glm::mat4(1.0f);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  draw3DObject(pig[i]);
}
  // canon
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translatecanon = glm::translate (glm::vec3(-3,-3, 0));        // glTranslatef

  glm::mat4 rotatecanon = glm::rotate((float)((canon_rotation)*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (0,0,1)
  Matrices.model *= (translatecanon * rotatecanon);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(canon);



	// Increment angles
	float increments = 1;

	// Render font on screen
	static int fontScale = 0;
	float fontScaleValue = 0.75 + 0.25*sinf(fontScale*M_PI/180.0f);
	glm::vec3 fontColor = getRGBfromHue (fontScale);



	// Use font Shaders for next part of code
	glUseProgram(fontProgramID);
	Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

	// Transform the text
	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translateText = glm::translate(glm::vec3(-3,3,0));
	//glm::mat4 scaleText = glm::scale(glm::vec3(fontScaleValue,fontScaleValue,fontScaleValue));
	Matrices.model *= (translateText );
	MVP = Matrices.projection * Matrices.view * Matrices.model;
	// send font's MVP and font color to fond shaders
	glUniformMatrix4fv(GL3Font.fontMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(GL3Font.fontColorID, 1, &fontColor[0]);
	
	// Render font
	if(actscore==0)
		GL3Font.font->Render("Score: 0");
	if(actscore==100)
		GL3Font.font->Render("Score: 100");
	if(actscore==200)
		GL3Font.font->Render("Score: 200");
	if(actscore==300)
		GL3Font.font->Render("Score: 300");
	if(actscore==400)
		GL3Font.font->Render("Score: 400");
	if(actscore==500)
		GL3Font.font->Render("Score: 500");
	if(actscore==600)
		GL3Font.font->Render("Score: 600");
	if(actscore==700)
		GL3Font.font->Render("Score: 700");
	if(actscore==800)
		GL3Font.font->Render("Score: 800");
	if(actscore==900)
		GL3Font.font->Render("Score: 900");
	if(actscore==1000)
		GL3Font.font->Render("Score: 1000");

	//camera_rotation_angle++; // Simulating camera rotation
  bird1_rotation = bird1_rotation + increments*bird1_rot_dir*bird1_rot_status;
  bird2_rotation = bird2_rotation + increments*bird2_rot_dir*bird2_rot_status;
  bird3_rotation = bird3_rotation + increments*bird3_rot_dir*bird3_rot_status;
  rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;
  canon_rotation = canon_rotation + (increments/5)*canon_rot_dir*canon_rot_status*-1;
  triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
  rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;

	// font size and color changes
	fontScale = (fontScale + 1) % 360;
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	 is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

	return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	// Load Textures
	 // pointed at -3   .5,-3
	for(int i=0;i<100;i++)flag[i]=0;
	// Enable Texture0 as current texture memory
	glActiveTexture(GL_TEXTURE0);
	// load an image file directly as a new OpenGL texture
	// GLuint texID = SOIL_load_OGL_texture ("beach.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS); // Buggy for OpenGL3
	GLuint textureID = createTexture("background.png");
	// check for an error during the load process
	if(textureID == 0 )
		cout << "SOIL loading error: '" << SOIL_last_result() << "'" << endl;

	// Create and compile our GLSL program from the texture shaders
	textureProgramID = LoadShaders( "TextureRender.vert", "TextureRender.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.TexMatrixID = glGetUniformLocation(textureProgramID, "MVP");


	/* Objects should be created before any other gl function and shaders */
	// Create the models
	createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
	createRectangle(textureID);
	readfile();
	create_circle(0,0);
	createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
	//createRectangle ();
    createcanon (2,0);
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL3.vert", "Sample_GL3.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialise FTGL stuff
	const char* fontfile = "arial.ttf";
	GL3Font.font = new FTExtrudeFont(fontfile); // 3D extrude style rendering

	if(GL3Font.font->Error())
	{
		cout << "Error: Could not load font `" << fontfile << "'" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Create and compile our GLSL program from the font shaders
	fontProgramID = LoadShaders( "fontrender.vert", "fontrender.frag" );
	GLint fontVertexCoordAttrib, fontVertexNormalAttrib, fontVertexOffsetUniform;
	fontVertexCoordAttrib = glGetAttribLocation(fontProgramID, "vertexPosition");
	fontVertexNormalAttrib = glGetAttribLocation(fontProgramID, "vertexNormal");
	fontVertexOffsetUniform = glGetUniformLocation(fontProgramID, "pen");
	GL3Font.fontMatrixID = glGetUniformLocation(fontProgramID, "MVP");
	GL3Font.fontColorID = glGetUniformLocation(fontProgramID, "fontColor");

	GL3Font.font->ShaderLocations(fontVertexCoordAttrib, fontVertexNormalAttrib, fontVertexOffsetUniform);
	GL3Font.font->FaceSize(1);
	GL3Font.font->Depth(0);
	GL3Font.font->Outset(0, 0);
	GL3Font.font->CharMap(ft_encoding_unicode);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

	double last_update_time = glfwGetTime(), current_time;

	/* Draw in loop */
	while (!glfwWindowShouldClose(window)) {

		// OpenGL Draw commands
		draw();

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();

		// Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			last_update_time = current_time;
		}
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
