#include "AppClass.h"
void Application::InitVariables(void)
{

	//init the mesh
	count = 46;

	for (uint i = 0; i < count; i++)
	{
		MyMesh* pMesh = new MyMesh();
		meshList.push_back(pMesh);
		meshList[i]->GenerateCube(1.0f, C_RED);
	}

	meshList[0]->x = -3;
	meshList[0]->y = 4;

	meshList[1]->x = 3;
	meshList[1]->y = 4;

	meshList[2]->x = -2;
	meshList[2]->y = 3;

	meshList[3]->x = 2;
	meshList[3]->y = 3;

	meshList[4]->x = -3;
	meshList[4]->y = 2;

	meshList[5]->x = -2;
	meshList[5]->y = 2;

	meshList[6]->x = -1;
	meshList[6]->y = 2;

	meshList[7]->x = 0;
	meshList[7]->y = 2;

	meshList[8]->x = 1;
	meshList[8]->y = 2;

	meshList[9]->x = 2;
	meshList[9]->y = 2;

	meshList[10]->x = 3;
	meshList[10]->y = 2;

	meshList[11]->x = -4;
	meshList[11]->y = 1;

	meshList[12]->x = -3;
	meshList[12]->y = 1;

	meshList[13]->x = -1;
	meshList[13]->y = 1;

	meshList[14]->x = 0;
	meshList[14]->y = 1;

	meshList[15]->x = 1;
	meshList[15]->y = 1;

	meshList[16]->x = 3;
	meshList[16]->y = 1;

	meshList[17]->x = 4;
	meshList[17]->y = 1;

	meshList[18]->x = -5;
	meshList[18]->y = 0;

	meshList[19]->x = -4;
	meshList[19]->y = 0;

	meshList[20]->x = -3;
	meshList[20]->y = 0;

	meshList[21]->x = -2;
	meshList[21]->y = 0;

	meshList[22]->x = -1;
	meshList[22]->y = 0;

	meshList[23]->x = 0;
	meshList[23]->y = 0;

	meshList[24]->x = 1;
	meshList[24]->y = 0;

	meshList[25]->x = 2;
	meshList[25]->y = 0;

	meshList[26]->x = 3;
	meshList[26]->y = 0;

	meshList[27]->x = 4;
	meshList[27]->y = 0;

	meshList[28]->x = 5;
	meshList[28]->y = 0;

	meshList[29]->x = -5;
	meshList[29]->y = -1;

	meshList[30]->x = -3;
	meshList[30]->y = -1;

	meshList[31]->x = -2;
	meshList[31]->y = -1;

	meshList[32]->x = -1;
	meshList[32]->y = -1;

	meshList[33]->x = 0;
	meshList[33]->y = -1;

	meshList[34]->x = 1;
	meshList[34]->y = -1;

	meshList[35]->x = 2;
	meshList[35]->y = -1;

	meshList[36]->x = 3;
	meshList[36]->y = -1;

	meshList[37]->x = 5;
	meshList[37]->y = -1;

	meshList[38]->x = -5;
	meshList[38]->y = -2;

	meshList[39]->x = -3;
	meshList[39]->y = -2;

	meshList[40]->x = 3;
	meshList[40]->y = -2;

	meshList[41]->x = 5;
	meshList[41]->y = -2;

	meshList[42]->x = -2;
	meshList[42]->y = -3;

	meshList[43]->x = -1;
	meshList[43]->y = -3;

	meshList[44]->x = 1;
	meshList[44]->y = -3;

	meshList[45]->x = 2;
	meshList[45]->y = -3;

}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();

	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	static float value = -5.0f;
	
	//matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	//matrix4 m4Translate1 = glm::translate(IDENTITY_M4, vector3(value + 1, 2.0f, 3.0f));
	//matrix4 m4Translate2 = glm::translate(IDENTITY_M4, vector3(value + 2, 2.0f, 3.0f));
	for (uint q = 0; q < count; q++)
	{

		matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value + meshList[q]->x, meshList[q]->y + 3, 0.0f));

		matrix4 m4Model = m4Scale * m4Translate;

		meshList[q]->Render(m4Projection, m4View, m4Model);

		
	}
	value += 0.01f;

	//matrix4 m4Model = m4Translate * m4Scale;
	//matrix4 m4Model = m4Scale * m4Translate;
	//matrix4 m4Model1 = m4Scale * m4Translate1;
	//matrix4 m4Model2 = m4Scale * m4Translate2;

	//meshList[0]->Render(m4Projection, m4View, m4Model);
	//meshList[1]->Render(m4Projection, m4View, m4Model1);
	//meshList[2]->Render(m4Projection, m4View, m4Model2);
	
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
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}