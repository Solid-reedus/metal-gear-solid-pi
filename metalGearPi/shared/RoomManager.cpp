#include "RoomManager.h"

#include "bullet/btBulletDynamicsCommon.h"
#include "collisionManager.h"
#include <nlohmann/json.hpp>
#include "TriggerManager.h"
#include "GamePlayScene.h"
#include "modelInstance.h"
#include "EnemyManager.h"
#include "player.h"
#include "common.h"
#include <fstream>
#include "Room.h"

RoomManager::RoomManager(
    BaseShader* p_shader, Camera& p_cam, 
    CollisionManager& p_collisionManager, 
    LightingManager& p_lightingManager, 
    Player& p_playerRef,
    uint& p_seed, 
    SceneGraphRoot& p_sceneGraphRoot,
    GamePlayScene& p_gameplayScene)
    : 
    m_cam(p_cam), m_collisionManager(p_collisionManager), m_playerRef(p_playerRef), m_gameplayScene(p_gameplayScene)
{
    m_shader = p_shader;

    m_sceneGraphNode = new SceneGraphNode(&p_sceneGraphRoot.m_worldTrans, glm::mat4(1.0f), false);
    m_sceneGraphNode->SetToDefaultLocalMat();
    p_sceneGraphRoot.m_childeren.push_back(m_sceneGraphNode);

    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "crate/crate.obj"), p_shader));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "door/door.obj"), p_shader, &p_collisionManager));

    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "fence/fence1/fence1.obj"), p_shader, &p_collisionManager));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "fence/fence2/fence2.obj"), p_shader));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "fence/fence3/fence3.obj"), p_shader));

    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "map/map1.obj"), p_shader, &p_collisionManager));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "map/map2.obj"), p_shader));

    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "rooms/roomSmall.obj"), p_shader, true, &p_collisionManager));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "rooms/roomMedium1.obj"), p_shader, true, &p_collisionManager));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "rooms/roomMedium2.obj"), p_shader, true, &p_collisionManager));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "rooms/roomMedium1.obj"), p_shader, true, &p_collisionManager));

    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "rooms/hallway.obj"), p_shader, true, &p_collisionManager));


    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "column/column.obj"), p_shader));
    m_models.push_back(new ModelInstance(new Model(std::string(ASSETS_DIR) + "wall/wall.obj"), p_shader, &p_collisionManager));


	LoadLevelJson(std::string(ASSETS_DIR) + "level/level1.json");
	LoadLevelJson(std::string(ASSETS_DIR) + "level/level2.json");
	LoadLevelJson(std::string(ASSETS_DIR) + "level/level3.json");
	LoadLevelJson(std::string(ASSETS_DIR) + "level/level4.json");
	LoadLevelJson(std::string(ASSETS_DIR) + "level/level5.json");
	LoadLevelJson(std::string(ASSETS_DIR) + "level/level6.json");


    for (auto& room : m_rooms)
    {
        for (auto& obj : room.objects)
        {
            switch (obj.type)
            {
                case Room::PATROLL_POINT:
                case Room::LOAD_ROOM_TRIGGER:
                case Room::UNKNOWN:
                case Room::ENEMY:
                default:
                {
                    continue;
                }
                case Room::CRATE:
                case Room::DOOR:
                case Room::FENCE1:
                case Room::FENCE2:
                case Room::FENCE3:
                case Room::MAP1:
                case Room::MAP2:
                case Room::COLUMN:
                case Room::WALL:
                case Room::ROOM_SMALL:
                case Room::ROOM_MEDIUM1:
                case Room::ROOM_MEDIUM2:
                case Room::ROOM_BIG:
                case Room::HALLWAY:
                {
                    break;
                }
            }

            glm::vec3 pos = glm::vec3(obj.mat[3]);
            btCollisionShape* usedShape = nullptr;

            if (m_models[static_cast<int>(obj.type)]->m_usedShape != nullptr)
            {
                usedShape = m_models[static_cast<int>(obj.type)]->m_usedShape;
            }
            else
            {
                usedShape = m_collisionManager.GetShape(0);
            }

            int combinedMask = CollisionManager::CollisionMask::Player | CollisionManager::CollisionMask::Enemy | CollisionManager::CollisionMask::Prop;
            Instance* ins = new Instance(
                obj.mat,
                m_sceneGraphNode->m_rootTrans,
                true,
                new RigidBody(
                    *m_collisionManager.m_dynamicsWorld,
                    usedShape,
                    btVector3(pos.x, pos.y, pos.z),
                    0.0f,
                    CollisionManager::CollisionMask::Environment,
                    combinedMask
                )
            );
            SetInstance(obj.type, ins);
        }
    }

    m_models[static_cast<int>(Room::ModelIns::CRATE)]->m_usedShape = m_collisionManager.GetShape(4);
    
    
    for (auto& model : m_models)
    {
        if (model->m_usedShape == nullptr)
        {
            const Model* usedModel = model->LookAtModel();
            const Instance* ins = model->LookAtInstance(0);
            btCollisionShape* newCollider = m_collisionManager.CreateMeshCollider(usedModel->LookAtMeshes()[0], false, ins->scl);
            model->m_usedShape = newCollider;
        }
    }

    for (auto& modelIns : m_models)
    {
        modelIns->Clear();
    }

    m_enemyManager = new EnemyManager(m_cam, m_collisionManager, p_lightingManager, p_shader, p_playerRef.GetPlayerPosRef(), p_seed, *m_sceneGraphNode);
}

