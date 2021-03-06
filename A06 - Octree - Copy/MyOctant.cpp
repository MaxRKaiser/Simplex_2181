#include "MyOctant.h"
#include "AppClass.h"
using namespace Simplex;
uint MyOctant::m_nCount = 0;
static int levels = 0;
//  MyOctant
void MyOctant::Init(void)
{
	m_nData = 0;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	//IsColliding();
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}

MyOctant::MyOctant(int level)
{
	Init();
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	std::vector<vector3> v3MaxMin_list;
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRG = l_Entity_List[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);
	}

	levels = level;
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_pRigidBody->MakeCubic();
	m_pRigidBody->SetColorColliding(C_YELLOW);
	m_pRigidBody->SetColorNotColliding(C_YELLOW);
	m_nCount = 0;
	colNum = 0;
	m_iID = m_nCount;
	Subdivide();
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize, int level)
{
	Init();
	std::vector<vector3> v3MaxMin_list;
	v3MaxMin_list.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMin_list.push_back(a_v3Center + vector3(a_fSize));
	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	m_pRigidBody->SetColorColliding(C_YELLOW);
	m_pRigidBody->SetColorNotColliding(C_YELLOW);
	m_nCount++;
	m_iID = m_nCount;
	levels = level;
	colNum = 0;
}

void MyOctant::Subdivide()
{
	//if you've reached the level limit, stop subdividing
	if (m_nLevel >= levels)
	{
		//since we're on a leaf, see what it collides with
		IsColliding();
		return;
	}

	//only divide again if there are enough objects to justify it
	IsColliding2();
	if (colNum > 3)
	{
		vector3 v3Center = m_pRigidBody->GetCenterLocal();
		vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();
		float fSize = (v3HalfWidth.x) / (2.0f);
		float fCenters = fSize;

		m_pChild[0] = new MyOctant(v3Center + vector3(fCenters, fCenters, fCenters), fSize, levels);
		m_pChild[1] = new MyOctant(v3Center + vector3(-fCenters, fCenters, fCenters), fSize, levels);
		m_pChild[2] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, fCenters), fSize, levels);
		m_pChild[3] = new MyOctant(v3Center + vector3(fCenters, -fCenters, fCenters), fSize, levels);

		m_pChild[4] = new MyOctant(v3Center + vector3(fCenters, fCenters, -fCenters), fSize, levels);
		m_pChild[5] = new MyOctant(v3Center + vector3(-fCenters, fCenters, -fCenters), fSize, levels);
		m_pChild[6] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, -fCenters), fSize, levels);
		m_pChild[7] = new MyOctant(v3Center + vector3(fCenters, -fCenters, -fCenters), fSize, levels);

		for (uint i = 0; i < 8; i++)
		{
			m_pChild[i]->m_nLevel = m_nLevel + 1;
			m_pChild[i]->m_pParent = this;
			m_pChild[i]->Subdivide();

		}
	}

	
}

void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}
void MyOctant::Release(void)
{
	m_lData.clear();
}
void Simplex::MyOctant::Display(int index)
{
	//displays only the current octant we've indexed to (if not a fallback index)
	if (index == m_iID)
	{
		m_pRigidBody->AddToRenderList();
	}
	//fallback index of -1 displays all octants
	else if (index == -1)
	{
		m_pRigidBody->AddToRenderList();
		for (uint i = 0; i < 8; i++)
		{
			if (m_pChild[i])
				m_pChild[i]->Display(-1);
		}
	}
	//if were not at the fallback index, but we haven't found the octant we're looking for, look at the children
	else if (index > -1)
	{
		for (uint i = 0; i < 8; i++)
		{
			if (m_pChild[i])
				m_pChild[i]->Display(index);
		}
	}
	
	//m_pMeshMngr->AddWireCubeToRenderList(glm::scale(vector3(70)), C_BLUE);
}

//check for how many cubes are colliding with the octant
void Simplex::MyOctant::IsColliding2(void)
{
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRB = l_Entity_List[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			//add to this whenever we find a colliding cube
			colNum++;
		}
	}
}

void Simplex::MyOctant::IsColliding(void)
{
	std::vector<MyEntity*> l_Entity_List = m_pEntityMngr->GetEntityList();
	uint iEntityCount = l_Entity_List.size();
	for (uint i = 0; i < iEntityCount; ++i)
	{
		MyRigidBody* pRB = l_Entity_List[i]->GetRigidBody();
		if (pRB->IsColliding(m_pRigidBody))
		{
			l_Entity_List[i]->AddDimension(m_iID);
		}
	}
}
MyOctant::MyOctant(MyOctant const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant(){Release();};
//Accessors
void MyOctant::SetData(int a_nData){ m_nData = a_nData; }
uint MyOctant::GetOctantCount(void) { return m_nCount; }
int MyOctant::GetData(void){ return m_nData; }
void MyOctant::SetDataOnVector(int a_nData){ m_lData.push_back(a_nData);}
int& MyOctant::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}
//--- Non Standard Singleton Methods

