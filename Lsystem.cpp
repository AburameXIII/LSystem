#include <Windows.h>
#include <Mmsystem.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include <time.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/GLUT.h>

using namespace std;

float PI = 3.14159; //Use for 3D rotation
float DEPTH = 4; //Default value for the last iteration of the Lsystem
float ANGLE = 20; //Default value for angle for rotations
float PROB = 1;  //Default value for probability rules 
float depth = 0; //Current Lsystem iteration rendered;
vector<string>* iterations = new vector<string>(); //Vector of the different Lsystem iterations
bool mode3D = false; //Flag to know if we are showing the tree in 3D view or not
float cameraAngleHorizontal = 0; //Horizontal angle of the camera in 3D mode

//Representation of the LSystem
struct LSystem {
	map<string, string> Parameters;
	map<string, string> Rules;
	string InitialAxiom;
};

//Current rendered LSystem
LSystem CurrentLSystem;

//Different config files
int nextFileToLoad = 0;
const vector<string> files = { "config1.txt", "config2.txt", "config3.txt", "config4.txt", "config5.txt", "config6.txt", "config7.txt", "config8.txt", "config9.txt", "config10.txt" };
int numberFiles = 10;


float eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ, fieldOfView; //Camera configurations
float LENGTH = 1; //Default value of the length of a line
float num = 0; //Used to calculate random number between 0 and 1 to use for probabilities

float WIDTH = 1; //Default value for the width of a line
string str = ""; //Current string iteration of the current displayed L-System

void push() {
	glPushMatrix();
}

void pop() {
	glPopMatrix();
}

void rotL() {
	glRotatef(-ANGLE, 0, 0, 1);
}
void rotR() {
	glRotatef(ANGLE, 0, 0, 1);
}

void rolL() {
	glRotatef(-ANGLE, 1, 0, 0);
}

void rolR() {
	glRotatef(ANGLE, 1, 0, 0);
}

void pitchU() {
	glRotatef(-ANGLE, 0, 1, 0);
}

void pitchD() {
	glRotatef(ANGLE, 0, 1, 0);
}

void leaf() {
	glPushAttrib(GL_LIGHTING_BIT); //saves current lighting stuff
	//light green color
	GLfloat ambient[4] = { 0.5, 1, 0 };
	GLfloat specular[4] = { 0.6, 1, 0 };
	GLfloat diffuse[4] = { 0.5, 1, 0 }; 
	// set the ambient reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	// set the diffuse reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	// set the specular reflection for the object      
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	// set the size of the specular highlights
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);
	glBegin(GL_TRIANGLES);
	glVertex3f(0, 0, 0);
	glVertex3f(0.2, 0, 0.3);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-0.2, 0, -0.3);
	glVertex3f(0, 1, 0);
	glEnd();
	glPopAttrib();
}

//draws 
void drawLine() {
	glPushAttrib(GL_LIGHTING_BIT);//saves current lighting stuff
	GLfloat ambient[4] = { 0.1, 0.5, 0.1 };    // green ambient reflection
	GLfloat diffuse[4] = { 0.1, 0.5, 0.1 };   // green diffuse reflection
	// set the ambient reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	// set the diffuse reflection for the object
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glLineWidth(WIDTH);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, LENGTH, 0);
	glEnd();
	glTranslatef(0, LENGTH, 0);
	glPopAttrib();
}

//draws the current iteration of the Lsystem
void draw() {
	string ch = "";
	string LSystem = iterations->at(depth);
	for (size_t  i = 0; i < LSystem.length(); ++i) {
		ch = LSystem.at(i);

		if (ch.compare("D") == 0 || ch.compare("X") == 0 || ch.compare("F") == 0) {
			drawLine();
		}
		else if (ch.compare("[") == 0) {
			push();
		}
		else if (ch.compare("]") == 0) {
			pop();
		}
		else if (ch.compare("V") == 0) {
			leaf();
		}
		else if (ch.compare("+") == 0) {
			rotR();
		}
		else if (ch.compare("/") == 0) {
			rolR();
		}
		else if (ch.compare("\\") == 0) {
			rolL();
		}
		else if (ch.compare("&") == 0) {
			pitchD();
		}
		else if (ch.compare("^") == 0) {
			pitchU();
		}
		else if (ch.compare("-") == 0) {
			rotL();
		}
	}
}

