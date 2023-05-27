#ifndef _UPGRADEBUTTONFIREBALLSCRIPT_H_
#define _UPGRADEBUTTONFIREBALLSCRIPT_H_

struct scriptBase;
struct UpgradeButtonFireballScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "UpgradeButtonFireballScript" }; //name identifier
	entityID textId;
	bool textLoaded = false;
	//functions
	UpgradeButtonFireballScript(const char* scriptName = "UpgradeButtonFireballScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 