#include "directx11.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <DirectXTex.h>

#include <unordered_map>

#include "resource_pool.h"

#pragma warning (disable: 4100)

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dxguid.lib")

#if defined(DEBUG) || defined(_DEBUG)
static constexpr DWORD SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
static constexpr DWORD SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

namespace Seed
{
	static constexpr const char * k_texture_dir = "../data/texture/";
	static constexpr const char * k_shader_dir = "../data/shader/";
	static constexpr const char * k_mesh_dir = "../data/mesh/";

	static constexpr const D3D11_PRIMITIVE_TOPOLOGY k_topology_s[] =
	{
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	static constexpr const D3D11_FILTER k_filter_s[] =
	{
		D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
		D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		D3D11_FILTER::D3D11_FILTER_ANISOTROPIC
	};

	static constexpr const D3D11_TEXTURE_ADDRESS_MODE k_adress_mode_s[] =
	{
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER
	};
}

using Device = Microsoft::WRL::ComPtr<ID3D11Device>;
using Context = Microsoft::WRL::ComPtr<ID3D11DeviceContext>;
using SwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain>;

using RenderTargetView = Microsoft::WRL::ComPtr<ID3D11RenderTargetView>;
using ShaderResourceView = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>;
using DepthStencilView = Microsoft::WRL::ComPtr<ID3D11DepthStencilView>;

using VertexShader = Microsoft::WRL::ComPtr<ID3D11VertexShader>;
using GeometryShader = Microsoft::WRL::ComPtr<ID3D11GeometryShader>;
using HullShader = Microsoft::WRL::ComPtr<ID3D11HullShader>;
using DomainShader = Microsoft::WRL::ComPtr<ID3D11DomainShader>;
using PixelShader = Microsoft::WRL::ComPtr<ID3D11PixelShader>;

using ConstantBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;
using IndexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;
using VertexBuffer = Microsoft::WRL::ComPtr<ID3D11Buffer>;
using InputLayout = Microsoft::WRL::ComPtr<ID3D11InputLayout>;

using SamplerState = Microsoft::WRL::ComPtr<ID3D11SamplerState>;

class Seed::Graphics::DirectX11::Impl
{
	friend class DirectX11;

	struct PerCameraCB
	{
		Math::matrix view_;
		Math::matrix projection_;
		__declspec(align(16)) Math::float2 view_port_;
		Math::color color_;
	};

	struct PerMeshCB
	{
		Math::matrix world_;
		Math::color diffuse_;
	};

	struct RenderTarget
	{
		RenderTargetView rtv_;
		ShaderResourceView srv_;
	};

	struct DepthStencil
	{
		DepthStencilView dsv_;
	};

	struct ViewPort
	{
		D3D11_VIEWPORT vp_ = {};
	};

	struct Texture
	{
		ShaderResourceView srv_;

		Math::float2 size_;
	};

	struct Shader
	{
		VertexShader vs_;
		GeometryShader gs_;
		HullShader hs_;
		DomainShader ds_;
		PixelShader ps_;

		ConstantBuffer cb_;
		InputLayout il_;

		unsigned char * variables_;
		unsigned int variables_size_;
		std::unordered_map<std::string, void*> variables_map_;

		~Shader(void) { if(this->variables_) delete[] this->variables_; }
	};

	struct Mesh
	{
		VertexBuffer vertex_buffer_;
		IndexBuffer index_buffer_;

		unsigned int vertex_cnt_ = 0;
		unsigned int index_cnt_ = 0;
	};

