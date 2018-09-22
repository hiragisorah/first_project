#include "main_window.hpp"

#include <seed/window>
#include <seed/graphics>
#include <seed/graphics/directx11>

#define WIN32_LEAD_AND_MEAN
#include <Windows.h>
#include <unordered_map>

#include "simple_node_system.hpp"

//static std::unordered_map<unsigned int, bool> _key_map;

using namespace Seed::Math;

class Seed::MainWindow::Impl
{
	friend Seed::MainWindow;

public:
	Impl(void)
		: window_(std::make_unique<Seed::Window>())
		, graphics_(std::make_unique<Seed::Graphics::DirectX11>())
		, node_system_(std::make_unique<Seed::NodeSystem::SimpleNodeSystem>(graphics_))
	{}

private:
	std::unique_ptr<Seed::Window> window_;
	std::unique_ptr<Seed::iGraphics> graphics_;
	std::unique_ptr<Seed::iNodeSystem> node_system_;
	
private:
	// rtv
	int back_buffer_;

	// dsv
	int default_dsv_;

	// vp
	int default_vp_;

	// font
	int meiryo_font_;

	// texture
	int test_text_;

	// shader
	int default_2d_shader_;

	// sampler
	int default_sampler_;

	// blend
	int alpha_blend_;
};

Seed::MainWindow::MainWindow(void)
	: impl_(std::make_unique<MainWindow::Impl>())
{

}

Seed::MainWindow::~MainWindow(void)
{

}

const bool Seed::MainWindow::Initialize(void)
{
	this->impl_->window_->Initalize("window_title", 1280U, 720U);

	void * hwnd = this->impl_->window_->get_hwnd<void*>();
	Math::float2 size = Math::float2(this->impl_->window_->get_width<float>(), this->impl_->window_->get_height<float>());

	this->impl_->graphics_->Initialize(hwnd, size);
	this->impl_->node_system_->Initialize();

	this->impl_->back_buffer_ = this->impl_->graphics_->LoadRenderTargetBackBuffer(static_cast<int>(size.x), static_cast<int>(size.y));

	this->impl_->default_dsv_ = this->impl_->graphics_->LoadDepthsStencil(static_cast<int>(size.x), static_cast<int>(size.y));

	this->impl_->default_vp_ = this->impl_->graphics_->LoadViewPort(static_cast<int>(size.x), static_cast<int>(size.y));

	this->impl_->meiryo_font_ = this->impl_->graphics_->LoadFont("meiryo.ttc");

	this->impl_->test_text_ = this->impl_->graphics_->LoadTextureFromFont(this->impl_->meiryo_font_, "ABC‚ ‚¢‚¤", 512);

	this->impl_->default_2d_shader_ = this->impl_->graphics_->LoadShader("default_2d.hlsl");

	this->impl_->default_sampler_ = this->impl_->graphics_->LoadSampler(iGraphics::Filter::Linear, iGraphics::AddressMode::Wrap);

	this->impl_->alpha_blend_ =
		this->impl_->graphics_->LoadBlend(iGraphics::BlendOption::Add, iGraphics::BlendType::SrcAlpha, iGraphics::BlendType::InvSrcAlpha,
			iGraphics::BlendOption::Add, iGraphics::BlendType::One, iGraphics::BlendType::Zero);

	return true;
}

const bool Seed::MainWindow::Run(void)
{
	auto ret = this->impl_->window_->Run();

	this->impl_->graphics_->ClearRenderTarget({ this->impl_->back_buffer_ }, Math::color(.2f, .4f, .8f, 1.f));
	this->impl_->graphics_->ClearDepthStencil({ });

	this->impl_->graphics_->SetRenderTargetAndDepthStencil({ this->impl_->back_buffer_ }, -1);
	this->impl_->graphics_->SetViewPort({ this->impl_->default_vp_ });
	this->impl_->graphics_->SetView(DirectX::XMMatrixIdentity());

	this->impl_->graphics_->UpdatePerCameraCB();

	this->impl_->graphics_->SetSampler(this->impl_->default_sampler_);
	this->impl_->graphics_->SetBlend(this->impl_->alpha_blend_);

	this->impl_->node_system_->Draw();

	float2 size = this->impl_->graphics_->GetTextureSize(this->impl_->test_text_);

	this->impl_->graphics_->SetShader(this->impl_->default_2d_shader_);

	this->impl_->graphics_->SetDiffuse(DirectX::Color(1, 1, 1, .8f));

	this->impl_->graphics_->SetShaderResourceFromTexture(0, { this->impl_->test_text_ });

	this->impl_->graphics_->SetWorld(matrix::CreateScale(size.x, size.y, 1));

	this->impl_->graphics_->UpdatePerMeshCB();
	this->impl_->graphics_->DrawVertices(iGraphics::Topology::TriangleStrip, 4);

	this->impl_->graphics_->Present();

	return ret;
}

const bool Seed::MainWindow::Finalize(void)
{
	return this->impl_->window_->Finalize();
}

void Seed::MainWindow::Destroy(void)
{
	delete this;
}

Seed::MainWindow * Seed::MainWindowDLL::Make(void)
{
	return new Seed::MainWindow;
}
 