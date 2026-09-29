#include "Events.h"


void Events::Initialize()
{

}

SDL_Event& Events::GetEvents() {
	return events;
}

SDL_FPoint Events::GetMousePosition()
{
	float x;
	float y;
	SDL_GetMouseState(&x, &y);
	return SDL_FPoint{ x, y };
}