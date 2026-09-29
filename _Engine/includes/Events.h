#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

class Events
{
public:
	void Initialize(); // Maybe it should be a constructor
	SDL_Event events;
	SDL_Event& GetEvents();
	SDL_FPoint GetMousePosition();
};

