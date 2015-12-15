//  Calculator v1.0
//  Created by Manjunath Chandrashekar on 21/03/14.
//  Copyright (c) 2014 Manjunath Chandrashekar. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

//Type definitions to clear ambiguities
typedef GLfloat point;
typedef int flag;

//A type defined structure to signify status of calculator
typedef struct  {
	flag currentOperand;        //1.Specifies which operand is entered. -0 if first & 1 if second-
	flag resultObtained;        //2.Specifies if the result is obtained so that it can be displayed
	flag operationDone;         //3.Specifies if operation is done so that display is no longer updated after that -0 if not done, 1 if done-
	flag operandLength;         //4.Useful in limiting the calculator to 10 digit operands
	flag clearScreen;           //5.Useful to clear the screen
	flag decimalOperation;      //6.Marks operation as decimal. so that decimal point can be used only once.
	flag ErrorOccured;          //7.If division by zero is attempted, this flag is set to 1
	flag turnedON;				//8.If calculator is turned on, its set to 1. By default its 0
} status;

//To index 1st & 2nd operands and total expression as well.
int opr1Index = 0, opr2Index = 0, expIndex = 0;
//Designated Initializer to set up a starting status of calculator
status stat = { 0, 0, 0, 0, 0, 0, 0, 0 };
//Strings to hold entered operands and oprtrs
char operand1Str[50], operand2Str[50], resStr[50], totalExp[100], oprtr, **endptr = NULL;
//Operands in strings converted to floats and result of the operation
double tempOperand, operand1, operand2, result = 0;
//Some temporary variables helpful in printing to calculator output
char tempStr[100];
int temp = -2, x = 0;

//specifies starting and ending Y co-ordinates for all buttons.
//Value is changed each time before drawing separate row.
point rowMinY, rowMaxY;

//Font/Text related stuff 
typedef enum {
	MODE_BITMAP,
	MODE_STROKE
} mode_type;
static mode_type mode;
static int font_index;

//Takes 4 points as arguments and draws a polygon using it.
void DrawButton(point p1, point p2, point p3, point p4);
//Calls DrawButtons numerous times to draw all buttons of the calculator
void DrawAllButtons(void);
//To print the given string or character at location (x, y)
void output(int x, int y, char *string);
//Initialize Display and Window.
void init(void);
//Display Callback.
void Display(void);
//Handles Mouse event.
void Mouse(int btn, int state, int x, int y);
//Handles Keyboard event.
void Keys(unsigned char key, int x, int y);
//Scales Window when resized.
void MyReshape(int w, int h);
//Print to calculator output screen
void PrintOutput(void);
//Reconfigure Operands for further operations
void ReconfigureOperands(void);
//Calculate the result from previous operation 
void CalculateResult(void);
//Clear the output screen of the calculator display
void ClearEverything(void);
//Read a character and assign it desired operand
void ReadCharacter(const char key);
//Read the operator to perform operation
void ReadOperator(const char key);

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(310, 500);
	glutCreateWindow("Calculator v1.0");
	init();
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keys);
	glutDisplayFunc(Display);
	glutReshapeFunc(MyReshape);
	glutMainLoop();
	return 0;
}

void DrawButton(point p1, point p2, point p3, point p4)    {
	glBegin(GL_POLYGON);
	glVertex2f(p1, rowMinY);
	glVertex2f(p2, rowMinY);
	glVertex2f(p3, rowMaxY);
	glVertex2f(p4, rowMaxY);
	glEnd();
}

