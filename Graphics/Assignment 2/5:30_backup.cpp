#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

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
						  0,				  // attribute 0. Vertices
						  3,				  // size (x,y,z)
						  GL_FLOAT,		   // type
						  GL_FALSE,		   // normalized?
						  0,				  // stride
						  (void*)0			// array buffer offset
						  );

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
						  1,				  // attribute 1. Color
						  3,				  // size (r,g,b)
						  GL_FLOAT,		   // type
						  GL_FALSE,		   // normalized?
						  0,				  // stride
						  (void*)0			// array buffer offset
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
						  0,				  // attribute 0. Vertices
						  3,				  // size (x,y,z)
						  GL_FLOAT,		   // type
						  GL_FALSE,		   // normalized?
						  0,				  // stride
						  (void*)0			// array buffer offset
						  );

	glBindBuffer (GL_ARRAY_BUFFER, vao->TextureBuffer); // Bind the VBO textures
	glBufferData (GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), texture_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
						  2,				  // attribute 2. Textures
						  2,				  // size (s,t)
						  GL_FLOAT,		   // type
						  GL_FALSE,		   // normalized?
						  0,				  // stride
						  (void*)0			// array buffer offset
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

void reshapeWindow(GLFWwindow*, int, int);
/**************************
 * Customizable functions *
 **************************/
#define rep(i,n) for(int i=0;i<n;i++)
string textureFiles[100];
GLuint texture[100];
int height_var[10][10];
int numTextures=0;
float zlevel[10][10],zspeed[10][10],curr_z[10][10];
int is_increase[10][10];
string inp_board[10];
float panx=0,panz=0,pany=0;
float eyefrom[4] = {0,1.2f,1.4f,1.2f} ;
float targetto[4] = {0,0,0,0};
float botpos[4] = {0,-0.97f,1.1f,-0.97f};
float triangle_rot_dir = 0;
float rectangle_rot_dir = 0;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
float rectangle_rotation = 180;
int xpos_player=9,ypos_player=9;
int is_obstacle[10][10];
float camfrom[4] ;
float camlook[4] ;
float posz=0;
float posx=0;
float mouposx;
int campos=0;
float mouposy ;
float mousez;
double lastx = 0;
double lasty =0 ;
double theta;
double phi=0;
double zoom =0 ;
int lives,health;
int no_cam=4;

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

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
        case 'Q':
        case 'q':
        case 27: //ESC
            exit (0);
        case 'a':
        case 'A':
        if(xpos_player<9&&is_obstacle[xpos_player+1][ypos_player]==0&&inp_board[xpos_player+1][ypos_player]=='L')
            xpos_player+=1;
        break;
        case 'd':
        case 'D':
            if(xpos_player>0&&is_obstacle[xpos_player-1][ypos_player]==0&&inp_board[xpos_player-1][ypos_player]=='L')
            xpos_player-=1;
        break;
        case 'w':
        case 'W':
            if(ypos_player>0&&is_obstacle[xpos_player][ypos_player-1]==0&&inp_board[xpos_player][ypos_player-1]=='L')
            ypos_player-=1;
        break;
        case 's':
        case 'S':
        	if(ypos_player<9&&is_obstacle[xpos_player][ypos_player+1]==0&&inp_board[xpos_player][ypos_player+1]=='L')
            	ypos_player+=1;
        break;
        case 'c':
        case 'C':
        	campos++;
            campos=campos%no_cam;
        break;
        case 'i':
        case 'I':
            if(zoom>=-300)
            	{
                	zoom-= 20;
                	reshapeWindow(window,1920,1080);
            	}
            	cout<<zoom<<endl;
        break;
        case 'o':
        case 'O':
            if(zoom<=300)
            	{
                	zoom+=20;
                	reshapeWindow(window,1920,1080);
            	}	
            	cout<<zoom<<endl;
        case 56:
			panx-=0.1f;
			break;
		case 50:
			panx+=0.1f;
			break;
		case 52:
			panz+=0.1f;
			break;
		case 57:
			pany-=0.1f;
			break;
		case 55:
			pany+=0.1f;
			break;
		case 54:
			panz-=0.1f;
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
            	break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (action == GLFW_RELEASE)
			break;
		default:
			break;
	}
}

