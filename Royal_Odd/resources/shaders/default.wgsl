struct VertexInput
{
    @location(0) position   : vec2f,
    @location(1) uv         : vec2f,
    //@location(2) color      : vec3f,
};

struct VertexOuput
{
    @builtin(position) position: vec4f,
    @location(0) uv: vec2f,
};

struct PVM
{
    projection  : mat4x4<f32>,
    view        : mat4x4<f32>,
    model       : mat4x4<f32>
}

struct Material
{
    color       : vec4f,
    uvRect      : vec4f
}

struct FrameData {
    resolution : vec4<f32>,   // xy = screen size in pixels
    time       : vec4<f32>,   // x  = seconds
};


@group(0) @binding(0) var<uniform> pvm: PVM;
@group(0) @binding(1) var<uniform> material: Material;
@group(0) @binding(2) var<uniform> frame : FrameData;
@group(0) @binding(3) var texture: texture_2d<f32>;
@group(0) @binding(4) var textureSampler: sampler;

@vertex
fn vs_main(in: VertexInput) -> VertexOuput
{
    var out: VertexOuput;
    out.position = pvm.projection * pvm.view * pvm.model * vec4f(in.position, 0.0, 1.0);
    out.uv = in.uv * material.uvRect.zw + material.uvRect.xy;
    return out;
}

@fragment
fn fs_main(in: VertexOuput) ->@location(0) vec4f
{
	let color = textureSample(texture, textureSampler, in.uv);

	// 
	let linear_color = color.rgb * material.color.rgb;
	
	// Game Correction
	//let corrected_color = pow(linear_color, vec3f(1.0 / 2.2));

	return vec4f(linear_color, color.a);
}
