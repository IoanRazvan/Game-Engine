// NewTrainingFramework.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "../Utilities/utilities.h" // if you use STL, please include this line AFTER all other include
#include "Vertex.h"
#include "Shaders.h"
#include <conio.h>
#include "Math.h"
#include "Globals.h"
#include "Camera.h"
#include "NFG.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include <iostream>

float Globals::alpha = 0.0f;
ResourceManager* rm = ResourceManager::getInstance();
SceneManager* sm = SceneManager::getInstance();

int Init(ESContext* esContext)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	rm->Init("../Configuration Files/resourceManager.xml");
	sm->Init("../Configuration Files/sceneManager.xml");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 0;
}

void Draw(ESContext* esContext)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sm->Draw();

	eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}

void Update(ESContext* esContext, float deltaTime)
{
	sm->setActiveCameraDeltaTime(deltaTime);
	sm->Update();
}

void Key(ESContext* esContext, unsigned char key, bool bIsPressed)
{
	if (bIsPressed)
		sm->RegisterKey(key);
}

void CleanUp()
{
	delete rm;
	delete sm;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//identifying memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	ESContext esContext;

	esInitContext(&esContext);
	

	esCreateWindow(&esContext,"Hello World!", 960, 720, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	
	if (Init(&esContext) != 0)
		return 0;
	

	esRegisterDrawFunc(&esContext, Draw);
	esRegisterUpdateFunc(&esContext, Update);
	esRegisterKeyFunc(&esContext, Key);

	esMainLoop(&esContext);

	//releasing OpenGL resources
	CleanUp();


	printf("Press any key...\n");
	_getch();


	return 0;
}