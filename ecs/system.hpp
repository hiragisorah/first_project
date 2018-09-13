#pragma once

namespace Seed
{
	class System
	{
	public:
		virtual bool Initialize(void) = 0;
		virtual bool Run(void) = 0;
		virtual bool Finalize(void) = 0;
		virtual void Destroy(void) = 0;
	};
}