void DrawAllButtons(void)  {
	glColor3f(0.11, 0.11, 0.11);
	rowMinY = 15.0, rowMaxY = 70.0;				    //1st row of buttons
	DrawButton(15.0, 70.0, 70.0, 15.0);
	glColor3f(0.79, 0.79, 0.79);
	DrawButton(90.0, 145.0, 145.0, 90.0);
	DrawButton(165.0, 220.0, 220.0, 165.0);
	DrawButton(240.0, 295.0, 295.0, 240.0);
	rowMinY = 90.0, rowMaxY = 145.0;				//2nd row of buttons
	DrawButton(15.0, 70.0, 70.0, 15.0);
	DrawButton(90.0, 145.0, 145.0, 90.0);
	DrawButton(165.0, 220.0, 220.0, 165.0);
	DrawButton(240.0, 295.0, 295.0, 240.0);
	rowMinY = 165.0, rowMaxY = 220.0;				//3rd row of buttons
	DrawButton(15.0, 70.0, 70.0, 15.0);
	DrawButton(90.0, 145.0, 145.0, 90.0);
	DrawButton(165.0, 220.0, 220.0, 165.0);
	DrawButton(240.0, 295.0, 295.0, 240.0);
	rowMinY = 240.0, rowMaxY = 295.0;				//4th row of buttons
	DrawButton(15.0, 70.0, 70.0, 15.0);
	DrawButton(90.0, 145.0, 145.0, 90.0);
	DrawButton(165.0, 220.0, 220.0, 165.0);
	DrawButton(240.0, 295.0, 295.0, 240.0);
	rowMinY = 315.0, rowMaxY = 370.0;				//5th row of buttons
	glColor3f(0.94, 0.4, 0.33);
	DrawButton(15.0, 70.0, 70.0, 15.0);
	DrawButton(90.0, 145.0, 145.0, 90.0);
	DrawButton(165.0, 295.0, 295.0, 165.0);
	rowMinY = 390, rowMaxY = 485;                   //Output screen
	glColor3f(0.93, 0.98, 0.75);
	DrawButton(15.0, 295.0, 295.0, 15.0);


	glColor3f(1.0, 1.0, 1.0);                       //Display each of the button names.
	output(25, 25, "0");
	output(100, 25, ".");
	output(175, 25, "=");
	output(250, 25, "+");

	output(25, 100, "1");
	output(100, 100, "2");
	output(175, 100, "3");
	output(250, 100, "-");

	output(25, 175, "4");
	output(100, 175, "5");
	output(175, 175, "6");
	output(250, 175, "*");

	output(25, 250, "7");
	output(100, 250, "8");
	output(175, 250, "9");
	output(250, 250, "/");

	output(25, 325, "ON");
	output(95, 325, "OFF");
	output(215, 325, "AC");
}

void output(int x, int y, char *string) {
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void MyReshape(int w, int h)    {
	glutReshapeWindow(310, 500);
}

void init(void)    {
	glClearColor(0.60f, 0.60f, 0.60f, 1.0f);  // gray background
	glMatrixMode(GL_PROJECTION);              // setup viewing projection
	glLoadIdentity();                         // start with identity matrix
	gluOrtho2D(0.0, 310.0, 0.0, 500.0);       // setup a 310x500 viewing window
	mode = MODE_BITMAP;
	font_index = 0;
}

void Display(void)  {
	glClear(GL_COLOR_BUFFER_BIT);
	DrawAllButtons();
	if (stat.turnedON)	{
		PrintOutput();
	}
	glFlush();
	glutSwapBuffers();
}

void Mouse(int btn, int state, int x, int y)    {
	if (stat.turnedON)	{
		if (btn == GLUT_LEFT_BUTTON && (x > 15 && x<70) && (y>455 && y<505) && state == GLUT_DOWN)    {
			ReadCharacter('0');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>90 && x<145) && (y>455 && y<505) && state == GLUT_DOWN)   {
			if (stat.decimalOperation == 0) {
				if (stat.currentOperand == 0)   {
					stat.operandLength++;
					if (stat.operandLength > 10) {
						return;
					}
					operand1Str[opr1Index++] = '.';
					stat.decimalOperation = 1;
				}
				else if (stat.currentOperand == 1)  {
					stat.operandLength++;
					if (stat.operandLength > 10) {
						return;
					}
					operand2Str[opr2Index++] = '.';
					stat.decimalOperation = 1;
				}
				totalExp[expIndex++] = '.';
			}
			else if (stat.decimalOperation == 1)
				return;
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x > 15 && x<70) && (y>380 && y<430) && state == GLUT_DOWN)   {
			ReadCharacter('1');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>90 && x<145) && (y>380 && y<430) && state == GLUT_DOWN)   {
			ReadCharacter('2');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>165 && x<220) && (y>380 && y<430) && state == GLUT_DOWN)   {
			ReadCharacter('3');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>15 && x<70) && (y>305 && y<355) && state == GLUT_DOWN)   {
			ReadCharacter('4');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>90 && x<145) && (y>305 && y<355) && state == GLUT_DOWN)   {
			ReadCharacter('5');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>165 && x<220) && (y>305 && y<355) && state == GLUT_DOWN)   {
			ReadCharacter('6');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>15 && x<70) && (y>230 && y<280) && state == GLUT_DOWN)   {
			ReadCharacter('7');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>90 && x<145) && (y>230 && y<280) && state == GLUT_DOWN)   {
			ReadCharacter('8');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>165 && x<220) && (y>230 && y<280) && state == GLUT_DOWN)   {
			ReadCharacter('9');
			glutPostRedisplay();
		}

		else if (btn == GLUT_LEFT_BUTTON && (x>240 && x<295) && (y>455 && y<505) && state == GLUT_DOWN)   {
			ReadOperator('+');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>240 && x<295) && (y>380 && y<430) && state == GLUT_DOWN)   {
			ReadOperator('-');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>240 && x<295) && (y>305 && y<355) && state == GLUT_DOWN)   {
			ReadOperator('*');
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>240 && x<295) && (y>230 && y<280) && state == GLUT_DOWN)   {
			ReadOperator('/');
			glutPostRedisplay();
		}

		else if (btn == GLUT_LEFT_BUTTON && (x > 165 && x<295) && (y>155 && y<205) && state == GLUT_DOWN)   {
			//AC is clicked
			ClearEverything();
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>165 && x<220) && (y>455 && y < 505) && state == GLUT_DOWN)   {
			// = is clicked
			CalculateResult();
			glutPostRedisplay();
		}
		else if (btn == GLUT_LEFT_BUTTON && (x>90 && x<145) && (y>155 && y<205) && state == GLUT_DOWN)   {
			exit(0);
		}
	}
	else if (btn == GLUT_LEFT_BUTTON && (x>15 && x<70) && (y>155 && y<205) && state == GLUT_DOWN)   {
		stat.turnedON = 1;
		glutPostRedisplay();
	}

}

