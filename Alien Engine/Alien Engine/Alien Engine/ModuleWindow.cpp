#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	name.assign("Window");
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(full_desktop)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * SCREEN_SIZE, height * SCREEN_SIZE, flags);
		SDL_SetWindowBrightness(window, brightness);
		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}
	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::LoadConfig(JSON_Object*& config)
{
	width = json_object_dotget_number(config, "Configuration.Window.Width");
	height = json_object_dotget_number(config, "Configuration.Window.Height");
	brightness = json_object_dotget_number(config, "Configuration.Window.Brightness");
	fullscreen = json_object_dotget_boolean(config, "Configuration.Window.Fullscreen");
	full_desktop = json_object_dotget_boolean(config, "Configuration.Window.Fulldesktop");
	resizable = json_object_dotget_boolean(config, "Configuration.Window.Resizable");
	borderless = json_object_dotget_boolean(config, "Configuration.Window.Borderless");
	style = json_object_dotget_number(config, "Configuration.Window.Style.Type");
	style_color.r = json_object_dotget_number(config, "Configuration.Window.Style.ColorR");
	style_color.b = json_object_dotget_number(config, "Configuration.Window.Style.ColorB");
	style_color.g = json_object_dotget_number(config, "Configuration.Window.Style.ColorG");
	style_color.a = json_object_dotget_number(config, "Configuration.Window.Style.ColorA");
	window_name = (char*)json_object_dotget_string(config, "Configuration.Application.Name");
	organitzation_name = (char*)json_object_dotget_string(config, "Configuration.Application.Organitzation");
}

void ModuleWindow::SaveConfig(JSON_Object*& config) 
{

	JSON_Value* user_data = json_parse_file("Configuration/DefaultConfiguration.json");

	user_data = json_value_init_object();

	json_object_dotset_number(json_object(user_data), "Configuration.Window.Width", width);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.height", height);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.Brightness", brightness);
	json_object_dotset_boolean(json_object(user_data), "Configuration.Window.Fullscreen", fullscreen);
	json_object_dotset_boolean(json_object(user_data), "Configuration.Window.Fulldesktop", full_desktop);
	json_object_dotset_boolean(json_object(user_data), "Configuration.Window.Resizable", resizable);
	json_object_dotset_boolean(json_object(user_data), "Configuration.Window.Borderless", borderless);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.Style.Type", style);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.Style.ColorR", style_color.r);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.Style.ColorG", style_color.g);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.Style.ColorB", style_color.b);
	json_object_dotset_number(json_object(user_data), "Configuration.Window.Style.ColorA", style_color.a);
	json_object_dotset_string(json_object(user_data), "Configuration.Application.Name", window_name);
	json_object_dotset_string(json_object(user_data), "Configuration.Application.Organitzation", organitzation_name);

	json_serialize_to_file_pretty(user_data, "Configuration/DefaultConfiguration.json");
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}