	struct Sampler
	{
		SamplerState sampler_state_;
	};

private:
	Context context_;
	Device device_;
	SwapChain swap_chain_;

private:
	HWND handle_;
	Math::float2 size_;

private:
	ResourcePool<RenderTarget> render_target_db_;
	ResourcePool<DepthStencil> depth_stencil_db_;
	ResourcePool<ViewPort> view_port_db_;
	ResourcePool<Shader> shader_db_;
	ResourcePool<Texture> texture_db_;
	ResourcePool<Mesh> mesh_db_;
	ResourcePool<Sampler> sampler_db_;

private:
	PerCameraCB per_camera_;
	PerMeshCB per_mesh_;

private:
	ConstantBuffer per_camera_cb_;
	ConstantBuffer per_mesh_cb_;

public:
	static void CreateConstantBufferFromShader(const Context & device_context, std::unique_ptr<Shader> & shader, ID3DBlob * blob);
	static void CreateInputLayoutFromShader(const Context & device_context, std::unique_ptr<Shader> & shader, ID3DBlob * blob);
};

Seed::Graphics::DirectX11::DirectX11(void)
	: impl_(std::make_unique<Impl>())
{

}

Seed::Graphics::DirectX11::~DirectX11(void)
{

}

void Seed::Graphics::DirectX11::Initialize(void * const handle, const Math::float2 & size)
{
	this->impl_->handle_ = static_cast<HWND>(handle);
	this->impl_->size_ = size;

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	auto & self = this->impl_;

	// �f�o�C�X�ƃX���b�v�`�F�[���̍쐬 
	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = static_cast<unsigned int>(self->size_.x);
	sd.BufferDesc.Height = static_cast<unsigned int>(self->size_.y);
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = static_cast<HWND>(self->handle_);
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	D3D_FEATURE_LEVEL feature_levels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL * feature_level = nullptr;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		0, &feature_levels, 1, D3D11_SDK_VERSION, &sd, self->swap_chain_.GetAddressOf(), device.GetAddressOf(),
		feature_level, &self->context_);

	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = sizeof(Impl::PerCameraCB);
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		bd.Usage = D3D11_USAGE_DEFAULT;

		if (FAILED(device->CreateBuffer(&bd, nullptr, self->per_camera_cb_.GetAddressOf())))
		{
		}
	}

	{
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = sizeof(Impl::PerMeshCB);
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		bd.Usage = D3D11_USAGE_DEFAULT;

		if (FAILED(device->CreateBuffer(&bd, nullptr, self->per_mesh_cb_.GetAddressOf())))
		{
		}
	}

	self->context_->UpdateSubresource(self->per_camera_cb_.Get(), 0, nullptr, &self->per_camera_, 0, 0);
	self->context_->VSSetConstantBuffers(1, 1, self->per_camera_cb_.GetAddressOf());
	self->context_->GSSetConstantBuffers(1, 1, self->per_camera_cb_.GetAddressOf());
	self->context_->HSSetConstantBuffers(1, 1, self->per_camera_cb_.GetAddressOf());
	self->context_->DSSetConstantBuffers(1, 1, self->per_camera_cb_.GetAddressOf());
	self->context_->PSSetConstantBuffers(1, 1, self->per_camera_cb_.GetAddressOf());

	self->context_->UpdateSubresource(self->per_mesh_cb_.Get(), 0, nullptr, &self->per_mesh_, 0, 0);
	self->context_->VSSetConstantBuffers(2, 1, self->per_mesh_cb_.GetAddressOf());
	self->context_->GSSetConstantBuffers(2, 1, self->per_mesh_cb_.GetAddressOf());
	self->context_->HSSetConstantBuffers(2, 1, self->per_mesh_cb_.GetAddressOf());
	self->context_->DSSetConstantBuffers(2, 1, self->per_mesh_cb_.GetAddressOf());
	self->context_->PSSetConstantBuffers(2, 1, self->per_mesh_cb_.GetAddressOf());

	//Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

	//D3D11_BLEND_DESC blend_desc = {};

	//blend_desc.AlphaToCoverageEnable = false;
	//blend_desc.IndependentBlendEnable = false;

	//blend_desc.RenderTarget[0].BlendEnable = true;
	//blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());

	//self->context_->OMSetBlendState(blend_state.Get(), nullptr, 0xffffffff);

	//{
	//	D3D11_RASTERIZER_DESC desc = {};
	//	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rs;

	//	desc.CullMode = D3D11_CULL_BACK;
	//	desc.FillMode = D3D11_FILL_WIREFRAME;
	//	desc.DepthClipEnable = true;
	//	desc.MultisampleEnable = true;
	//	desc.FillMode = D3D11_FILL_SOLID;

	//	device->CreateRasterizerState(&desc, rs.GetAddressOf());
	//	this->impl_->context_->RSSetState(rs.Get());
	//}

	//{
	//	D3D11_SAMPLER_DESC desc = {};
	//	Microsoft::WRL::ComPtr<ID3D11SamplerState> ss;

	//	desc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	//	desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.MipLODBias = 0;
	//	desc.MaxAnisotropy = 1;
	//	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	//	desc.BorderColor[0] = 0;
	//	desc.BorderColor[1] = 0;
	//	desc.BorderColor[2] = 0;
	//	desc.BorderColor[3] = 0;
	//	desc.MinLOD = 0;
	//	desc.MaxLOD = D3D11_FLOAT32_MAX;
	//	device->CreateSamplerState(&desc, ss.GetAddressOf());
	//	this->impl_->context_->PSSetSamplers(0, 1, ss.GetAddressOf());
	//}
}

