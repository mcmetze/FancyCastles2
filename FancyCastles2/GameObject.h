#pragma once

#include "Observer.h"
#include "TileTraits.h"

enum class GameObjectType { RESOURCE, BUILDING, UNIT, TIMER, INVALID };

class GameObject : public Observable
{
public:
	GameObject();
	virtual ~GameObject();

	virtual int GetPosition() const;
	virtual void SetPosition(int newPosition);
	
	virtual GameObjectType GetObjectType() const = 0;

	virtual int GetObjectID() const;
	
protected:
	GameObject(int objID, int objPos);

	int mObjectID;
	int mPosition;
};

inline bool operator ==(const GameObject& lhs, const GameObject& rhs)
{
	return lhs.GetPosition() == rhs.GetPosition()
		&& lhs.GetObjectType() == rhs.GetObjectType()
		&& lhs.GetObjectID() == rhs.GetObjectID();
}


class ResourceObject : public GameObject
{
public:
	ResourceObject(int objectID, int location, ResourceType type);

	GameObjectType GetObjectType() const override;

	ResourceType GetResourceType() const;

private:

	ResourceType mType;
};

class BuildingObject : public GameObject
{
public:
	BuildingObject(int objectID, int location, BuildingType type);

	GameObjectType GetObjectType() const override;

	BuildingType GetBuildingType() const;

private:
	BuildingType mType;
};