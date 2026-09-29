#include "Resources.h"
#include "Ankaria.h"

#include <iostream>
#include <cerrno>
#include <SDL3/SDL.h>

void Resources::Initialize(Ankaria& engine)
{
	this->engine = &engine;
}

// General

Image Resources::CreateImage(std::vector<uint8_t> pixels, int w, int h)
{

	// Texture
	wgpu::TextureDescriptor textureDescriptor = {};
	textureDescriptor.dimension = wgpu::TextureDimension::e2D;
	textureDescriptor.size = { (uint32_t)w, (uint32_t)h, 1 };
	textureDescriptor.mipLevelCount = 1;
	textureDescriptor.sampleCount = 1;
	textureDescriptor.format = wgpu::TextureFormat::RGBA8UnormSrgb;
	textureDescriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
	textureDescriptor.viewFormatCount = 0;

	// Texture View
	wgpu::TextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.aspect = wgpu::TextureAspect::All;
	textureViewDescriptor.baseArrayLayer = 0;
	textureViewDescriptor.arrayLayerCount = 1;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.dimension = wgpu::TextureViewDimension::e2D;
	textureViewDescriptor.format = textureDescriptor.format;

	// Sampler
	wgpu::SamplerDescriptor samplerDescriptor = {};
	samplerDescriptor.addressModeU = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.addressModeV = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.addressModeW = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.magFilter = wgpu::FilterMode::Nearest;
	samplerDescriptor.minFilter = wgpu::FilterMode::Nearest;
	samplerDescriptor.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
	samplerDescriptor.lodMinClamp = 0.0f;
	samplerDescriptor.lodMaxClamp = 1.0f;
	samplerDescriptor.compare = wgpu::CompareFunction::Undefined;
	samplerDescriptor.maxAnisotropy = 1;

	Image image = {};
	image.width = w;
	image.height = h;
	image.texture = engine->backend.device.CreateTexture(&textureDescriptor);
	image.textureView = image.texture.CreateView(&textureViewDescriptor);
	image.sampler = engine->backend.device.CreateSampler(&samplerDescriptor);

	wgpu::TexelCopyBufferLayout source = {};
	source.bytesPerRow = 4 * w;
	source.rowsPerImage = h;

	wgpu::TexelCopyTextureInfo destination = {};
	destination.texture = image.texture;
	destination.mipLevel = 0;

	engine->backend.queue.WriteTexture(
		&destination,
		pixels.data(),
		pixels.size(),
		&source,
		&textureDescriptor.size
	);

	return image;
}

Image Resources::CreateImageFromSDLSurface(SDL_Surface& surface)
{
	int bytesPerPixel = 4;
	const int rowSize = surface.w * bytesPerPixel;

	std::uint8_t* source = static_cast<uint8_t*>(surface.pixels);
	std::vector<uint8_t> destination(rowSize * surface.h);

	for (int y = 0; y < surface.h; y++)
	{
		std::memcpy(
			destination.data() + y * rowSize,
			source + y *surface.pitch,
			rowSize
		);
	}

	return CreateImage(
		std::move(destination),
		surface.w,
		surface.h
	);
}

// Sprite Related

Image Resources::LoadImage(std::string name, std::string path)
{
	SDL_Surface* original = SDL_LoadPNG(("Resources/Sprites/" + path).c_str());
	if (!original)
	{
		std::cerr << SDL_GetError() << std::endl;
		std::abort();
	}

	SDL_Surface* converted = SDL_ConvertSurface(original, SDL_PIXELFORMAT_RGBA32);
	SDL_DestroySurface(original);

	// Texture
	wgpu::TextureDescriptor textureDescriptor = {};
	textureDescriptor.dimension = wgpu::TextureDimension::e2D;
	textureDescriptor.size = { (uint32_t)converted->w, (uint32_t)converted->h, 1 };
	textureDescriptor.mipLevelCount = 1;
	textureDescriptor.sampleCount = 1;
	textureDescriptor.format = wgpu::TextureFormat::RGBA8UnormSrgb;
	textureDescriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
	textureDescriptor.viewFormatCount = 0;

	// Texture View
	wgpu::TextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.aspect = wgpu::TextureAspect::All;
	textureViewDescriptor.baseArrayLayer = 0;
	textureViewDescriptor.arrayLayerCount = 1;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.dimension = wgpu::TextureViewDimension::e2D;
	textureViewDescriptor.format = textureDescriptor.format;

	// Sampler
	wgpu::SamplerDescriptor samplerDescriptor = {};
	samplerDescriptor.addressModeU = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.addressModeV = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.addressModeW = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.magFilter = wgpu::FilterMode::Nearest;
	samplerDescriptor.minFilter = wgpu::FilterMode::Nearest;
	samplerDescriptor.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
	samplerDescriptor.lodMinClamp = 0.0f;
	samplerDescriptor.lodMaxClamp = 1.0f;
	samplerDescriptor.compare = wgpu::CompareFunction::Undefined;
	samplerDescriptor.maxAnisotropy = 1;

	Image image = {};
	image.width = converted->w;
	image.height = converted->h;
	image.texture = engine->backend.device.CreateTexture(&textureDescriptor);
	image.textureView = image.texture.CreateView(&textureViewDescriptor);
	image.sampler = engine->backend.device.CreateSampler(&samplerDescriptor);

	wgpu::TexelCopyTextureInfo dst = {};
	dst.texture = image.texture;
	dst.mipLevel = 0;

	wgpu::TexelCopyBufferLayout src = {};
	src.bytesPerRow = converted->pitch;
	src.rowsPerImage = converted->h;

	engine->backend.queue.WriteTexture(
		&dst,
		converted->pixels,
		converted->pitch * converted->h,
		&src,
		&textureDescriptor.size
	);

	SDL_DestroySurface(converted);

	images.emplace(name, image);
	return images.at(name);
}

Image Resources::CreateImageFromImage(const Image& input, std::string name, glm::ivec4 rect)
{
	uint32_t x = rect.x;
	uint32_t y = rect.y;
	uint32_t w = rect.z;
	uint32_t h = rect.w;

	// Texture
	wgpu::TextureDescriptor textureDescriptor = {};
	textureDescriptor.dimension = wgpu::TextureDimension::e2D;
	textureDescriptor.size = { w, h, 1 };
	textureDescriptor.mipLevelCount = 1;
	textureDescriptor.sampleCount = 1;
	textureDescriptor.format = wgpu::TextureFormat::RGBA8UnormSrgb;
	textureDescriptor.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
	textureDescriptor.viewFormatCount = 0;

	// Texture View
	wgpu::TextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.aspect = wgpu::TextureAspect::All;
	textureViewDescriptor.baseArrayLayer = 0;
	textureViewDescriptor.arrayLayerCount = 1;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.dimension = wgpu::TextureViewDimension::e2D;
	textureViewDescriptor.format = textureDescriptor.format;

	// Sampler
	wgpu::SamplerDescriptor samplerDescriptor = {};
	samplerDescriptor.addressModeU = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.addressModeV = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.addressModeW = wgpu::AddressMode::ClampToEdge;
	samplerDescriptor.magFilter = wgpu::FilterMode::Nearest;
	samplerDescriptor.minFilter = wgpu::FilterMode::Nearest;
	samplerDescriptor.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
	samplerDescriptor.lodMinClamp = 0.0f;
	samplerDescriptor.lodMaxClamp = 1.0f;
	samplerDescriptor.compare = wgpu::CompareFunction::Undefined;
	samplerDescriptor.maxAnisotropy = 1;

	Image image = {};
	image.width = w;
	image.height = h;
	image.texture = engine->backend.device.CreateTexture(&textureDescriptor);
	image.textureView = image.texture.CreateView(&textureViewDescriptor);
	image.sampler = engine->backend.device.CreateSampler(&samplerDescriptor);

	wgpu::TexelCopyTextureInfo source = {};
	source.texture = input.texture;
	source.mipLevel = 0;
	source.origin = wgpu::Origin3D{ x, y, 0 };
	source.aspect = wgpu::TextureAspect::All;

	wgpu::TexelCopyTextureInfo destination = {};
	destination.texture = image.texture;
	destination.mipLevel = 0;
	destination.origin = wgpu::Origin3D{ 0, 0, 0 };
	destination.aspect = wgpu::TextureAspect::All;

	wgpu::Extent3D extent3D = { .width = w, .height = h, .depthOrArrayLayers = 1 };

	wgpu::CommandEncoder encoder = engine->backend.device.CreateCommandEncoder();
	encoder.CopyTextureToTexture(&source, &destination, &extent3D);
	wgpu::CommandBuffer commandBuffer = encoder.Finish();
	engine->backend.queue.Submit(1, &commandBuffer);

	images.emplace(name, image);
	return images.at(name);
}

Image& Resources::GetImage(const std::string& name)
{
	if (!images.contains(name))
	{
		std::cerr << "Image not found" << std::endl;
		std::abort();
	}
	return images.at(name);
}

// Font Related

Font Resources::LoadFont(std::string name, std::string path, std::uint8_t size)
{
	Font font;

	font.font = TTF_OpenFont(("Resources/Fonts/" + path).c_str(), size);
	if (!font.font)
	{
		std::cerr << "Failed to load font: " << name << " at " << path << std::endl;
		std::abort();
	}

	fonts.emplace(name, font);
	return fonts.at(name);
}

Font& Resources::GetFont(const std::string& name)
{
	if (fonts.contains(name))
		return fonts.at(name);

	std::cerr << "Font not found." << std::endl;
	std::abort();
}

// TTF_SetFontOutline needs a rework imo, the color and the outline are the same color 
Image* Resources::CreateText(const std::string& fontName, std::string& content, SDL_Color color)
{
	Font& font = GetFont(fontName);
	int outlineSize = 1;
	//TTF_SetFontCharSpacing(font.font, 2);

	SDL_Color textColor = color;
	SDL_Color outlineColor = SDL_Color{ 0, 0, 0, 255 };

	TTF_SetFontOutline(font.font, outlineSize);
	SDL_Surface* outline = TTF_RenderText_Solid(font.font, content.c_str(), content.size(), outlineColor);

	TTF_SetFontOutline(font.font, 0);
	SDL_Surface* text = TTF_RenderText_Solid(font.font,content.c_str(),content.size(), textColor);

	SDL_Surface* textRGBA = SDL_ConvertSurface(text, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface* oultineRGBA = SDL_ConvertSurface(outline, SDL_PIXELFORMAT_RGBA32);

	SDL_DestroySurface(text);
	SDL_DestroySurface(outline);

	SDL_SetSurfaceBlendMode(textRGBA, SDL_BLENDMODE_BLEND);
	SDL_Rect offset = SDL_Rect{ outlineSize, outlineSize, textRGBA->w, textRGBA->h };

	SDL_BlitSurface(textRGBA, nullptr, oultineRGBA, &offset);

	Image image = CreateImageFromSDLSurface(*oultineRGBA);

	SDL_DestroySurface(oultineRGBA);
	SDL_DestroySurface(textRGBA);

	texts.push_back(std::make_unique<Image>(std::move(image)));
	return texts.back().get();
}