/* Executed when the mouse moves to position ('x', 'y') */
void mouseMotion (GLFWwindow* window, double x, double y)
{
                // cout<<x<<" "<<y<<endl;
    theta += (lastx-x) / 100.0;
   phi += (lasty-y) / 50.0;
   lastx = x/1000;
   lasty = y/1000;
   mouposx = x ;
    mouposx /=1000;
    mouposy = y;
    mouposy /=1000;

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
	 Matrices.projection = glm::perspective (fov, (GLfloat)(width - zoom) / (GLfloat)(height +zoom), 0.1f, 500.0f);

	// Ortho projection for 2D views
	//Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *triangle, *rectangle, *cube, *board[10][10], *water[10][10], *player, *obstacle[10][10];

// Creates the triangle object used in this sample code
void createTriangle (GLuint texture)
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

	static const GLfloat texture_buffer_data [] = {
		0,1, // TexCoord 1 - bot left
		1,1, // TexCoord 2 - bot right
		0.5,0, // TexCoord 3 - top right
	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	GLuint thistexture = texture;
	triangle = create3DTexturedObject(GL_TRIANGLES, 3, vertex_buffer_data, texture_buffer_data, thistexture, GL_FILL);
}

void createBrick(GLuint textureID)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data[] = {
	 -1.0f,-1.0f,-1.0f, // triangle 1 : begin
	 -1.0f,-1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f, // triangle 1 : end
	 1.0f, 1.0f,-1.0f, // triangle 2 : begin
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, // triangle 2 : end
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f
 };

	// Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
	static const GLfloat g_uv_buffer_data[] = {
	 0.000059f, 1.0f-0.000004f,
	 0.000103f, 1.0f-0.336048f,
	 0.335973f, 1.0f-0.335903f,
	 1.000023f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.336024f, 1.0f-0.671877f,
	 0.667969f, 1.0f-0.671889f,
	 1.000023f, 1.0f-0.000013f,
	 0.668104f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.000059f, 1.0f-0.000004f,
	 0.335973f, 1.0f-0.335903f,
	 0.336098f, 1.0f-0.000071f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.336024f, 1.0f-0.671877f,
	 1.000004f, 1.0f-0.671847f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.668104f, 1.0f-0.000013f,
	 0.335973f, 1.0f-0.335903f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.668104f, 1.0f-0.000013f,
	 0.336098f, 1.0f-0.000071f,
	 0.000103f, 1.0f-0.336048f,
	 0.000004f, 1.0f-0.671870f,
	 0.336024f, 1.0f-0.671877f,
	 0.000103f, 1.0f-0.336048f,
	 0.336024f, 1.0f-0.671877f,
	 0.335973f, 1.0f-0.335903f,
	 0.667969f, 1.0f-0.671889f,
	 1.000004f, 1.0f-0.671847f,
	 0.667979f, 1.0f-0.335851f
 };

	// create3DTexturedObject creates and returns a handle to a VAO that can be used later
 	
	rep(i,10)rep(j,10)board[i][j] = create3DTexturedObject(GL_TRIANGLES, 36, vertex_buffer_data, g_uv_buffer_data, textureID, GL_FILL);
}

