#pragma once

#include <webgpu/webgpu_cpp.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Ankaria;

class Backend
{
public:

	void Initialize(Ankaria& engine);

	// SDL
	SDL_Window* window = nullptr;
	SDL_Gamepad* gamepad = nullptr;
	SDL_DisplayID* display = nullptr;
	const bool* keys = nullptr;

	int displayCount = 0;

	// WebGPU
	wgpu::Instance instance;
	wgpu::Device device;
	wgpu::Queue queue;
	wgpu::Surface surface;
	wgpu::SurfaceTexture surfaceTexture;
	wgpu::Adapter adapter;
	wgpu::TextureFormat format;
	wgpu::CommandEncoder commandEncoder;

private:
	Ankaria* engine = nullptr;
	wgpu::Surface RequestSurfaceSDL();
	wgpu::Adapter RequestAdapter();
	wgpu::Device RequestDevice();
	void ConfigureSurface();


};

