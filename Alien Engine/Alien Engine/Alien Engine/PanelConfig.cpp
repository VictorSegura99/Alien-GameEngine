#include "PanelConfig.h"
#include "ModuleWindow.h"
#include "SDL/include/SDL.h"

PanelConfig::PanelConfig(const std::string& panel_name, const std::vector<SDL_Scancode>& shortcuts) : Panel(panel_name, shortcuts)
{
}

PanelConfig::~PanelConfig()
{
}

void PanelConfig::PanelLogic()
{
	ImGui::Begin("Configuration", &enabled, ImGuiWindowFlags_NoCollapse);

	if (ImGui::CollapsingHeader("Application"))
	{
	}
	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Spacing();
		if (ImGui::SliderInt(" Width", &App->window->width, 0, 1920)) {
			SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
		}
		if (ImGui::SliderInt(" Height", &App->window->height, 0, 1080)) {
			SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
		}
		if (ImGui::SliderFloat(" Brightness", &App->window->brightness, 0.0, 1.0)) {
			SDL_SetWindowBrightness(App->window->window, App->window->brightness);
		}
		ImGui::Spacing();
		ImGui::Text("Refresh Rate:"); ImGui::SameLine(); ImGui::TextColored({ 255,0,0,255 }, "TODOOOOOOOOOOOOOOOOOOO SAD");
		ImGui::Spacing();
		if (ImGui::Checkbox(" FullScreen", &App->window->fullscreen)) {
			if (App->window->fullscreen) {
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
			}
			else {
				SDL_SetWindowFullscreen(App->window->window, 0);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox(" FullDesktop", &App->window->full_desktop)) {
			if (App->window->full_desktop) {
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
			else {
				SDL_SetWindowFullscreen(App->window->window, 0);
			}
		}
		if (ImGui::Checkbox(" Borderless", &App->window->borderless)) {
			SDL_SetWindowBordered(App->window->window, (SDL_bool)!App->window->borderless);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox(" Resizable", &App->window->resizable)) {
			SDL_SetWindowResizable(App->window->window, (SDL_bool)App->window->resizable);
		}

	}
	ImGui::End();

}