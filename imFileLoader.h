#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace ImGui {
	class File;

	class Folder;

	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

	Folder listThisFolder(const std::string);

	size_t drawTreeNodes(Folder*, bool);

	std::vector<File> findFileList(const Folder, const char*);

	bool FileIcon(ImTextureID, const char*, bool*, ImVec2, ImVec4, ImVec4);

	std::vector<File> showFilesIcons(Folder, std::vector<std::string>, std::vector<ImTextureID>, bool*, char*);

	std::vector<std::string> imFileLoader(Folder*, std::vector<std::string>, std::vector<ImTextureID>);
}

static bool dialogOpened = false;
static bool folderStates[3000] = {};

std::string toLower(std::string);
std::string toUpper(std::string);
template<class T>
size_t find(std::vector<T>, T);


namespace ImGui {
	class File {
	public:
		File(fs::directory_entry _info) { getInfo(_info); }
		File() {}
		std::string filename;
		std::string ext;
		bool isFolder = false;
		std::string parent;
		std::string path;
		uintmax_t size = 0;
		fs::path rawPath;
		bool getInfo(const fs::directory_entry);
		bool refresh();
	};

	class Folder : public File {
	public:
		void refresh();
		bool load(const std::string _path);
		std::vector<File> getChilds() const { return childs; }
		void setChilds(const std::vector<File> _files) { childs = _files; }
		size_t numberOfFiles() const { return childs.size(); }
		File& operator[](size_t idx) { return childs.at(idx); }
	private:
		std::vector<fs::directory_entry> content;
		std::vector<File> childs;
	};

	static ImGui::Folder mainFolder;

