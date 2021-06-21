//#include "Wnd.hpp"
#include <iostream>
#include "buffer_view.hpp"
#include "dc_wnd.hpp"
#include "software_renderer.hpp"
#include "bmp_loader.hpp"

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
	using VS_IN = ::Vertex;
	using VS_OUT = ::Vertex;

	sr::Mat mat = sr::Mat::Unit();
	loader::Texture* tex0;

	VS_OUT VS(const VS_IN& v) const 
	{
		return { 
			mat * v.position,
			v.color,
			v.uv
		};
	}

	sr::Color FS(const VS_OUT& v) const
	{
		return tex0->Sampler(v.uv);
	}
};

::Vertex rect[4] = {
	{{-0.5f,0.5f,0,1},{1,0,0,1},{0,0}},
	{{ 0.5f,0.5f,0,1},{0,1,0,1},{1,0}},
	{{ 0.5f,-0.5f,0,1},{0,0,1,1},{1,1}},
	{{-0.5f,-0.5f,0,1},{1,0,0,1},{0,1}}
};

size_t index[6] = {
	0,1,2,
	0,2,3
};

const float pi = 3.14159265358979f;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	wnd::DC_WND wnd = wnd::DC_WND{ hInstance }.WndClassName(L"dc_wnd_cls").WndName(L"dc_wnd_wnd").Size(800, 600).AddWndStyle(~WS_MAXIMIZEBOX).Init().Move();

	sr::Context ctx;
	ctx.Viewport(800, 600, { 0.4f, 0.6f, 0.2f, 1.f });

	Material_Model m;

	sr::Renderer<Material_Model> renderer = { ctx,m };
	
	loader::Texture tex = loader::BmpLoader::LoadBmp(L"..\\resource\\pictures\\test.bmp");
	m.tex0 = &tex;

	float time = 0;
	while (!wnd.app_should_close())
	{
		wnd.PeekMsg();
		ctx.Clear({ 0.4f, 0.6f, 0.2f, 1.f });

		m.mat = sr::Mat::Projection( pi*0.5f,4/3, -0.1f, -1000) * sr::Mat::Camera(sr::Vec3{ 0,0,0 }, sr::Vec3{ 0,0,-1 }, sr::Vec3{ 0,1,0 }) * sr::Mat::Translate(0, 0,-1.5-cos(time)) * sr::Mat::Rotate(0, 0,sin(time)*pi/2) * sr::Mat::Scale(0.3f, 0.3f, 1);// *m.mat;

		renderer.DrawIndex(rect, index, 6);

		ctx.CopyToScreen(wnd.getFrameBufferView());
		wnd.drawBuffer();
 		time += 0.01f;
		//...
	}

	return 0;
}