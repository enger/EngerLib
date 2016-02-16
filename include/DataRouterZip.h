#pragma once
#include <config.h>
#include <DataRouter.h>
namespace engerlib
{
	class ENGERLIB_EXP CDataRouterZip:public CDataRouter
	{
	public:
		CDataRouterZip(void);
		~CDataRouterZip(void);
	protected:
		virtual void Apply();

	};

}

