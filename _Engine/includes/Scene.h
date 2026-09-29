#pragma once

#include <vector>

#include "Scene.h"
#include "Entity.h"
#include "Components.h"

class Ankaria;

class Scene
{
private:
	Ankaria&  engine;
public:
	
	std::vector<std::unique_ptr<Entity>> entities;
	Scene(Ankaria& engine) : engine(engine) {};
	Entity& Add(std::vector<Components> components);
};