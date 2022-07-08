#include "pch.h"
#include "CCameraEx.h"

#include <Engine\CTransform.h>


void CCameraEx::finalupdate()
{
	CalViewMat();
	CalProjMat();
	CalRay();

	m_frustum.finalupdate();
}
