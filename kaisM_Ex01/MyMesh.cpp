#include "MyMesh.h"
using namespace Simplex;
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
void MyMesh::AddQuadColor(vector3 a_v3Color)
{
	AddVertexColor(a_v3Color);
	AddVertexColor(a_v3Color);
	AddVertexColor(a_v3Color);
	AddVertexColor(a_v3Color);
	AddVertexColor(a_v3Color);
	AddVertexColor(a_v3Color);
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

	vector3 point0(-fValue, -fValue, fValue);	//0
	vector3 point1(fValue, -fValue, fValue);	//1
	vector3 point2(fValue, fValue, fValue);		//2
	vector3 point3(-fValue, fValue, fValue);	//3

	vector3 point4(-fValue, -fValue, -fValue);	//4
	vector3 point5(fValue, -fValue, -fValue);	//5
	vector3 point6(fValue, fValue, -fValue);	//6
	vector3 point7(-fValue, fValue, -fValue);	//7

	//F
	AddTri(point3, point0, point2);
	AddTri(point0, point1, point2);
	AddQuadColor(C_RED);

	//R
	AddTri(point1, point5, point2);
	AddTri(point5, point6, point2);
	AddQuadColor(C_GREEN);

	//U
	AddTri(point7, point3, point2);
	AddTri(point7, point2, point6);
	AddQuadColor(C_BLUE);

	//B
	AddTri(point7, point6, point5);
	AddTri(point7, point5, point4);
	AddQuadColor(C_ORANGE);

	//L
	AddTri(point7, point4, point0);
	AddTri(point3, point7, point0);
	AddQuadColor(C_WHITE);

	//D
	AddTri(point4, point5, point0);
	AddTri(point5, point1, point0);
	AddQuadColor(C_CYAN);

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

	//Solution replaced by generate Cube
	float height = a_fHeight;
	float rad = a_fRadius;
	int subDiv = 12;
	std::vector<vector3> coneVec; //for storing points in circle base
	float angleStep = (2 * PI) / subDiv; //internal angle of circle base
	float x = 0;
	float z = 0;

	std::vector<vector3> cylVec1; //For storing top cylinder circle
	std::vector<vector3> cylVec2; //For storing bottom cylinder circle

	std::vector<vector3> cylVec11; //For storing top cylinder circle eraser
	std::vector<vector3> cylVec22; //For storing bottom cylinder circle eraser


	vector3 point01(0, 0, 0); //top of cone
	vector3 point02(0, -height, 0); //bottom of cone

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep)
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height, z + (cosine * rad)); //point to be stored
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

	CompleteMesh(glm::vec3(0.4706f, 0.3569f, 0.2706f));

	//PENCIL MAIN BODY
	subDiv = 6;
	angleStep = (2 * PI) / subDiv; //internal angle of circle base
	vector3 point021(0, -height * 4.0f, 0); //bottom of cylinder

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //Top circle points
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height, z + (cosine * rad)); //point to be stored
		cylVec1.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //Bottom circle points
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height * 4.0f, z + (cosine * rad)); //point to be stored
		cylVec2.push_back(tempVec);
	}

	for (int i = 0; i < cylVec1.size(); i++)
	{
		if (i + 1 != cylVec1.size())
		{
			AddTri(point02, cylVec1[i], cylVec1[i + 1]); //top
			AddTri(point021, cylVec2[i + 1], cylVec2[i]); //bottom
			AddQuad(cylVec2[i], cylVec2[i + 1], cylVec1[i], cylVec1[i + 1]); //sides
		}

		else
		{
			AddTri(point02, cylVec1[i], cylVec1[0]); //top
			AddTri(point021, cylVec2[0], cylVec2[i]); //bottom
			AddQuad(cylVec2[i], cylVec2[0], cylVec1[i], cylVec1[0]); //sides
		}
	}
	CompleteMesh(glm::vec3(0.8824f, 0.5843f, 0.149f));


	//ERASER
	subDiv = 12;
	angleStep = (2 * PI) / subDiv; //internal angle of circle base
	vector3 point022(0, 4.75f * -height, 0); //bottom of cylinder

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //Top circle points
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height * 4.0f, z + (cosine * rad)); //point to be stored
		cylVec11.push_back(tempVec);
	}

	for (float angle = 0.0f; angle < 2 * PI; angle += angleStep) //Bottom circle points
	{
		float sine = sin(angle);
		float cosine = cos(angle);

		vector3 tempVec(x + (sine * rad), -height * 4.75f, z + (cosine * rad)); //point to be stored
		cylVec22.push_back(tempVec);
	}

	for (int i = 0; i < cylVec11.size(); i++)
	{
		if (i + 1 != cylVec11.size())
		{
			AddTri(point021, cylVec11[i], cylVec11[i + 1]); //top
			AddTri(point022, cylVec22[i + 1], cylVec22[i]); //bottom
			AddQuad(cylVec22[i], cylVec22[i + 1], cylVec11[i], cylVec11[i + 1]); //sides
		}

		else
		{
			AddTri(point021, cylVec11[i], cylVec11[0]); //top
			AddTri(point022, cylVec22[0], cylVec22[i]); //bottom
			AddQuad(cylVec22[i], cylVec22[0], cylVec11[i], cylVec11[0]); //sides
		}
	}

	CompleteMesh(glm::vec3(0.9569f, 0.6078f, 0.5843f));
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

	//Solution replaced by generate Cube
	float height = a_fHeight;
	float rad = a_fRadius;
	int subDiv = a_nSubdivisions;
	std::vector<vector3> cylVec1; //For storing top cylinder circle
	std::vector<vector3> cylVec2; //For storing bottom cylinder circle
	float angleStep = (2 * PI) / subDiv;
	float x = 0;
	float z = 0;
	vector3 point011(0, -height * .5, 0); //top of cylinder
	vector3 point021(0, (-height * .5) - height, 0); //bottom of cylinder

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
			AddTri(point011, cylVec1[i], cylVec1[i + 1]); //top
			AddTri(point021, cylVec2[i + 1], cylVec2[i]); //bottom
			AddQuad(cylVec2[i], cylVec2[i + 1], cylVec1[i], cylVec1[i + 1]); //sides
		}

		else
		{
			AddTri(point011, cylVec1[i], cylVec1[0]); //top
			AddTri(point021, cylVec2[0], cylVec2[i]); //bottom
			AddQuad(cylVec2[i], cylVec2[0], cylVec1[i], cylVec1[0]); //sides
		}
	}

	CompleteMesh(glm::vec3(0.8824f, 0.5843f, 0.149f));
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	Release();
	Init();

	//Solution replaced by generate Cube
	GenerateCube(a_fOuterRadius, a_v3Color);
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	Release();
	Init();

	//Solution replaced by generate Cube
	GenerateCube(a_fOuterRadius, a_v3Color);
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	Release();
	Init();

	//Solution replaced by generate Cube
	GenerateCube(a_fRadius, a_v3Color);
}
std::vector<vector3> MyMesh::GetVertexList(void) { return m_lVertexPos; }