#include "Renderer.h"
#include "Ankaria.h"

#include <glm/gtc/matrix_transform.hpp>

void Renderer::Initialize(Ankaria& engine)
{
	this->engine = &engine;
	// Create minimal 

	InitializeQuads();

	frameLayout = CreateFrameLayout();

	CreatePipeline("default", "default.wgsl");
}

void Renderer::BeginFrame()
{
	wgpu::TextureView targetView = GetCurrentTextureView();
	if (!targetView)
		return;

	wgpu::RenderPassColorAttachment colorAttachment = {};
	colorAttachment.view = targetView;
	colorAttachment.loadOp = wgpu::LoadOp::Clear;
	colorAttachment.storeOp = wgpu::StoreOp::Store;
	colorAttachment.clearValue = { 1.0f, 0.0, 0.0, 1.0f };

	wgpu::RenderPassDescriptor renderPassDescriptor = {};
	renderPassDescriptor.nextInChain = nullptr;
	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.colorAttachments = &colorAttachment;
	renderPassDescriptor.depthStencilAttachment = nullptr;
	renderPassDescriptor.timestampWrites = nullptr;
	
	commandEncoder = engine->backend.device.CreateCommandEncoder();
	pass = commandEncoder.BeginRenderPass(&renderPassDescriptor);

}

void Renderer::EndFrame()
{
	pass.End();
	
	wgpu::CommandBuffer commandBuffer = commandEncoder.Finish();
	engine->backend.queue.Submit(1, &commandBuffer);

	engine->backend.surface.Present();
}

void Renderer::Render(std::vector<std::unique_ptr<Entity>>& entities)
{

	//if (background)
	//DrawQuad(pass, );

	
	// Get Camera

	BeginFrame();

	glm::mat4 projection = glm::ortho(0.0f, (float)engine->settings.width, (float)engine->settings.height, 0.0f, -1.0f, 1.0f);

	for (auto layer : layers)
	{
		for (const auto& entity : entities)
		{
			if (entity->GetComponent<Layer>()->layerType != layer) continue;
			if (entity->GetComponent<Active>()->isActive != true)  continue;

			// Get Image
			Image* image = nullptr;

			if (entity->HasComponent<Sprite>())
				image = entity->GetComponent<Sprite>()->image;

			else if (entity->HasComponent<Text>())
				image = entity->GetComponent<Text>()->image;

			if (!image)
				continue;

			auto* position = entity->GetComponent<Position>();
			auto* scale = entity->GetComponent<Scale>();
			auto* anchor = entity->GetComponent<Anchor>();
			auto* opacity = entity->GetComponent<Opacity>();
			auto* shader = entity->GetComponent<Shader>();

			//auto it = entityRenderData.find(entity.get());
			//if (it == entityRenderData.end())
			//	continue;

			float w = image->width * scale->size.x;
			float h = image->height * scale->size.y;

			float x = std::round(position->x - w * anchor->offset.x);
			float y = std::round(position->y - h * anchor->offset.y);


			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, y, position->z));
			// rotation
			model = glm::scale(model, glm::vec3(w, h, 1.0f));
			model = glm::scale(model, glm::vec3(engine->settings.scale, engine->settings.scale, 1.0f)); // Global Scale
			// scale
			// translate


			// Uniforms
			PVM pvm;
			pvm.projection = projection;
			pvm.view = glm::mat4(1.0f);
			pvm.model = model;

			Material material;
			//material.uvRect = sprite->isFlipped ? glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			material.uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			material.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

			static float frames = 0.0f;
			frames++;

			FrameData frameData;
			frameData.resolution = glm::vec4(1920.0f, 1080.0f, 0.0f, 0.0f);
			frameData.time       = glm::vec4(frames, 0.0f, 0.0f, 0.0f);

			//RenderData& data = it->second;

			engine->backend.queue.WriteBuffer(shader->pvm, 0, &pvm, sizeof(PVM));
			engine->backend.queue.WriteBuffer(shader->material, 0, &material, sizeof(Material));
			engine->backend.queue.WriteBuffer(shader->frameData, 0, &frameData, sizeof(FrameData));


			pass.SetPipeline(shader->pipeline);
			pass.SetVertexBuffer(0, quadVertexBuffer);
			pass.SetIndexBuffer(quadIndexBuffer, wgpu::IndexFormat::Uint16);
			pass.SetBindGroup(0, shader->bindGroup);
			pass.DrawIndexed(6);
		}
	}

	EndFrame();

}

