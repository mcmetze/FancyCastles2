#include "GameObject.h"

GameObject::GameObject() 
	: mObjectID(-1), mPosition(-1)
{
}

GameObject::GameObject(int objID, int objPos)
	: mObjectID(objID), mPosition(objPos)
{
}

GameObject::~GameObject()
{
}

int
GameObject::GetPosition() const
{
	return mPosition;
}

void
GameObject::SetPosition(int newPosition)
{
	mPosition = newPosition;
}

int 
GameObject::GetObjectID() const
{
	return mObjectID;
}

ResourceObject::ResourceObject(int objectID, int location, ResourceType type)
	: GameObject(objectID, location), mType(type)
{
}

GameObjectType
ResourceObject::GetObjectType() const
{
	return GameObjectType::RESOURCE;
}

ResourceType
ResourceObject::GetResourceType() const
{
	return mType;
}

BuildingObject::BuildingObject(int objectID, int location, BuildingType type)
	: GameObject(objectID, location), mType(type)
{
}

GameObjectType
BuildingObject::GetObjectType() const
{
	return GameObjectType::BUILDING;
}

BuildingType
BuildingObject::GetBuildingType() const
{
	return mType;
}