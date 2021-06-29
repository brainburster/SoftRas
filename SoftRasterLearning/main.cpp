#include "render_test/render_test_unlit.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	RenderTest_Unlit sr_app{ hInstance };
	sr_app.Run();
}