void createWater(GLuint textureID)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data[] = {
	 -1.0f,-1.0f,-1.0f, // triangle 1 : begin
	 -1.0f,-1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f, // triangle 1 : end
	 1.0f, 1.0f,-1.0f, // triangle 2 : begin
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, // triangle 2 : end
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f
 };

	// Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
	static const GLfloat g_uv_buffer_data[] = {
	 0.000059f, 1.0f-0.000004f,
	 0.000103f, 1.0f-0.336048f,
	 0.335973f, 1.0f-0.335903f,
	 1.000023f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.336024f, 1.0f-0.671877f,
	 0.667969f, 1.0f-0.671889f,
	 1.000023f, 1.0f-0.000013f,
	 0.668104f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.000059f, 1.0f-0.000004f,
	 0.335973f, 1.0f-0.335903f,
	 0.336098f, 1.0f-0.000071f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.336024f, 1.0f-0.671877f,
	 1.000004f, 1.0f-0.671847f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.668104f, 1.0f-0.000013f,
	 0.335973f, 1.0f-0.335903f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.668104f, 1.0f-0.000013f,
	 0.336098f, 1.0f-0.000071f,
	 0.000103f, 1.0f-0.336048f,
	 0.000004f, 1.0f-0.671870f,
	 0.336024f, 1.0f-0.671877f,
	 0.000103f, 1.0f-0.336048f,
	 0.336024f, 1.0f-0.671877f,
	 0.335973f, 1.0f-0.335903f,
	 0.667969f, 1.0f-0.671889f,
	 1.000004f, 1.0f-0.671847f,
	 0.667979f, 1.0f-0.335851f
 };

	// create3DTexturedObject creates and returns a handle to a VAO that can be used later
 	cube = create3DTexturedObject(GL_TRIANGLES, 36, vertex_buffer_data, g_uv_buffer_data, textureID, GL_FILL);
	rep(i,10)rep(j,10)water[i][j] = create3DTexturedObject(GL_TRIANGLES, 36, vertex_buffer_data, g_uv_buffer_data, textureID, GL_FILL);
}

void createPlayer(GLuint textureID)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data[] = {
	 -1.0f,-1.0f,-1.0f, // triangle 1 : begin
	 -1.0f,-1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f, // triangle 1 : end
	 1.0f, 1.0f,-1.0f, // triangle 2 : begin
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, // triangle 2 : end
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f
 };

	// Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
	static const GLfloat g_uv_buffer_data[] = {
	 0.000059f, 1.0f-0.000004f,
	 0.000103f, 1.0f-0.336048f,
	 0.335973f, 1.0f-0.335903f,
	 1.000023f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.336024f, 1.0f-0.671877f,
	 0.667969f, 1.0f-0.671889f,
	 1.000023f, 1.0f-0.000013f,
	 0.668104f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.000059f, 1.0f-0.000004f,
	 0.335973f, 1.0f-0.335903f,
	 0.336098f, 1.0f-0.000071f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.336024f, 1.0f-0.671877f,
	 1.000004f, 1.0f-0.671847f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.668104f, 1.0f-0.000013f,
	 0.335973f, 1.0f-0.335903f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.668104f, 1.0f-0.000013f,
	 0.336098f, 1.0f-0.000071f,
	 0.000103f, 1.0f-0.336048f,
	 0.000004f, 1.0f-0.671870f,
	 0.336024f, 1.0f-0.671877f,
	 0.000103f, 1.0f-0.336048f,
	 0.336024f, 1.0f-0.671877f,
	 0.335973f, 1.0f-0.335903f,
	 0.667969f, 1.0f-0.671889f,
	 1.000004f, 1.0f-0.671847f,
	 0.667979f, 1.0f-0.335851f
 };

	// create3DTexturedObject creates and returns a handle to a VAO that can be used later
 	player = create3DTexturedObject(GL_TRIANGLES, 36, vertex_buffer_data, g_uv_buffer_data, textureID, GL_FILL);
}

