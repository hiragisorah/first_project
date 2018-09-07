#pragma once

namespace Seed
{
	class Module
	{
	public:
		virtual const bool Initialize(void) = 0;
		virtual const bool Run(void) = 0;
		virtual const bool Finalize(void) = 0;
		virtual void Destroy(void) = 0;
	};
}