#include <iostream>

#include "graphics.hpp"
#include "chess_engine_provider.hpp"
#include "settings_provider.hpp"

#define SETTINGS_PATH "./settings/settings.cfg"

int main(int argc, char** argv) 
{

	graphics::settings = SettingsProvider(SETTINGS_PATH);

	ChessEngineProvider engine = ChessEngineProvider(
		graphics::settings.get_setting<std::string>("engine_path"), 
		graphics::settings.get_setting<int>("engine_skill_level")
	);
	graphics::Scene scene(
		graphics::settings.get_setting<e_color>("game_host_color"), 
		&engine, 
		graphics::settings.get_setting<bool>("engine")
	);

	if (!scene.init_SDL("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, graphics::settings.get_setting<int>("window_width"), graphics::settings.get_setting<int>("window_height")))
	{
		std::cout << "Error! Unable to initialize SDL or TTF.";
		return 0;
	}

	if (!scene.init_objects())
	{
		std::cout << "Error! Unable to initialize objects.";
		return 0;
	}

	while (scene.running())
	{
		scene.handle_engine();
		scene.handle_events();
		scene.update();
		scene.render();
	}
	scene.clean_objects();
	scene.clean_SDL();

	graphics::settings.save_to_file();

	return 0;
}