void createObstacle(GLuint textureID)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data[] = {
	 -1.0f,-1.0f,-1.0f, // triangle 1 : begin
	 -1.0f,-1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f, // triangle 1 : end
	 1.0f, 1.0f,-1.0f, // triangle 2 : begin
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, // triangle 2 : end
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f,-1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f,
	 1.0f,-1.0f, 1.0f
 };

	// Texture coordinates start with (0,0) at top left of the image to (1,1) at bot right
	static const GLfloat g_uv_buffer_data[] = {
	 0.000059f, 1.0f-0.000004f,
	 0.000103f, 1.0f-0.336048f,
	 0.335973f, 1.0f-0.335903f,
	 1.000023f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.336024f, 1.0f-0.671877f,
	 0.667969f, 1.0f-0.671889f,
	 1.000023f, 1.0f-0.000013f,
	 0.668104f, 1.0f-0.000013f,
	 0.667979f, 1.0f-0.335851f,
	 0.000059f, 1.0f-0.000004f,
	 0.335973f, 1.0f-0.335903f,
	 0.336098f, 1.0f-0.000071f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.336024f, 1.0f-0.671877f,
	 1.000004f, 1.0f-0.671847f,
	 0.999958f, 1.0f-0.336064f,
	 0.667979f, 1.0f-0.335851f,
	 0.668104f, 1.0f-0.000013f,
	 0.335973f, 1.0f-0.335903f,
	 0.667979f, 1.0f-0.335851f,
	 0.335973f, 1.0f-0.335903f,
	 0.668104f, 1.0f-0.000013f,
	 0.336098f, 1.0f-0.000071f,
	 0.000103f, 1.0f-0.336048f,
	 0.000004f, 1.0f-0.671870f,
	 0.336024f, 1.0f-0.671877f,
	 0.000103f, 1.0f-0.336048f,
	 0.336024f, 1.0f-0.671877f,
	 0.335973f, 1.0f-0.335903f,
	 0.667969f, 1.0f-0.671889f,
	 1.000004f, 1.0f-0.671847f,
	 0.667979f, 1.0f-0.335851f
 };

	// create3DTexturedObject creates and returns a handle to a VAO that can be used later
 	rep(i,10)rep(j,10)obstacle[i][j]=create3DTexturedObject(GL_TRIANGLES, 36, vertex_buffer_data, g_uv_buffer_data, textureID, GL_FILL);
}
// Creates the rectangle object used in this sample code
void createRectangle (GLuint textureID)
{
	// GL3 accepts only Triangles. Quads are not supported
	static const GLfloat vertex_buffer_data [] = {
		-1.2,-1,0, // vertex 1
		1.2,-1,0, // vertex 2
		1.2, 1,0, // vertex 3

		1.2, 1,0, // vertex 3
		-1.2, 1,0, // vertex 4
		-1.2,-1,0  // vertex 1
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

void cameraposition(){
    if(campos==0)
    {
      // top view
        camfrom[1]= eyefrom[1]+-0.3+panx ;
        camfrom[2]= eyefrom[2]+-1.2+pany;
        camfrom[3] = eyefrom[3]+5.2+panz ;
        camlook[1]= targetto[1];
        camlook[2]= targetto[2];
        camlook[3] = targetto[3];
    }

    else if(campos==1)
    {
      // Adventure view
        camfrom[1]= botpos[1] +panx;
        camfrom[2]= botpos[2] + 0.3f + pany;
        camfrom[3] = botpos[3] + 0.2f + panz;
        camlook[1]= botpos[1] ;
        camlook[2]= botpos[2] ;
        camlook[3] = botpos[3] - 0.7f ;
        rectangle_rotation = theta;
    }
    else if(campos==2)
    {
      //   bot's head
        camfrom[1]= botpos[1]+posx ;
        camfrom[2]= botpos[2]+0.2f;
        camfrom[3] = botpos[3] + posz + 0.4f;
        camlook[1]= botpos[1]+posx ;
        camlook[2]= botpos[2];
        camlook[3] = botpos[3] + posz ;
        rectangle_rotation = theta;
    }
    else if(campos==3)
    {
      // tower view
        camfrom[1]= eyefrom[1]+-0.3-0.6-0.4+panx ;
        camfrom[2]= eyefrom[2]+-1.2-5.3+pany;
        camfrom[3] = eyefrom[3]+5.2-3.1+panz ;
        camlook[1]= targetto[1];
        camlook[2]= targetto[2];
        camlook[3] = targetto[3];
    }

}

float camera_rotation_angle = 0;
float triangle_rotation = 0;
long long int count =0;
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
	static float c = 0;
	//c++;
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    cameraposition();

	// Compute Camera matrix (view)
	 Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
	//  Don't change unless you are sure!!
	//Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(sinf(c*M_PI/180.0),3*cosf(c*M_PI/180.0),-10)); // Fixed camera for 2D (ortho) in XY plane
	//Matrices.view = glm::lookAt(glm::vec3(eyefrom[1]+panx,eyefrom[2]+pany,eyefrom[3]+panz), glm::vec3(targetto[1],targetto[2],targetto[3]), glm::vec3(tilt[0],tilt[1],tilt[2])); // Fixed camera for 2D (ortho) in XY plane
	 Matrices.view = glm::lookAt(glm::vec3(camfrom[1],camfrom[2],camfrom[3]), glm::vec3(targetto[1]+0*mouposx,targetto[2],targetto[3]+0*mouposy), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
	 //cout<<mouposx<<" "<<mouposy<<endl;
	// Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
	//  Don't change unless you are sure!!
	glm::mat4 VP = Matrices.projection * Matrices.view;

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// For each model you render, since the MVP will be different (at least the M part)
	//  Don't change unless you are sure!!
	glm::mat4 MVP;	// MVP = Projection * View * Model


	// Render with texture shaders now
	glUseProgram(textureProgramID);

	// Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
	// glPopMatrix ();
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateRectangle = glm::translate (glm::vec3(2, 0, 0));		// glTranslatef
	glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translateRectangle * rotateRectangle);
	MVP = VP * Matrices.model;

	// Copy MVP to texture shaders
	glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Set the texture sampler to access Texture0 memory
	glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

	// draw3DObject draws the VAO given to it using current MVP matrix
	//draw3DTexturedObject(rectangle);

	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translateCube = glm::translate (glm::vec3(-3, -3, 0));		// glTranslatef
	glm::mat4 rotateCube = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	glm::mat4 scaleCube = glm::scale(glm::vec3(0.5,0.5,0.75));

	Matrices.model *= (translateCube * rotateCube *scaleCube);
	MVP = VP * Matrices.model;

	// Copy MVP to texture shaders
	glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Set the texture sampler to access Texture0 memory
	glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

	// draw3DObject draws the VAO given to it using current MVP matrix
	//draw3DTexturedObject(cube);
	if(count>10000000000000000)count=0;
	//map
	for(int i=0;i<10;i++)
	{
  		for(int j=0;j<10;j++)
  		{
  			if(inp_board[i][j]=='L')
  			{
				Matrices.model = glm::mat4(1.0f);
				glm::mat4 translateCube = glm::translate (glm::vec3(-3.2+0.7*i, -3.2+0.7*j, curr_z[i][j]));		// glTranslatef
				if(count%100000==0)
				{if(is_increase[i][j]==0)
				{
					curr_z[i][j]-=zspeed[i][j]*0.003;
					//cout<<"Decreasing!"<<endl;
 					glm::mat4 translateCube = glm::translate (glm::vec3(-3.2+0.7*i, -3.2+0.7*j, curr_z[i][j]));		// glTranslatef
 					if(curr_z[i][j]<-1.0)is_increase[i][j]=1;
 					//cout<<panx<<" "<<pany<<" "<<panz<<endl;
				}
				if(is_increase[i][j]==1)
				{
					curr_z[i][j]+=zspeed[i][j]*0.003;
					//cout<<"Increasing!"<<endl;
 					glm::mat4 translateCube = glm::translate (glm::vec3(-3.2+0.7*i, -3.2+0.7*j, curr_z[i][j]));		// glTranslatef
 					if(curr_z[i][j]>1.0)is_increase[i][j]=0;
				}
				}
				//glm::mat4 rotateCube = glm::rotate((float)(90.0f/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
				glm::mat4 scaleCube = glm::scale(glm::vec3(0.40,0.40,0.75));
				Matrices.model *= (translateCube * scaleCube);
				MVP = VP * Matrices.model;
				// Copy MVP to texture shaders
				glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);
	
				// Set the texture sampler to access Texture0 memory
				glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);
	
				// draw3DObject draws the VAO given to it using current MVP matrix
				draw3DTexturedObject(board[i][j]);
			}
			if(inp_board[i][j]=='W')
  			{
				Matrices.model = glm::mat4(1.0f);
 				glm::mat4 translateCube = glm::translate (glm::vec3(-3.2+0.7*i, -3.2+0.7*j, 0));		// glTranslatef
				//glm::mat4 rotateCube = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
				glm::mat4 scaleCube = glm::scale(glm::vec3(0.40,0.40,0.75));
				Matrices.model *= (translateCube *scaleCube);
				MVP = VP * Matrices.model;
				// Copy MVP to texture shaders
				glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);
	
				// Set the texture sampler to access Texture0 memory
				glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);
	
				// draw3DObject draws the VAO given to it using current MVP matrix
				draw3DTexturedObject(water[i][j]);
			}
		}
	}

	//Place Objects on the Board
	for(int i=0;i<10;i++)
	{
  		for(int j=0;j<10;j++)
  		{
  			if(i==xpos_player&&j==ypos_player)
  			{
  				Matrices.model = glm::mat4(1.0f);

				/* Render your scene */
				glm::mat4 translatePlayer = glm::translate (glm::vec3(-3.2+0.7*i, -3.2+0.7*j, curr_z[i][j]+1)); // glTranslatef
				botpos[1]=i;
				botpos[2]=j;
				botpos[3]=curr_z[i][j]+1;
				glm::mat4 scalePlayer = glm::scale(glm::vec3(0.30,0.30,0.15));
				Matrices.model *= (translatePlayer * scalePlayer);
				MVP = VP * Matrices.model;

				/// Copy MVP to texture shaders
				glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Set the texture sampler to access Texture0 memory
				glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

				// draw3DObject draws the VAO given to it using current MVP matrix
				draw3DTexturedObject(player);
  			}

  			if(is_obstacle[i][j])
  			{
  				Matrices.model = glm::mat4(1.0f);

				/* Render your scene */
				glm::mat4 translateObstacle = glm::translate (glm::vec3(-3.2+0.7*i, -3.2+0.7*j, curr_z[i][j]+1)); // glTranslatef
				glm::mat4 scaleObstacle = glm::scale(glm::vec3(0.25,0.25,0.25));
				Matrices.model *= (translateObstacle * scaleObstacle);
				MVP = VP * Matrices.model;

				/// Copy MVP to texture shaders
				glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Set the texture sampler to access Texture0 memory
				glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

				// draw3DObject draws the VAO given to it using current MVP matrix
				draw3DTexturedObject(obstacle[i][j]);
  			}
  		}
  	}

	// Load identity to model matrix
	Matrices.model = glm::mat4(1.0f);

	/* Render your scene */
	glm::mat4 translateTriangle = glm::translate (glm::vec3(-2.0f, 0.0f, 0.0f)); // glTranslatef
	glm::mat4 rotateTriangle = glm::rotate((float)(triangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
	glm::mat4 triangleTransform = translateTriangle * rotateTriangle;
	Matrices.model *= triangleTransform;
	MVP = VP * Matrices.model; // MVP = p * V * M

	/// Copy MVP to texture shaders
	glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Set the texture sampler to access Texture0 memory
	glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);

	// draw3DObject draws the VAO given to it using current MVP matrix
	//draw3DTexturedObject(triangle);


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
	glm::mat4 translateText = glm::translate(glm::vec3(-3,2,0));
	glm::mat4 scaleText = glm::scale(glm::vec3(fontScaleValue,fontScaleValue,fontScaleValue));
	Matrices.model *= (translateText * scaleText);
	MVP = Matrices.projection * Matrices.view * Matrices.model;
	// send font's MVP and font color to fond shaders
	glUniformMatrix4fv(GL3Font.fontMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(GL3Font.fontColorID, 1, &fontColor[0]);

	// Render font
	GL3Font.font->Render("Round n Round we go !!");

	lives = 3;
	health = 100;

	camera_rotation_angle++; // Simulating camera rotation
	//triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
	//rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;

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

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);	  // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling
	glfwSetCursorPosCallback(window,mouseMotion);
	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
	glfwGetCursorPos(window, &lastx, &lasty);
	return window;
}

