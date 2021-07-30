#pragma once

#ifndef IM_FILE_LOADER
#define IM_FILE_LOADER

#ifndef IMGUI_VERSION
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#endif

#ifndef _STRING_
#include <string>
#endif

#ifndef _FSTREAM_
#include <fstream>
#endif

#ifndef _VECTOR_
#include <vector>
#endif

#include <filesystem>

#ifndef _MAP_
#include <map>
#endif
#include <iostream>
#include <tinydir.h>


#ifndef arraySize
#define arraySize(value) sizeof(value)/sizeof(value[0])
#endif // !arraySize


#ifndef _GL_TEXTURE_
#define _GL_TEXTURE_
namespace gl {
	class Texture {
	public:
		unsigned int id;
		const char* type = {};
		const char* path = {};
		int width = 0;
		int height = 0;
		void* storage = {};

		Texture() :id(NULL), type(""), path("") { }
		Texture(unsigned int* datain, int* x, int* y, void* d) : id(*datain), width(*x), height(*y), storage(d) { }
		Texture(unsigned int* datain, int* x, int* y) : id(*datain), width(*x), height(*y) { }
		Texture(unsigned int datain, int x, int y, void* d) : id(datain), width(x), height(y), storage(d) { }
		Texture(unsigned int datain, int x, int y) : id(datain), width(x), height(y) { }
		Texture(unsigned int* datain, int x, int y, void* d) : id(*datain), width(x), height(y), storage(d) { }
		Texture(unsigned int* datain, int x, int y) : id(*datain), width(x), height(y) { }
		template<class T>
		T operator[](int i) {
			switch (i) {
			default:
				return id;
				break;
			case 1:
				return width;
				break;
			case 2:
				return height;
				break;
			}
		}
		Texture operator=(int input[2]) {
			width = input[0];
			height = input[1];
		}
	};
	Texture LoadTextureFromFile(std::string filename);
}
#endif


namespace ImGui {
	typedef tinydir_file File;

	class Folder {
	public:

		void refresh();
		bool load(const char* _path);
		std::vector<File> getChildren() const { return children; }
		void appedChild(const File _file) { children.push_back(_file); }
		size_t numberOfFiles() const { return children.size(); }
		File& operator[](size_t idx) { return children.at(idx); }
		bool setPath(const char* _path);
		std::string getPath()const { return path; }
		tinydir_dir dir;
	private:
		std::string path = "";
		std::vector<File> children;
	};
	class FileManager {
	public:
		FileManager() {
			readUserPaths();
		}
		FileManager(const char* initialPath) {
			path = initialPath;
			readUserPaths();
		}
		~FileManager() {};

		void setTitle(const char* _title) { title = _title; }
		void setIcons(std::map<std::string, gl::Texture>* _icons) { icons = _icons; }
		void setPath(const char* _path) {
			std::string temp(_path);
			while (temp.find('\\') != std::string::npos)
				temp.replace(temp.find_first_of('\\'), 1, 1, '/');
			path = temp;
		}
		bool imFileSaver(const char* id);
		bool imFileLoader(const char* id);
		bool imFolderLoader(const char* id);
		void setFilters(std::vector<const char*> _filters) { filters = _filters; }
		void showLoader(const char* id) { loaderVisible = id; }
		void showSaver(const char* id) { saverVisible = id; }
		void showFolder(const char* id) { folderVisible = id; }
		bool isVisible() const { return loaderVisible.size() > 0 || saverVisible.size() > 0 || folderVisible.size() > 0; }
		void hide() { saverVisible.clear(); loaderVisible.clear(); folderVisible.clear(); }
		std::vector<std::string> getResult() const {
			return filesSelected;
		}
		std::string getFolderResult() const {
			return folderSelected;
		}
		bool showFilesIcons(bool*, char*, bool showFiles = true);
		bool readUserPaths();
		std::string drawUserFolders();
		std::vector<File> findFileList(Folder, const char*);

