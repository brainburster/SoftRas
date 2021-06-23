//#include "Wnd.hpp"
#include <iostream>
#include "buffer_view.hpp"
#include "dc_wnd.hpp"
#include "software_renderer.hpp"
#include "bmp_loader.hpp"
#include "obj_loader.hpp"

struct Vertex
{
	sr::Position position;
	sr::Color color;
	sr::Vec2 uv;

	Vertex operator+(const Vertex& rhs) const
	{
		return {
			position + rhs.position,
			color + rhs.color,
			uv + rhs.uv
		};
	}

	friend Vertex operator*(const Vertex& lhs, float rhs)
	{
		return {
			lhs.position * rhs,
			lhs.color * rhs,
			lhs.uv * rhs
		};
	}
};

struct Material_Model
{
	sr::Mat mat = sr::Mat::Unit();
	bmp_loader::Texture* tex0;

	Vertex VS(const obj_loader::Model_Vertex& v) const
	{
		return { 
			mat* sr::Vec4{v.position,1.0},
			sr::Vec4(v.normal,1),
			v.uv
		};
	}

	gmath::Vec4<float> FS(const Vertex& v) const
	{
		//return tex0->Sampler(v.uv);
		return v.color;
	}
};


const float pi = 3.14159265358979f;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	wnd::DC_WND wnd = wnd::DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init().Move();

	sr::Context ctx;
	ctx.Viewport(800, 600, { 0.4f, 0.6f, 0.2f, 1.f });

	Material_Model m;

	sr::Renderer<Material_Model> renderer = { ctx,m };
	
	bmp_loader::Texture tex = bmp_loader::BmpLoader::LoadBmp(L"..\\resource\\pictures\\test.bmp");
	auto ret = obj_loader::obj::LoadFromFile(L"..\\resource\\models\\box.obj");
	if (ret.has_value()) 
	{
		//...
	}

	m.tex0 = &tex;

	float time = 0;
	while (!wnd.app_should_close())
	{
		wnd.PeekMsg();
		ctx.Clear({ 0.4f, 0.6f, 0.2f, 1.f });

		m.mat = sr::Mat::Projection( pi*0.5f,4/3, -0.1f, -1000) * sr::Mat::Camera(sr::Vec3{ 0,0,0 }, sr::Vec3{ 0,0,-1 }, sr::Vec3{ 0,1,0 }) * sr::Mat::Translate(0.0, -1,-5) * sr::Mat::Rotate(time/2, time/3,time/4) * sr::Mat::Scale(1.f, 1.f, 1);// *m.mat;

		renderer.DrawTriangles(&ret->mesh[0],ret->mesh.size());


		ctx.CopyToScreen(wnd.getFrameBufferView());
		wnd.drawBuffer();
 		time += 0.05f;
		//...
	}

	return 0;
}