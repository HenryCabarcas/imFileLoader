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

#ifndef _FILESYSTEM_
#include <filesystem>
#endif

#ifndef _MAP_
#include <map>
#endif
#include <iostream>

namespace fs = std::filesystem;

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
	class File {
	public:
		File(fs::directory_entry _info) { getInfo(_info); }
		File() {}
		char filename[260] = {};
		char ext[10] = {};
		bool isFolder = false;
		char parent[1024] = {};
		char path[1024] = {};
		uintmax_t size = 0;
		fs::path rawPath;
		std::vector<fs::path> parentsTree;
		bool getInfo(fs::directory_entry);
		bool getInfo(fs::path);
		bool refresh();
	};

	class Folder : public File {
	public:
		void refresh();
		bool load(std::string _path);
		std::vector<File> getChilds() const { return childs; }
		void setChilds(const std::vector<File> _files) { childs = _files; }
		size_t numberOfFiles() const { return childs.size(); }
		File& operator[](size_t idx) { return childs.at(idx); }
	private:
		std::vector<fs::directory_entry> content;
		std::vector<File> childs;
	};

	bool splitter(bool, float, float*, float*, float, float, float splitterLongAxisSize = -1.0f);

	Folder listThisFolder(const char*);

	size_t drawTreeNodes(Folder*, bool);

	std::vector<File> findFileList(Folder, const char*);

	bool FileIcon(ImTextureID, const char*, bool*, ImVec2, ImVec4, ImVec4);

	std::vector<std::string> showFilesIcons(Folder*, std::map<std::string, gl::Texture>&, bool*, char*);

	std::vector<std::string> imFileLoader(const char*, bool*, Folder*, std::map<std::string, gl::Texture>&);

}

static bool dialogOpened = false;
static bool folderStates[3000] = {};
static float fileListWidth = 250;
static float filePreviewWidth = 200;
static char charFindFile[128] = "";
static std::vector<ImGui::File> bufferFiles;
static std::vector<std::string> stringSelectedBuffer;
static ImGui::Folder mainFolder;
static std::string _f = "folder";
static std::vector<fs::path> _back, _forward;

const char* toLower(const char*);
const char* toUpper(const char*);
template<class T>
size_t find(std::vector<T>, T);

#include <imFileLoader.h>

static std::string f = "";
#ifdef STBI_INCLUDE_STB_IMAGE_H
inline gl::Texture gl::LoadTextureFromFile(char* filename) {
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
	glActiveTexture(GL_TEXTURE0);
	return  gl::Texture(&image_texture, (float)image_width, (float)image_height);
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

void CloseIsolated(const char* title) {
	ImGui::CloseCurrentPopup();
}
bool ImGui::splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitterLongAxisSize)
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

bool ImGui::File::getInfo(fs::directory_entry _source) {
	if (!_source.exists()) return false;
	strcpy_s(filename, _source.path().filename().string().c_str());
	//filename = _source.path().filename().string().c_str();
	std::string f = _source.path().filename().extension().string().substr(_source.path().filename().extension().string().find_first_of(".") + 1);
	strcpy_s(ext, f.data());
	isFolder = _source.is_directory();
	//parent = _source.path().parent_path().string();
	strcpy_s(path, _source.path().string().c_str());
	size = _source.file_size();
	rawPath = _source.path();
	return true;
}

bool ImGui::File::getInfo(fs::path _source) {
	if (_source.empty()) return false;
	strcpy_s(filename, _source.filename().string().c_str());
	if (_source.has_extension())
		strcpy_s(ext, _source.filename().extension().string().c_str());
	isFolder = true;
	strcpy_s(parent, _source.parent_path().string().c_str());
	strcpy_s(path, _source.string().c_str());
	rawPath = _source;
	fs::path tempPath = _source.make_preferred();
	parentsTree.insert(parentsTree.begin(), tempPath);
	if (_source != _source.root_path()) {
		do {
			tempPath = tempPath.parent_path().make_preferred();
			parentsTree.insert(parentsTree.begin(), tempPath);
		} while (tempPath != tempPath.root_path());
	}
	//parentsTree.insert(parentsTree.begin(), tempPath.root_path());
	return true;
}

inline bool ImGui::File::refresh() {
	return true;
}

inline bool ImGui::Folder::load(std::string _path) {
	if (_path.empty()) return false;
	*this = listThisFolder(_path.c_str());
	strcpy_s(path, _path.c_str());
	return true;
}

inline void ImGui::Folder::refresh() { *this = listThisFolder(path); }