	private:
		bool content(bool showFiles = true);
		std::string title = "", path = "";
		std::map<const char*, std::string> paths;
		std::vector<const char*> filters = { "*" };
		std::string loaderVisible = "", saverVisible = "", folderVisible = "";
		bool multipleSelection = false;
		std::map<std::string, gl::Texture>* icons;
		ImGui::Folder folder;
		std::vector<ImGui::File> bufferFiles;
		std::vector<std::string> filesSelected;
		std::string folderSelected = "";
		std::string _f = "folder";
		std::vector<Folder> _back, _forward;
	};

	bool splitter(bool, float, float*, float*, float, float, float splitterLongAxisSize = -1.0f);

	Folder listThisFolder(const char*);

	const char* drawDiskLetters();



	bool FileIcon(ImTextureID, const char*, bool*, bool, ImVec2, ImVec4, ImVec4);

	;



}


static bool dialogOpened = false;
static float fileListWidth = 250;
static float filePreviewWidth = 200;
static char charFindFile[128] = "";
static bool folderStates[2048];

static DWORD dwSize = MAX_PATH;
static char szLogicalDrives[MAX_PATH] = { 0 };
static std::vector<const char*> drives;
const KNOWNFOLDERID defaultFoldersId[] = {
		FOLDERID_Profile, FOLDERID_Documents, FOLDERID_Desktop, FOLDERID_Music, FOLDERID_Objects3D, FOLDERID_OneDrive, FOLDERID_Pictures, FOLDERID_Videos
};
const const char* defaultFolders[] = {
	"User", "Documents","Desktop","Music","Objects 3D","OneDrive","Pictures","Videos"
};

const char* toLower(const char*);
const char* toUpper(const char*);

template<class T>
size_t find(std::vector<T>, T);
static std::string f = "";

#if defined(STBI_INCLUDE_STB_IMAGE_H) && !defined(_GL_IMG_) && !defined(LOAD_TEXTURE_DEFINED)
#define LOAD_TEXTURE_DEFINED
namespace gl {
	inline gl::Texture LoadTextureFromFile(std::string filename) {
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 0);
		if (filename.find_last_of(".png") > 0)
			image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return gl::Texture((unsigned int)0, 0.f, 0.f);

		// Create a OpenGL texture identifier
		glActiveTexture(GL_TEXTURE4);
		unsigned int image_texture;
		glGenTextures(1, &image_texture);
		glBindTexture(GL_TEXTURE_2D, image_texture);

		// Setup filtering parameters for display
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		stbi_image_free(image_data);
		glActiveTexture(GL_TEXTURE4);
		return  gl::Texture(&image_texture, (float)image_width, (float)image_height);
	}
}
#endif

inline bool BeginIsolated(const char* title, bool* opened = (bool*)0, ImGuiWindowFlags flags = 0) {
	ImGuiViewport* view = ImGui::GetMainViewport();
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec4 c = colors[ImGuiCol_WindowBg];
	colors[ImGuiCol_WindowBg] = colors[ImGuiCol_ModalWindowDimBg];
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(view->Size);
	bool clicked = ImGui::Begin("__f0__", opened,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
	ImGui::End();
	if (clicked)ImGui::SetNextWindowFocus();
	colors[ImGuiCol_WindowBg] = c;
	ImGui::SetNextWindowFocus();
	ImGui::SetNextWindowSize(ImVec2(view->Size.x * 0.75, view->Size.y * 0.75));
	ImGui::SetNextWindowPos(ImVec2(view->Size.x * 0.5f, view->Size.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin(title, opened, flags);
	ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
	return ImGui::IsItemClicked() ? true : clicked;
}
inline void EndIsolated() {
	ImGui::End();
}
inline void CloseIsolated(bool* opened) {
	*opened = false;
}

inline void CloseIsolated(const char* title) {
	ImGui::CloseCurrentPopup();
}

inline bool ImGui::splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitterLongAxisSize)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##splitter");
	ImRect bb;
	ImVec2 im = split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1);
	bb.Min = ImVec2(window->DC.CursorPos.x + im.x, window->DC.CursorPos.y + im.y);
	ImVec2 it = CalcItemSize(split_vertically ? ImVec2(thickness, splitterLongAxisSize) : ImVec2(splitterLongAxisSize, thickness), 0.0f, 0.0f);
	bb.Max = ImVec2(bb.Min.x + it.x, bb.Min.y + it.y);
	return splitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}


