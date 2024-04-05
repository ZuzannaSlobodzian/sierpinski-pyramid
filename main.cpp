#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"

#include "shaderLoader.h"

#include "glm/vec3.hpp" 
#include "glm/vec4.hpp" 
#include "glm/mat4x4.hpp" 
#include "glm/gtc/matrix_transform.hpp" 
#include <iostream>

using namespace std;

int screenWidth = 1240;
int screenHeight = 840;
int mousePositionX; 
int mousePositionY;
int mButton; 

double cameraX= 60.0;
double cameraZ = 20.0;
double cameraD = -3.0;
double cameraV;
double cameraAngle = 20;
double cameraVRotation;
double previousCameraX;
double previousCameraZ;
double previousCameraD;
double rotation = 0;

glm::mat4 MV;
glm::mat4 P; 

GLuint programIDPolygons = 0;
GLuint programIDLines = 0;
GLuint programIDTransparent = 0;

unsigned int VBO, CBO, NBO;
unsigned int VAO;

int numberOfDivisions;
int numberOfData;
int numberOfVertices;

int index = 0;

float firstVertices[][3] = {
	{-0.5, 0, 0},
	{0.5, 0, 0},
	{0, 0.816, -0.287},
	{0,  0, -0.866},
};

float vertices[10000000];
float colors[10000000];
float normals[10000000];

glm::vec3 lightPos(0.0f, -5.0f, 40.0f);

GLuint lightColor_id = 0;
GLuint lightPos_id = 0;
GLuint viewPos_id = 0;

int menuID;
int displayMode = 1;

void calcNormal(float v[3][3], float out[3]) {
	float v1[3], v2[3];

	static const int x = 0; 
	static const int y = 1; 
	static const int z = 2;

	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];
}

void mouse(int button, int state, int x, int y){
	mButton = button;

	switch (state){
		case GLUT_UP:
			break;
		case GLUT_DOWN:
			mousePositionX = x;
			mousePositionY = y;
			previousCameraX = cameraX;
			previousCameraZ = cameraZ;
			previousCameraD = cameraD;
			break;
	}
}

void mouseMove(int x, int y){
	if (mButton == GLUT_LEFT_BUTTON){
		cameraX = previousCameraX - (mousePositionX - x) * 0.1;
		cameraZ = previousCameraZ - (mousePositionY - y) * 0.1;
	}
	if (mButton == GLUT_RIGHT_BUTTON){
		cameraD = previousCameraD + (mousePositionY - y) * 0.1;
	}
}

void keyOptions(GLubyte key, int x, int y){
	switch (key) {
		case 27: 
			exit(1);
			break;
		case 'f':
			glutFullScreen();
			break;
	}
}

void idle() {
	glutPostRedisplay();
}

void timer(int value) {
	glutTimerFunc(20, timer, 0);
}

void screenSize(int width, int height) {
	screenWidth = width;
	screenHeight = height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screenWidth, screenHeight);

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 1000.0f);

	glutPostRedisplay();
}

void saveVertices(GLfloat A[], GLfloat B[], GLfloat C[]) {
	vertices[index] = A[0];
	vertices[index + 1] = A[1];
	vertices[index + 2] = A[2];

	vertices[index + 3] = B[0];
	vertices[index + 4] = B[1];
	vertices[index + 5] = B[2];

	vertices[index + 6] = C[0];
	vertices[index + 7] = C[1];
	vertices[index + 8] = C[2];

	colors[index] = abs(A[0]);
	colors[index + 1] = abs(A[1]);
	colors[index + 2] = abs(A[2]);

	colors[index + 3] = abs(B[0]);
	colors[index + 4] = abs(B[1]);
	colors[index + 5] = abs(B[2]);

	colors[index + 6] = abs(B[0]);
	colors[index + 7] = abs(B[1]);
	colors[index + 8] = abs(B[2]);

	index = index + 9;
}

void division(GLfloat V1[], GLfloat V2[], GLfloat V3[], GLfloat V4[], int n) {
	GLfloat V12[3];
	GLfloat V23[3];
	GLfloat V31[3];
	GLfloat V14[3];
	GLfloat V24[3];
	GLfloat V34[3];

	if (n > 0) {
		V12[0] = (V1[0] + V2[0]) / 2;
		V12[1] = (V1[1] + V2[1]) / 2;
		V12[2] = (V1[2] + V2[2]) / 2;

		V23[0] = (V2[0] + V3[0]) / 2;
		V23[1] = (V2[1] + V3[1]) / 2;
		V23[2] = (V2[2] + V3[2]) / 2;

		V31[0] = (V3[0] + V1[0]) / 2;
		V31[1] = (V3[1] + V1[1]) / 2;
		V31[2] = (V3[2] + V1[2]) / 2;

		V14[0] = (V1[0] + V4[0]) / 2;
		V14[1] = (V1[1] + V4[1]) / 2;
		V14[2] = (V1[2] + V4[2]) / 2;

		V24[0] = (V2[0] + V4[0]) / 2;
		V24[1] = (V2[1] + V4[1]) / 2;
		V24[2] = (V2[2] + V4[2]) / 2;

		V34[0] = (V3[0] + V4[0]) / 2;
		V34[1] = (V3[1] + V4[1]) / 2;
		V34[2] = (V3[2] + V4[2]) / 2;

		division(V1, V12, V31, V14, n - 1);
		division(V12, V2, V23, V24, n - 1);
		division(V31, V23, V3, V34, n - 1);
		division(V14, V24, V34, V4, n - 1);

	}
	else {
		saveVertices(V1, V2, V3);
		saveVertices(V1, V3, V4);
		saveVertices(V2, V3, V4);
		saveVertices(V1, V2, V4);
	}
}

