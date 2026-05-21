///////////////////////////////////////////////////////////////////////////////
// cppDriver.h

#ifndef __CPPDRIVER_H__
#define __CPPDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define CPPDRV_NAME        "CPP"
#define CPPDRV_Major       1
#define CPPDRV_Minor       0

#define DEVICE_CLASS CcppDriver

#include "ObjDriver.h"

class CcppDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl CPPDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(CPPDRV)
	VxD_Service( CPPDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __CPPDRIVER_H__