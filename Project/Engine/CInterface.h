#pragma once

class CInterface
{
protected:
	virtual void SaveToScene(FILE* _pFile) = 0;
	virtual void LoadFromScene(FILE* _pFile) = 0;
};

