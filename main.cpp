#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/system/FPSController.h"
#include "classes/buffers/StaticBuffer.h"
#include "classes/level/Cave.h"
#include "classes/level/Dungeon.h"

bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
GLFWwindow* window;
stFPSController FPSController;

int TilesCount[2] = {60, 60};//{30, 30}
glm::vec2 Edge(1, 1);//{2, 2}
glm::vec2 TileSize(10, 10);//(20, 20)
glm::vec2 MouseSceneCoord;

MShader Shader;
MScene Scene;

MCave Cave = MCave(TilesCount[0], TilesCount[1], 51, 2, 4, 30, 30);
//MDungeon Cave = MDungeon(TilesCount[0], TilesCount[1], 6, 20, 3);
MStaticBuffer RoomsBuffer;
MStaticBuffer LeafsBuffer;

bool GenerateLevel() {
	RoomsBuffer.Clear();
	
	glm::vec3 Color = glm::vec3(1, 1, 1);
	if(!Cave.Generate()) return false;
	for(int i=0; i<TilesCount[0]; i++) {
		for(int j=0; j<TilesCount[1]; j++) {
			if(!Cave.GetValue(i, j)) continue;
			RoomsBuffer.AddQuad(glm::vec2(i * TileSize.x + Edge.x, j * TileSize.y + Edge.y), glm::vec2((i+1) * TileSize.x - Edge.x, (j+1) * TileSize.y - Edge.y), Color);
		}
	}
	
	if(!RoomsBuffer.Dispose()) return false;
	
	return true;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void mousepos_callback(GLFWwindow* window, double x, double y) {
	MouseSceneCoord = Scene.WindowPosToWorldPos(x, y);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	if(key == 'R' && action == GLFW_PRESS) {
		if(!GenerateLevel()) cout<<"Error while generate level"<<endl;
	}
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.AddUnifrom("MVP", "MVP")) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
    
    //other initializations
    //init buffers
    if(!LeafsBuffer.Initialize()) return false;
    LeafsBuffer.SetPrimitiveType(GL_LINES);
    if(!RoomsBuffer.Initialize()) return false;
    RoomsBuffer.SetPrimitiveType(GL_QUADS);
    //generate level
	if(!GenerateLevel()) return false;
	
	//turn off pause
	Pause = false;
    
    return true;
}

void RenderStep() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(Shader.ProgramId);
	glUniformMatrix4fv(Shader.Uniforms["MVP"], 1, GL_FALSE, Scene.GetDynamicMVP());
	
	//draw functions
	RoomsBuffer.Begin();
		//glEnable(GL_DEPTH_TEST);
		RoomsBuffer.Draw();
		//glDisable(GL_DEPTH_TEST);
		//LeafsBuffer.Draw();
	RoomsBuffer.End();
}

void ClearApp() {
	//clear funstions
	Cave.Close();
	LeafsBuffer.Close();
	RoomsBuffer.Close();
	
	memset(keys, 0, 1024);
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		LogFile.close();
		return 0;
	}
	FPSController.Initialize(glfwGetTime());
	while(!glfwWindowShouldClose(window)) {
		FPSController.FrameStep(glfwGetTime());
    	FPSController.FrameCheck();
		RenderStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
	ClearApp();
    glfwTerminate();
    LogFile.close();
}