void Keys(unsigned char key, int x, int y)  {
	if (stat.turnedON)	{
		switch (key) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			ReadCharacter(key);
			break;

		case '.':
			if (stat.decimalOperation == 0) {
				if (stat.currentOperand == 0)   {
					stat.operandLength++;
					if (stat.operandLength > 10) {
						return;
					}
					operand1Str[opr1Index++] = '.';
					stat.decimalOperation = 1;
				}
				else if (stat.currentOperand == 1)  {
					stat.operandLength++;
					if (stat.operandLength > 10) {
						return;
					}
					operand2Str[opr2Index++] = '.';
					stat.decimalOperation = 1;
				}
				totalExp[expIndex++] = '.';
			}
			else if (stat.decimalOperation == 1)
				return;
			break;

		case '+':
			ReadOperator('+');
			break;
		case '-':
			ReadOperator('-');
			break;
		case '*':
			ReadOperator('*');
			break;
		case '/':
			ReadOperator('/');
			break;

		case '=':
		case 13:							//ASCII value of return key
			CalculateResult();
			break;

		case 27:							//ASCII value of Escape key
			ClearEverything();
			break;

		default:
			printf("Invalid Character Chosen\n");
			return;
			break;
		}
		glutPostRedisplay();
	}
	else
		return;
}

void PrintOutput(void)  {
	glColor3f(0.0f, 0.0f, 0.0f);
	if (stat.operationDone != 1) {
		glColor3f(0.0, 0.0, 0.0);
		temp++;
		tempStr[temp] = totalExp[temp];
		output(20, 460, tempStr);
	}
	if (stat.resultObtained) {
		if (expIndex > 25)
			totalExp[23] = '\0';
		output(20, 460, totalExp);
		if (stat.ErrorOccured)	{
			output(20, 400, "Error.");
			stat.ErrorOccured = 0;
		}
		else	{
			output(20, 400, "=   ");
			output(35, 400, resStr);
		}
	}
	if (stat.clearScreen) {
		rowMinY = 390, rowMaxY = 485;
		glColor3f(0.93, 0.98, 0.75);
		DrawButton(15.0, 295.0, 295.0, 15.0);
		stat.clearScreen = 0;
	}
	if (stat.turnedON)	{
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(15, 390);
		glVertex2f(295, 390);
		glVertex2f(295, 485);
		glVertex2f(15, 485);
		glEnd();
	}
}