RoomManager::~RoomManager()
{
    for (auto& model : m_models)
    {
        delete model;
    }

    delete m_enemyManager;
    delete m_sceneGraphNode;
}

void RoomManager::LoadLevelJson(const std::string& p_filePath)
{
    m_rooms.push_back(Room{});
    Room& newRoom = m_rooms.back();

    // Open the JSON file
    std::ifstream input_file(p_filePath);
    if (!input_file.is_open())
    {
        std::cerr << "Could not open the file: " << p_filePath << std::endl;
        return;
    }

    try
    {
        // Parse the JSON content
        nlohmann::json j;
        input_file >> j;

        // Check if the root contains an "objects" array
        if (j.contains("objects") && j["objects"].is_array())
        {
            const auto& objects = j["objects"];
            for (const auto& item : objects)
            {
                if (item.is_object())
                {
                    glm::vec3 position(0.0f), rotation(0.0f), scale(1.0f);

                    Room::ModelIns modelInstance = Room::ModelIns::UNKNOWN;

                    
                    // Extract object name
                    if (item.contains("name") && item["name"].is_string())
                    {
                        const std::string& name = item["name"];
                        modelInstance = GetInstanceFromName(name);
                    }

                    // Extract position
                    if (item.contains("location") && item["location"].is_array())
                    {
                        const auto& loc = item["location"];
                        position = glm::vec3(loc[0].get<float>(), loc[2].get<float>(), -loc[1].get<float>());
                    }

                    // Extract rotation
                    if (item.contains("rotation") && item["rotation"].is_array())
                    {
                        const auto& rot = item["rotation"];
                        rotation = glm::vec3(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>());
                    }

                    // Extract scale
                    if (item.contains("scale") && item["scale"].is_array())
                    {
                        const auto& scl = item["scale"];
                        scale = glm::vec3(scl[0].get<float>(), scl[2].get<float>(), scl[1].get<float>());
                    }

                    glm::mat4 modelMat = glm::mat4(1.0f);

                    modelMat = glm::translate(modelMat, position);

                    modelMat = glm::rotate(modelMat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X-axis
                    modelMat = glm::rotate(modelMat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
                    modelMat = glm::rotate(modelMat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis

                    modelMat = glm::scale(modelMat, scale);

                    std::cout << (item.contains("name") ? item["name"].get<std::string>() : "Unknown") << "\n";
                    
                    switch (modelInstance)
                    {
                        case Room::ENEMY:
                        {
                            Room::EnemyInfo newEnemyInfo = {};
                            newEnemyInfo.spawnPos = position;

                            if (item.contains("children") && item["children"].is_array())
                            {
                                for (const auto& child : item["children"])
                                {
                                    glm::vec3 childPos = glm::vec3(0.0f);

                                    if (child.contains("location") && child["location"].is_array())
                                    {
                                        const auto& loc = child["location"];
                                        
                                        childPos = glm::vec3(loc[0].get<float>(), loc[2].get<float>(), -loc[1].get<float>());
                                    }
                                    newEnemyInfo.patrolPoints.push_back(childPos);
                                }
                            }

                            newRoom.enemyInfo.push_back(newEnemyInfo);
                            break;
                        }
                        case Room::DOOR:
                        {
                            newRoom.objects.push_back(Room::RoomObjects(modelMat, modelInstance));

                            if (item.contains("children") && item["children"].is_array())
                            {
                                for (const auto& child : item["children"])
                                {
                                    glm::vec3 childPos = glm::vec3(0.0f);

                                    if (child.contains("location") && child["location"].is_array())
                                    {
                                        const auto& loc = child["location"];

                                        childPos = glm::vec3(loc[0].get<float>(), loc[2].get<float>(), -loc[1].get<float>());
                                    }

                                    int roomIndex = RoomIndexLookUp(child["name"]);

                                    Trigger newTrigger =
                                        Trigger(
                                            *m_collisionManager.m_dynamicsWorld,
                                            m_collisionManager.GetShape(3),
                                            btVector3(0.0f, 3.0f, 40.0f),
                                            [this, roomIndex, childPos]()
                                            {LoadNewRoom(roomIndex, childPos); },
                                            CollisionManager::CollisionMask::Trigger,
                                            CollisionManager::CollisionMask::Player

                                        );
                                    newRoom.roomTriggers.push_back({ newTrigger, position });
                                }
                            }

                            break;
                        }

                        default:
                        case Room::UNKNOWN: 
                        case Room::PATROLL_POINT:     // <-- this should only be a child
                        case Room::LOAD_ROOM_TRIGGER: // <-- this should only be a child
                        {
                            continue;
                        }
                        case Room::CRATE:
                        case Room::FENCE1:
                        case Room::FENCE2:
                        case Room::FENCE3:
                        case Room::MAP1:
                        case Room::MAP2:
                        case Room::COLUMN:
                        case Room::WALL:
                        case Room::ROOM_SMALL:
                        case Room::ROOM_MEDIUM1:
                        case Room::ROOM_MEDIUM2:
                        case Room::ROOM_BIG:
                        case Room::HALLWAY:
                        {
                            newRoom.objects.push_back(Room::RoomObjects(modelMat, modelInstance));
                            break;
                        }

                    }

                    // Optional: Debug output
                    //std::cout << "Instance created with:\n"
                    //    << "Name: " << (item.contains("name") ? item["name"].get<std::string>() : "Unknown") << "\n"
                    //    << "Type: " << modelInstance << "\n"
                    //    << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")\n"
                    //    << "Rotation: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << ")\n"
                    //    << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")\n\n";
                }
            }
        }
        else
        {
            std::cerr << "The JSON does not contain a valid 'objects' array." << std::endl;
        }
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

}

int RoomManager::RoomIndexLookUp(const std::string& inputString)
{
    const std::unordered_map<std::string, int> levelLookUp =
    {
        {"loadRoom1", 0},
        {"loadRoom2", 1},
        {"loadRoom3", 2},
        {"loadRoom4", 3},
        {"loadRoom5", 4},
        {"loadRoom6", 5},
    };

    for (auto it = levelLookUp.begin(); it != levelLookUp.end(); ++it)
    {
        if (inputString.find(it->first) != std::string::npos) // Check if key is a substring
        {
            return it->second;
        }
    }

    return 10; // Indicates the room name was not found
}


Room::ModelIns RoomManager::GetInstanceFromName(const std::string& p_name)
{
	for (auto it = m_modelLookUpTable.begin(); it != m_modelLookUpTable.end(); ++it)
	{
		if (p_name.find(it->first) != std::string::npos)
		{
			return it->second;
		}
	}
	return Room::ModelIns::UNKNOWN;
}

void RoomManager::SetInstance(Room::ModelIns p_model, Instance* p_newIns)
{
	if (static_cast<int>(p_model) >= m_models.size())
	{
		printf("error: m_models isnt the size of ModelIns please make sure to update both teh m_models vector and ModelIns enum, RoomManager::SetInstance \n");
        delete p_newIns;
		return;
	}

	m_models[static_cast<int>(p_model)]->AddInstance(p_newIns);

}

void RoomManager::Render()
{
	for (auto& modelIns : m_models)
	{
		modelIns->Draw(m_cam);
	}
    m_enemyManager->Render();
}

void RoomManager::Update(float p_deltaTime)
{
    m_enemyManager->Update(p_deltaTime);
}

void RoomManager::LoadNewRoom(int p_index, glm::vec3 p_playerPos)
{

	if (p_index >= m_rooms.size())
	{
		printf("error: p_index is out of the m_rooms index, RoomManager::LoadNewRoom \n");
        return;
	}

    if (p_index >= m_rooms.size() - 1)
    {
        m_gameplayScene.WinGame();
    }

    printf("new pos %f %f %f \n", p_playerPos.x, p_playerPos.y, p_playerPos.z);
    m_playerRef.SetPos(p_playerPos);

    m_collisionManager.ClearSpecificObjects(CollisionManager::CollisionMask::Environment);
    m_collisionManager.m_triggerManager->m_triggers.clear();
    for (auto& triggerObj : m_rooms[p_index].roomTriggers)
    {
        m_collisionManager.m_triggerManager->m_triggers.push_back(triggerObj.trigger);
        triggerObj.trigger.UpdatePosition(triggerObj.pos);
    
    }

	for (auto& modelIns : m_models)
	{
		modelIns->Clear();
	}

    m_enemyManager->DisableEnemies();

    for (auto& enemy : m_rooms[p_index].enemyInfo)
    {
        m_enemyManager->SpawnEnemy(enemy.spawnPos, enemy.patrolPoints);
    }


    for (auto& obj : m_rooms[p_index].objects)
    {
        switch (obj.type)
        {
            case Room::PATROLL_POINT:
            case Room::LOAD_ROOM_TRIGGER:
            case Room::UNKNOWN:
            case Room::ENEMY:
            default:
            {
                continue;
            }
            case Room::CRATE:
            case Room::DOOR:
            case Room::FENCE1:
            case Room::FENCE2:
            case Room::FENCE3:
            case Room::MAP1:
            case Room::MAP2:
            case Room::COLUMN:
            case Room::WALL:
            case Room::ROOM_SMALL:
            case Room::ROOM_MEDIUM1:
            case Room::ROOM_MEDIUM2:
            case Room::ROOM_BIG:
            case Room::HALLWAY:
            {
                break;
            }
        }

        glm::vec3 pos = glm::vec3(obj.mat[3]);
        btCollisionShape* usedShape = nullptr;

        if (m_models[static_cast<int>(obj.type)]->m_usedShape != nullptr)
        {
            usedShape = m_models[static_cast<int>(obj.type)]->m_usedShape;
        }
        else
        {
            usedShape = m_collisionManager.GetShape(0);
        }

        int combinedMask = CollisionManager::CollisionMask::Player | CollisionManager::CollisionMask::Enemy | CollisionManager::CollisionMask::Prop;
        Instance* ins = new Instance(
        obj.mat, 
        m_sceneGraphNode->m_rootTrans,
        true,
        new RigidBody(
            *m_collisionManager.m_dynamicsWorld,
            usedShape,
            btVector3(pos.x, pos.y, pos.z),
            0.0f,
            CollisionManager::CollisionMask::Environment,
            combinedMask
        )
        );
        SetInstance(obj.type, ins);
    }
    
}

