#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Scene.h"

class Scenes
{

private:
	Ankaria* engine = nullptr;

public:

	Scene* nextScene = nullptr;
	void Initialize(Ankaria& engine);
	std::unordered_map<std::string, Scene> scenes; 

	Scene& Add(const std::string name, Scene& scene);
	Scene& Get(std::string name);
	void ChangeScene(std::string name);
	//void ChangeScene(Scene& scene);
	void ApplyChangeScene();

};

