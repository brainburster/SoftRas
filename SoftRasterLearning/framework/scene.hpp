#pragma once

#include "object.hpp"
#include "render_engine.hpp"
#include <memory.h>

namespace framework
{
	class IScene
	{
	public:
		virtual void Init(IRenderEngine& engine) = 0;
		virtual void Update(const IRenderEngine&) = 0;
		virtual void HandleInput(const IRenderEngine&) = 0;
		virtual void OnMouseMove(const IRenderEngine&) = 0;
		virtual void OnMouseWheel(const IRenderEngine&) = 0;
		virtual void RenderFrame(IRenderEngine&) = 0;
		virtual const ICamera* GetMainCamera() const = 0;
	};

	class Scene : public IScene
	{
	public:
		template<typename T>
		std::shared_ptr<T> Spawn(core::Vec3 p = core::Vec3{ 0,0,0 }, core::Vec3 r = core::Vec3{ 0,0,0 }, core::Vec3 s = core::Vec3{ 1, 1 ,1 })
		{
			std::shared_ptr<T> o = std::make_shared<T>();
			objects.push_back(o);

			if constexpr (std::is_base_of_v<Object, T>)
			{
				o->transform = { p ,r, s };
			}

			return o;
		}

		auto begin() const
		{
			return objects.begin();
		}

		auto end() const
		{
			return objects.end();
		}
		virtual const ICamera* GetMainCamera() const override
		{
			return nullptr;
		}
		virtual void Init(IRenderEngine& engine) override {};
		virtual void Update(const IRenderEngine&) override {};
		virtual void HandleInput(const IRenderEngine&) override {};
		virtual void OnMouseMove(const IRenderEngine&) override {};
		virtual void OnMouseWheel(const IRenderEngine&) override {};
		virtual void RenderFrame(IRenderEngine& engine) override
		{
			for (auto& object : objects)
			{
				object->Render(engine);
			}
		};

		virtual ~Scene() = default;

	protected:
		std::vector<std::shared_ptr<IRenderAble>> objects;
	};
}
