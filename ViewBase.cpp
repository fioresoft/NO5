// ViewBase.cpp: implementation of the CViewBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "imainframe.h"
#include "ViewBase.h"
#include "no5app.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewBase::CViewBase(void)
{
	m_pFrame = g_app.GetMainFrame();
	m_bActive = false;
	m_pFrame->AddView(this);
}

CViewBase::~CViewBase()
{
	m_pFrame->RemoveView(this);
	ATLTRACE("~CViewBase\n");
}

