#pragma once

#include <vector>
#include <memory>

#ifdef LINUX_BUILD
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif // LINUX_BUILD

#ifdef WINDOWS_BUILD
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#endif // WINDOWS_BUILD


class SceneGraphNode
{
	public:
	SceneGraphNode(const glm::mat4* p_rootTrans, const glm::mat4 p_localTrans, bool p_ignoreParent);
	~SceneGraphNode();
    void UpdateMat(const glm::mat4& p_parentSGN);
    void SetToDefaultLocalMat();

    glm::mat4 m_localTrans; // < -- relative offset
    glm::mat4 m_worldTrans; // < -- world pos
    const glm::mat4* m_rootTrans;
    std::vector<SceneGraphNode*> m_childeren;
    protected:

    bool m_ignoreParentTranslation; // <-- this is for objects that have a rigid body and so doesnt make sense to inherit from parent
};

class SceneGraphRoot
{
    public:
    SceneGraphRoot();
    ~SceneGraphRoot();
    void UpdateMat();

    glm::mat4 m_worldTrans;
    std::vector<SceneGraphNode*> m_childeren;
};

