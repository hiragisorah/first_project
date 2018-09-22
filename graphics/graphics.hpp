#pragma once

#include <vector>

#include <seed/math>

namespace Seed
{
	class iGraphics
	{
	public:
		enum class Topology
		{
			PointList,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip
		};

		enum class Filter
		{
			Point,
			Linear,
			Anisotropic
		};

		enum class AddressMode
		{
			Wrap,
			Clamp,
			Mirror,
			MirrorOnce,
			Border
		};

		enum class BlendType
		{
			Zero,
			One,
			SrcColor,
			InvSrcColor,
			SrcAlpha,
			InvSrcAlpha,
			DestAlpha,
			InvDestAlpha,
			DestColor,
			InvDestColor,
			SrcAlphaSat,
			BlendFactor,
			InvBlendFactor,
			Src1Color,
			InvSrc1Color,
			Src1Alpha,
			InvSrc1Alpha
		};

		enum class BlendOption
		{
			Add,
			Subtract,
			RevSubtract,
			Min,
			Max
		};

	public:
		iGraphics(void) {};
		iGraphics(iGraphics const&) = delete;
		iGraphics& operator= (iGraphics const&) = delete;

		virtual ~iGraphics(void) {};

	public:
		virtual void Initialize(void * const handle, const Math::float2 & size) = 0;

	public:
		virtual void * const handle(void) const = 0;
		virtual const Math::float2 & size(void) const = 0;

	public:
		virtual void ClearRenderTarget(const std::vector<int> & render_target_id_s, const Math::color & color) = 0;
		virtual void ClearDepthStencil(const std::vector<int> & depth_stencil_id_s) = 0;
		virtual void SetRenderTargetAndDepthStencil(const std::vector<int> & render_target_id_s, const int & depth_stencil_id) = 0;
		virtual void SetViewPort(const std::vector<int> & view_port_id_s) = 0;
		virtual void SetShaderResourceFromRenderTarget(const int & start_slot, const std::vector<int> & render_target_id_s) = 0;
		virtual void SetShaderResourceFromTexture(const int & start_slot, const std::vector<int> & texture_id_s) = 0;
		virtual void SetShader(const int & shader_id) = 0;
		virtual void SetSampler(const int & sampler_id) = 0;
		virtual void SetBlend(const int & blend_id) = 0;
		virtual void SetWorld(const Math::matrix & world) = 0;
		virtual void SetView(const Math::matrix & view) = 0;
		virtual void SetProjection(const Math::matrix & projection) = 0;
		virtual void SetDiffuse(const Math::color & diffuse) = 0;
		virtual void UpdatePerCameraCB(void) = 0;
		virtual void UpdatePerMeshCB(void) = 0;
		virtual void UpdateUniqueCB(const int & shader_id) = 0;
		virtual void Present(void) = 0;

	public:
		template<class _Type> _Type * GetShaderVariable(const int & shader_id, const std::string & variable_name)
		{
			return static_cast<_Type*>(this->GetShaderVariable(shader_id, variable_name));
		}
		
	private:
		virtual void * GetShaderVariable(const int & shader_id, const std::string & variable_name) = 0;

	public:
		virtual const int LoadRenderTargetBackBuffer(const int & width, const int & height) = 0;
		virtual const int LoadRenderTargetR32(const int & width, const int & height) = 0;
		virtual const int LoadRenderTargetRGBA32(const int & width, const int & height) = 0;
		virtual const int LoadRenderTargetRGBA128(const int & width, const int & height) = 0;
		virtual const int LoadDepthsStencil(const int & width, const int & height) = 0;
		virtual const int LoadViewPort(const int & width, const int & height) = 0;
		virtual const int LoadShader(const std::string & file_name) = 0;
		virtual const int LoadTexture(const std::string & file_name) = 0;
		virtual const int LoadTextureFromFont(const int & font_id, const std::string & text, const int & size) = 0;
		virtual const int LoadMesh(const std::string & file_name) = 0;
		virtual const int LoadFont(const std::string & file_name) = 0;
		virtual const int LoadSampler(const Filter & filter, const AddressMode & address_mode) = 0;
		virtual const int LoadBlend(const BlendOption & blend_option, const BlendType & src, const BlendType & dest) = 0;
		virtual void UnloadRenderTarget(const int & render_target_id) = 0;
		virtual void UnloadDepthsStencil(const int & depth_stencil_id) = 0;
		virtual void UnloadViewPort(const int & view_port_id) = 0;
		virtual void UnloadShader(const int & shader_id) = 0;
		virtual void UnloadTexture(const int & texture_id) = 0;
		virtual void UnloadMesh(const int & mesh_id) = 0;
		virtual void UnloadFont(const int & font_id) = 0;
		virtual void UnloadSampler(const int & sampler_id) = 0;
		virtual void UnloadBlend(const int & blend_id) = 0;

		virtual const Math::float2 GetTextureSize(const int & texture_id) const = 0;

	public:
		virtual void DrawVertices(const Topology & topology, const int & vertex_cnt) = 0;

	};
}