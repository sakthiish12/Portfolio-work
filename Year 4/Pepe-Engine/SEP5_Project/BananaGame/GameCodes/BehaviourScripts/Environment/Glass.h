#pragma once
#ifndef	GLASS_H
#define GLASS_H

#include <BananaEngine.h>

namespace TH
{
	using namespace BE;
	class GlassScript : public IScript
	{
	public:
		void Init();
		void Update(double);
		template<typename Script>
		Script& GetScriptOnThisEntity()
		{
			return static_cast<Script&>(*ECS->GetComponent<GameLogic>(id).GetScript<Script>());
		}
		RTTR_ENABLE(IScript)
		RTTR_REGISTRATION_FRIEND
		
	};

}

#endif
