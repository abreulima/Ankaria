#include "Scenes.h"

#include "Ankaria.h"
#include <iostream>

Scene& Scenes::Add(std::string name, Scene& scene)
{
	if (scenes.contains(name))
	{
		std::cerr << "Scene already inserted." << std::endl;
		abort();
	}
	
	scenes.insert({ name, std::move(scene) });
	return scenes.at(name);
}

Scene& Scenes::Get(std::string name)
{ 
	if (!scenes.contains(name))
	{
		std::cerr << "Scene not found." << std::endl;
		abort();
	}

	return scenes.find(name)->second;
}



void Scenes::Initialize(Ankaria& engine)
{
	this->engine = &engine;
}

// The scene doesn't actually change up to the end of the frame
void Scenes::ChangeScene(std::string name)
{

	nextScene = &Get(name);
}

void Scenes::ApplyChangeScene()
{
	if (!nextScene)
		return;

	
	engine->currentScene = nextScene;
	this->nextScene = nullptr;
	
}