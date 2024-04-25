#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <vector>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// 3D models
C3dglTerrain terrain, water;
C3dglModel table;
C3dglModel vase;
C3dglModel chicken;
C3dglModel lamp;
C3dglModel room;
C3dglModel stove;
C3dglModel mirror;
C3dglModel mirror2;
C3dglModel mirror3;
C3dglModel character;
C3dglModel pier;
C3dglProgram programBasic;
C3dglProgram programWater;
C3dglProgram programTerrain;
C3dglProgram programParticle;
C3dglSkyBox skybox;

C3dglBitmap bm1;
C3dglBitmap bm2;
C3dglBitmap bm5;

GLuint daylight = 1;
GLuint LightAtt = 0;
GLuint idFBO;
GLuint idBufferVelocity = 10;
GLuint idBufferStartTime = 1;

unsigned nPyramidBuf = 0;

// texture ids
GLuint idTexWood;
GLuint idTexCloth;
GLuint idTexNone;
GLuint idTexWoodM;
GLuint idTexMirrorr;
GLuint idTexParticle;

// GLSL Program
C3dglProgram program;

// Water specific variables
float waterLevel = 4.0f;

// The View Matrix
mat4 matrixView;
mat4 matrixReflection;

// Camera & navigation
float maxspeed = 4.f;	// camera max speed
float accel = 4.f;		// camera acceleration
vec3 _acc(0), _vel(0);	// camera acceleration and velocity vectors
float _fov = 60.f;		// field of view (zoom)
float fogColour = (0.2f, 0.22f, 0.1f);
float fogDensity = 0.3f;
float currentTime = 0.0f;
// light switches
float fAmbient = 1, fDir = 1, fPoint1 = 1, fPoint2 = 1;

const float PERIOD = 0.00075f;
const float LIFETIME = 6;
const float M_PI = 3.14;
const int NPARTICLES = (int)(LIFETIME / PERIOD);


bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!
	glEnable(GL_CLIP_PLANE0);



	// Initialise Shaders
	GLuint idTexGrass, idTexSand;


	C3dglShader vertexShader;
	C3dglShader fragmentShader;

	bm2.load("models/grass.png", GL_RGBA);
	bm1.load("models/Sand.png", GL_RGBA);
	bm5.load("models/water.bmp", GL_RGBA);

	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/basic.vert")) return false;
	if (!vertexShader.compile()) return false;

	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/basic.frag")) return false;
	if (!fragmentShader.compile()) return false;

	if (!programBasic.create()) return false;
	if (!programBasic.attach(vertexShader)) return false;
	if (!programBasic.attach(fragmentShader)) return false;
	if (!programBasic.link()) return false;
	if (!programBasic.use(true)) return false;



	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/particles.vert")) return false;
	if (!vertexShader.compile()) return false;

	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/particles.frag")) return false;
	if (!fragmentShader.compile()) return false;

	if (!programParticle.create()) return false;
	if (!programParticle.attach(vertexShader)) return false;
	if (!programParticle.attach(fragmentShader)) return false;
	if (!programParticle.link()) return false;
	if (!programParticle.use(true)) return false;

	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/water.vert")) return false;
	if (!vertexShader.compile()) return false;

	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/water.frag")) return false;
	if (!fragmentShader.compile()) return false;

	if (!programWater.create()) return false;
	if (!programWater.attach(vertexShader)) return false;
	if (!programWater.attach(fragmentShader)) return false;
	if (!programWater.link()) return false;
	if (!programWater.use(true)) return false;



	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;
	if (!vertexShader.loadFromFile("shaders/terrain.vert")) return false;
	if (!vertexShader.compile()) return false;

	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;
	if (!fragmentShader.loadFromFile("shaders/terrain.frag")) return false;
	if (!fragmentShader.compile()) return false;

	if (!programTerrain.create()) return false;
	if (!programTerrain.attach(vertexShader)) return false;
	if (!programTerrain.attach(fragmentShader)) return false;
	if (!programTerrain.link()) return false;
	if (!programTerrain.use(true)) return false;


	// glut additional setup
	glutSetVertexAttribCoord3(program.getAttribLocation("aVertex"));
	glutSetVertexAttribNormal(program.getAttribLocation("aNormal"));
	glutSetVertexAttribCoord3(programTerrain.getAttribLocation("aVertex"));
	glutSetVertexAttribNormal(programTerrain.getAttribLocation("aNormal"));

	// load your 3D models here!
	if (!pier.load("models\\pier.obj")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!chicken.load("models\\chicken.obj")) return false;
	if (!lamp.load("models\\lamp.obj")) return false;
	if (!room.load("models\\LivingRoomObj\\LivingRoom.obj")) return false;
	if (!stove.load("models\\stove.fbx")) return false;
	if (!mirror.load("models\\mirror.obj")) return false;
	if (!mirror2.load("models\\mirror2.obj")) return false;
	if (!mirror3.load("models\\mirror3.obj")) return false;
	
	
	stove.loadMaterials("models\\GasStove_Base_Color.png");

	room.loadMaterials("models\\LivingRoomObj\\");

	programBasic.use();
	if (!character.load("models\\fishing.fbx")) return false;
	character.loadAnimations();
	character.loadMaterials("models\\");

	programTerrain.use();
	if (!terrain.load("models\\skull3.png", 10)) return false;
	if (!water.load("models\\watermap.png", 10, &programWater)) return false;

	if (!skybox.load("models\\Tropical\\TropicalSunnyDayRight1024.jpg",
		"models\\Tropical\\TropicalSunnyDayFront1024.jpg",
		"models\\Tropical\\TropicalSunnyDayLeft1024.jpg",
		"models\\Tropical\\TropicalSunnyDayBack1024.jpg",
		"models\\Tropical\\TropicalSunnyDayUp1024.jpg",
		"models\\Tropical\\TropicalSunnyDayDown1024.jpg")) return false;

	programBasic.sendUniform("texture0", 0);



	// setup lights (for basic and terrain programs only, water does not use these lights):
	programBasic.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1));
	programBasic.sendUniform("lightDir.direction", vec3(1.0, 0.5, 1.0));
	programBasic.sendUniform("lightDir.diffuse", vec3(1.0, 1.0, 1.0));
	programParticle.sendUniform("initialPos", vec3(0.0, 21.58, 0.0));
	programParticle.sendUniform("gravity", vec3(0.0, 0.0, 0.0));
	programParticle.sendUniform("particleLifetime", LIFETIME);
	programTerrain.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1));
	programTerrain.sendUniform("lightDir.direction", vec3(1.0, 0.5, 1.0));
	programTerrain.sendUniform("lightDir.diffuse", vec3(1.0, 1.0, 1.0));
	programWater.sendUniform("lightAmbient.color", vec3(1.0, 1.0, 1.0));


	// setup materials (for basic and terrain programs only, water does not use these materials):
	programBasic.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	programTerrain.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	programWater.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	programBasic.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));
	programTerrain.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));


	programWater.sendUniform("waterColor", vec3(0.2f, 0.22f, 0.02f));
	programWater.sendUniform("skyColor", vec3(0.2f, 0.6f, 1.f));
	programTerrain.sendUniform("waterColor", vec3(0.2f, 0.22f, 0.02f));
	programTerrain.sendUniform("waterLevel", waterLevel);


	

	
	glDepthMask(GL_FALSE);
	glGenTextures(1, &idTexParticle);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, idTexParticle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm5.getWidth(), bm5.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm5.getBits());
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE);
	glPointSize(5);
	glDepthMask(GL_TRUE);

	programParticle.sendUniform("texture0", 3);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &idTexSand);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm1.getWidth(), bm1.getHeight(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, bm1.getBits());
	programTerrain.sendUniform("textureBed", 1);

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &idTexGrass);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm2.getWidth(), bm2.getHeight(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, bm2.getBits());
	programTerrain.sendUniform("textureShore", 2);

	// revert to texture unit 0
	glActiveTexture(GL_TEXTURE0);

	// Create a framebuffer object (FBO)
	glGenFramebuffers(1, &idFBO);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, idFBO);
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// create pyramid
	float vermals[] = {
	  -4, 0,-4, 0, 4,-7, 4, 0,-4, 0, 4,-7, 0, 7, 0, 0, 4,-7, 
	  -4, 0, 4, 0, 4, 7, 4, 0, 4, 0, 4, 7, 0, 7, 0, 0, 4, 7, 
	  -4, 0,-4,-7, 4, 0,-4, 0, 4,-7, 4, 0, 0, 7, 0,-7, 4, 0,
	   4, 0,-4, 7, 4, 0, 4, 0, 4, 7, 4, 0, 0, 7, 0, 7, 4, 0, 
	  -4, 0,-4, 0,-1, 0,-4, 0, 4, 0,-1, 0, 4, 0,-4, 0,-1, 0, 
	   4, 0, 4, 0,-1, 0,-4, 0, 4, 0,-1, 0, 4, 0,-4, 0,-1, 0 };

	vec3 p(-6.4f, 3.86f, -2.5f);
	vec3 n(sin(30) * cos(10), sin(10), cos(10) * cos(10));

	float a = n.x, b = n.y, c = n.z, d = -dot(p, n);

	mat4 matrixReflection = mat4(
		1 - 2 * a * a, -2 * a * b, -2 * a * c, 0,
		-2 * a * b, 1 - 2 * b * b, -2 * b * c, 0,
		-2 * a * c, -2 * b * c, 1 - 2 * c * c, 0,
		-2 * a * d, -2 * b * d, -2 * c * d, 1);


	program.sendUniform("planeClip", vec4(a, b, c, d));
	// Generate 1 buffer name
	glGenBuffers(1, &nPyramidBuf);
	// Bind (activate) the buffer
	glBindBuffer(GL_ARRAY_BUFFER, nPyramidBuf);
	// Send data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vermals), vermals, GL_STATIC_DRAW);

	// Setup Lights - note that diffuse and specular values are moved to onRender
	program.sendUniform("lightEmissive.color", vec3(0));
	program.sendUniform("lightDir.direction", vec3(1.0, 0.5, 1.0));
	program.sendUniform("lightPoint1.position", vec3(-2.95, 4.24, -1.0));
	program.sendUniform("lightPoint2.position", vec3(1.05, 4.24, 1.0));

	// Setup materials
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialSpecular", vec3(0.0, 0.0, 0.0));
	program.sendUniform("shininess", 10.0);

	// create & load textures
	C3dglBitmap bm;
	C3dglBitmap bm3;
	C3dglBitmap bm5;
	glActiveTexture(GL_TEXTURE0);
	
	
	// cloth texture
	bm.load("models/cloth.png", GL_RGBA);
	if (!bm.getBits()) return false;
	glGenTextures(1, &idTexCloth);
	glBindTexture(GL_TEXTURE_2D, idTexCloth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.getWidth(), bm.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.getBits());

	// wood texture
	bm.load("models/oak.png", GL_RGBA);
	if (!bm.getBits()) return false;
	glGenTextures(1, &idTexWood);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.getWidth(), bm.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.getBits());

	bm3.load("models/mirror.jpg", GL_RGBA);
	if (!bm3.getBits()) return false;
	glGenTextures(1, &idTexWoodM);
	glBindTexture(GL_TEXTURE_2D, idTexWoodM);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm3.getWidth(), bm3.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm3.getBits());

	// none (simple-white) texture
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 0, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	glGenTextures(1, &idTexMirrorr);
	glBindTexture(GL_TEXTURE_2D, idTexMirrorr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);



	// Send the texture info to the shaders
	program.sendUniform("texture0", 0);

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1), radians(12.f), vec3(1, 0, 0));
	matrixView *= lookAt(
		vec3(-1.75, 5.0, 6.0),
		vec3(-1.75, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.2f, 0.6f, 1.f, 1.0f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift to speed up your movement" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;
	cout << "  1 to switch the lamp #1 on/off" << endl;
	cout << "  2 to switch the lamp #2 on/off" << endl;
	cout << "  9 to switch directional light on/off" << endl;
	cout << "  0 to switch ambient light on/off" << endl;
	cout << endl;
	
	return true;
}

void renderScene(mat4& matrixView, float time, float deltaTime)
{
	mat4 m;

	



	if (daylight)
	{

		programBasic.sendUniform("lightAmbient.color", vec3(1.0, 1.0, 1.0));
		programBasic.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
		programBasic.sendUniform("materialDiffuse", vec3(0.0, 0.0, 0.0));

		programBasic.use();
		m = matrixView;
		skybox.render(m);

		programBasic.sendUniform("lightDir.direction", vec3(1.0, 0.5, 1.0));
		programBasic.sendUniform("lightDir.diffuse", vec3(0.2, 0.2, 0.2));
		programBasic.sendUniform("matrixView", matrixView);
		programBasic.sendUniform("materialDiffuse", vec3(0.2, 0.2, 0.6));
		programBasic.sendUniform("lightAmbient.color", vec3(0.4, 0.4, 0.4));
	}

	// setup lights
	program.sendUniform("lightAmbient.color", fAmbient * vec3(0.05, 0.05, 0.05));
	program.sendUniform("lightDir.diffuse", fDir * vec3(0.3, 0.3, 0.3));	  // dimmed white light
	program.sendUniform("lightPoint1.diffuse", fPoint1 * vec3(0.5, 0.5, 0.5));
	program.sendUniform("lightPoint1.specular", fPoint1 * vec3(1.0, 1.0, 1.0));
	program.sendUniform("lightPoint2.diffuse", fPoint2 * vec3(0.5, 0.5, 0.5));
	program.sendUniform("lightPoint2.specular", fPoint2 * vec3(1.0, 1.0, 1.0));

	// setup materials
	program.sendUniform("materialSpecular", vec3(0.0, 0.0, 0.0));

	//Stove
	program.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialSpecular", vec3(0.0f, 0.0f, 0.0f));
	m = matrixView;
	m = translate(m, vec3(0, 12, 21.5));
	m = scale(m, vec3(0.05, 0.05, 0.05));
		stove.render(m);

	// room
	program.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	
	m = matrixView;
	m = glm::translate(m, glm::vec3(0, 10, 25));
	m = scale(m, vec3(0.03f, 0.03f, 0.03f));
	
	room.render(m);
	

	programTerrain.sendUniform("materialDiffuse", vec3(0.2f, 0.3f, 0.2f));

	programTerrain.use();
	m = matrixView;
	terrain.render(m);

	programWater.use();
	m = matrixView;
	m = translate(m, vec3(0, waterLevel, 0));
	m = scale(m, vec3(2.7f, 0.7f, 2.5f));
	programWater.sendUniform("matrixModelView", m);
	water.render(m);

	programWater.sendUniform("waterColor", vec3(0.2f, 0.22f, 0.02f));
	programWater.sendUniform("skyColor", vec3(0.2f, 0.6f, 1.f));
	programTerrain.sendUniform("waterColor", vec3(0.2f, 0.22f, 0.02f));
	programTerrain.sendUniform("waterLevel", waterLevel);
	programBasic.use();
	// table & chairs
	program.sendUniform("materialDiffuse", vec3(1.0, 1.0, 1.0));
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	m = matrixView;
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	m = translate(m, vec3(0, 2500, 7500));
	table.render(1, m);
	glBindTexture(GL_TEXTURE_2D, idTexCloth);
	table.render(0, m);
	m = rotate(m, radians(180.f), vec3(0.f, 1.f, 0.f));
	table.render(0, m);
	m = translate(m, vec3(250, 0, 0));
	m = rotate(m, radians(90.f), vec3(0.f, 1.f, 0.f));
	table.render(0, m);
	m = translate(m, vec3(0, 0, -500));
	m = rotate(m, radians(180.f), vec3(0.f, 1.f, 0.f));
	table.render(0, m);


	glBindTexture(GL_TEXTURE_2D, idTexWood);
	m = matrixView;
		float angle = glm::radians(90.0f);
	glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
	m = translate(m, vec3(-33.0, 3.4, -6.0));
	m = rotate(m, angle, axis);
	pier.render(0, m);
	
	
	// mirror
	glBindTexture(GL_TEXTURE_2D, idTexWoodM);
	m = matrixView;
	m = translate(m, vec3(-8.0f, 10.0f, 25.0f));
	m = rotate(m, radians(90.f), vec3(0.f, 1.f, 0.f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	mirror.render(1, m);

	glBindTexture(GL_TEXTURE_2D, idTexWoodM);
	m = matrixView;
	m = translate(m, vec3(-8.0f, 10.0f, 25.0f));
	m = rotate(m, radians(90.f), vec3(0.f, 1.f, 0.f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	mirror2.render(1, m);

	glBindTexture(GL_TEXTURE_2D, idTexMirrorr);
	m = matrixView;
	m = translate(m, vec3(-8.0f, 10.0f, 25.0f));
	m = rotate(m, radians(90.f), vec3(0.f, 1.f, 0.f));
	m = scale(m, vec3(0.004f, 0.004f, 0.004f));
	mirror3.render(m);

	// vase
	program.sendUniform("materialDiffuse", vec3(0.2, 0.4, 0.8));
	program.sendUniform("materialAmbient", vec3(0.2, 0.4, 0.8));
	program.sendUniform("materialSpecular", vec3(1.0, 1.0, 1.0));
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	m = matrixView;
	m = translate(m, vec3(0.f, 13.f, 30.f));
	m = scale(m, vec3(0.12f, 0.12f, 0.12f));
	vase.render(m);

	// teapot
	program.sendUniform("materialDiffuse", vec3(0.1, 0.8, 0.3));
	program.sendUniform("materialAmbient", vec3(0.1, 0.8, 0.3));
	program.sendUniform("materialSpecular", vec3(1.0, 1.0, 1.0));
	m = matrixView;
	m = translate(m, vec3(1.8f, 13.4f, 25.0f));
	program.sendUniform("matrixModelView", m);
	glutSolidTeapot(0.5);

	// pyramid
	program.sendUniform("materialDiffuse", vec3(1.0, 0.2, 0.2));
	program.sendUniform("materialAmbient", vec3(1.0, 0.2, 0.2));
	program.sendUniform("materialSpecular", vec3(0.0, 0.0, 0.0));
	m = matrixView;
	m = translate(m, vec3(-29.0, 3.7, -18.0));
	m = rotate(m, radians(180.f), vec3(1, 0, 0));
	m = rotate(m, radians(-40 * time), vec3(0, 1, 0));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	program.sendUniform("matrixModelView", m);

	GLuint attribVertex = program.getAttribLocation("aVertex");
	GLuint attribNormal = program.getAttribLocation("aNormal");
	glBindBuffer(GL_ARRAY_BUFFER, nPyramidBuf);
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribNormal);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glDrawArrays(GL_TRIANGLES, 0, 18);
	glDisableVertexAttribArray(GL_VERTEX_ARRAY);
	glDisableVertexAttribArray(GL_NORMAL_ARRAY);

	// chicken
	program.sendUniform("materialDiffuse", vec3(0.8, 0.8, 0.2));
	program.sendUniform("materialAmbient", vec3(0.8, 0.8, 0.2));
	program.sendUniform("materialSpecular", vec3(0.6, 0.6, 1.0));
	m = translate(m, vec3(15.0f, -5.0f, 0.0f));
	m = rotate(m, radians(180.f), vec3(1, 0, 0));
	m = scale(m, vec3(0.2f, 0.2f, 0.2f));
	
	chicken.render(m);

	// lamp 1
	m = matrixView;
	m = translate(m, vec3(-2.2f, 13.075f, 30.0f));
	m = scale(m, vec3(0.02f, 0.02f, 0.02f));
	lamp.render(m);

	// lamp 2
	m = matrixView;
	m = translate(m, vec3(1.8f, 13.075f, 30.0f));
	m = scale(m, vec3(0.02f, 0.02f, 0.02f));
	lamp.render(m);

	

	// light bulb 1
	program.sendUniform("materialDiffuse", vec3(0.8, 0.8, 0.8));
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	program.sendUniform("lightEmissive.color", vec3(fPoint1));
	m = matrixView;
	m = translate(m, vec3(-2.95f, 13.24f, 30.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	program.sendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);

	// light bulb 2
	program.sendUniform("materialDiffuse", vec3(0.8, 0.8, 0.8));
	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));
	program.sendUniform("lightEmissive.color", vec3(fPoint2));
	m = matrixView;
	m = translate(m, vec3(1.05f, 13.24f, 30.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	program.sendUniform("matrixModelView", m);
	glutSolidSphere(1, 32, 32);
	program.sendUniform("lightEmissive.color", vec3(0));

	

	programBasic.use();
	programBasic.sendUniform("materialDiffuse", vec3(1.0f));
	std::vector<mat4> transforms;
	character.getAnimData(0, time, transforms);
	programBasic.sendUniform("bones", &transforms[0], transforms.size());
	programBasic.use();
	float angle2 = glm::radians(180.0f);
	m = matrixView;
	m = translate(m, vec3(-32.0, 5.5, -10.5));
	m = scale(m, vec3(0.013f, 0.013f, 0.013f));
	m = rotate(m, angle2, axis);

	character.render(m);

	std::vector<float> bufferVelocity;
	std::vector<float> bufferStartTime;

	for (int i = 0; i < NPARTICLES; i++)
	{
		float theta = (float)M_PI / 1.f * (float)rand() / (float)RAND_MAX;
		float phi = (float)M_PI * 2.f * (float)rand() / (float)RAND_MAX;
		float x = sin(theta) * cos(phi);
		float y = cos(theta);
		float z = sin(theta) * sin(phi);
		float v = 900 + 0.5f * (float)rand() / (float)RAND_MAX;
		bufferVelocity.push_back(x * 90);
		bufferVelocity.push_back(y * 90);
		bufferVelocity.push_back(z * 90);
		bufferStartTime.push_back(time);
		time += PERIOD;
	}
	glGenBuffers(1, &idBufferVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferVelocity);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVelocity.size(), &bufferVelocity[0],
		GL_STATIC_DRAW);
	glGenBuffers(1, &idBufferStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferStartTime);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferStartTime.size(), &bufferStartTime[0],
		GL_STATIC_DRAW);

	programParticle.use();
	m = matrixView;
	programParticle.sendUniform("matrixModelView", m);
	// render the buffer
	GLint aVelocity = programParticle.getAttribLocation("aVelocity");
	GLint aStartTime = programParticle.getAttribLocation("aStartTime");
	glEnableVertexAttribArray(aVelocity); // velocity
	glEnableVertexAttribArray(aStartTime); // start time
	glBindBuffer(GL_ARRAY_BUFFER, idBufferVelocity);
	glVertexAttribPointer(aVelocity, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferStartTime);
	glVertexAttribPointer(aStartTime, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, NPARTICLES);
	glDisableVertexAttribArray(aVelocity);
	glDisableVertexAttribArray(aStartTime);

	// Update camera position
	vec3 cameraPosition = vec3(inverse(matrixView)[3]);

	// Update particle system position to follow the camera
	programParticle.sendUniform("initialPos", cameraPosition);

	// Rest of the renderScene function remains the same...
}

void onRender()
{
	// these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;										// framerate is 1/deltaTime
	currentTime += deltaTime;

	programParticle.sendUniform("time", time);
	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;

	float terrainY = -std::max(terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]), waterLevel);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));


	// setup View Matrix
	program.sendUniform("matrixView", matrixView);
	programTerrain.sendUniform("matrixView", matrixView);
	programWater.sendUniform("matrixView", matrixView);



	// render the scene objects
	renderScene(matrixView, time, deltaTime);

	matrixView = translate(matrixView, vec3(0, -terrainY, 0));

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();

	programWater.use();
	programWater.sendUniform("t", currentTime);
	programTerrain.use();
	programTerrain.sendUniform("t", currentTime);
}

// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 m = perspective(radians(_fov), ratio, 0.02f, 1000.f);
	programBasic.sendUniform("matrixProjection", m);
	programParticle.sendUniform("matrixProjection", m);
	programTerrain.sendUniform("matrixProjection", m);
	programWater.sendUniform("matrixProjection", m);

	program.sendUniform("matrixProjection", perspective(radians(_fov), ratio, 0.02f, 1000.f));
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': _acc.z = accel; break;
	case 's': _acc.z = -accel; break;
	case 'a': _acc.x = accel; break;
	case 'd': _acc.x = -accel; break;
	case 'e': _acc.y = accel; break;
	case 'q': _acc.y = -accel; break;
	case '1': fPoint1 = 1 - fPoint1; break;
	case '2': fPoint2 = 1 - fPoint2; break;
	case '9': fDir = 1 - fDir; break;
	case '0': fAmbient = 1 - fAmbient; break;
	}
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': _acc.z = _vel.z = 0; break;
	case 'a':
	case 'd': _acc.x = _vel.x = 0; break;
	case 'q':
	case 'e': _acc.y = _vel.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
void onMouse(int button, int state, int x, int y)
{
	glutSetCursor(state == GLUT_DOWN ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	if (button == 1)
	{
		_fov = 60.0f;
		onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

// handle mouse move
void onMotion(int x, int y)
{
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

	// find delta (change to) pan & pitch
	float deltaYaw = 0.005f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
	float deltaPitch = 0.005f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

	if (abs(deltaYaw) > 0.3f || abs(deltaPitch) > 0.3f)
		return;	// avoid warping side-effects

	// View = Pitch * DeltaPitch * DeltaYaw * Pitch^-1 * View;
	constexpr float maxPitch = radians(80.f);
	float pitch = getPitch(matrixView);
	float newPitch = glm::clamp(pitch + deltaPitch, -maxPitch, maxPitch);
	matrixView = rotate(rotate(rotate(mat4(1.f),
		newPitch, vec3(1.f, 0.f, 0.f)),
		deltaYaw, vec3(0.f, 1.f, 0.f)), 
		-pitch, vec3(1.f, 0.f, 0.f)) 
		* matrixView;
}

void onMouseWheel(int button, int dir, int x, int y)
{
	_fov = glm::clamp(_fov - dir * 5.f, 5.0f, 175.f);
	onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: WOOOO!MK1");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		C3dglLogger::log("GLEW Error {}", (const char*)glewGetErrorString(err));
		return 0;
	}
	C3dglLogger::log("Using GLEW {}", (const char*)glewGetString(GLEW_VERSION));

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	C3dglLogger::log("Vendor: {}", (const char *)glGetString(GL_VENDOR));
	C3dglLogger::log("Renderer: {}", (const char *)glGetString(GL_RENDERER));
	C3dglLogger::log("Version: {}", (const char*)glGetString(GL_VERSION));
	C3dglLogger::log("");

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		C3dglLogger::log("Application failed to initialise\r\n");
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