wgpu::TextureView Renderer::GetCurrentTextureView()
{
	wgpu::TextureView textureView = nullptr;

	engine->backend.surface.GetCurrentTexture(&engine->backend.surfaceTexture);

	if (engine->backend.surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
		engine->backend.surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal)
	{
		return nullptr;
	}

	wgpu::TextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.nextInChain = nullptr;
	textureViewDescriptor.format = engine->backend.format;
	textureViewDescriptor.dimension = wgpu::TextureViewDimension::e2D;
	textureViewDescriptor.baseMipLevel = 0;
	textureViewDescriptor.mipLevelCount = 1;
	textureViewDescriptor.aspect = wgpu::TextureAspect::All;

	textureView = engine->backend.surfaceTexture.texture.CreateView(&textureViewDescriptor);

	if (!textureView)
		return nullptr;
	return textureView;

}

wgpu::Buffer Renderer::CreateUniformBuffer(size_t size)
{
	wgpu::BufferDescriptor descriptor = {};
	descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
	descriptor.size = size;
	return engine->backend.device.CreateBuffer(&descriptor);
}

wgpu::BindGroup Renderer::CreateBindGroup(Image* image, const wgpu::Buffer& pvmBuffer, const wgpu::Buffer& frameData, const wgpu::Buffer& materialBuffer)
{
	std::array<wgpu::BindGroupEntry, 5> entries = {};

	// Transform 
	entries[0].binding = 0;
	entries[0].buffer = pvmBuffer;
	entries[0].offset = 0;
	entries[0].size = sizeof(PVM);

	// Material
	entries[1].binding = 1;
	entries[1].buffer = materialBuffer;
	entries[1].offset = 0;
	entries[1].size = sizeof(Material);

	// frameData
	entries[2].binding = 2;
	entries[2].buffer = frameData;
	entries[2].offset = 0;
	entries[2].size = sizeof(FrameData);

	// Texture View
	entries[3].binding = 3;
	entries[3].textureView = image->textureView;

	// Sampler
	entries[4].binding = 4;
	entries[4].sampler = image->sampler;

	wgpu::BindGroupLayout layout = CreateBindgroupLayout();

	wgpu::BindGroupDescriptor descriptor = {};
	descriptor.entryCount = entries.size();
	descriptor.entries = entries.data();
	descriptor.layout = layout;

	return engine->backend.device.CreateBindGroup(&descriptor);
}

void Renderer::InitializeEntity(Entity& entity)
{



}

wgpu::RenderPipeline Renderer::CreatePipeline(std::string name, std::string path) 
{

	void* shaderFile = SDL_LoadFile(("Resources/Shaders/" + path).c_str(), NULL);
	if (!shaderFile)
	{
		std::cerr << "Failed to find file: " << path << std::endl;
		std::abort();
	}

	wgpu::ShaderSourceWGSL shaderSource = {};
	shaderSource.sType = wgpu::SType::ShaderSourceWGSL;
	shaderSource.code = static_cast<char*>(shaderFile);

	wgpu::ShaderModuleDescriptor shaderModuleDescriptor = {};
	shaderModuleDescriptor.nextInChain = &shaderSource;

	wgpu::ShaderModule shaderModule = {};
	shaderModule = engine->backend.device.CreateShaderModule(&shaderModuleDescriptor);
	if (!shaderModule)
	{
		std::cerr << "Unable to create shader module." << std::endl;
		std::abort();
	}

	std::array<wgpu::VertexAttribute, 2> attributes = {};

	// Vertex Position
	attributes[0].shaderLocation = 0;
	attributes[0].format = wgpu::VertexFormat::Float32x2;
	attributes[0].offset = 0;

	// UV
	attributes[1].shaderLocation = 1;
	attributes[1].format = wgpu::VertexFormat::Float32x2;
	attributes[1].offset = 2 * sizeof(float);

	wgpu::VertexBufferLayout vertexBufferLayout = {};
	vertexBufferLayout.attributeCount = attributes.size();
	vertexBufferLayout.attributes = attributes.data();
	vertexBufferLayout.arrayStride = 4 * sizeof(float);
	vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;


	// Vertex State
	wgpu::VertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = "vs_main";
	vertexState.bufferCount = 1;
	vertexState.buffers = &vertexBufferLayout;

	// Blend State
	wgpu::BlendState blendState = {};
	blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
	blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
	blendState.color.operation = wgpu::BlendOperation::Add;
	blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
	blendState.alpha.dstFactor = wgpu::BlendFactor::One;
	blendState.alpha.operation = wgpu::BlendOperation::Add;

	// Color Target State
	wgpu::ColorTargetState colorTargetState = {};
	colorTargetState.format = engine->backend.format;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = wgpu::ColorWriteMask::All;

	// Fragment State
	wgpu::FragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTargetState;


	// Primiteve State
	wgpu::PrimitiveState primitiveState = {};
	primitiveState.topology = wgpu::PrimitiveTopology::TriangleList;
	primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
	primitiveState.frontFace = wgpu::FrontFace::CCW;
	primitiveState.cullMode = wgpu::CullMode::None;

	wgpu::MultisampleState multisampleState = {};
	multisampleState.count = 1;
	multisampleState.mask = ~0u;
	multisampleState.alphaToCoverageEnabled = false;


	wgpu::BindGroupLayout bindGroupLayoutDescriptor = CreateBindgroupLayout();

	wgpu::PipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindGroupLayoutDescriptor;
	wgpu::PipelineLayout pipelineLayout = engine->backend.device.CreatePipelineLayout(&pipelineLayoutDescriptor);

	// Final Steps
	wgpu::RenderPipelineDescriptor descriptor = {};
	descriptor.layout = pipelineLayout;
	descriptor.vertex = vertexState;
	descriptor.fragment = &fragmentState;
	descriptor.primitive = primitiveState;
	descriptor.multisample = multisampleState;
	descriptor.depthStencil = nullptr;

	wgpu::RenderPipeline pipeline = engine->backend.device.CreateRenderPipeline(&descriptor);
	
	pipelines.insert({ name, pipeline });
	return pipelines.at(name);
}

