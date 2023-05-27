#ifndef _UPGRADEBUTTONSSHIELDCRIPT_H_
#define _UPGRADEBUTTONSSHIELDCRIPT_H_

struct scriptBase;
struct UpgradeButtonShieldScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "UpgradeButtonShieldScript" }; //name identifier
	entityID textId;
	bool textLoaded = false;

	//functions
	UpgradeButtonShieldScript(const char* scriptName = "UpgradeButtonShieldScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 