#pragma once

#include <vector>

#include <seed/math>

using namespace Seed::Math;

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
			TriangleStrip,
		};

	public:
		iGraphics(void) {};
		iGraphics(iGraphics const&) = delete;
		iGraphics& operator= (iGraphics const&) = delete;

		virtual ~iGraphics(void) {};

	public:
		virtual void Initialize(void * const handle, const float2 & size) = 0;

	public:
		virtual void * const handle(void) const = 0;
		virtual const float2 & size(void) const = 0;

	public:
		virtual void ClearRenderTarget(const std::vector<int> & render_target_id_s) = 0;
		virtual void ClearDepthStencil(const std::vector<int> & depth_stencil_id_s) = 0;
		virtual void SetRenderTargetAndDepthStencil(const std::vector<int> & render_target_id_s, const int & depth_stencil_id) = 0;
		virtual void SetViewPort(const std::vector<int> & view_port_id_s) = 0;
		virtual void SetShaderResourceFromRenderTarget(const int & start_slot, const std::vector<int> & render_target_id_s) = 0;
		virtual void SetShaderResourceFromTexture(const int & start_slot, const std::vector<int> & texture_id_s) = 0;
		virtual void SetShader(const int & shader_id) = 0;
		virtual void SetConstantBuffer(const int & shader_id, void * constant_buffer) = 0;
		virtual void SetWorld(const matrix & world) = 0;
		virtual void SetView(const matrix & view) = 0;
		virtual void SetProjection(const matrix & projection) = 0;
		virtual void SetDiffuse(const color & diffuse) = 0;
		virtual void Present() = 0;

	public:
		virtual const int LoadRenderTargetBackBuffer(const int & width, const int & height) = 0;
		virtual const int LoadRenderTargetR32(const int & width, const int & height) = 0;
		virtual const int LoadRenderTargetRGBA32(const int & width, const int & height) = 0;
		virtual const int LoadRenderTargetRGBA128(const int & width, const int & height) = 0;

		virtual const int LoadDepthsStencil(const int & width, const int & height) = 0;

		virtual const int LoadViewPort(const int & width, const int & height) = 0;

		virtual const int LoadShader(const std::string & file_name) = 0;
		virtual const int LoadTexture(const std::string & file_name) = 0;
		virtual const int LoadMesh(const std::string & file_name) = 0;

		virtual void UnloadRenderTarget(const int & render_target_id) = 0;
		virtual void UnloadDepthsStencil(const int & depth_stencil_id) = 0;
		virtual void UnloadViewPort(const int & view_port_id) = 0;
		virtual void UnloadShader(const int & shader_id) = 0;
		virtual void UnloadTexture(const int & texture_id) = 0;
		virtual void UnloadMesh(const int & mesh_id) = 0;

		virtual const float2 GetTextureSize(const int & texture_id) const = 0;

	public:
		virtual void DrawVertices(const Topology & topology, const int & vertex_cnt) = 0;

	};
}