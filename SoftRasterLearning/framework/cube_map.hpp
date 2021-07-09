#pragma once

#include "object.hpp"
#include "../core/texture.hpp"
#include "render_engine.hpp"
#include <memory>

namespace framework
{
	class CubeMap
	{
	protected:
		std::shared_ptr<core::Texture> front;
		std::shared_ptr<core::Texture> back;
		std::shared_ptr<core::Texture> top;
		std::shared_ptr<core::Texture> bottom;
		std::shared_ptr<core::Texture> left;
		std::shared_ptr<core::Texture> right;

	public:
		CubeMap(std::shared_ptr<core::Texture> front, std::shared_ptr<core::Texture> back, std::shared_ptr<core::Texture> top,
			std::shared_ptr<core::Texture> bottom, std::shared_ptr<core::Texture> left, std::shared_ptr<core::Texture> right) :
			front{ front }, back{ back }, top{ top }, bottom{ bottom }, left{ left }, right{ right }
		{
		}

		core::Vec4 Sample(core::Vec3 dir)
		{
			//3�������о���ֵ����,���������ĸ���
			//��ʣ�µ���������"��һ��"������uv
			float abs_x = fabs(dir.x);
			float abs_y = fabs(dir.y);
			float abs_z = fabs(dir.z);

			if (abs_x > abs_y && abs_x > abs_z)
			{
				if (dir.x > 0)
				{
					//��������
					return core::Texture::Sample(right.get(), { (-dir.z / abs_x + 1) / 2,(-dir.y / abs_x + 1) / 2 });
				}
				else
				{
					//��������
					return core::Texture::Sample(left.get(), { (dir.z / abs_x + 1) / 2,(-dir.y / abs_x + 1) / 2 });
				}
			}
			else if (abs_y > abs_x && abs_y > abs_z)
			{
				if (dir.y > 0)
				{
					//��������
					return core::Texture::Sample(top.get(), { (dir.x / abs_y + 1) / 2,(dir.z / abs_y + 1) / 2 });
				}
				else
				{
					//��������
					return core::Texture::Sample(bottom.get(), { (dir.x / abs_y + 1) / 2,(-dir.z / abs_y + 1) / 2 });
				}
			}
			else if (abs_z > abs_x && abs_z > abs_y)
			{
				if (dir.z > 0)
				{
					//����ǰ��
					return core::Texture::Sample(front.get(), { (dir.x / abs_z + 1) / 2,(-dir.y / abs_z + 1) / 2 });
				}
				else
				{
					//��������
					return core::Texture::Sample(back.get(), { (-dir.x / abs_z + 1) / 2,(-dir.y / abs_z + 1) / 2 });
				}
			}
			return { 0,0,0,1.f };
		}

		////��������Ⱦ������ǰ
		//virtual void Render(IRenderEngine& engine) const override
		//{
		//	//todo,�ĳ�˳ʱ��
		//	static float box[] =
		//	{
		//		   -1.f, 1.f, -1.f,
		//			-1.f, -1.f, -1.f,
		//			1.f, -1.f, -1.f,
		//			1.f, -1.f, -1.f,
		//			1.f, 1.f, -1.f,
		//			-1.f, 1.f, -1.f,

		//			-1.f, -1.f, 1.f,
		//			-1.f, -1.f, -1.f,
		//			-1.f, 1.f, -1.f,
		//			-1.f, 1.f, -1.f,
		//			-1.f, 1.f, 1.f,
		//			-1.f, -1.f, 1.f,

		//			1.f, -1.f, -1.f,
		//			1.f, -1.f, 1.f,
		//			1.f, 1.f, 1.f,
		//			1.f, 1.f, 1.f,
		//			1.f, 1.f, -1.f,
		//			1.f, -1.f, -1.f,

		//			-1.f, -1.f, 1.f,
		//			-1.f, 1.f, 1.f,
		//			1.f, 1.f, 1.f,
		//			1.f, 1.f, 1.f,
		//			1.f, -1.f, 1.f,
		//			-1.f, -1.f, 1.f,

		//			-1.f, 1.f, -1.f,
		//			1.f, 1.f, -1.f,
		//			1.f, 1.f, 1.f,
		//			1.f, 1.f, 1.f,
		//			-1.f, 1.f, 1.f,
		//			-1.f, 1.f, -1.f,

		//			-1.f, -1.f, -1.f,
		//			-1.f, -1.f, 1.f,
		//			1.f, -1.f, -1.f,
		//			1.f, -1.f, -1.f,
		//			-1.f, -1.f, 1.f,
		//			1.f, -1.f, 1.f
		//	};

		//	static auto cube = reinterpret_cast<core::Vec3*>(box);

		//	static struct Shader
		//	{
		//		core::Vec3 VS(const core::Vec3& point)
		//		{
		//			return point; //ֱ�ӷ��ر��ؿռ䣬Ӧ�ûᱻ�ü�
		//		}
		//		core::Color FS(const core::Vec3& point)
		//		{
		//			//���ؿռ�
		//			core::Color color = Sample(point);
		//			return core::Vec4{ point,1.f };
		//		}
		//	} shader;

		//	core::Renderer<Shader> renderer = { engine.GetCtx(), shader };
		//	renderer.DrawTriangles(cube, sizeof(box) / sizeof(core::Vec3));
		//}
	};
}
