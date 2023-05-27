#ifndef _MENUSELECTORLEVEL1SCRIPT_H_
#define _MENUSELECTORLEVEL1SCRIPT_H_

struct scriptBase;
struct MenuSelectorLevel1Script : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "MenuSelectorLevel1Script" }; //name identifier

	//functions
	MenuSelectorLevel1Script(const char* scriptName = "MenuSelectorLevel1Script");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 