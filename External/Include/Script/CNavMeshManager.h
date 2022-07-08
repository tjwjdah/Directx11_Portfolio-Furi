#pragma once
#include <Engine\CScript.h>
class CNavMeshManager :
    public CScript
{
private:
	vector<vector<NavMesh>*> m_NavMeshVector;
	
public:
	virtual void update();
	virtual void awake();
	CLONE(CNavMeshManager);

public:
	void NavMeshVector();
	void NavMeshVectorClear() { m_NavMeshVector.clear(); }
	vector<vector<NavMesh>*>* NavMeshVectorGet() { return &m_NavMeshVector; }
public:
	CNavMeshManager();
	~CNavMeshManager();
};

