//
// Created by ajaxian on 11/14/23.
//

#ifndef ORANGE_ENTRYPOINT_H
#define ORANGE_ENTRYPOINT_H

extern Orange::Application* Orange::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace Orange {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			Orange::Application* app = Orange::CreateApplication(argc, argv);
			app->Run();
			delete app;
		}

		return 0;
	}

}

#ifdef PLATFORM_WINDOWS

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Orange::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Orange::Main(argc, argv);
}

#endif // WL_PLATFORM_WINDOWS

#endif //ORANGE_ENTRYPOINT_H
