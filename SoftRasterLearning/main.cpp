#include "render_test/render_test_unlit.hpp"
#include "render_test/render_test_light_blinn_phong.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//RenderTest_Unlit{ hInstance }.Run();
	RenderTest_Blinn_Phong{ hInstance }.Run();
}
