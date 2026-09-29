#pragma once

#include "Components.h"

#include <variant>
#include <functional>
#include <iostream>

class Ankaria;

class Entity {

private:
	Ankaria* engine;

	template <typename T>
	void InitializeComponent()
	{
		auto* component = GetComponent<T>();
		if (!component)
			return;
		InitializeComponent(*component);
	}

	// Default
	template<typename T>
	void InitializeComponent(T&) { }

	// Custom
	void InitializeComponent(Sprite& sprite);
	void InitializeComponent(Anchor& anchor);
	void InitializeComponent(Text& text);
	void InitializeComponent(Shader& shader);

public:
	Entity(Ankaria& engine, std::vector<Components> components);
	SDL_FRect GetBoundingBox();
	
	//Events
	std::function<void(float)> OnUpdate = nullptr;
	std::function<void()> OnStart = nullptr;
	std::function<void()> onClick = nullptr;
	std::function<void()> onHover = nullptr;
	std::function<void()> onHoverEnd = nullptr;
	bool isOnHoverStarted = false;

	std::vector<Components> components;

	template<typename T>
	T* GetComponent()
	{
		for (auto& component : components)
		{
			if (auto* value = std::get_if<T>(&component))
			{
				return value;
			}
		}
		return nullptr;
	}

	template <typename T>
	bool HasComponent()
	{
		return GetComponent<T>() != nullptr;
	}
	
};

