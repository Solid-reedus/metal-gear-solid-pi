#pragma once

#include "Room.h"
#include "unordered_map"
#include "modelInstance.h"
#include "common.h"

class CollisionManager;
class LightingManager;
class GamePlayScene;
class EnemyManager;
class BaseShader;
struct Camera;
class Player;

class RoomManager
{
	public:


	RoomManager(BaseShader* p_shader, Camera& p_cam, CollisionManager& p_collisionManager, LightingManager& p_lightingManager, Player& p_playerRef, uint& p_seed, SceneGraphRoot& p_sceneGraphRoot, GamePlayScene& p_gameplayScene);
	~RoomManager();
	void Render();
	void Update(float p_deltaTime);
	void LoadNewRoom(int p_index, glm::vec3 p_playerPos);


	private:

	const std::unordered_map<std::string, Room::ModelIns> m_modelLookUpTable =
	{
		{"crate"		, Room::ModelIns::CRATE			   },
		{"door"			, Room::ModelIns::DOOR			   },

		{"fence1"		, Room::ModelIns::FENCE1		   },
		{"fence2"		, Room::ModelIns::FENCE2		   },
		{"fence3"		, Room::ModelIns::FENCE3		   },

		{"map1"			, Room::ModelIns::MAP1			   },
		{"map2"			, Room::ModelIns::MAP2			   },

		{"roomSmall"	, Room::ModelIns::ROOM_SMALL},
		{"roomMedium1"	, Room::ModelIns::ROOM_MEDIUM2},
		{"roomMedium2"	, Room::ModelIns::ROOM_MEDIUM2},
		{"roomBig"		, Room::ModelIns::ROOM_BIG},
		{"hallway"		, Room::ModelIns::HALLWAY},

		{"column"		, Room::ModelIns::COLUMN		   },
		{"wall"			, Room::ModelIns::WALL			   },
		{"enemy"		, Room::ModelIns::ENEMY			   },
		{"patrollPoint" , Room::ModelIns::PATROLL_POINT	   },
		{"loadRoom"		, Room::ModelIns::LOAD_ROOM_TRIGGER},
	};


	SceneGraphNode* m_sceneGraphNode = nullptr;

	BaseShader* m_shader = nullptr;

	Player& m_playerRef;
	GamePlayScene& m_gameplayScene;

	std::vector<Room> m_rooms;
	//Room m_rooms[LEVEL_COUNT] = {};
	Camera& m_cam;
	CollisionManager& m_collisionManager;

	void LoadLevelJson(const std::string& p_filePath);
	int RoomIndexLookUp(const std::string& string);

	Room::ModelIns GetInstanceFromName(const std::string& p_name);
	void SetInstance(Room::ModelIns p_model, Instance* p_newIns);

	std::vector<ModelInstance*> m_models;

	EnemyManager* m_enemyManager = nullptr;

};