void * const Seed::Graphics::DirectX11::handle(void) const
{
	return this->impl_->handle_;
}

const Seed::Math::float2 & Seed::Graphics::DirectX11::size(void) const
{
	return this->impl_->size_;
}

void Seed::Graphics::DirectX11::ClearRenderTarget(const std::vector<int>& render_target_id_s, const Math::color & color)
{
	for (auto & id : render_target_id_s)
		this->impl_->context_->ClearRenderTargetView(this->impl_->render_target_db_.Get(id)->rtv_.Get(), color);
}

void Seed::Graphics::DirectX11::ClearDepthStencil(const std::vector<int>& depth_stencil_id_s)
{
	for (auto & id : depth_stencil_id_s)
		this->impl_->context_->ClearDepthStencilView(this->impl_->depth_stencil_db_.Get(id)->dsv_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Seed::Graphics::DirectX11::SetRenderTargetAndDepthStencil(const std::vector<int>& render_target_id_s, const int & depth_stencil_id)
{
	auto & self = this->impl_;

	std::vector<ID3D11RenderTargetView*> rtvs_;

	for (auto & rt : render_target_id_s)
		rtvs_.emplace_back(self->render_target_db_.Get(rt)->rtv_.Get());

	ID3D11DepthStencilView * dsv = nullptr;

	if (depth_stencil_id != -1)
		dsv = self->depth_stencil_db_.Get(depth_stencil_id)->dsv_.Get();

	self->context_->OMSetRenderTargets(static_cast<unsigned int>(rtvs_.size()), rtvs_.data(), dsv);
}

void Seed::Graphics::DirectX11::SetViewPort(const std::vector<int>& view_port_id_s)
{
	auto & self = this->impl_;

	std::vector<D3D11_VIEWPORT> view_ports;

	for (auto & vp : view_port_id_s)
		view_ports.emplace_back(self->view_port_db_.Get(vp)->vp_);

	this->impl_->per_camera_.view_port_ = Math::float2(static_cast<float>(view_ports[0].Width), static_cast<float>(view_ports[0].Height));

	self->context_->RSSetViewports(static_cast<unsigned int>(view_ports.size()), view_ports.data());
}

void Seed::Graphics::DirectX11::SetShaderResourceFromRenderTarget(const int & start_slot, const std::vector<int>& render_target_id_s)
{
	auto & self = this->impl_;

	std::vector<ID3D11ShaderResourceView*> srvs_;

	for (auto & rt : render_target_id_s)
		srvs_.emplace_back(self->render_target_db_.Get(rt)->srv_.Get());

	self->context_->PSSetShaderResources(start_slot, static_cast<int>(render_target_id_s.size()), srvs_.data());
}

void Seed::Graphics::DirectX11::SetShaderResourceFromTexture(const int & start_slot, const std::vector<int>& texture_id_s)
{
	auto & self = this->impl_;

	std::vector<ID3D11ShaderResourceView*> srvs_;

	for (auto & tex : texture_id_s)
		srvs_.emplace_back(self->texture_db_.Get(tex)->srv_.Get());

	self->context_->PSSetShaderResources(start_slot, static_cast<int>(texture_id_s.size()), srvs_.data());
}

void Seed::Graphics::DirectX11::SetShader(const int & shader_id)
{
	auto & self = this->impl_;

	auto & shader = self->shader_db_.Get(shader_id);

	self->context_->IASetInputLayout(shader->il_.Get());

	self->context_->VSSetShader(shader->vs_.Get(), nullptr, 0);
	self->context_->GSSetShader(shader->gs_.Get(), nullptr, 0);
	self->context_->HSSetShader(shader->hs_.Get(), nullptr, 0);
	self->context_->DSSetShader(shader->ds_.Get(), nullptr, 0);
	self->context_->PSSetShader(shader->ps_.Get(), nullptr, 0);

	self->context_->VSSetConstantBuffers(0, 1, shader->cb_.GetAddressOf());
	self->context_->GSSetConstantBuffers(0, 1, shader->cb_.GetAddressOf());
	self->context_->HSSetConstantBuffers(0, 1, shader->cb_.GetAddressOf());
	self->context_->DSSetConstantBuffers(0, 1, shader->cb_.GetAddressOf());
	self->context_->PSSetConstantBuffers(0, 1, shader->cb_.GetAddressOf());
}

void Seed::Graphics::DirectX11::SetSampler(const int & sampler_id)
{
	auto & sampler = this->impl_->sampler_db_.Get(sampler_id);
	this->impl_->context_->PSSetSamplers(0, 1, sampler->sampler_state_.GetAddressOf());
}

void Seed::Graphics::DirectX11::SetWorld(const Math::matrix & world)
{
	this->impl_->per_mesh_.world_ = world;
}

void Seed::Graphics::DirectX11::SetView(const Math::matrix & view)
{
	this->impl_->per_camera_.view_ = view;
}

void Seed::Graphics::DirectX11::SetProjection(const Math::matrix & projection)
{
	this->impl_->per_camera_.projection_ = projection;
}

void Seed::Graphics::DirectX11::SetDiffuse(const Math::color & diffuse)
{
	this->impl_->per_mesh_.diffuse_ = diffuse;
}

void Seed::Graphics::DirectX11::UpdatePerCameraCB(void)
{
	this->impl_->context_->UpdateSubresource(this->impl_->per_camera_cb_.Get(), 0, nullptr, &this->impl_->per_camera_, 0, 0);
}

void Seed::Graphics::DirectX11::UpdatePerMeshCB(void)
{
	this->impl_->context_->UpdateSubresource(this->impl_->per_mesh_cb_.Get(), 0, nullptr, &this->impl_->per_mesh_, 0, 0);
}

void Seed::Graphics::DirectX11::UpdateUniqueCB(const int & shader_id)
{
	auto & shader = this->impl_->shader_db_.Get(shader_id);
	this->impl_->context_->UpdateSubresource(shader->cb_.Get(), 0, nullptr, &shader->variables_[0], shader->variables_size_, 0);
}

void Seed::Graphics::DirectX11::Present()
{
	this->impl_->swap_chain_->Present(0, 0);
}

void * Seed::Graphics::DirectX11::GetShaderVariable(const int & shader_id, const std::string & variable_name)
{
	auto & shader = this->impl_->shader_db_.Get(shader_id);
	return shader->variables_map_[variable_name];
}

const int Seed::Graphics::DirectX11::LoadRenderTargetBackBuffer(const int & width, const int & height)
{
	auto & self = this->impl_;

	auto render_target = std::make_unique<Impl::RenderTarget>();

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	self->context_->GetDevice(device.GetAddressOf());

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

		self->swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&tex_2d);

		device->CreateRenderTargetView(tex_2d.Get(), nullptr, render_target->rtv_.GetAddressOf());
	}

	return self->render_target_db_.Load(render_target);
}

