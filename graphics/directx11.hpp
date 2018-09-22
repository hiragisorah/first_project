#pragma once

#include <seed/graphics>
#include <seed/math>

#include <memory>

namespace Seed
{
	namespace Graphics
	{
		class DirectX11 : public iGraphics
		{
		public:
			DirectX11(void);
			DirectX11(DirectX11 const&) = delete;
			DirectX11& operator= (DirectX11 const&) = delete;

			~DirectX11(void);

		public:
			void Initialize(void * const handle, const Math::float2 & size) override;
			void * const handle(void) const override;
			const Math::float2 & size(void) const override;
			void ClearRenderTarget(const std::vector<int>& render_target_id_s, const Math::color & color) override;
			void ClearDepthStencil(const std::vector<int>& depth_stencil_id_s) override;
			void SetRenderTargetAndDepthStencil(const std::vector<int>& render_target_id_s, const int & depth_stencil_id) override;
			void SetViewPort(const std::vector<int>& view_port_id_s) override;
			void SetShaderResourceFromRenderTarget(const int & start_slot, const std::vector<int>& render_target_id_s) override;
			void SetShaderResourceFromTexture(const int & start_slot, const std::vector<int>& texture_id_s) override;
			void SetShader(const int & shader_id) override;
			void SetSampler(const int & sampler_id) override;
			void SetBlend(const int & blend_id) override;
			void SetWorld(const Math::matrix & world) override;
			void SetView(const Math::matrix & view) override;
			void SetProjection(const Math::matrix & projection) override;
			void SetDiffuse(const Math::color & diffuse) override;
			void SetColor(const Math::color & color) override;
			void UpdatePerCameraCB(void) override;
			void UpdatePerMeshCB(void) override;
			void UpdateUniqueCB(const int & shader_id) override;
			void Present(void) override;
			void * GetShaderVariable(const int & shader_id, const std::string & variable_name) override;
			const int LoadRenderTargetBackBuffer(const int & width, const int & height) override;
			const int LoadRenderTargetR32(const int & width, const int & height) override;
			const int LoadRenderTargetRGBA32(const int & width, const int & height) override;
			const int LoadRenderTargetRGBA128(const int & width, const int & height) override;
			const int LoadDepthsStencil(const int & width, const int & height) override;
			const int LoadViewPort(const int & width, const int & height) override;
			const int LoadShader(const std::string & file_name) override;
			const int LoadTexture(const std::string & file_name) override;
			const int LoadTextureFromFont(const int & font_id, const std::string & text, const int & size) override;
			const int LoadFont(const std::string & file_name) override;
			const int LoadMesh(const std::string & file_name) override;
			const int LoadSampler(const Filter & filter, const AddressMode & address_mode) override;
			const int LoadBlend(const BlendOption & blend_option, const BlendType & src, const BlendType & dest, const BlendOption & blend_option_a, const BlendType & src_a, const BlendType & dest_a) override;
			void UnloadRenderTarget(const int & render_target_id) override;
			void UnloadDepthsStencil(const int & depth_stencil_id) override;
			void UnloadViewPort(const int & view_port_id) override;
			void UnloadShader(const int & shader_id) override;
			void UnloadTexture(const int & texture_id) override;
			void UnloadFont(const int & font_id) override;
			void UnloadMesh(const int & mesh_id) override;
			void UnloadSampler(const int & sampler_id) override;
			void UnloadBlend(const int & blend_id) override;
			const Math::float2 GetTextureSize(const int & texture_id) const override;
			void DrawVertices(const Topology & topology, const int & vertex_cnt) override;

		private:
			class Impl;

			std::unique_ptr<Impl> impl_;
		};
	}
}