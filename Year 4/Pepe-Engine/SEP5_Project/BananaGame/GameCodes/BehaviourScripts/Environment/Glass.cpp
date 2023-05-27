#include "Glass.h"
#include <Character/CharacterScript.h>
#include <Misc/SFXPlayer.h>
const BE::AssetID glassBreakSFX = BE::AssetID(1780105953022451738);
const BE::AssetID glassHitSFX = BE::AssetID(13806083505021939052);
float current = 0.0f;
void TH::GlassScript::Init()
{
	current = GetScriptOnThisEntity<CharacterScript>().GetCurrentHealth();
}
void TH::GlassScript::Update(double)
{
	CharacterScript& characterRef = GetScriptOnThisEntity<CharacterScript>();
	if(characterRef.IsDead())
	{
		BEID sfxPlayerID = ECS->CreateEntity();
		ECS->AddComponent<GameLogic>(sfxPlayerID);
		ECS->GetComponent<GameLogic>(sfxPlayerID).AddScript<SFXPlayer>();
		SFXPlayer& sfxPlayer = static_cast<SFXPlayer&>(*ECS->GetComponent<GameLogic>(sfxPlayerID).GetScript("SFXPlayer"));
		sfxPlayer.Setup(glassBreakSFX, 2.0f,0.1f);
	}
	else if (current != characterRef.GetCurrentHealth())
	{
		BEID sfxPlayerID = ECS->CreateEntity();
		ECS->AddComponent<GameLogic>(sfxPlayerID);
		ECS->GetComponent<GameLogic>(sfxPlayerID).AddScript<SFXPlayer>();
		SFXPlayer& sfxPlayer = static_cast<SFXPlayer&>(*ECS->GetComponent<GameLogic>(sfxPlayerID).GetScript("SFXPlayer"));
		sfxPlayer.Setup(glassHitSFX, 1.0f);
		current = characterRef.GetCurrentHealth();
	}
}
RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace BE;
	using namespace BE::Serialisation;
	using namespace TH;
	registration::class_<GlassScript>(RegistrationNameCreator<GlassScript>())
		.constructor();
}