const int Seed::Graphics::DirectX11::LoadRenderTargetR32(const int & width, const int & height)
{
	auto & self = this->impl_;

	auto render_target = std::make_unique<Impl::RenderTarget>();

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	self->context_->GetDevice(device.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, tex_2d.GetAddressOf());

	RTVDesc.Format = desc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(tex_2d.Get(), &RTVDesc, render_target->rtv_.GetAddressOf());

	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(tex_2d.Get(), &SRVDesc, render_target->srv_.GetAddressOf());

	return self->render_target_db_.Load(render_target);
}

const int Seed::Graphics::DirectX11::LoadRenderTargetRGBA32(const int & width, const int & height)
{
	auto & self = this->impl_;

	auto render_target = std::make_unique<Impl::RenderTarget>();

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	self->context_->GetDevice(device.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, tex_2d.GetAddressOf());

	RTVDesc.Format = desc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(tex_2d.Get(), &RTVDesc, render_target->rtv_.GetAddressOf());

	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(tex_2d.Get(), &SRVDesc, render_target->srv_.GetAddressOf());

	return self->render_target_db_.Load(render_target);
}

const int Seed::Graphics::DirectX11::LoadRenderTargetRGBA128(const int & width, const int & height)
{
	auto & self = this->impl_;

	auto render_target = std::make_unique<Impl::RenderTarget>();

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	self->context_->GetDevice(device.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
	D3D11_TEXTURE2D_DESC desc = {};

	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	device->CreateTexture2D(&desc, nullptr, tex_2d.GetAddressOf());

	RTVDesc.Format = desc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(tex_2d.Get(), &RTVDesc, render_target->rtv_.GetAddressOf());

	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(tex_2d.Get(), &SRVDesc, render_target->srv_.GetAddressOf());

	return self->render_target_db_.Load(render_target);
}

const int Seed::Graphics::DirectX11::LoadDepthsStencil(const int & width, const int & height)
{
	auto & self = this->impl_;

	auto depth_stencil = std::make_unique<Impl::DepthStencil>();

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	self->context_->GetDevice(device.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex_2d;

	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_D32_FLOAT;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;

	device->CreateTexture2D(&tex_desc, nullptr, tex_2d.GetAddressOf());
	device->CreateDepthStencilView(tex_2d.Get(), nullptr, depth_stencil->dsv_.GetAddressOf());

	return self->depth_stencil_db_.Load(depth_stencil);
}

const int Seed::Graphics::DirectX11::LoadViewPort(const int & width, const int & height)
{
	auto & self = this->impl_;

	auto view_port = std::make_unique<Impl::ViewPort>();

	view_port->vp_.Width = static_cast<float>(width);
	view_port->vp_.Height = static_cast<float>(height);
	view_port->vp_.MaxDepth = 1.f;
	view_port->vp_.MinDepth = 0.f;
	view_port->vp_.TopLeftX = 0;
	view_port->vp_.TopLeftY = 0;
	
	return self->view_port_db_.Load(view_port);
}

static DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask)
{
	if (mask == 0x0F)
	{
		// xyzw
		switch (type)
		{
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		}
	}

	if (mask == 0x07)
	{
		// xyz
		switch (type)
		{
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32B32_UINT;
		}
	}

	if (mask == 0x3)
	{
		// xy
		switch (type)
		{
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32G32_FLOAT;
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32G32_UINT;
		}
	}

	if (mask == 0x1)
	{
		// x
		switch (type)
		{
		case D3D_REGISTER_COMPONENT_FLOAT32:
			return DXGI_FORMAT_R32_FLOAT;
		case D3D_REGISTER_COMPONENT_UINT32:
			return DXGI_FORMAT_R32_UINT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}

const int Seed::Graphics::DirectX11::LoadShader(const std::string & file_name)
{
	auto file_path = k_shader_dir + file_name;

	auto & self = this->impl_;

	auto shader = std::make_unique<Impl::Shader>();

	Microsoft::WRL::ComPtr<ID3D11Device> device;

	self->context_->GetDevice(device.GetAddressOf());

	ID3DBlob * blob = nullptr;
	ID3DBlob * error = nullptr;

	if (FAILED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", SHADER_FLAGS, 0, &blob, &error)))
	{
		if (error)
		{
			//char * err = (char*)error->GetBufferPointer();
			//if (error != nullptr)
			//	std::cout << __FUNCTION__ << "::" << err << std::endl;
			//else
			//	std::cout << __FUNCTION__ << "::�V�F�[�_�[�̓ǂݍ��݂Ɏ��s���܂����B" << std::endl;
		}
	}
	else
	{
		device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->vs_.GetAddressOf());
		this->impl_->CreateInputLayoutFromShader(self->context_, shader, blob);
		this->impl_->CreateConstantBufferFromShader(self->context_, shader, blob);
	}

	if (SUCCEEDED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "GS", "gs_5_0", SHADER_FLAGS, 0, &blob, &error)))
	{
		device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->gs_.GetAddressOf());
		if (shader->cb_ == nullptr)
			this->impl_->CreateConstantBufferFromShader(self->context_, shader, blob);
	}

	if (SUCCEEDED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "HS", "hs_5_0", SHADER_FLAGS, 0, &blob, &error)))
	{
		device->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->hs_.GetAddressOf());
		if (shader->cb_ == nullptr)
			this->impl_->CreateConstantBufferFromShader(self->context_, shader, blob);
	}

	if (SUCCEEDED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "DS", "ds_5_0", SHADER_FLAGS, 0, &blob, &error)))
	{
		device->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->ds_.GetAddressOf());
		if (shader->cb_ == nullptr)
			this->impl_->CreateConstantBufferFromShader(self->context_, shader, blob);
	}

	if (SUCCEEDED(D3DCompileFromFile(std::wstring(file_path.begin(), file_path.end()).c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", SHADER_FLAGS, 0, &blob, &error)))
	{
		device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, shader->ps_.GetAddressOf());
		if (shader->cb_ == nullptr)
			this->impl_->CreateConstantBufferFromShader(self->context_, shader, blob);
	}

	return this->impl_->shader_db_.Load(shader);
}

const int Seed::Graphics::DirectX11::LoadTexture(const std::string & file_name)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	this->impl_->context_->GetDevice(device.GetAddressOf());

	auto file_path = k_texture_dir + file_name;

	auto texture = std::make_unique<Impl::Texture>();

	auto image = std::make_unique<DirectX::ScratchImage>();

	DirectX::LoadFromWICFile(std::wstring(file_path.begin(), file_path.end()).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, *image);

	texture->size_.x = static_cast<float>(image->GetMetadata().width);
	texture->size_.y = static_cast<float>(image->GetMetadata().height);

	DirectX::CreateShaderResourceView(device.Get(), image->GetImages(), image->GetImageCount(), image->GetMetadata(), texture->srv_.GetAddressOf());

	return this->impl_->texture_db_.Load(texture);
}

