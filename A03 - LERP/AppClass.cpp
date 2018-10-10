#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Max Kaiser - mrk5790@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}

	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	//create stops list for each orbit
	for (int i = 0; i < m_uOrbits; i++)
	{
		curStops.push_back(0); //for every orbit, set the current stop to 0 (first element on that orbit)
		curStopsReverse.push_back(0); //for every orbit, set the current stop to 0 (first element on that orbit)
		float angleStep = (2 * PI) / (uSides + i); //internal angle of taurus base
		float rad = (1.0f + (0.5f * i)) - .005f; //start with radius of 1 for first orbit, increase by .5 for each subsequent orbit
		for (int j = 0; j < uSides + i; j++) //for each side/subdivide calculate a point around the orbit
		{
			float sine = sin(angle);
			float cosine = cos(angle);
			vector3 tempVec(sine * rad ,cosine * rad, 0); //point to be stored
			stopsList.push_back(tempVec); //add point to a vector containing points
			angle += angleStep;
		}

		stopsManager.push_back(stopsList); //add calculated vector above (stopsList) to the overall vector containg the orbits
		stopsList.clear(); //clear temp vector for next orbit
	}
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

	//Get a timer
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	static float percent = 0.0f; //percent towards lerp goal

	if (fTimer - curTime > .001)
	{
		curTime = fTimer;
		percent += .01;

		if (percent >= 1) //if you have reached the lerp goal, reset percent and either update to the next stop or reset the stops
		{
			percent = 0.0f;

			for (uint i = 0; i < m_uOrbits; i++) //updates the current stop for each orbit
			{
				if (curStops[i] + 1 > stopsManager[i].size() - 1)
				{
					curStops[i] = 0; //reset the stops if you were about to leave the available stop index
				}
				else
				{
					curStops[i] = curStops[i] + 1; //next stop
				}
			}

			for (uint j = 0; j < m_uOrbits; j++) //Same as above loop, but for elements moving in reverse
			{
				if (curStopsReverse[j] - 1 < 0)
				{
					curStopsReverse[j] = (3 + j) - 1; //reset the stops to the last index if you were about to leave the available stop index
				}
				else
				{
					curStopsReverse[j] = curStopsReverse[j] - 1; //next stop
				}
			}
		}
	}

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//calculate the current position
		if (curStops[i] + 1 >= stopsManager[i].size()) //Close the loop by lerping back to the beginning
		{
			v3CurrentPos = glm::lerp(stopsManager[i][curStops[i]], stopsManager[i][0], percent);
			
		}
		else //lerp to the next stop
		{
			v3CurrentPos = glm::lerp(stopsManager[i][curStops[i]], stopsManager[i][curStops[i] + 1], percent);
		}

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	for (uint j = 0; j < m_uOrbits; ++j)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[j], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//calculate the current position
		if (curStopsReverse[j] - 1 < 0) //if about to go out of index, loop back around
		{
			v3CurrentPos = glm::lerp(stopsManager[j][0], stopsManager[j][(j + 3) - 1], percent); //lerp from first element to last

		}
		else 
		{
			v3CurrentPos = glm::lerp(stopsManager[j][curStopsReverse[j]], stopsManager[j][curStopsReverse[j] - 1], percent); //lerp to previous element
		}

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}