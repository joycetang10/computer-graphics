#include "mesh.h"
#include "glut.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>

float R;
float old_x, old_y;
float tmp_x = 0, tmp_y = 0;
double theta = 0;
double degree = 0;
int objstate = 9;//0~8
int num = 0;//numbers of object
int k = 0;
////initilize scene
float sxx[10];
float syy[10];
float szz[10];
float anglee[10];
float rxx[10];
float ryy[10];
float rzz[10];
float txx[10];
float tyy[10];
float tzz[10];
float sx, sy, sz, angle, rx, ry, rz, tx, ty, tz;

////initilize view
double eyex, eyey, eyez;
double vatx, vaty, vatz;
double vupx, vupy, vupz;
double fovy, dnear, dfar;
int viewx, viewy, vieww, viewh;

////initilize light
int lightnum = 0;
float f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11;
float x[8], y[8], z[8], ar[8], ag[8], ab[8], dr[8], dg[8], db[8], sr[8], sg[8], sb[8];
float am0[8], am1[8], am2[8];

mesh *object;
mesh *Obj[10];

int windowSize[2];

void light();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void mouse_move(int, int);

int main(int argc, char** argv)
{
	FILE *file;//scene
	FILE *file2;//view
	FILE *file3;//light
	char objname[10];

	file = fopen("scene.scene", "r");
	file2 = fopen("view.view", "r");
	file3 = fopen("light.light", "r");

	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Mesh Example");
	char token[20];
	while (!feof(file)){ //scene
		token[0] = NULL;
		fscanf(file, "%s", token);
		if (!strcmp(token, "model")){
			fscanf(file, "%s%f%f%f%f%f%f%f%f%f%f", objname, &sx, &sy, &sz, &angle, &rx, &ry, &rz, &tx, &ty, &tz);
			object = new mesh(objname);
			Obj[num] = object;
			sxx[num] = sx;
			syy[num] = sy;
			szz[num] = sz;
			anglee[num] = angle;
			rxx[num] = rx;
			ryy[num] = ry;
			rzz[num] = rz;
			txx[num] = tx;
			tyy[num] = ty;
			tzz[num] = tz;
			num++;
		}
	}
	while (!feof(file2)){ //view
		token[0] = NULL;
		fscanf(file2, "%s", token);
		if (!strcmp(token, "eye"))
			fscanf(file2, "%lf%lf%lf", &eyex, &eyey, &eyez);
		else if (!strcmp(token, "vat"))
			fscanf(file2, "%lf%lf%lf", &vatx, &vaty, &vatz);
		else if (!strcmp(token, "vup"))
			fscanf(file2, "%lf%lf%lf", &vupx, &vupy, &vupz);
		else if (!strcmp(token, "fovy"))
			fscanf(file2, "%lf", &fovy);
		else if (!strcmp(token, "dnear"))
			fscanf(file2, "%lf", &dnear);
		else if (!strcmp(token, "dfar"))
			fscanf(file2, "%lf", &dfar);
		else if (!strcmp(token, "viewport"))
			fscanf(file2, "%d %d %d %d\n", &viewx, &viewy, &vieww, &viewh);
	}
	R = sqrt((eyex - vatx)*(eyex - vatx) + (eyez - vatz)*(eyez - vatz));
	degree = acos((eyex - vatx) / (sqrt(((eyex - vatx)*(eyex - vatx)) + ((eyez - vatz)*(eyez - vatz)) + (vaty*vaty))));
	lightnum = 0;
	while (!feof(file3)){ //light
		token[0] = NULL;
		fscanf(file3, "%s", token);
		if (!strcmp(token, "light")){
			fscanf(file3, "%f%f%f%f%f%f%f%f%f%f%f%f", &f0, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &f11);
			x[lightnum] = f0;
			y[lightnum] = f1;
			z[lightnum] = f2;
			///environment light
			ar[lightnum] = f3;
			ag[lightnum] = f4;
			ab[lightnum] = f5;
			///diffuse
			dr[lightnum] = f6;
			dg[lightnum] = f7;
			db[lightnum] = f8;
			///specular
			sr[lightnum] = f9;
			sg[lightnum] = f10;
			sb[lightnum] = f11;
			lightnum++;
		}
		else if (!strcmp(token, "ambient")){
			fscanf(file3, "%f%f%f", &f0, &f1, &f2);
			am0[lightnum] = f0;
			am1[lightnum] = f1;
			am2[lightnum] = f2;
		}
	}
	printf("lightnum=%d\n", lightnum);
	printf("num=%d\n", num);
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouse_move);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}

