#include <common/logging.hpp>

#include <platforms/desktop_platform.hpp>

int main(int, char **)
{
	remus::DesktopPlatform platform;

	auto window = platform.create_window("Remus", {800, 600});

	while (true)
	{
		window->update();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return 0;
}