const int Seed::Graphics::DirectX11::LoadMesh(const std::string & file_name)
{
	auto file_path = k_mesh_dir + file_name;

	auto mesh = std::make_unique<Impl::Mesh>();

	return this->impl_->mesh_db_.Load(mesh);
}

const int Seed::Graphics::DirectX11::LoadSampler(const Filter & filter, const AddressMode & address_mode)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	this->impl_->context_->GetDevice(device.GetAddressOf());

	auto sampler = std::make_unique<Impl::Sampler>();

	D3D11_SAMPLER_DESC desc = {};

	desc.Filter = k_filter_s[static_cast<unsigned int>(filter)];

	desc.AddressU = k_adress_mode_s[static_cast<unsigned int>(address_mode)];
	desc.AddressV = k_adress_mode_s[static_cast<unsigned int>(address_mode)];
	desc.AddressW = k_adress_mode_s[static_cast<unsigned int>(address_mode)];

	desc.MaxAnisotropy = (device->GetFeatureLevel() > D3D_FEATURE_LEVEL_9_1) ? D3D11_MAX_MAXANISOTROPY : 2;

	desc.MaxLOD = FLT_MAX;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	device->CreateSamplerState(&desc, sampler->sampler_state_.GetAddressOf());

	return this->impl_->sampler_db_.Load(sampler);
}

