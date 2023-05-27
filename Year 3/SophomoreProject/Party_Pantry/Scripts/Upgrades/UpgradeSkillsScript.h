#ifndef _UPGRADESKILLSSCRIPT_H_
#define _UPGRADESKILLSSCRIPT_H_

struct scriptBase;
struct UpgradeSkillsScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "UpgradeSkillsScript" }; //name identifier

	size_t tomatoesCollected = 0;//tracks the number of tomatoes collected

	//functions
	UpgradeSkillsScript(const char* scriptName = "UpgradeSkillsScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 