#ifndef _UPGRADEBODYSCRIPT_H_
#define _UPGRADEBODYSCRIPT_H_

struct scriptBase;
struct UpgradeBodyScript : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "UpgradeBodyScript" }; //name identifier

	size_t tomatoesCollected = 0;//tracks the number of tomatoes collected

	//functions
	UpgradeBodyScript(const char* scriptName = "UpgradeBodyScript");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 