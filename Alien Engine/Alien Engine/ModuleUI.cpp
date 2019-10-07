#include "Globals.h"
#include "Application.h"
#include "ModuleUI.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include <gl/GL.h>
#include "PanelAbout.h"
#include "PanelConfig.h"
#include "PanelConsole.h"
#include "PanelCreateObject.h"
#include "SDL/include/SDL_assert.h"
#include "ModuleObjects.h"

ModuleUI::ModuleUI(bool start_enabled) : Module(start_enabled)
{
	name.assign("UI");
}

ModuleUI::~ModuleUI()
{
}

// Load assets
bool ModuleUI::Start()
{
	LOG("Loading UI assets");
	bool ret = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ChangeStyle(App->window->style);


	// Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);

	InitPanels();
	InitShortCuts();
	
	return ret;
}


// Load assets
bool ModuleUI::CleanUp()
{
	LOG("Unloading UI scene");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	std::vector<Panel*>::iterator item = panels.begin();
	for (; item != panels.end(); ++item) {
		if ((*item) != nullptr) {
			delete *item;
			*item = nullptr;
		}
	}
	panels.clear();

	return true;
}

update_status ModuleUI::PreUpdate(float dt)
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

void ModuleUI::Draw() {


	if (show_demo_wndow)
		ImGui::ShowDemoWindow(&show_demo_wndow);

	MainMenuBar();
	UpdatePanels();

	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		for (int i = 0;i<100;++i)
			App->objects->CreatePrimitive(PrimitiveType::SPHERE_ALIEN);

	}
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
		App->objects->DeleteAllObjects();

	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleUI::MainMenuBar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::BeginMenu("New")) 
		{
				if (ImGui::MenuItem("haha"))
				{
					LOG("haha");
				}
				ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Open")) 
		{

		}
		if (ImGui::MenuItem("Close", "Alt + F4"))
		{
			App->QuitApp();
		}

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Configuration", panel_config->shortcut->GetNameScancodes()))
		{
			panel_config->ChangeEnable();
		}
		if (ImGui::MenuItem("Console", panel_console->shortcut->GetNameScancodes()))
		{
			panel_console->ChangeEnable();
		}
		if (ImGui::MenuItem("View Mesh", shortcut_view_mesh->GetNameScancodes())) {
			App->objects->ChangeViewMeshMode();
		}
		if (ImGui::MenuItem("Wireframe Mode", shortcut_wireframe->GetNameScancodes())) {
			App->objects->ChangeWireframeMode();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Create"))
	{
		if (ImGui::MenuItem("Cube"))
		{
			App->objects->CreatePrimitive(PrimitiveType::CUBE);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			App->objects->CreatePrimitive(PrimitiveType::SPHERE_ALIEN);
		}
		if (ImGui::MenuItem("Rock"))
		{
			App->objects->CreatePrimitive(PrimitiveType::ROCK);
		}
		if (ImGui::BeginMenu("Other"))
		{
			if (ImGui::MenuItem("Cone"))
			{
			}
			if (ImGui::MenuItem("Cylinder"))
			{
			}
			if (ImGui::MenuItem("Point"))
			{
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Create...", panel_create_object->shortcut->GetNameScancodes()))
		{
			panel_create_object->ChangeEnable();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About", panel_about->shortcut->GetNameScancodes()))
		{
			panel_about->ChangeEnable();
		}
		if (ImGui::MenuItem("Show Gui Demo", shortcut_demo->GetNameScancodes()))
		{
			ChangeEnableDemo();
		}
		if (ImGui::MenuItem("Documentation"))
		{
			LOG("Put link wiki");
		}
		if (ImGui::MenuItem("Report a bug", shortcut_report_bug->GetNameScancodes()))
		{
			ReportBug();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void ModuleUI::ReportBug()
{
	App->OpenWebsite("https://github.com/VictorSegura99/Alien-GameEngine/issues");
}

void ModuleUI::ChangeStyle(const int& style_number)
{
	switch (style_number) {
	case 0:
		ImGui::StyleColorsClassic();
		break;
	case 1:
		ImGui::StyleColorsDark();
		break;
	case 2:
		ImGui::StyleColorsLight();
		break;
	case 3:
		break;
	}
}

void ModuleUI::ChangeEnableDemo()
{
	show_demo_wndow = !show_demo_wndow;
}

void ModuleUI::InitPanels()
{
	panel_about = new PanelAbout("About Alien Engine", SDL_SCANCODE_A, SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL);
	panel_config = new PanelConfig("Configuration", SDL_SCANCODE_O, SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL);
	panel_console = new PanelConsole("Console", SDL_SCANCODE_T, SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL);
	panel_create_object = new PanelCreateObject("Create Object", SDL_SCANCODE_P, SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL);

	panels.push_back(panel_about);
	panels.push_back(panel_config);
	panels.push_back(panel_console);
	panels.push_back(panel_create_object);

}

void ModuleUI::UpdatePanels()
{
	std::vector<Panel*>::iterator item = panels.begin();
	for (; item != panels.end(); ++item) {
		if (*item != nullptr) {
			if ((*item)->IsEnabled()) {
				(*item)->PanelLogic();
			}
		}
	}
}

Panel*& ModuleUI::GetPanelByName(const std::string& panel_name)
{
	std::vector<Panel*>::iterator item = panels.begin();
	for (; item != panels.end(); ++item) {
		if (*item != nullptr && (*item)->GetName() == panel_name) {
			return (*item);
		}
	}
	SDL_assert(1 == 0); //panel name is not correct, revise panels names!!
}

void ModuleUI::InitShortCuts()
{
	shortcut_demo = App->shortcut_manager->AddShortCut("imGui Demo", SDL_SCANCODE_D, std::bind(&ModuleUI::ChangeEnableDemo, App->ui), SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL);
	shortcut_report_bug = App->shortcut_manager->AddShortCut("Report Bug", SDL_SCANCODE_F1, std::bind(&ModuleUI::ReportBug, App->ui), SDL_SCANCODE_LALT, SDL_SCANCODE_RALT);
	shortcut_view_mesh = App->shortcut_manager->AddShortCut("Mesh View", SDL_SCANCODE_F3, std::bind(&ModuleObjects::ChangeViewMeshMode, App->objects));
	shortcut_wireframe = App->shortcut_manager->AddShortCut("Wireframe Mode", SDL_SCANCODE_F4, std::bind(&ModuleObjects::ChangeWireframeMode, App->objects));

	// OrderShortCuts must be called after all shortcuts have been created!! Victor read this...
	App->shortcut_manager->OrderShortCuts();
}

void ModuleUI::FramerateRegister(float frames, float ms)
{
	if (panel_config->IsEnabled())
		panel_config->FramerateInfo(frames,ms);
}



