#pragma once

#include <array>

#include <glm/glm.hpp>
#include <dawn/webgpu_cpp.h>

#include "Components.h"
#include "Backend.h"
#include "Entity.h"

class Ankaria;

struct PVM {
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;
};

struct Material {
	glm::vec4 color;
	glm::vec4 uvRect;
};

struct RenderData 
{
	wgpu::Buffer pvmBuffer;
	wgpu::Buffer materialBuffer;
	wgpu::Buffer frameData;
	wgpu::BindGroup bindGroup;

	std::string pipelineName = "default";
};

struct FullQuadData
{
	std::string pipelineName;

	wgpu::BindGroup bindGroup;
};

struct FrameData
{
	glm::vec4 resolution;
	glm::vec4 time;
};

class Renderer
{

private:
	Ankaria* engine = nullptr;

	std::unordered_map<std::string, wgpu::RenderPipeline> pipelines;

	wgpu::TextureView GetCurrentTextureView();

	// Shared for every quad
	wgpu::Buffer quadVertexBuffer;
	wgpu::Buffer quadIndexBuffer;

	wgpu::BindGroupLayout CreateBindgroupLayout();


	wgpu::CommandEncoder commandEncoder;
	wgpu::RenderPassEncoder pass;

	// DrawQuad
	void DrawQuad(FullQuadData& data);

	void InitializeQuads();

	void BeginFrame();
	void EndFrame();

	// Bind Groups
	wgpu::BindGroupLayout CreateFrameLayout();

	wgpu::BindGroupLayout frameLayout;

public:
	void Initialize(Ankaria& engine);
	void InitializeEntity(Entity& Entity);
	void Render(std::vector<std::unique_ptr<Entity>>& entities);

	wgpu::Buffer CreateUniformBuffer(size_t size);
	wgpu::BindGroup CreateBindGroup(Image* image, const wgpu::Buffer& pvmBuffer, const wgpu::Buffer& frameData, const wgpu::Buffer& materialBuffer);
	std::unordered_map<const Entity*, RenderData> entityRenderData;

	wgpu::RenderPipeline CreatePipeline(std::string name, std::string path);
	wgpu::RenderPipeline& GetPipeline(const std::string& name);


	std::array<LayerType, 4> layers{ LayerType::BG, LayerType::GAME, LayerType::FG, LayerType::UI };
};