inline ImGui::Folder ImGui::listThisFolder(const char* _path) {
	Folder folder;
	bufferFiles.clear();
	int j = 0;
	for (const auto& entry : fs::directory_iterator(_path)) {
		if (j == 0) folder.getInfo(entry.path().parent_path()), j++;

		File _file(entry);
		bufferFiles.push_back(_file);
	}
	for (size_t i = 0; i < bufferFiles.size(); ++i) {
		const auto item = bufferFiles[i];
		if (item.isFolder) {
			bufferFiles.insert(bufferFiles.begin(), item);
			bufferFiles.erase(bufferFiles.begin() + i + 1);
		}
	}
	folder.setChilds(bufferFiles);
	//folder.File::refresh();
	return folder;
};

inline size_t ImGui::drawTreeNodes(Folder* folder, bool reload = true) {
	static int selection_mask = (1 << 2);
	size_t node_clicked = -1u;
	ImGui::File item;
	for (int i = 0; i < folder->numberOfFiles(); ++i) {
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		const bool is_selected = (selection_mask & (1 << i)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;
		item = folder->getChilds()[i];
		if (item.isFolder) {
			if (ImGui::TreeNode(item.filename)) {
				//drawTreeNodes(&listThisFolder(item.path));
				ImGui::TreePop();
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				_back.push_back(folder->rawPath);
				folder->load(item.path);
			}
		}
		else if (arraySize(item.filename) > 0) {
			ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, item.filename);
			if (ImGui::IsItemClicked())
				node_clicked = i;
		}
	}
	/*if (node_clicked != -1)
	{
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
			selection_mask = (1 << node_clicked);           // Click to single-select
		return node_clicked;
	}*/
	return node_clicked;
}

inline std::vector<ImGui::File> ImGui::findFileList(ImGui::Folder inputFolder, const char* value) {
	const char* vl = toLower(value);
	std::vector<ImGui::File> temp;
	std::string actual;
	for (size_t i = 0; i < inputFolder.numberOfFiles(); ++i) {
		actual = toLower(inputFolder[i].filename);
		if (actual.find(vl) != std::string::npos)
			temp.push_back(inputFolder[i]);
	}
	return temp;
}

inline bool ImGui::FileIcon(ImTextureID id, const char* title, bool* selected, ImVec2 size = { 85,95 }, ImVec4 tintColor = { 1, 1, 1, 1 }, ImVec4 borderColor = { 0,0,0,0 }) {
	ImGuiStyle& style = ImGui::GetStyle();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec4* colors = style.Colors;
	ImGuiWindow* this_win = ImGui::GetCurrentWindow();
	ImVec2 p = this_win->DC.CursorPos;
	ImRect bb(p.x, p.y, p.x + size.x, p.y + size.y),
		bb2(p.x, p.y, p.x + 16, p.y + 16);
	ImGuiID this_id = this_win->GetID(title),
		check_id = this_win->GetID(title + 1);
	bool hovered = false, held, check_hovered = false, check_held;

	ImGui::ItemAdd(bb2, check_id);
	bool check_clicked = ButtonBehavior(bb2, check_id, &check_hovered, &check_held);
	ImGui::ItemSize(bb, style.FramePadding.y);
	bb.Min.y += 18, bb.Max.y -= 18;
	ImGui::ItemAdd(bb, this_id);
	bb.Min.y -= 18, bb.Max.y += 18;
	bool clicked = ButtonBehavior(bb, this_id, &hovered, &held);

	if (check_clicked)
	{
		*selected = !(*selected);
		MarkItemEdited(check_id);
	}
	else if (clicked)
	{
		*selected = !(*selected);
		MarkItemEdited(this_id);
	}
	//RenderNavHighlight(bb, this_id);
	if (*selected) {
		RenderFrame(p, bb.Max, GetColorU32(hovered || clicked ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]));
		draw_list->AddCircleFilled(ImVec2(bb2.Min.x + 9, bb2.Min.y + 9), 8.f, GetColorU32(check_hovered ? colors[ImGuiCol_ButtonActive] : ImVec4(0, 0, 0, 0)));
		RenderCheckMark(draw_list, ImVec2(bb2.Min.x + 1, bb2.Min.y + 1), GetColorU32(colors[ImGuiCol_ButtonActive]), 16.f);
	}
	else {
		RenderFrame(p, ImVec2(p.x + size.x, p.y + size.y), GetColorU32(hovered ? colors[ImGuiCol_FrameBgHovered] : ImVec4(0, 0, 0, 0)));
		draw_list->AddCircleFilled(ImVec2(bb2.Min.x + 9, bb2.Min.y + 9), 8.f, GetColorU32(check_hovered ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]));
	}
	draw_list->AddImage(id, ImVec2(p.x + 10, p.y + 10), ImVec2(p.x + size.x - 10, p.y + size.y - 20), ImVec2(0, 0), ImVec2(1, 1), GetColorU32(tintColor));
	ImVec2 label_size = CalcTextSize(title, NULL, true);
	RenderTextClipped(ImVec2(p.x + 1, p.y + size.y - 18), ImVec2(p.x + 1 + label_size.x, p.y + size.y - 18 + label_size.y),
		title, NULL, &label_size, ImVec2(0.5, 0.5), &bb);
	return check_clicked ? true : false;
}