int readfile()
{
	ifstream myfile;
	string inp;
	myfile.open("textures.txt");
	while(getline (myfile,inp))
	{
		textureFiles[numTextures++]=inp;
		cout<<textureFiles[numTextures-1]<<endl;
	}
	//for(int i=0;i<30;i++)cout<<mapchar[i]<<endl;
}

int readboard()
{
	ifstream myfile;
	myfile.open("blockmap.txt");
	int i=0;
	string inp;
	while(getline (myfile,inp))
	{
		inp_board[i]=inp;
		i++;
	}
	//for(int i=0;i<10;i++){for(int j=0;j<10;j++)cout<<inp_board[i][j]; cout<<endl;}
	
}

int readheight()
{
	ifstream myfile;
	myfile.open("height_var.txt");
	int i=0;
	string inp;
	while(getline (myfile,inp))
	{
		rep(j,inp.length())height_var[i][j]=inp[j]-'0';
		i++;
	}
	rep(i,10){rep(j,10)cout<<height_var[i][j]; cout<<endl;}
	
}

int readspeed()
{
	ifstream myfile;
	myfile.open("z_speed.txt");
	int i=0;
	string inp;
	while(getline(myfile,inp))
	{
		rep(j,inp.length())zspeed[i][j]=inp[j]-'0';
		i++;
	}
	rep(i,10){rep(j,10)cout<<zspeed[i][j]; cout<<endl;}
	
}

