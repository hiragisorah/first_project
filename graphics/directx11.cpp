#include "directx11.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma warning (disable: 4100)

class Seed::Graphics::DirectX11::Impl
{
	friend class DirectX11;

	struct PerFrameCB
	{
		matrix view_;
		matrix projection_;
		color screen_color_;
	};

	struct PerItemCB
	{
		matrix world_;
		color diffuse_;
	};

private:
	

private:
	HWND hwnd_;
	float2 size_;
};

Seed::Graphics::DirectX11::DirectX11(void)
	: impl_(std::make_unique<Impl>())
{

}

Seed::Graphics::DirectX11::~DirectX11(void)
{

}

void Seed::Graphics::DirectX11::Initialize(void * const handle, const float2 & size)
{
	this->impl_->hwnd_ = static_cast<HWND>(handle);
	this->impl_->size_ = size;
}

void * const Seed::Graphics::DirectX11::handle(void) const
{
	return nullptr;
}

const float2 & Seed::Graphics::DirectX11::size(void) const
{
	// TODO: return ステートメントをここに挿入します
	return this->impl_->size_;
}

void Seed::Graphics::DirectX11::ClearRenderTarget(const std::vector<int>& render_target_id_s)
{
}

void Seed::Graphics::DirectX11::ClearDepthStencil(const std::vector<int>& depth_stencil_id_s)
{
}

void Seed::Graphics::DirectX11::SetRenderTargetAndDepthStencil(const std::vector<int>& render_target_id_s, const int & depth_stencil_id)
{
}

void Seed::Graphics::DirectX11::SetViewPort(const std::vector<int>& view_port_id_s)
{
}

void Seed::Graphics::DirectX11::SetShaderResourceFromRenderTarget(const int & start_slot, const std::vector<int>& render_target_id_s)
{
}

void Seed::Graphics::DirectX11::SetShaderResourceFromTexture(const int & start_slot, const std::vector<int>& texture_id_s)
{
}

void Seed::Graphics::DirectX11::SetShader(const int & shader_id)
{
}

void Seed::Graphics::DirectX11::SetConstantBuffer(const int & shader_id, void * constant_buffer)
{
}

void Seed::Graphics::DirectX11::SetWorld(const matrix & world)
{
}

void Seed::Graphics::DirectX11::SetView(const matrix & view)
{
}

void Seed::Graphics::DirectX11::SetProjection(const matrix & projection)
{
}

void Seed::Graphics::DirectX11::SetDiffuse(const color & diffuse)
{
}

void Seed::Graphics::DirectX11::Present()
{
}

const int Seed::Graphics::DirectX11::LoadRenderTargetBackBuffer(const int & width, const int & height)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadRenderTargetR32(const int & width, const int & height)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadRenderTargetRGBA32(const int & width, const int & height)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadRenderTargetRGBA128(const int & width, const int & height)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadDepthsStencil(const int & width, const int & height)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadViewPort(const int & width, const int & height)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadShader(const std::string & file_name)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadTexture(const std::string & file_name)
{
	return 0;
}

const int Seed::Graphics::DirectX11::LoadMesh(const std::string & file_name)
{
	return 0;
}

void Seed::Graphics::DirectX11::UnloadRenderTarget(const int & render_target_id)
{
}

void Seed::Graphics::DirectX11::UnloadDepthsStencil(const int & depth_stencil_id)
{
}

void Seed::Graphics::DirectX11::UnloadViewPort(const int & view_port_id)
{
}

void Seed::Graphics::DirectX11::UnloadShader(const int & shader_id)
{
}

void Seed::Graphics::DirectX11::UnloadTexture(const int & texture_id)
{
}

void Seed::Graphics::DirectX11::UnloadMesh(const int & mesh_id)
{
}

const float2 Seed::Graphics::DirectX11::GetTextureSize(const int & texture_id) const
{
	return float2();
}

void Seed::Graphics::DirectX11::DrawVertices(const Topology & topology, const int & vertex_cnt)
{
}
