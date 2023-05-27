#ifndef MOVEABLEBOUNDER_H
#define MOVEABLEBOUNDER_H

struct scriptBase;
struct MoveableBounderScript : scriptBase
{
	MoveableBounderScript(const char* scriptName = "MoveableBounder");
	virtual void init(float dt, unsigned int idToSet) override;
	virtual void update(float dt) override;
	virtual void end(float dt) override;

	std::shared_ptr<scriptBase> createCopy() override;
	std::string getScriptName();
};

#endif