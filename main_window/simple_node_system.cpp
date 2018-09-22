#include "simple_node_system.hpp"

using namespace Seed::Math;

Seed::NodeSystem::SimpleNodeSystem::SimpleNodeSystem(std::unique_ptr<iGraphics>& graphics)
	: iNodeSystem(graphics)
{
}

void Seed::NodeSystem::SimpleNodeSystem::Initialize(void)
{
	this->repeat_shader_ = this->graphics_->LoadShader("repeat.hlsl");
	this->bg_tex_ = this->graphics_->LoadTexture("bg.png");
	this->default_sampler_ = this->graphics_->LoadSampler(iGraphics::Filter::Linear, iGraphics::AddressMode::Wrap);

	this->bg_scale_ = this->graphics_->GetShaderVariable<float2>(this->repeat_shader_, "g_scale");
	this->bg_scroll_ = this->graphics_->GetShaderVariable<float2>(this->repeat_shader_, "g_scroll");
}

void Seed::NodeSystem::SimpleNodeSystem::DrawBackground(void)
{
	this->graphics_->SetSampler(this->default_sampler_);

	auto screen_size = this->graphics_->size();

	float2 size = this->graphics_->GetTextureSize(this->bg_tex_) * 0.1f;

	auto view_pos = float2(0, 0);

	*this->bg_scale_ = float2(screen_size.x / size.x, screen_size.y / size.y) * .5f;
	*this->bg_scroll_ = float2(-view_pos.x / size.x / 2, view_pos.y / size.y / 2);

	this->graphics_->SetShader(this->repeat_shader_);

	this->graphics_->SetDiffuse(DirectX::Color(1, 1, 1, 1));

	this->graphics_->SetShaderResourceFromTexture(0, { this->bg_tex_ });

	auto world = matrix::CreateScale(size.x * this->bg_scale_->x, size.y * this->bg_scale_->y, 1.f);

	this->graphics_->SetWorld(world);

	this->graphics_->UpdatePerMeshCB();
	this->graphics_->UpdateUniqueCB(this->repeat_shader_);
	this->graphics_->DrawVertices(iGraphics::Topology::TriangleStrip, 4);
}

void Seed::NodeSystem::SimpleNodeSystem::DrawNode(Node * const node)
{
}
