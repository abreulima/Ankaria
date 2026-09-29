#include <iostream>

#include "Ankaria.h"

void Ankaria::Initiliaze()
{
	backend.Initialize(*this);
	renderer.Initialize(*this);
	resources.Initialize(*this);
	scenes.Initialize(*this);
	events.Initialize();
}

void Ankaria::Update()
{

	bool isMouseLeftClicked = false;

	// For the inputs/events
	while (SDL_PollEvent(&events.events))
	{
		if (events.GetEvents().type == SDL_EVENT_QUIT)
		{
			isRunning = false;
		}

		if (events.GetEvents().type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			if (events.GetEvents().button.button == SDL_BUTTON_LEFT)
			{
				isMouseLeftClicked = true;
			}
		}

		if (events.GetEvents().type == SDL_EVENT_KEY_DOWN)
		{
			if (events.GetEvents().key.scancode == SDL_SCANCODE_ESCAPE)
			{
				isRunning = false;
			}
		}
	}

	// Game Logic

	// find the scene
	SDL_FPoint mousePosition = events.GetMousePosition();

	for (auto& entity : currentScene->entities)
	{
		
		if (entity->OnUpdate)
		{
			entity->OnUpdate(0.0f);
		}

		SDL_FRect boundingBox = entity->GetBoundingBox();
		bool isMouseOver = SDL_PointInRectFloat(&mousePosition, &boundingBox);
		
		if (entity->onHover && isMouseOver)
		{
			entity->onHover();
			entity->isOnHoverStarted = true;
		}

		if (entity->onClick && isMouseLeftClicked && isMouseOver)
		{
			entity->onClick();
		}

		if (entity->onHoverEnd && !isMouseOver && entity->isOnHoverStarted)
		{
			entity->onHoverEnd();
			entity->isOnHoverStarted = false;
		}
	}
	
}

void Ankaria::Render()
{
	renderer.Render(currentScene->entities);
}

void Ankaria::End()
{

}

void Ankaria::Run()
{

	Initiliaze();
	
	Game(); // Project function

	// try to set current scene
	currentScene = &scenes.Get("main");
	
	time.last = SDL_GetTicks();

	while (isRunning)
	{
		
		time.now = SDL_GetTicks();
		time.deltaTime = (time.now - time.last);
		time.last = time.now;
		
		Update();
		Render();

		// Apply Change Scene
		scenes.ApplyChangeScene();
		time.frames++;
	}

	End();
}