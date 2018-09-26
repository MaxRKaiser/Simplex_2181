#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	float height = a_fHeight;
	float rad = a_fRadius;
	int subDiv = a_nSubdivisions;
	std::vector<vector3> coneVec; //for storing points in circle base
	float angleStep = (2*PI) / subDiv; //internal angle of circle base
	float x = 0;
	float z = 0;

	vector3 point01(0, height * .5, 0); //top of cone
	vector3 point02(0, -height * .5, 0); //bottom of cone

	for (float angle = 0.0f; angle < 2*PI; angle += angleStep)
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height * .5, z + (cosine * rad)); //point to be stored
		coneVec.push_back(tempVec);
	}
	
	for (int i = 0; i < coneVec.size(); i++) //loop through points vector and create tris
	{
		if (i + 1 != coneVec.size()) 
		{
			AddTri(point02, coneVec[i + 1], coneVec[i]);
			AddTri(point01, coneVec[i], coneVec[i + 1]);
		}
		
		else
		{
			AddTri(point02, coneVec[0], coneVec[i]);
			AddTri(point01, coneVec[i], coneVec[0]);
		}
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	float height = a_fHeight;
	float rad = a_fRadius;
	int subDiv = a_nSubdivisions;
	std::vector<vector3> cylVec1; //For storing top cylinder circle
	std::vector<vector3> cylVec2; //For storing bottom cylinder circle
	float angleStep = (2 * PI) / subDiv;
	float x = 0;
	float z = 0;

	vector3 point01(0, height * .5, 0); //top of cylinder
	vector3 point02(0, -height * .5, 0); //bottom of cylinder

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //Top circle points
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), height * .5, z + (cosine * rad)); //point to be stored
		cylVec1.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //Bottom circle points
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height * .5, z + (cosine * rad)); //point to be stored
		cylVec2.push_back(tempVec);
	}

	for (int i = 0; i < cylVec1.size(); i++) 
	{
		if (i + 1 != cylVec1.size())
		{
			AddTri(point01, cylVec1[i], cylVec1[i + 1]); //top
			AddTri(point02, cylVec2[i + 1], cylVec2[i]); //bottom
			AddQuad(cylVec2[i], cylVec2[i + 1], cylVec1[i], cylVec1[i + 1]); //sides
		}

		else
		{
			AddTri(point01, cylVec1[i], cylVec1[0]); //top
			AddTri(point02, cylVec2[0], cylVec2[i]); //bottom
			AddQuad(cylVec2[i], cylVec2[0], cylVec1[i], cylVec1[0]); //sides
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	float height = a_fHeight;
	float outRad = a_fOuterRadius;
	float inRad = a_fInnerRadius;
	int subDiv = a_nSubdivisions;
	std::vector<vector3> tubVec11; //inner rad top
	std::vector<vector3> tubVec12; //outer rad top
	std::vector<vector3> tubVec21; //inner rad bottom
	std::vector<vector3> tubVec22; //outer rad bottom
	float angleStep = (2 * PI) / subDiv;
	float x = 0;
	float z = 0;

	//vector3 point01(0, height * .5, 0); //top of cylinder
	//vector3 point02(0, -height * .5, 0); //bottom of cylinder

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //inner circle top
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * inRad), height * .5, z + (cosine * inRad)); //point to be stored
		tubVec11.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //outer circle top
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * outRad), height * .5, z + (cosine * outRad)); //point to be stored
		tubVec12.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //inner circle bottom
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * inRad), -height * .5, z + (cosine * inRad)); //point to be stored
		tubVec21.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //outer circle bottom
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * outRad), -height * .5, z + (cosine * outRad)); //point to be stored
		tubVec22.push_back(tempVec);
	}

	for (int i = 0; i < tubVec11.size(); i++)
	{
		if (i + 1 != tubVec11.size())
		{
			AddQuad(tubVec22[i], tubVec22[i + 1], tubVec12[i], tubVec12[i + 1]); //outer wall
			AddQuad(tubVec11[i], tubVec11[i + 1], tubVec21[i], tubVec21[i + 1]); //inner wall
			AddQuad(tubVec12[i], tubVec12[i + 1], tubVec11[i], tubVec11[i + 1]); //top cap
			AddQuad(tubVec21[i], tubVec21[i + 1], tubVec22[i], tubVec22[i + 1]); //bottom cap
		}

		else
		{
			AddQuad(tubVec22[i], tubVec22[0], tubVec12[i], tubVec12[0]); //outer wall
			AddQuad(tubVec11[i], tubVec11[0], tubVec21[i], tubVec21[0]); //inner wall
			AddQuad(tubVec12[i], tubVec12[0], tubVec11[i], tubVec11[0]); //top cap
			AddQuad(tubVec21[i], tubVec21[0], tubVec22[i], tubVec22[0]); //bottom cap
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	float outRad = a_fOuterRadius;
	float inRad = a_fInnerRadius;
	float midRad = (outRad + inRad) / 2;
	int subDiv = a_nSubdivisionsA;
	std::vector<vector3> tubVec11; //circle top
	std::vector<vector3> tubVec12; //inner circle side
	std::vector<vector3> tubVec21; //circle bottom
	std::vector<vector3> tubVec22; //outer circle side
	float angleStep = (2 * PI) / subDiv;
	float x = 0;
	float z = 0;


	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) // circle top
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * midRad), .2, z + (cosine * midRad)); //point to be stored
		tubVec11.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //inner circle side
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * inRad), 0, z + (cosine * inRad)); //point to be stored
		tubVec12.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) // circle bottom
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * midRad), -.2, z + (cosine * midRad)); //point to be stored
		tubVec21.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //outer circle side
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * outRad), 0, z + (cosine * outRad)); //point to be stored
		tubVec22.push_back(tempVec);
	}

	for (int i = 0; i < tubVec11.size(); i++)
	{
		if (i + 1 != tubVec11.size())
		{
			AddQuad(tubVec22[i], tubVec22[i + 1], tubVec11[i], tubVec11[i + 1]); 
			AddQuad(tubVec11[i], tubVec11[i + 1], tubVec12[i], tubVec12[i + 1]); 
			AddQuad(tubVec12[i], tubVec12[i + 1], tubVec21[i], tubVec21[i + 1]); 
			AddQuad(tubVec21[i], tubVec21[i + 1], tubVec22[i], tubVec22[i + 1]); 
		}

		else
		{
			AddQuad(tubVec22[i], tubVec22[0], tubVec11[i], tubVec11[0]); 
			AddQuad(tubVec11[i], tubVec11[0], tubVec12[i], tubVec12[0]); 
			AddQuad(tubVec12[i], tubVec12[0], tubVec21[i], tubVec21[0]); 
			AddQuad(tubVec21[i], tubVec21[0], tubVec22[i], tubVec22[0]); 
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	float rad = a_fRadius;
	int subDiv = a_nSubdivisions;
	float tempRad;
	float angleStep = (2 * PI) / subDiv;
	float x = 0;
	float z = 0;
	std::vector<std::vector<vector3>> vecVector; //vector to store vectors storing points (kill me)

	for (int i = 0; i < subDiv; i++)
	{
		std::vector<vector3> newVec; //For storing circle points
		vecVector.push_back(newVec);
	}

	vector3 point01(0, rad * .75, 0); //top of sphere
	vector3 point02(0, -rad * .75, 0); //bottom of sphere

	for (int i = 0; i < vecVector.size(); i++)
	{

		if (subDiv == 3) //hardcoded, if small shape, adjust radius
		{
			if (i == 0 || i == 2)
				tempRad = rad * .5;
			if (i == 1)
				tempRad = rad * .8;
		}
		else if (subDiv == 5) //hardcoded, if large shape, adjust radius
		{
			if (i == 0 || i == 4)
				tempRad = rad * .333333;
			if (i == 1 || i == 3)
				tempRad = rad * .6666666;
			if (i == 2)
				tempRad = rad * .8;
		}
		else //if shape is changed, default to prism
		{
			tempRad = rad * .8;
		}

		for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //create indavidual 2d circles
		{
			float sine = sin(angle);
			float cosine = cos(angle);

			vector3 tempVec(x + (sine * tempRad), rad - (((rad * 2) / (subDiv + 1)) * (i + 1)), z + (cosine * tempRad)); //point to be stored
			vecVector[i].push_back(tempVec);
		}
	}

	for (int q = 0; q < vecVector.size(); q++) //loop through vector containing vectors
	{
		for (int i = 0; i < vecVector[q].size(); i++) //loop through vector containing points
		{
			if (i + 1 != vecVector[q].size()) //if is not last subdivision
			{
				if (q == 0) //if first subdivision
				{
					AddTri(vecVector[0][i], vecVector[0][i + 1], point01);
					AddQuad(vecVector[q + 1][i], vecVector[q + 1][i + 1], vecVector[q][i], vecVector[q][i + 1]);
				}
				else if (q + 1 != vecVector.size())
				{
					AddQuad(vecVector[q + 1][i], vecVector[q + 1][i + 1], vecVector[q][i], vecVector[q][i + 1]);
				}
				else if (q == vecVector.size() - 1)
				{
					AddTri(vecVector[q][i + 1], vecVector[q][i], point02);
				}
			}
			else
			{
				if (q == 0)
				{
					AddTri(vecVector[0][i], vecVector[0][0], point01);
					AddQuad(vecVector[q + 1][i], vecVector[q + 1][0], vecVector[q][i], vecVector[q][0]);
				}
				else if (q + 1 != vecVector.size())
				{
					AddQuad(vecVector[q + 1][i], vecVector[q + 1][0], vecVector[q][i], vecVector[q][0]);
				}
				else if (q == vecVector.size() - 1)
				{
					AddTri(vecVector[q][0], vecVector[q][i], point02);
				}
			}
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}