void Seed::Graphics::DirectX11::UnloadRenderTarget(const int & render_target_id)
{
	this->impl_->render_target_db_.Unload(render_target_id);
}

void Seed::Graphics::DirectX11::UnloadDepthsStencil(const int & depth_stencil_id)
{
	this->impl_->depth_stencil_db_.Unload(depth_stencil_id);
}

void Seed::Graphics::DirectX11::UnloadViewPort(const int & view_port_id)
{
	this->impl_->view_port_db_.Unload(view_port_id);
}

void Seed::Graphics::DirectX11::UnloadShader(const int & shader_id)
{
	this->impl_->shader_db_.Unload(shader_id);
}

void Seed::Graphics::DirectX11::UnloadTexture(const int & texture_id)
{
	this->impl_->texture_db_.Unload(texture_id);
}

void Seed::Graphics::DirectX11::UnloadMesh(const int & mesh_id)
{
	this->impl_->mesh_db_.Unload(mesh_id);
}

void Seed::Graphics::DirectX11::UnloadSampler(const int & sampler_id)
{
	this->impl_->sampler_db_.Unload(sampler_id);
}

const Seed::Math::float2 Seed::Graphics::DirectX11::GetTextureSize(const int & texture_id) const
{
	return this->impl_->texture_db_.Get(texture_id)->size_;
}

