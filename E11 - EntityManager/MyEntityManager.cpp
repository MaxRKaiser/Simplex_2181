#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	/*
	USAGE: Gets the index (from the list of entities) of the entity specified by UniqueID
	ARGUMENTS: String a_sUniqueID -> queried index
	OUTPUT: index from the list of entities, -1 if not found
	*/
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	/*
	USAGE: Will add an entity to the list
	ARGUMENTS:
	-	String a_sFileName -> Name of the model to load
	-	String a_sUniqueID -> Name wanted as identifier, if not available will generate one
	OUTPUT: ---
	*/
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//return *m_entityList[a_uIndex];

	/*
	USAGE: Gets the model associated with this entity
	ARGUMENTS: String a_sUniqueID -> id of the queried entity
	OUTPUT: Model
	*/
	return nullptr;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	/*
	USAGE: Gets the Rigid Body associated with this entity
	ARGUMENTS: uint a_uIndex = -1 -> index in the list of entities; if less than 0 it will add it to the last in the list
	OUTPUT: Rigid Body
	*/
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	/*
	USAGE: Gets the Rigid Body associated with this entity
	ARGUMENTS: String a_sUniqueID -> id of the queried entity
	OUTPUT: Rigid Body
	*/
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	/*
	USAGE: Gets the model matrix associated with this entity
	ARGUMENTS: uint a_uIndex = -1 -> index in the list of entities; if less than 0 it will add it to the last in the list
	OUTPUT: model to world matrix
	*/

	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	/*
	USAGE: Gets the model matrix associated with this entity
	ARGUMENTS: String a_sUniqueID -> id of the queried entity
	OUTPUT: model to world matrix
	*/
	return IDENTITY_M4;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	/*
	USAGE: Sets the model matrix associated to the entity specified in the ID
	ARGUMENTS:
	-	matrix4 a_m4ToWorld -> model matrix to set
	-	uint a_uIndex = -1 -> index in the list of entities; if less than 0 it will add it to the last in the list
	OUTPUT: ---
	*/
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	/*
	USAGE: Sets the model matrix associated to the entity specified in the ID
	ARGUMENTS:
	-	matrix4 a_m4ToWorld -> model matrix to set
	-	String a_sUniqueID -> id of the queried entity
	OUTPUT: ---
	*/
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	/*
	USAGE: Will add the specified entity to the render list
	ARGUMENTS:
	-	uint a_uIndex = -1 -> index (from the list) of the entity queried if < 0 will add all
	-	bool a_bRigidBody = false -> draw rigid body?
	OUTPUT: ---
	*/
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	/*
	USAGE: Gets the model associated with this entity
	ARGUMENTS: uint a_uIndex = -1 -> index in the list of entities; if less than 0 it will add it to the last in the list
	OUTPUT: Model
	*/
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	/*
	USAGE: Deletes the Entity Specified by unique ID and removes it from the list
	ARGUMENTS: uint a_uIndex -> index of the queried entry, if < 0 asks for the last one added
	OUTPUT: ---
	*/
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	/*
	USAGE: Deletes the Entity Specified by unique ID and removes it from the list
	ARGUMENTS: String a_sUniqueID -> Unique Identifier
	OUTPUT: ---
	*/
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	/*
	USAGE: Gets the uniqueID name of the entity indexed
	ARGUMENTS: uint a_uIndex = -1 -> index of the queried entry, if < 0 asks for the last one added
	OUTPUT: UniqueID of the entity, if the list is empty will return blank
	*/
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	/*
	USAGE: Gets the uniqueID name of the entity indexed
	ARGUMENTS: uint a_uIndex = -1 -> index of the queried entry, if < 0 asks for the last one added
	OUTPUT: UniqueID of the entity, if the list is empty will return blank
	*/
	return "";
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	/*
	USAGE: Will update the Entity manager
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	return nullptr;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	/*
	USAGE: Will add the specified entity by uniqueID to the render list
	ARGUMENTS:
	-	String a_sUniqueID -> unique identifier of the entity queried
	-	bool a_bRigidBody = false -> draw rigid body?
	OUTPUT: ---
	*/
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{

}