void light()
{
	int m = 0;
	int lighti = GL_LIGHT0;
	for (m = 0; m<lightnum; m++){
		GLfloat light_specular[] = { sr[m], sg[m], sb[m], 1.0 };
		GLfloat light_diffuse[] = { dr[m], dg[m], db[m], 1.0 };
		GLfloat light_ambient[] = { ar[m], ag[m], ab[m], 0.0 };
		GLfloat light_position[] = { x[m], y[m], z[m], 0.0 };
		GLfloat model_ambient[] = { am0[m], am1[m], am2[m], 1.0 };
		glShadeModel(GL_SMOOTH);

		// z buffer enable
		glEnable(GL_DEPTH_TEST);

		// enable lighting
		glEnable(GL_LIGHTING);
		// set light property

		glEnable(lighti);
		glLightfv(lighti, GL_POSITION, light_position);
		glLightfv(lighti, GL_DIFFUSE, light_diffuse);
		glLightfv(lighti, GL_SPECULAR, light_specular);
		glLightfv(lighti, GL_AMBIENT, light_ambient);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
		lighti++;
	}
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	// viewport transformation
	glViewport(viewx, viewy, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], dnear, dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyex, eyey, eyez,//eye
		vatx, vaty, vatz,//center
		vupx, vupy, vupz);//up

	//注意light位置的設定，要在gluLookAt之後
	light();

	glTranslatef(vatx, vaty, vatz);//
	glRotatef(theta, 0, 1, 0);//
	glTranslatef(-vatx, -vaty, -vatz);//
	for (k = 0; k<num; k++){
		glPushMatrix();
		int lastMaterial = -1;
		glTranslatef(txx[k], tyy[k], tzz[k]);
		glRotatef(anglee[k], rxx[k], ryy[k], rzz[k]);
		glScalef(sxx[k], syy[k], szz[k]);
		for (size_t i = 0; i < Obj[k]->fTotal; ++i){
			// set material property if this face used different material
			if (lastMaterial != Obj[k]->faceList[i].m){
				lastMaterial = (int)Obj[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, Obj[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Obj[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, Obj[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &Obj[k]->mList[lastMaterial].Ns);

				//you can obtain the texture name by object1->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j){
				//textex corrd. Obj[0]->tList[Obj[0]->faceList[i][j].t].ptr
				glNormal3fv(Obj[k]->nList[Obj[k]->faceList[i][j].n].ptr);
				glVertex3fv(Obj[k]->vList[Obj[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		glPopMatrix();
	}
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w': //zoom in//camera position - center to do translation
		eyex = eyex + (vatx - eyex)*0.1;
		eyey = eyey + (vaty - eyey)*0.1;
		eyez = eyez + (vatz - eyez)*0.1;
		glutPostRedisplay();
		break;
	case 'a': //move left(circle the center)
		theta = theta + 5;
		glutPostRedisplay();
		break;
	case 's'://zoom out
		eyex = eyex - (vatx - eyex)*0.1;
		eyey = eyey - (vaty - eyey)*0.1;
		eyez = eyez - (vatz - eyez)*0.1;
		glutPostRedisplay();
		break;
	case 'd'://move right (circle the center)
		theta = theta - 5;
		glutPostRedisplay();
		break;
	case '1'://'1'~'9' : select n-th object
		objstate = 0;
		break;
	case '2':
		objstate = 1;
		break;
	case '3':
		objstate = 2;
		break;
	case '4':
		objstate = 3;
		break;
	case '5':
		objstate = 4;
		break;
	case '6':
		objstate = 5;
		break;
	case '7':
		objstate = 6;
		break;
	case '8':
		objstate = 7;
		break;
	case '9':
		objstate = 8;
		break;
	}
}

void mouse(int button, int state, int mx, int my)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN){
			old_x = mx;
			old_y = my;
		}
		if(state == GLUT_UP){
			old_x = 0;
			old_y = 0;
		}
		break;
	}
}
void mouse_move(int x, int y)
{
	txx[objstate] = txx[objstate] + (x-old_x)*0.5;
	tyy[objstate] = tyy[objstate] - (y-old_y)*0.5;
	old_x = x;
	old_y = y;
	glutPostRedisplay();
}
