#include "Entity.h"
#include "Ankaria.h"
#include "Renderer.h"
#include <iostream>

Entity::Entity(Ankaria& engine, std::vector<Components> components)
{
	this->engine = &engine;
	this->components = std::move(components);

	// Obligatory components
	if (!HasComponent<Position>())
		this->components.emplace_back(Position{});
	
	if (!HasComponent<Shader>())
	{
		this->components.emplace_back(Shader{});
	}

	if (!HasComponent<Active>())
		this->components.emplace_back(Active{});
	
	if (!HasComponent<Layer>())
		this->components.emplace_back(Layer{});

	if (!HasComponent<Opacity>())
		this->components.emplace_back(Opacity{});
	
	if (!HasComponent<Tag>())
		this->components.emplace_back(Tag{ "Noname" });
	
	if (!HasComponent<Scale>())
		this->components.emplace_back(Scale{1.0f});

	if (!HasComponent<Rotation>())
		this->components.emplace_back(Rotation{});

	if (!HasComponent<Anchor>())
		this->components.emplace_back(Anchor{});

	if (!HasComponent<Color>())
		this->components.emplace_back(Color{});

	// Some components are more complex, and depends on other components
	if (HasComponent<Sprite>())
		InitializeComponent<Sprite>(); // will depend on color

	if (HasComponent<Text>())
		InitializeComponent<Text>(); // depends on color

	InitializeComponent<Anchor>();
	InitializeComponent<Shader>(); // It needs sprite or text


	// Must be after sprite!
	//engine.renderer.InitializeEntity(*this);
}


void Entity::InitializeComponent(Sprite& sprite)
{
	sprite.image = &engine->resources.GetImage(sprite.name);
}

void Entity::InitializeComponent(Shader& shader)
{
	Image* image = nullptr;

	if (HasComponent<Sprite>())
		image = GetComponent<Sprite>()->image;

	else if (HasComponent<Text>())
		image = GetComponent<Text>()->image;

	shader.pipeline = engine->renderer.GetPipeline(shader.shaderName);
	shader.pvm = engine->renderer.CreateUniformBuffer(sizeof(PVM));
	shader.material = engine->renderer.CreateUniformBuffer(sizeof(Material));
	shader.frameData = engine->renderer.CreateUniformBuffer(sizeof(FrameData));
	shader.bindGroup = engine->renderer.CreateBindGroup(image, shader.pvm, shader.frameData, shader.material);
}

void Entity::InitializeComponent(Text& text)
{
	auto* color = GetComponent<Color>();

	SDL_Color textColor = SDL_Color{ 
		(unsigned char)color->r, 
		(unsigned char)color->g, 
		(unsigned char)color->b, 
		(unsigned char)color->a 
	};

	text.image = engine->resources.CreateText(text.fontName, text.content, textColor);
}

void Entity::InitializeComponent(Anchor& anchor)
{
	switch (anchor.anchor)
	{
		case AnchorPosition::CENTER:
			anchor.offset = glm::vec2(0.5f);
			break;

		case AnchorPosition::TOPLEFT:
			anchor.offset = glm::vec2(0.0f);
			break;

		case AnchorPosition::BOTLEFT:
			anchor.offset = glm::vec2(0.0, 1.0f);
			break;

		case AnchorPosition::BOTCENTER:
			anchor.offset = glm::vec2(0.5, 1.0f);
			break;

		case AnchorPosition::BOTRIGHT:
			anchor.offset = glm::vec2(1.0, 1.0f);
			break;

		default:
			anchor.offset = glm::vec2(0.0f); // same as TOPLEFT
			break;
	}
}

SDL_FRect Entity::GetBoundingBox()
{

	auto* sprite = GetComponent<Sprite>();
	if (!sprite)
		return SDL_FRect{ 0 };

	auto* position = GetComponent<Position>();
	if (!position)
		return SDL_FRect{ 0 };

	auto* anchor = GetComponent<Anchor>();
	if (!anchor)
		return SDL_FRect{ 0 };

	glm::vec2 size = glm::vec2(sprite->image->width, sprite->image->height);
	glm::vec2 topLeft = glm::vec2(position->x, position->y) - (anchor->offset * size);

	return SDL_FRect{ topLeft.x, topLeft.y, size.x, size.y };
}