//performs an iteration of the Lsystem
void expand() {
	string ch = "";
	for (int  i = 0; i < str.length(); i++) {
		ch = str.at(i);

				if (CurrentLSystem.Rules.find(ch) != CurrentLSystem.Rules.end()) {
					

					string value = CurrentLSystem.Rules.find(ch)->second;

					//for stochastic Lsystems
					if (value.find("p") != string::npos) {
						int pos_end = value.find("|", 0);
						num = (float)rand() / RAND_MAX;
							if (num < PROB) {
								value = value.substr(1, pos_end - 1);
								
							}
							else {
								value = value.substr(pos_end + 1, value.length() - 1);
							}
						
					}

					str.replace(i, 1, value);
					i = i + value.length() - 1;
				}

	}
	iterations->push_back(str);
}




void display(void) {
	// start by clearing the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, 1.0, 1, 2000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, lookX, lookY, lookZ, 0, 1, 0);

	draw();
	glutSwapBuffers();
	glutPostRedisplay();
}

void idle() {
	glutPostRedisplay();
}

//Parses the line of a configuration file
void split(string line, string valuedelimiter, string nextdelimiter) {
	size_t pos_start = 0, pos_end, vdlen = valuedelimiter.length(), ndlen = nextdelimiter.length();

	size_t value_pos_end;
	string pair_label_value;
	string label, value;

	while ((pos_end = line.find(nextdelimiter, pos_start)) != string::npos) {
		pair_label_value = line.substr(pos_start, pos_end - pos_start);


		value_pos_end = pair_label_value.find(valuedelimiter, 0);
		label = pair_label_value.substr(0, value_pos_end);
		value = pair_label_value.substr(value_pos_end + vdlen, pair_label_value.length() - 1);

		if (valuedelimiter == "=") {
			CurrentLSystem.Parameters.insert(make_pair(label.c_str(), value.c_str()));
		}
		else if (valuedelimiter == ":") {
			CurrentLSystem.Rules.insert(make_pair( label.c_str(), value.c_str()));
		}


		pos_start = pos_end + ndlen;
	}
}

//Resets parameters
void resetParameters() {
	DEPTH = 4;
	ANGLE = 20;
	PROB = 0; 
	depth = 0;
	LENGTH = 1;
	WIDTH = 1;
	CurrentLSystem.InitialAxiom = "";
	CurrentLSystem.Parameters.clear();
	CurrentLSystem.Rules.clear();
	iterations = new vector<string>();
}