void Seed::Graphics::DirectX11::DrawVertices(const Topology & topology, const int & vertex_cnt)
{
	this->impl_->context_->IASetPrimitiveTopology(k_topology_s[static_cast<unsigned int>(topology)]);
	this->impl_->context_->Draw(vertex_cnt, 0U);
}

void Seed::Graphics::DirectX11::Impl::CreateConstantBufferFromShader(const Context & device_context, std::unique_ptr<Shader>& shader, ID3DBlob * blob)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	device_context->GetDevice(device.GetAddressOf());

	ID3D11ShaderReflection * reflector = nullptr;
	D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);

	D3D11_SHADER_DESC shader_desc;
	reflector->GetDesc(&shader_desc);

	auto cb = reflector->GetConstantBufferByName("unique");

	unsigned int size = 0;
	D3D11_SHADER_BUFFER_DESC desc = {};
	cb->GetDesc(&desc);

	for (unsigned int i = 0; desc.Name != nullptr && i < desc.Variables; ++i)
	{
		auto v = cb->GetVariableByIndex(i);
		D3D11_SHADER_VARIABLE_DESC vdesc;
		v->GetDesc(&vdesc);

		if (vdesc.Size % 16)
			size += vdesc.Size + 16 - (vdesc.Size % 16);
		else
			size += vdesc.Size;
	}

	shader->variables_size_ = size;

	if (size)
		shader->variables_ = new unsigned char[size];

	size = 0;

	for (unsigned int i = 0; desc.Name != nullptr && i < desc.Variables; ++i)
	{
		auto v = cb->GetVariableByIndex(i);
		D3D11_SHADER_VARIABLE_DESC vdesc;
		v->GetDesc(&vdesc);

		shader->variables_map_[vdesc.Name] = shader->variables_ + size;

		if (vdesc.Size % 16)
			size += vdesc.Size + 16 - (vdesc.Size % 16);
		else
			size += vdesc.Size;
	}

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = size;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	device->CreateBuffer(&bd, nullptr, shader->cb_.GetAddressOf());
}

void Seed::Graphics::DirectX11::Impl::CreateInputLayoutFromShader(const Context & device_context, std::unique_ptr<Shader>& shader, ID3DBlob * blob)
{
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	device_context->GetDevice(device.GetAddressOf());

	ID3D11ShaderReflection * reflector = nullptr;
	D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);

	D3D11_SHADER_DESC shader_desc;
	reflector->GetDesc(&shader_desc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> element;
	for (unsigned int i = 0; i < shader_desc.InputParameters; ++i) {
		D3D11_SIGNATURE_PARAMETER_DESC sigdesc;
		reflector->GetInputParameterDesc(i, &sigdesc);

		auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);

		D3D11_INPUT_ELEMENT_DESC eledesc =
		{
			sigdesc.SemanticName
			, sigdesc.SemanticIndex
			, format
			, 0
			, D3D11_APPEND_ALIGNED_ELEMENT
			, D3D11_INPUT_PER_VERTEX_DATA
			, 0
		};

		element.emplace_back(eledesc);
	}

	if (!element.empty())
		device->CreateInputLayout(&element[0], static_cast<unsigned int>(element.size()),
			blob->GetBufferPointer(), blob->GetBufferSize(), shader->il_.GetAddressOf());
}
