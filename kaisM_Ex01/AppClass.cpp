#include "AppClass.h"
#include <SFML/Audio.hpp>
#include <iostream>
using namespace Simplex;
void Application::InitVariables(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This part initialize the camera position so I can see the scene better; 
		the guide cube and the tip of the pencil so all solutions on the exam 
		look the same to the grader
	*/
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 35.0f, ZERO_V3, AXIS_Y);
		
	m_pGuideCube = new MyMesh();
	m_pGuideCube->GenerateCube(10.0f, C_WHITE);

	m_pMesh = new MyMesh();
	m_pMesh->GenerateCone(1.0f, 2.0f, 8, vector3(0.0f, 0.0f, 0.0f));

	myPoints = m_pGuideCube->GetVertexList();

#pragma endregion

	//Please change to your name and email
	m_sProgramer = "Max Kaiser - mrk5790@rit.edu";

	if (!m_soundBGM.openFromFile("..\\_Binary\\Data\\Audio\\mySound.wav"))
	{
		std::cout << "error" << std::endl; // error
	}
	m_soundBGM.play();
	std::cout << m_soundBGM.getVolume();
}
void Application::Update(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This updates the internal clock of the Simplex library and process the Arcball
	*/
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();
#pragma endregion
}
void Application::Display(void)
{
	static int curStop = 0;
	static float fPercentage = 0;
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	//Get a timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock
	static float fTimer = m_pSystem->GetTimeSinceStart(uClock); //get the start time for that clock 
	fTimer += m_pSystem->GetDeltaTime(uClock);
	fPercentage = MapValue(fTimer, 0.0f, m_fTimeBetweenStops, 0.0f, 1.0f);

	if (fPercentage >= 1.0f)
	{
		if (curStop != 35)
			fPercentage = 0.0f;

		if (curStop < 35)
			curStop++;

		fTimer = 0.0f;

	}

	//Add points
	//myPoints = m_pGuideCube->GetVertexList();
	
	//calculate the current position
	if (curStop + 1 >= myPoints.size())
	{
		v3CurrentPos = glm::lerp(myPoints[curStop], myPoints[0], fPercentage);
		if (fPercentage >= 1.0f)
		{
			curStop = 0;
		}
	}
	else
	{
		v3CurrentPos = glm::lerp(myPoints[curStop], myPoints[curStop + 1], fPercentage);
	}

	matrix4 m4Model = glm::translate(IDENTITY_M4, v3CurrentPos);

	//Change orientation
	
	if (curStop >= 0 && curStop < 6) //front
	{
		qOrient = glm::angleAxis(glm::radians(-90.0f), vector3(1.0f, 0.0f, 0.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	else if (curStop >= 6 && curStop < 11) //right
	{
		qOrient = glm::angleAxis(glm::radians(90.0f), vector3(0.0f, 0.0f, 1.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	else if (curStop >= 11 && curStop < 18) //top
	{
		qOrient = glm::angleAxis(glm::radians(180.0f), vector3(1.0f, 0.0f, 0.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	else if (curStop >= 18 && curStop < 24) //back
	{
		qOrient = glm::angleAxis(glm::radians(90.0f), vector3(1.0f, 0.0f, 0.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	else if (curStop >= 24 && curStop < 30) //left
	{
		qOrient = glm::angleAxis(glm::radians(-90.0f), vector3(0.0f, 0.0f, 1.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	else if (curStop >= 30 && curStop < 35) //bottom
	{
		qOrient = glm::angleAxis(glm::radians(0.0f), vector3(1.0f, 0.0f, 0.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	else if (curStop == 35) //bottom
	{
		qOrient = glm::angleAxis(glm::radians(-90.0f), vector3(1.0f, 0.0f, 0.0f));
		//m4Model *= glm::toMat4(qOrient);
	}
	
	m4Model *= glm::toMat4(m_qArcBall);
	m4Model *= glm::toMat4(qOrient);

	m_pMesh->Render(m4Projection, m4View, m4Model);
	
#pragma region DOES NOT NEED CHANGES
	/*
		This part does not need any changes at all, it is just for rendering the guide cube, the 
		skybox as a frame of reference and the gui display
	*/
	//Render the guide cube
	m_pGuideCube->Render(m4Projection, m4View, ToMatrix4(m_qArcBall)); 

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();
	//clear the render list
	m_pMeshMngr->ClearRenderList();
		
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
#pragma endregion
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();

	//I already deallocated the memory for the meshes if you allocate new memory please
	//deallocate it before ending the program
	SafeDelete(m_pMesh);
	SafeDelete(m_pGuideCube);
}