void calculate(void) {
	division(firstVertices[0], firstVertices[1], firstVertices[2], firstVertices[3], numberOfDivisions);

	int i = 0;
	while (i < numberOfData) {
		float tempNormal[3];

		float p[3][3] = { {vertices[i], vertices[i + 1], vertices[i + 2]}, {vertices[i + 3], 
			vertices[i + 4], vertices[i + 5]}, {vertices[i + 6], vertices[i + 7], vertices[i + 8]} };

		calcNormal(p, tempNormal);

		normals[i] = tempNormal[0];
		normals[i + 1] = tempNormal[1];
		normals[i + 2] = tempNormal[2];

		normals[i + 3] = tempNormal[0];
		normals[i + 4] = tempNormal[1];
		normals[i + 5] = tempNormal[2];

		normals[i + 6] = tempNormal[0];
		normals[i + 7] = tempNormal[1];
		normals[i + 8] = tempNormal[2];

		i = i + 9;
	}
}

void drawPolygons(void) {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programIDPolygons);

	MV = glm::mat4(1.0f);
	MV = glm::translate(MV, glm::vec3(0, 0, cameraD));

	MV = glm::rotate(MV, (float)glm::radians(cameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(cameraX), glm::vec3(0, 1, 0));
	MV = glm::rotate(MV, (float)glm::radians(160.0), glm::vec3(0, 1, 0));

	MV = glm::scale(MV, glm::vec3(2.2f, 2.2f, 2.2f));
	MV = glm::translate(MV, glm::vec3(0, -0.18, 0.12));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programIDPolygons, "MVP");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));

	glUniform3f(lightColor_id, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPos_id, lightPos[0], lightPos[1], lightPos[2]);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices);

	glFlush();
	glutSwapBuffers();
}

void drawLines(void){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programIDLines);

	MV = glm::mat4(1.0f);
	MV = glm::translate(MV, glm::vec3(0, 0, cameraD));

	MV = glm::rotate(MV, (float)glm::radians(cameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(cameraX), glm::vec3(0, 1, 0));
	MV = glm::rotate(MV, (float)glm::radians(160.0), glm::vec3(0, 1, 0));

	MV = glm::scale(MV, glm::vec3(2.2f, 2.2f, 2.2f));
	MV = glm::translate(MV, glm::vec3(0, -0.18, 0.12));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programIDLines, "MVP");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, numberOfVertices);

	glFlush();
	glutSwapBuffers();
}

void drawTransparent(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programIDTransparent);

	MV = glm::mat4(1.0f);
	MV = glm::translate(MV, glm::vec3(0, 0, cameraD));

	MV = glm::rotate(MV, (float)glm::radians(cameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(cameraX), glm::vec3(0, 1, 0));
	MV = glm::rotate(MV, (float)glm::radians(160.0), glm::vec3(0, 1, 0));

	MV = glm::scale(MV, glm::vec3(2.2f, 2.2f, 2.2f));
	MV = glm::translate(MV, glm::vec3(0, -0.18, 0));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programIDTransparent, "MVP");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	glBindVertexArray(VBO);

	GLfloat attrib[] = { 0.0, 1.0, 1.0, 0.35};
	glVertexAttrib4fv(3, attrib);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glDisable(GL_POLYGON_OFFSET_FILL);

	attrib[0] = 0.0;
	attrib[1] = 0.0;
	attrib[2] = 0.80;
	attrib[3] = 1.0;
	glVertexAttrib4fv(3, attrib);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	glDrawArrays(GL_LINES, 0, numberOfVertices);

	glFlush();
	glutSwapBuffers();
}

void display() {
	switch (displayMode) {
		case 1:
			drawPolygons();
			break;
		case 2:
			drawLines();
			break;
		case 3:
			drawTransparent();
			break;
		case 4:
			exit(0);
			break;
	}
}

void menu(int id) {
	switch (id) {
		case 1:
			displayMode = 1;
			break;
		case 2:
			displayMode = 2;
			break;
		case 3:
			displayMode = 3;
			break;
		case 4:
			exit(0);
			break;
	}
}

int main(int argc, char **argv){
	cout << "Podaj ilosc podzialow" << endl;
	cin >> numberOfDivisions;

	numberOfData = pow(12 * (pow(2, numberOfDivisions-1)), 2);
	numberOfVertices = numberOfData / 3;

	calculate();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Piramida Sierpinskiego");

	glewInit(); 
    glutIdleFunc(idle);	
	glutReshapeFunc(screenSize); 
	glutKeyboardFunc(keyOptions);	
	glutMouseFunc(mouse); 	
	glutMotionFunc(mouseMove);

	glutDisplayFunc(display);
	menuID = glutCreateMenu(menu);
	glutAddMenuEntry("Polygons", 1);
	glutAddMenuEntry("Lines", 2);
	glutAddMenuEntry("Transparent", 3);
	glutAddMenuEntry("Exit", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(20, timer, 0);

	glEnable(GL_DEPTH_TEST);

	programIDPolygons = loadShaders("vertexShader.glsl", "fragmentShaderPolygons.glsl");
	programIDLines = loadShaders("vertexShader.glsl", "fragmentShaderLines.glsl");
	programIDTransparent = loadShaders("vertexShader.glsl", "fragmentShaderTransparent.glsl");

	lightColor_id = glGetUniformLocation(programIDPolygons, "lightColor");
	lightPos_id = glGetUniformLocation(programIDPolygons, "lightPos");
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &CBO);
	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, CBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glutMainLoop();

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &CBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &VAO);

	return(0);
}