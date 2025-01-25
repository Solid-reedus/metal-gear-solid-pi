#pragma once

#include <vector>
#include "modelInstance.h"
#include "Trigger.h"

struct Room
{

	enum ModelIns
	{
		CRATE,
		DOOR,

		FENCE1,
		FENCE2,
		FENCE3,

		MAP1,
		MAP2,

		ROOM_SMALL,
		ROOM_MEDIUM1,
		ROOM_MEDIUM2,
		ROOM_BIG,
		HALLWAY,

		COLUMN,
		WALL,
		ENEMY,
		PATROLL_POINT,
		LOAD_ROOM_TRIGGER,
		UNKNOWN
	};

	//ModelIns
	struct EnemyInfo
	{
		glm::vec3 spawnPos;
		std::vector<glm::vec3> patrolPoints;
	};

	struct RoomObjects
	{
		RoomObjects(glm::mat4 p_mat, ModelIns p_type)
			: mat(p_mat), type(p_type) {
		}

		glm::mat4 mat;
		ModelIns type;
	};

	struct TriggerObject
	{
		Trigger trigger;
		glm::vec3 pos;
	};

	std::vector<glm::vec3> lights;
	std::vector<EnemyInfo> enemyInfo;
	std::vector<RoomObjects> objects;

	std::vector<TriggerObject> roomTriggers;
	//ModelInstance* map;

};