	inline bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");
		ImRect bb;
		ImVec2 im = split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1);
		bb.Min = ImVec2(window->DC.CursorPos.x + im.x, window->DC.CursorPos.y + im.y);
		ImVec2 it = CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
		bb.Max = ImVec2(bb.Min.x + it.x, bb.Min.y + it.y);
		return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}

	inline bool File::getInfo(const fs::directory_entry _source) {
		if (!_source.exists()) return false;
		filename = _source.path().filename().string();
		ext = _source.path().filename().extension().string();
		ext = ext.substr(ext.find_first_of(".") + 1);
		isFolder = _source.is_directory();
		parent = _source.path().parent_path().string();
		path = _source.path().string();
		size = _source.file_size();
		rawPath = _source.path();
		return true;
	}

	inline bool File::refresh() {
	}

	inline bool Folder::load(const std::string _path) {
		*this = listThisFolder(_path);
		path = _path;
	}

	inline void Folder::refresh() { *this = listThisFolder(path); }

	inline Folder listThisFolder(const std::string _path) {
		std::vector<File> files;
		for (const auto& entry : fs::directory_iterator(_path)) {
			File _file(entry);
			files.push_back(_file);
		}
		for (size_t i = 0; i < files.size(); ++i) {
			const auto item = files[i];
			if (item.isFolder) {
				files.insert(files.begin(), item);
				files.erase(files.begin() + i + 1);
			}
		}
		Folder folder;
		folder.setChilds(files);
		folder.File::refresh();
		return folder;
	};

	inline size_t drawTreeNodes(Folder* folder, bool reload = true) {
		static int selection_mask = (1 << 2);
		size_t node_clicked = -1;
		for (int i = 0; i < folder->numberOfFiles(); ++i) {
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			const bool is_selected = (selection_mask & (1 << i)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			auto item = folder[i];
			if (item.isFolder) {
				if (ImGui::TreeNode(item.filename.c_str())) {
					drawTreeNodes(&listThisFolder(item.path));
					ImGui::TreePop();
				}
			}
			else {
				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, item.filename.c_str());
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

	inline std::vector<File> findFileList(const Folder inputFolder, const char* value) {
		std::string vl = toLower(value);
		std::vector<File> temp;
		for (size_t i = 0; i < inputFolder.numberOfFiles(); ++i) {
			std::string actual = toLower(inputFolder[i].path);
			if (actual.find(vl) != -1)
				temp.push_back(inputFolder[i]);
		}
		return temp;
	};

	inline bool FileIcon(ImTextureID id, const char* title, bool* selected, ImVec2 size = { 90,100 }, ImVec4 tintColor = { 1, 1, 1, 1 }, ImVec4 borderColor = { 0,0,0,0 }) {
		ImGuiStyle& style = ImGui::GetStyle();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec4* colors = style.Colors;
		ImGuiWindow* this_win = ImGui::GetCurrentWindow();
		ImVec2 p = this_win->DC.CursorPos;
		ImRect bb(p.x, p.y, p.x + size.x, p.y + size.y),
			bb2(p.x, p.y, p.x + 16, p.y + 16);
		ImGuiID this_id = this_win->GetID(title),
			check_id = this_win->GetID(title + 1);
		ImGui::ItemSize(bb, style.FramePadding.y);
		ImGui::ItemAdd(bb2, check_id);
		ImGui::ItemAdd(bb, this_id);
		bool hovered, held, check_hovered, check_held;
		bool clicked = ButtonBehavior(bb, this_id, &hovered, &held);
		bool check_clicked = ButtonBehavior(bb2, check_id, &check_hovered, &check_held);
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
		RenderNavHighlight(bb, this_id);
		if (*selected) {
			RenderFrame(p, ImVec2(p.x + size.x, p.y + size.y), GetColorU32(hovered || clicked ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]));
			draw_list->AddCircleFilled(ImVec2(bb.Min.x + 9, bb.Min.y + 9), 8.f, GetColorU32(colors[ImGuiCol_Button]));
			RenderCheckMark(draw_list, ImVec2(bb.Min.x + 1, bb.Min.y + 1), GetColorU32(colors[ImGuiCol_ButtonActive]), 16.f);
		}
		else {
			RenderFrame(p, ImVec2(p.x + size.x, p.y + size.y), GetColorU32(hovered ? colors[ImGuiCol_FrameBgHovered] : ImVec4(0, 0, 0, 0)));
			draw_list->AddCircleFilled(ImVec2(bb.Min.x + 9, bb.Min.y + 9), 8.f, GetColorU32(colors[ImGuiCol_Button]));
		}
		draw_list->AddImage(id, ImVec2(p.x + 10, p.y + 10), ImVec2(p.x + size.x - 10, p.y + size.y - 20), ImVec2(0, 0), ImVec2(1, 1), GetColorU32(tintColor));
		ImVec2 label_size = CalcTextSize(title, NULL, true);
		RenderTextClipped(ImVec2(p.x + 1, p.y + size.y - 18), ImVec2(p.x + 1 + label_size.x, p.y + size.y - 18 + label_size.y),
			title, NULL, &label_size, ImVec2(0.5, 0.5), &bb);
		return check_clicked ? clicked : false;
	}

	inline std::vector<File> showFilesIcons(Folder folder, std::vector<std::string> iconsNames, std::vector<ImTextureID> iconsID, bool* list, char* value) {
		std::vector<File> files = findFileList(folder, value);
		std::vector<File> filesSelected;
		int previewsRendered = 2;
		if (files.size() > 0)
			for (size_t i = 0; i < files.size(); ++i) {
				File item = files[i];
				size_t j = find(iconsNames, item.ext);
				std::string _f = "folder";
				if (item.isFolder)
					j = find(iconsNames, _f);
				else if (j < 0) _f = "file", j = find(iconsNames, _f);
				ImGui::FileIcon(iconsID[i], item.path.c_str(), &list[i]);
				if (list[i]) filesSelected.push_back(item);
				if (ImGui::IsItemHovered())ImGui::SetTooltip(item.path.c_str());
				if ((ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorStartPos().x - 30) / 90 > previewsRendered) {
					ImGui::SameLine();
					previewsRendered++;
				}
				else previewsRendered = 2;
			}
		return filesSelected;
	};

	std::vector<std::string> imFileLoader(const char* Label, Folder* folder, std::vector<std::string> iconsNames, std::vector<ImTextureID> iconsTextures) {
		if (Begin(Label, &dialogOpened))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
			if (ImGui::ImageButton(iconTextures[find(iconsNames, std::string("left"))],
				ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), { 0,0,0,0 }, { 1,1,1,1 }, 0)) {


			} ImGui::SameLine();
			if (ImGui::ImageButton(iconTextures[find(iconsNames, std::string("right"))],
				ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), { 0,0,0,0 }, { 1,1,1,1 }, 0)) {

			}
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 4));
			ImGui::PopStyleVar();
			if (ImGui::ImageButton(iconTextures[find(iconsNames, std::string("up"))],
				ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), { 0,0,0,0 }, { 1,1,1,1 }, 0)) {


			} ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			if (ImGui::BeginChild("0", ImVec2(ImGui::GetContentRegionAvailWidth() - 220, ImGui::GetFrameHeight() + 2), true, ImGuiWindowFlags_NoScrollbar)) {
				if (ImGui::Button(" C: ")) {

				} ImGui::SameLine();
				if (ImGui::ImageButton(iconTextures[find(iconsNames, std::string("sort-down"))],
					ImVec2(ImGui::GetFrameHeight() - 5, ImGui::GetFrameHeight() - 2), { 0,0,0,0 }, { 1,1,1,1 }, 1)) {
				} ImGui::SameLine();
				ImGui::EndChild();
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::SameLine();
			if (ImGui::InputTextEx("", "Search", buf1, 128, ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetFrameHeight()), ImGuiInputTextFlags_None)) {}
			ImGui::Text(buf1);

			Splitter(true, 8.0f, &fileListWidth, &filePreviewWidth, 8, 8, ImGui::GetContentRegionAvail().y - 30);
			if (ImGui::BeginChild("Filelist", ImVec2(fileListWidth, ImGui::GetContentRegionAvail().y - 30))) {
				drawTreeNodes(folder);
			}
			ImGui::EndChild();
			filePreviewWidth = ImGui::GetContentRegionAvailWidth() - fileListWidth;
			ImGui::SameLine();
			if (ImGui::BeginChild("Filepreview", ImVec2(filePreviewWidth - 8, ImGui::GetContentRegionAvail().y - 30))) {
				showFilesIcons(folder, iconsNames, iconsTextures, )
					ImGui::EndChild();
				if (ImGui::Button("Cancel", ImVec2(120, 0))) { selectFile = false; }
				End();
			}
		}
	}

	inline std::string toLower(std::string value) {
		std::for_each(value.begin(), value.end(), [](char& c) { c = ::tolower(c); });
		return value;
	};

	inline std::string toUpper(std::string value) {
		std::for_each(value.begin(), value.end(), [](char& c) { c = ::toupper(c); });
		return value;
	};

	template<class T>
	size_t find(std::vector<T> _vec, T _value) {
		for (size_t i = 0; i < _vec.size(), ++i) {
			if (_vec[i] == _value) return i;
		}
		return -1;
	}

