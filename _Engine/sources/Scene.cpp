#include "Scene.h"

#include "Ankaria.h"

Entity& Scene::Add(std::vector<Components> components)
{
	auto entity = std::make_unique<Entity>(engine, std::move(components));
	entities.push_back(std::move(entity));
	return *entities.back();
}