int readobstacles()
{
	ifstream myfile;
	myfile.open("obstacles.txt");
	int i=0;
	string inp;
	while(getline(myfile,inp))
	{
		rep(j,inp.length())is_obstacle[i][j]=inp[j]-'0';
		i++;
	}
	rep(i,10){rep(j,10)cout<<is_obstacle[i][j]; cout<<endl;}
	
}

void create_textures()
{
	char print[100];

	rep(i,numTextures)
	{
		for(int j=0;j<textureFiles[i].length();j++)print[j]=textureFiles[i][j];
		texture[i] = createTexture(print);
		// check for an error during the load process
		if(texture[i] == 0 )
			cout << "SOIL loading error: '" << SOIL_last_result() << "'" << endl;
	}
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	// Load Textures
	// Enable Texture0 as current texture memory
	rep(i,10)rep(j,10){zlevel[i][j]=0.0;is_increase[i][j]=0;}
	glActiveTexture(GL_TEXTURE0);
	// load an image file directly as a new OpenGL texture
	// GLuint texID = SOIL_load_OGL_texture ("beach.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS); // Buggy for OpenGL3
	readfile();
	create_textures();
	readboard();
	readheight();
	readspeed();
	readobstacles();
	// Create and compile our GLSL program from the texture shaders
	textureProgramID = LoadShaders( "TextureRender.vert", "TextureRender.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.TexMatrixID = glGetUniformLocation(textureProgramID, "MVP");

	/* Objects should be created before any other gl function and shaders */
	// Create the models
	createTriangle (texture[0]); // Generate the VAO, VBOs, vertices data & copy into the array buffer
	createRectangle (texture[2]);
	createBrick (texture[4]);
	createWater(texture[2]);
	createObstacle(texture[3]);
	createPlayer(texture[1]);
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL3.vert", "Sample_GL3.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glDepthFunc (GL_LEQUAL);
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
	int width = 1920;
	int height = 1080;

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
