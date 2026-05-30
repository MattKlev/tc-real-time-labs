///////////////////////////////////////////////////////////////////////////////
// cppDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "cppDriver.h"
#include "cppClassFactory.h"

DECLARE_GENERIC_DEVICE(CPPDRV)

IOSTATUS CcppDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CcppClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CcppDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CcppDriver::CPPDRV_GetVersion( )
{
	return( (CPPDRV_Major << 8) | CPPDRV_Minor );
}

