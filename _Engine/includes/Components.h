#pragma once

#include <glm/glm.hpp>

#include <string>
#include <variant>

#include "Resources.h"


// UTILS
enum class LayerType
{
	BG,
	GAME,
	FG,
	UI
};

enum class AnchorPosition
{
	CENTER,
	TOPLEFT,
	BOTLEFT,
	BOTRIGHT,
	BOTCENTER
};

struct Position 
{
	float x = 0;
	float y = 0;
	float z = 0;

	Position() {};
	Position(float x, float y) : x(x), y(y) {}
	Position(glm::vec2 position) : x(position.x), y(position.y) {}
};

struct Color	
{
	int r = 255;
	int g = 255;
	int b = 255;
	int a = 0;

	Color() {};
	Color(int r, int g, int b) : r(r), g(g), b(b) {}
	Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {}
};

struct Tag		{ std::string name; };
struct Active	{ bool isActive = true; };
struct Sprite	{ std::string name; bool isFlipped = false; Image* image = nullptr; };
struct Opacity	{ float alpha = 1.0f; };

struct Scale	
{ 
	glm::vec2 size = glm::vec2(1, 1);
	
	Scale(float s) : size(glm::vec2(s)) {};
	Scale(float x, float y) : size(glm::vec2(x, y)) {};
};

struct Fixed	{ };
struct Layer	{ LayerType layerType = LayerType::GAME; };
struct Rotation { uint16_t degrees = 0; };
struct Anchor	{ AnchorPosition anchor = AnchorPosition::TOPLEFT; glm::vec2 offset;  };

// maybe make Image an unique_ptr, actually shared is easy,
struct Text		{ std::string content; std::string fontName = "monogram"; Image* image = nullptr; bool isDirt = false; }; // if isDirt, recreate the text
struct RichText { std::string fontName; std::string content; };

struct Shader 	{ 
	std::string shaderName = "default"; 

	wgpu::RenderPipeline pipeline = nullptr;
	wgpu::Buffer pvm = nullptr;
	wgpu::Buffer material = nullptr;
	wgpu::Buffer frameData = nullptr;
	wgpu::BindGroup bindGroup = nullptr;

};

using Components = std::variant<
	Tag,
	Fixed,
	Position,
	Rotation,
	Active,
	Sprite,
	Scale,
	Layer,
	Opacity,
	Color,
	Anchor,
	Text,
	RichText,
	Shader
>;