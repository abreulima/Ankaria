#pragma once

#include <string>
#include "Backend.h"

#include <dawn/webgpu_cpp.h>
#include <glm/glm.hpp>

#include <SDL3_ttf/SDL_ttf.h>

struct Image
{
	wgpu::Texture texture;
	wgpu::TextureView textureView;
	wgpu::Sampler sampler;
	uint32_t width;
	uint32_t height;
};

struct Font
{
	TTF_Font* font;
	std::uint8_t size;
};

class Ankaria;

class Resources
{
private:
	std::unordered_map<std::string, Image> images;
	std::unordered_map<std::string, Font> fonts;
	//std::unordered_map<std::string, Image> text;
	std::vector<std::unique_ptr<Image>> texts;


	Ankaria* engine = nullptr;
public:
	void Initialize(Ankaria& engine);
	
	// General
	Image CreateImageFromSDLSurface(SDL_Surface& surface);
	Image CreateImage(std::vector<uint8_t> pixels, int w, int h);

	// Image
	Image LoadImage(std::string name, std::string path);
	Image& GetImage(const std::string& name);
	Image CreateImageFromImage(const Image& image, std::string name, glm::ivec4 rect);

	// Font
	Font LoadFont(std::string name, std::string path, std::uint8_t size);
	Font& GetFont(const std::string& name);
	Image* CreateText(const std::string& fontName, std::string& contentm, SDL_Color color);
};