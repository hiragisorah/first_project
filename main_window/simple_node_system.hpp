#pragma once

#include <seed/node_system>

namespace Seed
{
	namespace NodeSystem
	{
		class SimpleNodeSystem : public iNodeSystem
		{
		public:
			SimpleNodeSystem(std::unique_ptr<iGraphics> & graphics);

		private:
			void Initialize(void) override;

			void DrawBackground(void) override;
			void DrawNode(Node * const node) override;

		private:
			// texture
			int bg_tex_;

			// shader
			int repeat_shader_;

			// sampler
			int default_sampler_;

		private:
			Math::float2 * bg_scale_;
			Math::float2 * bg_scroll_;
		};
	}
}