wgpu::BindGroupLayout Renderer::CreateBindgroupLayout()
{
	std::array<wgpu::BindGroupLayoutEntry, 5> entries{};

	// PVM 
	entries[0].binding = 0;
	entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
	entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
	entries[0].buffer.minBindingSize = sizeof(PVM);

	// Material
	entries[1].binding = 1;
	entries[1].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
	entries[1].buffer.type = wgpu::BufferBindingType::Uniform;
	entries[1].buffer.minBindingSize = sizeof(Material);

	// FrameData
	entries[2].binding = 2;
	entries[2].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
	entries[2].buffer.type = wgpu::BufferBindingType::Uniform;
	entries[2].buffer.minBindingSize = sizeof(FrameData);

	// Texture
	entries[3].binding = 3;
	entries[3].visibility = wgpu::ShaderStage::Fragment;
	entries[3].texture.sampleType = wgpu::TextureSampleType::Float;
	entries[3].texture.viewDimension = wgpu::TextureViewDimension::e2D;

	// Sampler
	entries[4].binding = 4;
	entries[4].visibility = wgpu::ShaderStage::Fragment;
	entries[4].sampler.type = wgpu::SamplerBindingType::Filtering;

	wgpu::BindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = entries.size();
	bindGroupLayoutDescriptor.entries = entries.data();
	
	return engine->backend.device.CreateBindGroupLayout(&bindGroupLayoutDescriptor);
}

wgpu::RenderPipeline& Renderer::GetPipeline(const std::string& name)
{
	if (!pipelines.contains(name))
	{
		std::cerr << "Image not found" << std::endl;
		std::abort();
	}
	return pipelines.at(name);
}

void Renderer::InitializeQuads()
{
	// Datas

	std::array<float, 16> vertices =
	{
		0.0f, 0.0f,		0.0f, 0.0f,
		1.0f, 0.0f,		1.0f, 0.0f,
		1.0f, 1.0f,		1.0f, 1.0f,
		0.0f, 1.0f,		0.0f, 1.0f
	};

	std::array<uint16_t, 6> indeces =
	{
		0, 1, 2,
		0, 2, 3
	};

	// Base
	wgpu::BufferDescriptor descriptor = {};
	descriptor.mappedAtCreation = false;

	// Vertices
	descriptor.size = vertices.size() * sizeof(float);
	descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
	quadVertexBuffer = engine->backend.device.CreateBuffer(&descriptor);
	engine->backend.queue.WriteBuffer(quadVertexBuffer, 0, vertices.data(), descriptor.size);

	// Indeces
	descriptor.size = indeces.size() * sizeof(uint16_t);
	descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
	quadIndexBuffer = engine->backend.device.CreateBuffer(&descriptor);
	engine->backend.queue.WriteBuffer(quadIndexBuffer, 0, indeces.data(), descriptor.size);

}


wgpu::BindGroupLayout Renderer::CreateFrameLayout()
{
	//std::array<wgpu::BindGroupLayoutEntry, 3> entries;
	wgpu::BindGroupLayoutEntry entry{};

	entry.binding = 0;
	entry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
	entry.buffer.type = wgpu::BufferBindingType::Uniform;

	wgpu::BindGroupLayoutDescriptor descriptor{};
	descriptor.entryCount = 1;
	descriptor.entries = &entry;

	return engine->backend.device.CreateBindGroupLayout(&descriptor);

}

void Renderer::DrawQuad(FullQuadData& data)
{

}