#ifndef _MENUSELECTORLEVEL2SCRIPT_H_
#define _MENUSELECTORLEVEL2SCRIPT_H_

struct scriptBase;
struct MenuSelectorLevel2Script : scriptBase
{
	//CPP TaskListScript
	//base data
	std::string name{ "MenuSelectorLevel2Script" }; //name identifier

	//functions
	MenuSelectorLevel2Script(const char* scriptName = "MenuSelectorLevel2Script");
	virtual void init(float dt, unsigned int idToBeSet) override; //start
	virtual void update(float dt) override; //every update frame
	virtual void end(float dt) override; //end
	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif 