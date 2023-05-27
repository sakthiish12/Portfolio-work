#pragma once
#ifndef	APPLICATION_H
#define APPLICATION_H
#pragma once
#include <BananaEngine.h>

class Application
{
	BE::Singleton<BE::GameEngine>::SingletonPtr& m_engine;

public:
	static constexpr int WIDTH = 1900;
	static constexpr int HEIGHT = 1200;
	Application(const std::string& name);
	~Application();

	void Init();
	void Awake();
	void Run();
	void Shutdown();
private:
	BE::BeWindow beWindow;
	BE::BeDevice beDevice{ beWindow };
	BE::BeRenderer beRenderer{ beWindow, beDevice };
	BE::OffScreen MOffScreen{ beDevice };
	bool shouldPause{};
};
#endif