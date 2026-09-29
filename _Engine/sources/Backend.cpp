
#include <iostream>
#include <array>
#include <Windows.h>

#include "Backend.h"
#include "Ankaria.h"

void Backend::Initialize(Ankaria& engine)
{

	this->engine = &engine;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
	TTF_Init();

	window = SDL_CreateWindow(
		"Game",
		engine.settings.width,
		engine.settings.height,
		SDL_WINDOW_HIGH_PIXEL_DENSITY
	);

	if (!window)
	{
		std::cerr << "Failed to create a window" << std::endl;
		abort();
	}

	std::array<wgpu::InstanceFeatureName, 1> requiredFeatures = 
	{
		wgpu::InstanceFeatureName::TimedWaitAny
	};

	wgpu::InstanceDescriptor instanceDescriptor = {};
	instanceDescriptor.nextInChain = nullptr;
	instanceDescriptor.requiredFeatureCount = requiredFeatures.size();
	instanceDescriptor.requiredFeatures = requiredFeatures.data();

	instance = wgpu::CreateInstance(&instanceDescriptor);
	if (!instance)
	{
		std::cerr << "Failed to create instance." << std::endl;
		abort();
	}

	surface = RequestSurfaceSDL();
	if (!surface)
	{
		std::cerr << "Failed to create surface" << std::endl;
		abort();
	}

	adapter = RequestAdapter();
	if (!adapter)
	{
		std::cerr << "Failed to request an adapter" << std::endl;
		abort();
	}

	device = RequestDevice();
	if (!device)
	{
		std::cerr << "Failed to request a device" << std::endl;
		abort();
	}

	queue = device.GetQueue();

	ConfigureSurface();



}

wgpu::Surface Backend::RequestSurfaceSDL()
{
	SDL_PropertiesID props = SDL_GetWindowProperties(window);

	HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
	if (!hwnd) return nullptr;

	HINSTANCE hinstance = GetModuleHandle(nullptr);

	wgpu::SurfaceSourceWindowsHWND fromWindowsHWND;
	fromWindowsHWND.nextInChain = nullptr;
	fromWindowsHWND.sType = wgpu::SType::SurfaceSourceWindowsHWND;
	fromWindowsHWND.hinstance = hinstance;
	fromWindowsHWND.hwnd = hwnd;

	wgpu::SurfaceDescriptor surfaceDescriptor{};
	surfaceDescriptor.nextInChain = &fromWindowsHWND;
	surfaceDescriptor.label = "Windows";

	return instance.CreateSurface(&surfaceDescriptor);

}

wgpu::Adapter Backend::RequestAdapter()
{
	wgpu::RequestAdapterOptions requestAdapterOptions{};
	requestAdapterOptions.compatibleSurface = surface;
	requestAdapterOptions.backendType = wgpu::BackendType::D3D12;

	wgpu::Adapter adapter;

	wgpu::Future f = instance.RequestAdapter(
		&requestAdapterOptions,
		wgpu::CallbackMode::WaitAnyOnly,
		[&adapter](
			wgpu::RequestAdapterStatus status,
			wgpu::Adapter a,
			wgpu::StringView message
			)
		{
			if (status != wgpu::RequestAdapterStatus::Success)
			{
				std::cerr << "Failed to get an adapter" << message.data << std::endl;
				abort();
			}

			adapter = std::move(a);
		}
	);

	instance.WaitAny(f, UINT64_MAX);
	if (!adapter)
	{
		return nullptr;
	}
	return adapter;
}

wgpu::Device Backend::RequestDevice()
{
	wgpu::DeviceDescriptor deviceDescriptor{};

	deviceDescriptor.SetUncapturedErrorCallback([](
		const wgpu::Device&,
		wgpu::ErrorType,
		wgpu::StringView message
		) 
		{
			std::cerr << message.data << std::endl;
		}
	);

	deviceDescriptor.SetDeviceLostCallback(
		wgpu::CallbackMode::AllowSpontaneous,
		[](
			const wgpu::Device&,
			wgpu::DeviceLostReason reason,
			wgpu::StringView message
		)
		{
			// Normally closed
			if (reason == wgpu::DeviceLostReason::Destroyed)
				return;

			std::cerr << "Device lost due to " << message.data << std::endl;
		});

	wgpu::Device device;

	wgpu::Future f = adapter.RequestDevice(
		&deviceDescriptor,
		wgpu::CallbackMode::WaitAnyOnly,
		[&device](
			wgpu::RequestDeviceStatus status,
			wgpu::Device d,
			wgpu::StringView message
			)
		{
			if (status != wgpu::RequestDeviceStatus::Success)
			{
				std::cerr << "Failed to get a device" << message.data << std::endl;
				return;
			}
			device = std::move(d);
		}
	);

	instance.WaitAny(f, UINT64_MAX);
	if (!device)
		return nullptr;
	return device;
}

void Backend::ConfigureSurface()
{
	wgpu::SurfaceCapabilities surfaceCapabilities = {};
	surface.GetCapabilities(adapter, &surfaceCapabilities);

	wgpu::TextureFormat surfaceFormat = surfaceCapabilities.formats[0];
	wgpu::TextureFormat renderFormat = surfaceFormat;

	format = renderFormat;

	if (surfaceFormat == wgpu::TextureFormat::BGRA8Unorm)
		format = wgpu::TextureFormat::BGRA8UnormSrgb;

	else if (surfaceFormat == wgpu::TextureFormat::RGBA8Unorm)
		format = wgpu::TextureFormat::BGRA8UnormSrgb;

	wgpu::SurfaceConfiguration surfaceConfiguration = {};
	surfaceConfiguration.nextInChain = nullptr;
	surfaceConfiguration.width = engine->settings.width;
	surfaceConfiguration.height = engine->settings.height;
	surfaceConfiguration.usage = wgpu::TextureUsage::RenderAttachment;
	surfaceConfiguration.format = surfaceFormat;
	surfaceConfiguration.alphaMode = surfaceCapabilities.alphaModes[0];
	surfaceConfiguration.device = device;
	surfaceConfiguration.presentMode = wgpu::PresentMode::Fifo;
	surfaceConfiguration.viewFormats = &format;
	surfaceConfiguration.viewFormatCount = 1;

	
	surface.Configure(&surfaceConfiguration);

}