#pragma once

namespace Seed
{
	class Component
	{
	public:
		virtual bool Initialize(void) = 0;
		virtual void Run(void) = 0;
		virtual void Pause(void) = 0;
		virtual void Pause(void) = 0;
		virtual bool Finalize(void) = 0;

	private:

	};
}