inline bool readLocalDisks() {

	DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		drives.clear();
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive)
		{
			drives.push_back(szSingleDrive);
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
		drives.push_back(szSingleDrive);
		return true;
	}
	return false;
}

static inline bool dirExists(char* path)
{
	struct stat info;

	if (stat(path, &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;
	else
		return false;
}


inline bool ImGui::FileManager::readUserPaths() {
#define func(...) [&](__VA_ARGS__)->


	auto load = func(const KNOWNFOLDERID profile)std::string{
		std::wstring temp = L"";
		PWSTR buffer = L"";

		if (SHGetKnownFolderPath(profile,0,NULL,&buffer) == S_OK) {
			temp = buffer;
			CoTaskMemFree(buffer);
			while (temp.find('\\') != std::string::npos)
				temp.replace(temp.find_first_of('\\'), 1, 1, '/');
			return std::string(temp.begin(),temp.end());
		}
		return "";
	};
	for (size_t i = 0; i < arraySize(defaultFolders); ++i) {
		std::string ph = load(defaultFoldersId[i]);
		if (dirExists(ph.data())) {
			paths.insert({ defaultFolders[i], ph });
		}
	}

	return paths.size() > 0;
#undef func

}

inline bool ImGui::Folder::load(const char* _path) {
	if (_path == NULL) return false;
	*this = listThisFolder(_path);
	return true;
}

inline bool ImGui::Folder::setPath(const char* _path) {
	path.assign(_path);
	return path.size() > 0 ? true : false;
}


inline void ImGui::Folder::refresh() { *this = listThisFolder(path.c_str()); }

inline ImGui::Folder ImGui::listThisFolder(const char* path) {
	Folder folder;
	if (tinydir_open_sorted(&folder.dir, path) > -1) {
		size_t i = 0;
		for (i = 0; i < folder.dir.n_files; ++i)
		{
			ImGui::File file;
			if (tinydir_readfile_n(&folder.dir, &file, i) > -1) {
				std::string name = file.name;

				if (name != "." && name != "..") {

					folder.appedChild(file);
				}
			}
		}
		folder.setPath(folder.dir.path);
		tinydir_close(&folder.dir);

	}
	return folder;
};

inline const char* ImGui::drawDiskLetters() {
	readLocalDisks();
	ImGui::Text("Drives:");
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	for (int i = 0; i < drives.size(); ++i) {
		ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, drives[i]);
		if (ImGui::IsItemClicked()) {
			return drives[i];

		}
	}
	return "";

}

inline std::string ImGui::FileManager::drawUserFolders() {
	ImGui::Text("User Folders:");
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	for (size_t i = 0; i < paths.size(); ++i) {
		try {
			std::string name = paths[defaultFolders[i]];
			ImGui::TreeNodeEx((void*)(i + 100), node_flags, defaultFolders[i]);
			if (ImGui::IsItemClicked()) {
				return name;
			}
		}
		catch (std::exception e) {
			continue;
		}
	}
	return "";
}

inline std::vector<ImGui::File> ImGui::FileManager::findFileList(ImGui::Folder inputFolder, const char* value) {
	std::string vl = toLower(value);
	std::vector<ImGui::File> temp;
	std::string actual;
	for (size_t i = 0; i < inputFolder.numberOfFiles(); ++i) {
		actual = toLower(inputFolder[i].name);
		if (actual.find(vl) != std::string::npos)
			temp.push_back(inputFolder[i]);
	}
	return temp;
}

inline bool ImGui::FileIcon(ImTextureID id, const char* title, bool* selected, bool selectable = true, ImVec2 size = { 85,95 }, ImVec4 tintColor = { 1, 1, 1, 1 }, ImVec4 borderColor = { 0,0,0,0 }) {
	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec4* colors = style.Colors;
	ImGuiWindow* this_win = ImGui::GetCurrentWindow();
	ImVec2 p = this_win->DC.CursorPos;
	ImRect bb(p.x, p.y, p.x + size.x, p.y + size.y), bb2;
	bool hovered = false, held = false, check_hovered = false, check_held = false, check_clicked = false;
	if (selectable) {
		bb2 = { p.x, p.y, p.x + 16, p.y + 16 };
		ImGuiID	check_id = this_win->GetID(title + 1);
		ImGui::ItemAdd(bb2, check_id);
		check_clicked = ButtonBehavior(bb2, check_id, &check_hovered, &check_held);
		if (check_clicked)
		{
			*selected = !(*selected);
			MarkItemEdited(check_id);
		}
	}
	ImGuiID this_id = this_win->GetID(title);

	ImGui::ItemSize(bb, style.FramePadding.y);
	if (selectable)
		bb.Min.y += 18, bb.Max.y -= 18;
	ImGui::ItemAdd(bb, this_id);
	if (selectable)
		bb.Min.y -= 18, bb.Max.y += 18;
	bool clicked = ButtonBehavior(bb, this_id, &hovered, &held);


	if (clicked)
	{
		*selected = !(*selected);
		MarkItemEdited(this_id);
	}
	//RenderNavHighlight(bb, this_id);
	if (*selected) {
		RenderFrame(p, bb.Max, GetColorU32(hovered || clicked ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]));
		if (selectable) {
			draw_list->AddCircleFilled(ImVec2(bb2.Min.x + 9, bb2.Min.y + 9), 8.f, GetColorU32(check_hovered ? colors[ImGuiCol_ButtonActive] : ImVec4(0, 0, 0, 0)));
			RenderCheckMark(draw_list, ImVec2(bb2.Min.x + 1, bb2.Min.y + 1), GetColorU32(colors[ImGuiCol_ButtonActive]), 16.f);
		}
	}
	else {
		RenderFrame(p, ImVec2(p.x + size.x, p.y + size.y), GetColorU32(hovered ? colors[ImGuiCol_FrameBgHovered] : ImVec4(0, 0, 0, 0)));
		if (selectable)draw_list->AddCircleFilled(ImVec2(bb2.Min.x + 9, bb2.Min.y + 9), 8.f, GetColorU32(check_hovered ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]));
	}
	draw_list->AddImage(id, ImVec2(p.x + 10, p.y + 10), ImVec2(p.x + size.x - 10, p.y + size.y - 20), ImVec2(0, 0), ImVec2(1, 1), GetColorU32(tintColor));
	ImVec2 label_size = CalcTextSize(title, NULL, true);
	RenderTextClipped(ImVec2(p.x + 1, p.y + size.y - 18), ImVec2(p.x + 1 + label_size.x, p.y + size.y - 18 + label_size.y),
		title, NULL, &label_size, ImVec2(0.5, 0.5), &bb);
	return selectable ? check_clicked : clicked;
}


