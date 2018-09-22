#pragma once

#include <memory>
#include <vector>

namespace Seed
{
	template<class _Data, class _Key = unsigned int>
	class ResourcePool
	{
	public:
		ResourcePool(void) {};
		virtual ~ResourcePool(void) {};

	private:
		std::vector<_Key> reuse_;
		std::vector<std::unique_ptr<_Data>> database_;

	public:
		const _Key Load(std::unique_ptr<_Data> & data)
		{
			auto size = this->reuse_.size();
			if (size > 0)
			{
				auto empty_slot = this->reuse_[size - 1];
				this->reuse_.pop_back();
				this->database_[empty_slot] = std::move(data);
				return empty_slot;
			}
			else
			{
				this->database_.emplace_back(std::move(data));
				return static_cast<unsigned int>(this->database_.size() - 1);
			}
		}
		void Unload(const _Key & num)
		{
			this->database_[num].reset();
			this->reuse_.emplace_back(num);
		}
		const std::unique_ptr<_Data> & Get(const _Key & num)
		{
			return this->database_[num];
		}
		void UnloadAll(void)
		{
			this->database_.clear();
		}
	};
}