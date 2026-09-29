#ifndef ANKARIA_HPP
#define ANKARIA_HPP

#include <map>
#include <string>

#include "Backend.h"
#include "Resources.h"
#include "Renderer.h"
#include "Events.h"
#include "Scenes.h"
#include "Scene.h"

struct Settings
{
	std::string title = "Game";
	uint16_t width = 1920;
	uint16_t height = 1080;
	float scale = 1.0f;
};

struct Time
{
	std::uint64_t frames = 0;
	std::uint64_t deltaTime = 0;
	std::uint64_t now = 0;
	std::uint64_t last = 0;
};

class Ankaria
{
    private:
	std::string tile;
    
    public:
    
	bool isRunning = true;
	
	void Initiliaze();
	void Update();
	void Render();
	void End();
    
	// Maybe Static?
	Backend backend;
	Renderer renderer;
	Events events;
	Resources resources;
	Scenes scenes;			// aka SceneManager
	
	//std::string currentScene = "main";
	Time time{};
	Settings settings{};
	Scene* currentScene = nullptr;
    
	Ankaria(const Settings& settings) : settings(settings) {};
	virtual void Game() = 0;
	void Run();
    virtual ~Ankaria() = default;
};

//ANKARIA_API void hello();

#endif