inline bool ImGui::FileManager::showFilesIcons(bool* list, char* value, bool showFiles) {
	if (value != "") {
		bufferFiles = findFileList(folder, value);
	}
	int previewsRendered = 2;
	ImGui::File item;
	bool applyFilter = find(filters, "*") == -1 ? true : false;
	if (bufferFiles.size() > 0)
		for (size_t i = 0; i < bufferFiles.size(); ++i) {
			item = bufferFiles[i];
			if (applyFilter) {
				if (!item.is_dir) {
					int k = 0;
					for (size_t j = 0; j < filters.size(); ++j)
						if (std::string(item.name).find(filters[j]) != std::string::npos)
							k++;
					if (k == 0) continue;
				}
			}
			_f = "folder";
			if (!item.is_dir) _f = "file";
			if (!showFiles && !item.is_dir) continue;
			if (ImGui::FileIcon((ImTextureID)icons->at(_f).id, item.name, &list[i], multipleSelection)) {
				if (item.is_dir)
					folderSelected = item.path;
				else
					filesSelected.push_back(item.path);
			}
			else if (ImGui::IsItemClicked()) {
				if (ImGui::GetIO().KeyCtrl)
					filesSelected.push_back(item.path);
				else if (ImGui::GetIO().KeyShift) {
					for (size_t j = 0; j < bufferFiles.size(); j++)
						if (j != i) list[j] = false;
				}
				else {
					for (size_t j = 0; j < bufferFiles.size(); j++)
						if (j != i) list[j] = false;
					filesSelected.clear();
					filesSelected.push_back(item.path);
				}

			}
			if (ImGui::IsItemHovered()) {
				if (ImGui::IsMouseDoubleClicked(0)) {
					if (item.is_dir) {
						filesSelected.clear();
						folderSelected = item.path;
						_back.push_back(folder);
						folder.load(item.path);
						_forward.clear();
					}
					else {
						return true;
					}
				}
				ImGui::SetTooltip(item.name);
			}
			if ((ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorStartPos().x - 30) / 90 > previewsRendered) {
				ImGui::SameLine();
				previewsRendered++;
			}
			else previewsRendered = 2;
		}
	return false;
};
static bool createFolder = false;
static char newFolderName[128] = {};
inline bool ImGui::FileManager::content(bool showFiles) {

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
	if (ImGui::ImageButton((ImTextureID)icons->at("left").id,
		ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImVec2(0, 0), ImVec2(1, 1), 0)
		&& !_back.empty()) {
		_forward.push_back(folder);
		folder.load(_back.back().getPath().c_str());
		folderSelected = _back.back().getPath();
		_back.pop_back();

	} ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID)icons->at("right").id,
		ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImVec2(0, 0), ImVec2(1, 1), 0)
		&& !_forward.empty()) {
		_back.push_back(folder);
		folder.load(_forward.back().getPath().c_str());
		folderSelected = _forward.back().getPath();
		_forward.pop_back();
	}
	ImGui::PopStyleVar();
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 4));
	ImGui::PopStyleVar();
	if (ImGui::ImageButton((ImTextureID)icons->at("up").id,
		ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImVec2(0, 0), ImVec2(1, 1), 0)) {
		_back.push_back(folder);
		std::string parent = folder.getPath();
		parent = parent.substr(0, parent.find_last_of('/'));
		folder.load(parent.c_str());
		folderSelected = parent.c_str();
	}

	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginChild("0", ImVec2(ImGui::GetContentRegionAvailWidth() - 260, ImGui::GetFrameHeight() + 2), true, ImGuiWindowFlags_NoScrollbar)) {
		std::string allPath = folder.getPath(), temp = "";
		size_t i;
		for (i = 0; i < allPath.size(); ++i) {
			if (allPath[i] == '/') {
				if (ImGui::Button(temp.substr(temp.find_last_of('/') + 1).c_str())) {
					_back.push_back(folder);
					folder.load(temp.c_str());
					folderSelected = temp.c_str();
					_forward.clear();
				}
				ImGui::SameLine();

			}
			temp += allPath[i];
		}
		ImGui::Button(temp.substr(temp.find_last_of('/') + 1).c_str());
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::SameLine();
	if (ImGui::Button("Copy Path")) {
		ImGui::SetClipboardText(folder.getPath().c_str());
	}
	ImGui::SameLine();
	if (ImGui::InputTextEx("", "Search", charFindFile, 128, ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetFrameHeight()), ImGuiInputTextFlags_None)) {}
	if (!createFolder) {
		if (ImGui::Button("Create folder", ImVec2(120, 0))) {
			if (!createFolder) createFolder = true;
			else {
				createFolder = false;
			}
		}
	}
	else {
		ImGui::InputTextEx(".", "Folder Name", newFolderName, 128, ImVec2(275, ImGui::GetFrameHeight()), ImGuiInputTextFlags_None);
		ImGui::SameLine();
		if (ImGui::Button("Confirm")) {
			std::string command = folder.getPath() + "/" + newFolderName;
			namespace fs = std::filesystem;
			if (fs::create_directory(command.c_str())) {
				folder.load(command.c_str());
				folderSelected = command;
				createFolder = false;

			}
		}
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0.1, 1));
		if (ImGui::Button("Cancel")) {
			createFolder = false;

		}
		ImGui::PopStyleColor();
	}

	ImGui::splitter(true, 8.0f, &fileListWidth, &filePreviewWidth, 8, 8, ImGui::GetContentRegionAvail().y - 30);
	if (ImGui::BeginChild("Filelist", ImVec2(fileListWidth, ImGui::GetContentRegionAvail().y - 30))) {
		const char* driveLetter = drawDiskLetters();
		if (driveLetter != "" && driveLetter != folder.getPath().c_str()) {
			_back.push_back(folder);
			_forward.clear();
			folder.load(driveLetter);
			folderSelected = driveLetter;
		}
		std::string userFolder = drawUserFolders();
		if (userFolder != "" && userFolder != folder.getPath().c_str()) {
			_back.push_back(folder);
			_forward.clear();
			Log(userFolder);
			folder.load(userFolder.c_str());
			folderSelected = userFolder;
		}
	}
	ImGui::EndChild();
	filePreviewWidth = ImGui::GetContentRegionAvailWidth() - fileListWidth;
	ImGui::SameLine();
	if (ImGui::BeginChild("Filepreview", ImVec2(filePreviewWidth - 8, ImGui::GetContentRegionAvail().y - 30))) {
		if (showFilesIcons(folderStates, charFindFile, showFiles)) {
			ImGui::EndChild();
			return true;

		}
	}
	ImGui::EndChild();
	return false;

}
inline bool ImGui::FileManager::imFolderLoader(const char* id) {
	bool visible = false;
	if (folderVisible == id) {
		visible = true;
		multipleSelection = false;
		//stringSelectedBuffer.clear();
		if (folder.getPath().size() == 0)
			folder.load(path.c_str());
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (BeginIsolated(title.c_str(), &visible))
		{
			if (content(false)) {
				hide();
				EndIsolated();
				return true;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0.1, 1));

			if (ImGui::Button("Cancel", ImVec2(80, 0))) {
				hide();
				filesSelected.clear();
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			if (ImGui::Button("Select", ImVec2(100, 0)) && !folderSelected.empty()) {
				Log(folderSelected);
				hide();
				EndIsolated();
				return true;
			}


		}
		EndIsolated();
	}
	return false;
}
inline bool ImGui::FileManager::imFileLoader(const char* id) {
	bool visible = false;
	if (loaderVisible == id) {
		visible = true;
		multipleSelection = false;
		//stringSelectedBuffer.clear();
		if (folder.getPath().size() == 0)
			folder.load(path.c_str());
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (BeginIsolated(title.c_str(), &visible))
		{
			if (content()) {
				Log(filesSelected[0]);
				hide();
				EndIsolated();
				return true;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0.1, 1));

			if (ImGui::Button("Cancel", ImVec2(80, 0))) {
				hide();
				filesSelected.clear();
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			if (ImGui::Button("Open", ImVec2(100, 0)) && filesSelected.size() > 0) {
				Log(filesSelected[0]);
				hide();
				EndIsolated();
				return true;
			}


		}
		EndIsolated();
	}
	return false;
}

inline bool ImGui::FileManager::imFileSaver(const char* id) {
	bool visible = false;
	if (saverVisible == id) {
		visible = true;
		multipleSelection = true;
		filesSelected.clear();
		if (folder.getPath().size() == 0)
			folder.load(path.c_str());
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (BeginIsolated(title.c_str(), &visible))
		{
			if (content()) {

			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0.1, 1));
			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				hide();
				filesSelected.clear();

			}
			ImGui::PopStyleColor();
			if (ImGui::Button("Save")) {
				hide();
				EndIsolated();
				return true;
			}


		}
		EndIsolated();
	}
	return false;
}

inline const char* toLower(const char* value) {
	f = "";
	for (size_t i = 0; i < arraySize(value); ++i) { f += std::tolower(value[i]); }
	return f.c_str();
}

inline const char* toUpper(const char* value) {
	f = "";
	for (size_t i = 0; i < arraySize(value); ++i) { f += std::toupper(value[i]); }
	return f.c_str();
}

template<class T>

inline size_t find(std::vector<T> _vec, T _value) {
	for (size_t i = 0; i < _vec.size(); ++i) {
		if (_vec[i] == _value) return i;
	}
	return -1;
}



#endif
