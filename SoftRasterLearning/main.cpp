#include "render_test/render_test_1.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPreInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	RenderApp sr_app{ hInstance };
	sr_app.Run();
}