inline std::vector<std::string> ImGui::showFilesIcons(ImGui::Folder* folder, std::map<std::string, gl::Texture>& icons, bool* list, char* value) {
	if (value != " ")
		bufferFiles = findFileList(*folder, value);
	std::vector<std::string> filesSelected;
	int previewsRendered = 2;
	ImGui::File item;
	if (bufferFiles.size() > 0)
		for (size_t i = 0; i < bufferFiles.size(); ++i) {
			item = bufferFiles[i];
			_f = "folder";
			if (!item.isFolder) _f = "file";
			if (ImGui::FileIcon((ImTextureID)icons[_f].id, item.filename, &list[i])) {
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
					if (item.isFolder) {
						_back.push_back(folder->rawPath);
						folder->load(item.path);
					}
				}
				ImGui::SetTooltip(item.path);
			}
			if ((ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorStartPos().x - 30) / 90 > previewsRendered) {
				ImGui::SameLine();
				previewsRendered++;
			}
			else previewsRendered = 2;
		}
	return filesSelected;
};

inline std::vector<std::string> ImGui::imFileLoader(const char* Label, bool* handle, ImGui::Folder* folder, std::map<std::string, gl::Texture>& icons) {
	stringSelectedBuffer.clear();
	ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (BeginIsolated(Label, handle))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
		if (ImGui::ImageButton((ImTextureID)icons["left"].id,
			ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImVec2(0, 0), ImVec2(1, 1), 0)
			&& !_back.empty()) {
			_forward.push_back(folder->rawPath);
			folder->load(_back.back().string().c_str());
			_back.pop_back();

		} ImGui::SameLine();
		if (ImGui::ImageButton((ImTextureID)icons["right"].id,
			ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImVec2(0, 0), ImVec2(1, 1), 0)
			&& !_forward.empty()) {
			_back.push_back(folder->rawPath);
			folder->load(_forward.back().string().c_str());
			_forward.pop_back();
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 4));
		ImGui::PopStyleVar();
		if (ImGui::ImageButton((ImTextureID)icons["up"].id,
			ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), ImVec2(0, 0), ImVec2(1, 1), 0)) {
			_back.push_back(folder->rawPath);
			folder->load(folder->rawPath.parent_path().string());
		}

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::BeginChild("0", ImVec2(ImGui::GetContentRegionAvailWidth() - 220, ImGui::GetFrameHeight() + 2), true, ImGuiWindowFlags_NoScrollbar)) {
			fs::path item;
			const char* _fName = "";
			for (size_t i = 0; i < folder->parentsTree.size(); ++i) {
				item = folder->parentsTree[i];
				if (item == folder->parentsTree[0].root_path())
					_fName = item.root_name().string().c_str();
				else _fName = item.filename().string().c_str();
				if (ImGui::Button(_fName)) {
					_back.push_back(folder->rawPath);
					folder->load(item.string().c_str());
				}
				if (ImGui::IsItemHovered()) {
					ImGui::SetTooltip(item.string().c_str());
				}
				ImGui::SameLine();
				if (ImGui::ButtonEx("v", { 16,ImGui::GetFrameHeight() })) {//ImGui::ImageButton((void*)icons["sort-down"].id,
					ImGui::OpenPopup("folder-list");															   //ImVec2(ImGui::GetFrameHeight() - 5, ImGui::GetFrameHeight() - 2), ImVec2(0, 0), ImVec2(1, 1), 1)) {
				} ImGui::SameLine();

			}
			if (ImGui::BeginPopup("folder-list"))
			{
				ImGui::Text("Aquarium");
				ImGui::Separator();
				ImGui::Selectable("Hello!");
				ImGui::EndPopup();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::SameLine();
		if (ImGui::InputTextEx("", "Search", charFindFile, 128, ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetFrameHeight()), ImGuiInputTextFlags_None)) {}
		ImGui::Text(charFindFile);

		ImGui::splitter(true, 8.0f, &fileListWidth, &filePreviewWidth, 8, 8, ImGui::GetContentRegionAvail().y - 30);
		if (ImGui::BeginChild("Filelist", ImVec2(fileListWidth, ImGui::GetContentRegionAvail().y - 30))) {
			//drawTreeNodes(folder);
		}
		ImGui::EndChild();
		filePreviewWidth = ImGui::GetContentRegionAvailWidth() - fileListWidth;
		ImGui::SameLine();
		if (ImGui::BeginChild("Filepreview", ImVec2(filePreviewWidth - 8, ImGui::GetContentRegionAvail().y - 30))) {
			stringSelectedBuffer = showFilesIcons(folder, icons, folderStates, charFindFile);
		}
		ImGui::EndChild();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { *handle = false; }

	}
	EndIsolated();
	return stringSelectedBuffer;
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