void printLsystem() {
	printf("Parameters :\n");
	for (std::map<string, string>::iterator it = CurrentLSystem.Parameters.begin(); it != CurrentLSystem.Parameters.end(); ++it) {
		std::cout << it->first << " = " << it->second << '\n';
	}

	printf("Rules:\n");
	for (std::map<string, string>::iterator it = CurrentLSystem.Rules.begin(); it != CurrentLSystem.Rules.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';
	std::cout << "Initial Axiom: " << CurrentLSystem.InitialAxiom << '\n';

	
}


//Goes to the next Lsystem
void openNextDocument() {
	resetParameters();

	ifstream inFile(files[nextFileToLoad % numberFiles]);
	if (!inFile) {
		cerr << "Unable to open file input " << files[nextFileToLoad] << ", exiting...\n";
		exit(-1);
	}

	nextFileToLoad += 1;

	char ch;
	vector<string> parsedStrings;
	//read each chars
	string line;
	while (inFile.get(ch)) {
		
		//cout.put(ch);
		if (ch == '\n'  || ch == EOF)
		{
			
			parsedStrings.push_back(line.c_str());
			line = "";
			continue;
		}

		line += ch;
		
	}

	split(parsedStrings[0].c_str(), "=", ";");
	split(parsedStrings[1].c_str(), ":", ";");
	CurrentLSystem.InitialAxiom = line;

	if (CurrentLSystem.Parameters.find("angle") != CurrentLSystem.Parameters.end()) {
		const std::string& value = CurrentLSystem.Parameters.find("angle")->second;
		ANGLE = stof(value);
	}

	if (CurrentLSystem.Parameters.find("n") != CurrentLSystem.Parameters.end()) {
		const std::string& value = CurrentLSystem.Parameters.find("n")->second;
		DEPTH = stof(value);
	}

	if (CurrentLSystem.Parameters.find("l") != CurrentLSystem.Parameters.end()) {
		const std::string& value = CurrentLSystem.Parameters.find("l")->second;
		LENGTH = stof(value);
	}

	if (CurrentLSystem.Parameters.find("p") != CurrentLSystem.Parameters.end()) {
		const std::string& value = CurrentLSystem.Parameters.find("p")->second;
		PROB = stof(value);
	}

	if (CurrentLSystem.Parameters.find("w") != CurrentLSystem.Parameters.end()) {
		const std::string& value = CurrentLSystem.Parameters.find("w")->second;
		WIDTH = stof(value);
	}

	printLsystem();


	str = CurrentLSystem.InitialAxiom;

	iterations->push_back(str);
	for (int i = 0; i <= DEPTH -1; i++) {
		expand();
	}
}



void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 113:			// q - Exit the program
		exit(0);
		break;
	case 119:			// w - Move Camera Up
		lookY += 5;
		eyeY += 5;
		glutPostRedisplay();
		break;
	case 114:			// r - Resets the camera
		fieldOfView = 75;
		eyeX = 0;
		eyeY = 100;
		eyeZ = 200;
		lookX = 0;
		lookY = 100;
		lookZ = 0;
		mode3D = false;
		break;
	case 101:			// e - 3D camera
		fieldOfView = 45;
		eyeX = 200;
		eyeY = 100;
		eyeZ = 0;
		lookX = 0;
		lookY = 50;
		lookZ = 0;
		mode3D = true;
		cameraAngleHorizontal = 0;
		break;
	case 122:			// z - ZoomIn
		fieldOfView -= 5;
		glutPostRedisplay();
		break;
	case 120:			// x - ZoomOut
		fieldOfView += 5;
		glutPostRedisplay();
		break;
	case 115:			// s - Move Camera Down
		lookY -= 5;
		eyeY -= 5;
		glutPostRedisplay();
		break;
	case 97:			// a - Move Camera to the left
		if (!mode3D) {
			lookX -= 5;
			eyeX -= 5;
		}
		else {
			cameraAngleHorizontal += 1;
			eyeX = 200 * cos((double)cameraAngleHorizontal * PI / 180.0);
			eyeZ = 200 * sin((double)cameraAngleHorizontal * PI / 180.0);
		}
		glutPostRedisplay();
		break;
	case 100:			// d - Move Camera to the Right
		if (!mode3D) {
			lookX += 5;
			eyeX += 5;
		}
		else {
			cameraAngleHorizontal -= 1;
			eyeX = 200 * cos((double)cameraAngleHorizontal * PI / 180.0);
			eyeZ = 200 * sin((double) cameraAngleHorizontal * PI / 180.0);
		}
		glutPostRedisplay();
		break;
	case 102:			// f - Increase Angle by 0.1
		ANGLE += 0.1;
		cout << "Current Angle: " << ANGLE << "\n";
		break;
	case 103:			// g - Decrease Angle by 0.1
		ANGLE -= 0.1;
		cout << "Current Angle: " << ANGLE << "\n";
		break;
	case 104:			// h - Increase Angle by 10
		ANGLE += 10;
		cout << "Current Angle: " << ANGLE << "\n";
		break;
	case 106:			// j - Decrease Angle by 10
		ANGLE -= 10;
		cout << "Current Angle: " << ANGLE << "\n";
		break;
	case 110:      //n - go to next iteration
		if(depth < DEPTH) depth++;
		cout << "Current Iterarion:" << iterations->at(depth) << "\n";
		break;
	case 109:      //m - go to previous iteration
		if (depth > 0) depth--;
		cout << "Current Iterarion:" << iterations->at(depth) << "\n";
		break;
	case 111:			// o - Increase Line Width
		WIDTH += 0.1;
		cout << "Current Line Width: " << WIDTH << "\n";
		break;
	case 112:			// p - Decrease Line Width
		if(WIDTH > 0.1) WIDTH -= 0.1;
		cout << "Current Line Width: " << WIDTH << "\n";
		break;

	case 98:      //b - load next config
		openNextDocument();
		break;
	case 99:      //c - load previous config
		nextFileToLoad -= 2;
		if (nextFileToLoad < 0) nextFileToLoad += 10;
		openNextDocument();
		break;
	case 118:      //v - reload config file
		nextFileToLoad--;
		openNextDocument();
		break;
	}
	

}



int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(720, 720);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("L-Systems");
	
	fieldOfView = 70;
	eyeX = 0;
	eyeY = 120;
	eyeZ = 200;
	lookX = 0;
	lookY = 120;
	lookZ = 0;

	//White Background
	glClearColor(255,255, 255, 1);
	glClear(GL_COLOR_BUFFER_BIT);


	// set the lighting
	glShadeModel(GL_SMOOTH);
	GLfloat lightP[4] = { 0.0, 800.0, 0.0,1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightP);

	// set the ambient light colour    
	GLfloat lightA[4] = { 0.0,0.9,0.9,1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightA);

	// set the specular light colour         
	GLfloat lightS[4] = { 0.9,0.9,0.9,1.0 };
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightS);

	// set the diffuse light colour
	GLfloat lightD[4] = { 0.9,0.9,0.9,1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightD);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);


	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);


	openNextDocument();
	
	glutMainLoop();
	return 0;
}