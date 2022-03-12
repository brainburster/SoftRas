#pragma once

#include <unordered_map>
#include <memory>
#include <optional>

namespace framework
{
	template<typename T>
	class Resource
	{
	private:
		std::unordered_map<std::wstring, std::shared_ptr<T>> data;
	private:
		Resource() = default;
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

		static Resource& GetInstance()
		{
			static Resource resource;
			return resource;
		}
	public:
		static std::optional<std::shared_ptr<T>> Get(const std::wstring& name)
		{
			auto& data = GetInstance().data;
			if (data.find(name) == data.end())
			{
				return std::nullopt;
			}
			return data.at(name);
		}

		static void Set(const std::wstring& name, std::shared_ptr<T> resource)
		{
			auto& data = GetInstance().data;
			data.insert({ name, resource });
		}
	};
	template<typename T>
	inline void SetResource(const std::wstring& name, std::shared_ptr<T> resource)
	{
		Resource<T>::Set(name, resource);
	}

	template<typename T>
	inline std::optional<std::shared_ptr<T>> GetResource(const std::wstring& name)
	{
		return Resource<T>::Get(name);
	}
}
