#include <iostream>
#include "DataTypes/GeneralCredential.hpp"
#include "Database.hpp"
#include <string>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <SDL3/SDL.h>
#include <filesystem>

void NewEntryForm(int& selected, Database& db) {
	static char title[64] = { 0 };
	static char username[64] = { 0 };
	static char password[64] = { 0 };
	static char email[64] = { 0 };
	static char wallet[512] = { 0 };

	static bool showTitleError = false;

	ImGui::TextColored(ImVec4(1, 1, 1, 1), "=== Yeni Bilgi Ekle ===");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Title");
	ImGui::InputText("##title", title, IM_ARRAYSIZE(title));
	ImGui::Text("Username");
	ImGui::InputText("##username", username, IM_ARRAYSIZE(username));
	ImGui::Text("Password");
	ImGui::InputText("##password", password, IM_ARRAYSIZE(password));
	ImGui::Text("Email");
	ImGui::InputText("##email", email, IM_ARRAYSIZE(email));
	ImGui::Text("Wallet");
	ImGui::InputTextMultiline("##wallet", wallet, IM_ARRAYSIZE(wallet), ImVec2(-FLT_MIN, 100));

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Kaydet tuşu
	if (ImGui::Button("Kaydet", ImVec2(120, 0))) {
		GeneralCredential gc(title, username, password, wallet, email);
		if (gc.Title.length() < 2) {
			showTitleError = true;
		} else {
			db.Insert({ gc });
			selected = -2; // Listeye dön
		}
	}

	ImGui::SameLine();

	if (showTitleError) {
		ImGui::OpenPopup("TitleError");
	}

	if (ImGui::BeginPopupModal("TitleError", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Title 2 karakterden kısa olamaz.");
		ImGui::Spacing();

		if (ImGui::Button("Tamam", ImVec2(120, 0))) {
			showTitleError = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ShowForm(int& selected, GeneralCredential& credential, Database& db) {
	static char sfTitle[64] = { 0 };
	static char sfUsername[64] = { 0 };
	static char sfPassword[64] = { 0 };
	static char sfEmail[64] = { 0 };
	static char sfWallet[512] = { 0 };
	static int lastID = -1;
	static bool showResetConfirm = false;
	static bool updated = false;
	static bool showKaydetDurum = false;

	if (credential.ID != lastID) {
		strcpy_s(sfTitle, sizeof(sfTitle) - 1, credential.Title.c_str());
		strcpy_s(sfUsername, sizeof(sfUsername) - 1, credential.Username.c_str());
		strcpy_s(sfPassword, sizeof(sfPassword) - 1, credential.Password.c_str());
		strcpy_s(sfEmail, sizeof(sfEmail) - 1, credential.Email.c_str());
		strcpy_s(sfWallet, sizeof(sfWallet) - 1, credential.Wallet.c_str());

		sfTitle[sizeof(sfTitle) - 1] = '\0';
		sfUsername[sizeof(sfUsername) - 1] = '\0';
		sfPassword[sizeof(sfPassword) - 1] = '\0';
		sfEmail[sizeof(sfEmail) - 1] = '\0';
		sfWallet[sizeof(sfWallet) - 1] = '\0';

		lastID = credential.ID;
	}

	ImGui::Text("Title"); ImGui::SameLine(); ImGui::InputText("##title", sfTitle, IM_ARRAYSIZE(sfTitle));
	ImGui::Text("Username"); ImGui::SameLine(); ImGui::InputText("##username", sfUsername, IM_ARRAYSIZE(sfUsername));
	ImGui::Text("Password"); ImGui::SameLine(); ImGui::InputText("##password", sfPassword, IM_ARRAYSIZE(sfPassword));
	ImGui::Text("Email"); ImGui::SameLine(); ImGui::InputText("##email", sfEmail, IM_ARRAYSIZE(sfEmail));
	ImGui::Text("Wallet"); ImGui::SameLine(); ImGui::InputTextMultiline("##wallet", sfWallet, IM_ARRAYSIZE(sfWallet), ImVec2(-FLT_MIN, 100));

	ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
	
	// Kaydet tuşu
	if (ImGui::Button("Kaydet", ImVec2(120, 0))) {
		credential.Username = sfUsername;
		credential.Password = sfPassword;
		credential.Title = sfTitle;
		credential.Email = sfEmail;
		credential.Wallet = sfWallet;
		updated = db.Update(credential);
		showKaydetDurum = !updated;
	}
	ImGui::SameLine();
	if (ImGui::Button("Sil", ImVec2(120, 0))) {
		showResetConfirm = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Kapat", ImVec2(120, 0))) {
		selected = -2;
	}

	if (showKaydetDurum) {
		ImGui::OpenPopup("KaydetDurum");
	}
	if (showResetConfirm) {
		ImGui::OpenPopup("SilOnay");
	}

	if (ImGui::BeginPopupModal("KaydetDurum", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Güncelleme başarısız oldu. Tekrar deneyiniz.");
		if (ImGui::Button("Tamam", ImVec2(120, 0))) {
			selected = -2;
			lastID = -1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("SilOnay", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("İçeriği silmek istediğinize. Emin misiniz?");
		ImGui::Spacing();

		if (ImGui::Button("Evet", ImVec2(120, 0))) {
			memset(sfTitle, 0, sizeof(sfTitle));
			memset(sfUsername, 0, sizeof(sfUsername));
			memset(sfPassword, 0, sizeof(sfPassword));
			memset(sfEmail, 0, sizeof(sfEmail));
			memset(sfWallet, 0, sizeof(sfWallet));
			showResetConfirm = false;
			db.Delete(credential.ID);
			ImGui::CloseCurrentPopup();
			selected = -2;
		}

		ImGui::SameLine();

		if (ImGui::Button("Vazgeç", ImVec2(120, 0))) {
			showResetConfirm = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainList(std::vector<GeneralCredential>& list, Database &db) {
	float menuBarHeight = ImGui::GetFrameHeight();
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 windowPos = ImVec2(0, menuBarHeight);
	ImVec2 windowSize = ImVec2(displaySize.x, displaySize.y - menuBarHeight);

	ImGui::SetNextWindowSize(windowSize);
	ImGui::SetNextWindowPos(windowPos);

	ImGui::Begin("Passwords", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	static char searchText[32] = { 0 };
	static int selected = -2;

	ImVec2 fullSize = ImGui::GetContentRegionAvail();
	float leftPanelWidth = fullSize.x * 0.4f;
	leftPanelWidth = std::max(leftPanelWidth, 300.0f);

	{
		ImGui::BeginChild("Aside", ImVec2(leftPanelWidth, 0), ImGuiChildFlags_Borders);

		ImGui::InputTextWithHint("##searchArea", "Search...", searchText, IM_ARRAYSIZE(searchText));
		ImGui::SeparatorText("Passwords");

		int i = 0;
		if (ImGui::Selectable("All##all", selected == -2)) {
			selected = -2;
		}
		if (ImGui::Selectable("Add New##addnew", selected == -1)) {
			selected = -1;
		}
		std::string s(searchText);
		for (const GeneralCredential& item : list) {
			if (s.length() > 0) {
				if (item.Search(s) < 0.1) continue;
			}
			std::string label = std::to_string(item.ID) + " " + item.Title;
			std::string id = "##item" + std::to_string(i);
			if (ImGui::Selectable((label + id).c_str(), selected == i)) {
				selected = i;
			}
			i++;
		}

		ImGui::EndChild();
	}
	ImGui::SameLine();
	{
		ImGui::BeginChild("Detail", ImVec2(fullSize.x * 0.6f, 0), ImGuiChildFlags_None);
		if (selected >= 0 && selected < (int)list.size()) {
			ShowForm(selected, list[selected], db);
		}
		else if (selected == -1) {
			NewEntryForm(selected, db);
		}
		else if (selected == -2) {
			list = db.GetAll();
		}
		ImGui::EndChild();
	}
	

	ImGui::End();
}

int main()
{
	Database db(getExePath());
	db.RunSQL(GeneralCredential::CreateTableSQL());

	std::vector<GeneralCredential> list;
	list = db.GetAll();

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "Error " << SDL_GetError() << std::endl;
		return -1;
	}

	Uint32 WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	SDL_Window* window = SDL_CreateWindow("Password Manager", 800, 600, WindowFlags);

	if (window == nullptr) {
		std::cerr << "Error " << SDL_GetError() << std::endl;
		return -2;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
	if(renderer == nullptr) {
		std::cerr << "Error " << SDL_GetError() << std::endl;
		return -3;
	}
	SDL_SetRenderVSync(renderer, 1);

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	
	io.FontGlobalScale = 1.25f;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGuiStyle& style = ImGui::GetStyle();

	// Size & Layout
	style.FrameRounding = 6.0f;
	style.FramePadding = ImVec2(6.0f, 4.0f);
	style.ItemSpacing = ImVec2(10.0f, 4.0f);
	style.ChildRounding = 12.0f;
	style.ScrollbarRounding = 8.0f;
	style.GrabRounding = 12.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

	ImVec2 textAlign = ImVec2(0.5f, 0.5f);
	ImVec2 textPadding = ImVec2(20.0f, 6.0f);

	// Colors
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.137f, 0.486f, 1.0f, 1.0f);       // #237CFFFF
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.254f, 0.412f, 0.882f, 1.0f);         // RoyalBlue
	colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.482f, 1.0f, 1.0f);          // #007bff
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.412f, 0.851f, 1.0f);       // #0069d9

	bool Loop = true;
	
	while (Loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (
				(event.type == SDL_EVENT_QUIT)
				||
				(event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
			) {
				Loop = false;
			}
		}

		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(100);
			continue;
		}

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		//BODY
		{

			//Toolbar
			if (ImGui::BeginMainMenuBar()) {
				ImGui::Text("Password Manager");
				ImGui::EndMainMenuBar();
			}
			//Toolbar
			
			//List
			MainList(list, db);
			//List


		}
		//BODY

		ImGui::Render();
		SDL_SetRenderDrawColorFloat(renderer, clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	/*
	* Insert
	* 
	GeneralCredential *gc = new GeneralCredential("Amazon", "Username", "123456");
	db.Insert(std::vector<GeneralCredential>{*gc});

	delete gc;
	*/

	/*
	* Select All
	* 
	std::vector<GeneralCredential> credentials = db.GetAll();
	for (GeneralCredential gc : credentials) gc.print();
	*/

	/*
	* Delete
	GeneralCredential* gc = new GeneralCredential("Amazon", "Username", "123456");
	gc->ID = 2;
	db.Delete(*gc);
	*/

	/*
	* Update
	GeneralCredential* gc = new GeneralCredential("Amazon", "Username", "123456");
	gc->ID = 3;
	db.Update(*gc);
	*/

	return 0;
}
