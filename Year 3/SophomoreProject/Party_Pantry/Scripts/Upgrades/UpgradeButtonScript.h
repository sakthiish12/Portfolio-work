#ifndef _UPGRADEBUTTONSCRIPT_H_
#define _UPGRADEBUTTONSCRIPT_H_

struct scriptBase;
struct UpgradeButtonScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "UpgradeButtonScript" }; //name identifier
	entityID textId;
	bool textLoaded = false;
	//functions
	UpgradeButtonScript(const char* scriptName = "UpgradeButtonScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 