void ReconfigureOperands(void)  {
	sprintf(operand1Str, "%f", result);
	memset(totalExp, '\0', sizeof(totalExp));             //reset totalExp with null string
	memset(operand2Str, '\0', sizeof(operand2Str));
	strcpy(totalExp, operand1Str);
	expIndex = 0, opr1Index = 0, opr2Index = 0;
	while (operand1Str[expIndex] != '\0')
		expIndex += 1;
	opr2Index = 0;
	stat.currentOperand = 1;
	stat.resultObtained = 1;
	stat.operationDone = 1;
	stat.decimalOperation = 0;
	stat.operandLength = 0;
}

void CalculateResult(void)  {
	totalExp[expIndex] = '\0';
	printf("oprand1 = %s\n", operand1Str);
	printf("oprand2 = %s\n", operand2Str);
	operand1 = strtod(operand1Str, endptr);
	operand2 = strtod(operand2Str, endptr);
	switch (oprtr) {
	case '+':
		result = operand1 + operand2;
		sprintf(resStr, "%f", result);				//convert double to string
		ReconfigureOperands();
		printf("%s is the result.\n", resStr);
		break;
	case '*':
		result = operand1 * operand2;
		sprintf(resStr, "%f", result);
		if (expIndex > 20)	{
			resStr[22] = '\0';
		}
		ReconfigureOperands();
		printf("%f is the result.\n", result);
		break;
	case '-':
		result = operand1 - operand2;
		sprintf(resStr, "%f", result);
		ReconfigureOperands();
		printf("%f is the result.\n", result);
		break;
	case '/':
		if (operand2 == 0)	{
			stat.ErrorOccured = 1;
			stat.resultObtained = 1;
			glutPostRedisplay();
			return;
		}
		result = operand1 / operand2;
		sprintf(resStr, "%f", result);
		ReconfigureOperands();
		printf("%f is the result.\n", result);
		break;
	default:
		printf("Invalid Character Chosen\n");
		break;
	}
}

void ClearEverything(void)  {
	printf("AC is selected.\n");
	memset(operand1Str, '\0', sizeof(operand1Str));             //reset operand1 with null string
	memset(operand2Str, '\0', sizeof(operand2Str));             //reset operand2 with null string
	memset(totalExp, '\0', sizeof(totalExp));                   //reset the total expression with null string
	memset(tempStr, '\0', sizeof(tempStr));
	opr1Index = 0; opr2Index = 0, expIndex = 0, temp = -2;      //reset index of both operand strings and total expression
	stat.currentOperand = 0;        //1.Specifies which operand is entered. -0 if first & 1 if second-
	stat.resultObtained = 0;        //2.Specifies if the result is obtained so that it can be displayed
	stat.operationDone = 0;         //3.Specifies if operation is done so that display is no longer updated after that -0 if not done, 1 if done-
	stat.operandLength = 0;         //4.Useful in limiting the calculator to 10 digit operands
	stat.clearScreen = 1;           //5.Useful to clear the screen
	stat.decimalOperation = 0;
	printf("Both Operands cleared.\n");
}

void ReadCharacter(const char key)  {
	if (stat.currentOperand == 0)   {
		stat.operandLength++;
		if (stat.operandLength > 10) {
			return;
		}
		operand1Str[opr1Index++] = key;
	}
	else if (stat.currentOperand == 1)  {
		stat.operandLength++;
		if (stat.operandLength > 10) {
			return;
		}
		operand2Str[opr2Index++] = key;
	}
	totalExp[expIndex++] = key;
}

void ReadOperator(const char key)	{
	if (stat.currentOperand == 0 && stat.operandLength == 0)	{
		ClearEverything();
		return;
	}
	if (stat.currentOperand == 1 && stat.operandLength > 0)	{
		return;
	}
	oprtr = key;
	totalExp[expIndex++] = key;
	stat.currentOperand = 1;
	stat.operandLength = 0;
	stat.decimalOperation = 0;
}
