// /******************************************************************************/
// /*!
// \file         UIAnimationEditor.cpp
// \project name Engine
// \author(s)    Wilfred
// \date         22nd January, 2022
// \brief        Animation Editor for sprite sheets
// \lines        
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"
#include "../../ECS/ECS.h"
#include "../../InputMgr/InputMgr.h"//keymanager
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Serializer/Settings/AnimationDataSerializer.h"//animation serializer 

//If file name ends with (No C++20)
bool stringEndsWithAnimation(std::string const& toCheck, std::string const& endsWith)
{
	size_t checkedLength = toCheck.length();
	size_t endLength = endsWith.length();

	return !(toCheck.substr(checkedLength - endLength, endLength).compare(endsWith));
}

//Assess the status of the current directory
void assessCurrentDirectoryAnimation(std::filesystem::path const& path)
{
	size_t totalSize{ 0 };
	size_t fileCounter{ 0 };
	for (auto const& dir_entry : std::filesystem::directory_iterator{ path }) {
		++fileCounter;
		static_cast<void>(dir_entry);
	}

	for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ path }) {
		totalSize += std::filesystem::file_size(dir_entry);
		static_cast<void>(dir_entry);
	}

	ImGui::Separator();
	ImGui::Text("%d total item(s), occupying %d bytes", fileCounter, totalSize);
	ImGui::Text("%d bytes available", std::filesystem::space(path).available);


}

//Get time of file
void fileTimeAnimation(std::filesystem::path const& path)
{
	//https://stackoverflow.com/a/61067330 (RIP NO C++20 IN THIS VS)
	auto fileTime = std::filesystem::last_write_time(path); //Get the file time
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()); //Turn it into correct type of time point
	std::time_t fileTime_t = std::chrono::system_clock::to_time_t(sctp); //Convert into time_t
	char timeString[26];
	struct tm lt; //Readable time_t
	localtime_s(&lt, &fileTime_t); //Convert fileTime_t into readable format at put results in lt
	asctime_s(timeString, 26, &lt); //Prints out contents of struct tm lt into a C string format
	ImGui::Text("%s", timeString); //Finally put the text in ImGui
}

/**************

@Brief - Calculating the uv frame points of the frame based off the animation sprite sheet

***************/
std::array<Mathf::Vector2, 4> CalculateFrameUV(transform2D& spriteSheet, transform2D& frame) {

	Mathf::Vector2 topRight, topLeft, bottomRight, bottomLeft;//for storing the various uv frame points
	Mathf::Vector2 topRightFrame, topLeftFrame, bottomRightFrame, bottomLeftFrame;//storing of the frame points

	//getting the frame points in the world for spritesheet
	topRight = { spriteSheet.pos.x + spriteSheet.scale.x / 2.f , spriteSheet.pos.y + spriteSheet.scale.y /2.f };
	topLeft = { spriteSheet.pos.x - spriteSheet.scale.x / 2.f , spriteSheet.pos.y + spriteSheet.scale.y / 2.f };
	bottomRight = { spriteSheet.pos.x + spriteSheet.scale.x / 2.f , spriteSheet.pos.y - spriteSheet.scale.y / 2.f };
	bottomLeft = { spriteSheet.pos.x - spriteSheet.scale.x / 2.f ,spriteSheet.pos.y - spriteSheet.scale.y / 2.f };

	//getting the frame points in the world for the frame
	topRightFrame = { frame.pos.x + frame.scale.x / 2.f , frame.pos.y + frame.scale.y / 2.f };
	topLeftFrame = { frame.pos.x - frame.scale.x / 2.f , frame.pos.y + frame.scale.y / 2.f };
	bottomRightFrame = { frame.pos.x + frame.scale.x / 2.f , frame.pos.y - frame.scale.y / 2.f };
	bottomLeftFrame = { frame.pos.x - frame.scale.x / 2.f ,frame.pos.y - frame.scale.y / 2.f };
	
	
	Mathf::Vector2 topRightFrameResult, topLeftFrameResult, bottomRightFrameResult, bottomLeftFrameResult;//the resulting uv frame points

	//converting matrix 
	Mathf::Matrix3 convertingMatrix = { 1 / spriteSheet.scale.x ,0,0,0,1 / spriteSheet.scale.y,0,-spriteSheet.pos.x,-spriteSheet.pos.y,1.0 };


	topRightFrameResult = convertingMatrix * Mathf::vec3(topRightFrame, 1.f);
	topLeftFrameResult = convertingMatrix * Mathf::vec3(topLeftFrame, 1.f);
	bottomRightFrameResult = convertingMatrix * Mathf::vec3(bottomRightFrame, 1.f);
	bottomLeftFrameResult = convertingMatrix * Mathf::vec3(bottomLeftFrame, 1.f);


	return  { bottomLeftFrameResult +0.5f, bottomRightFrameResult +0.5f, topRightFrameResult +0.5f, topLeftFrameResult +0.5f} ;//returning the frame points
}


void UiLayer::ShowAnimationEditorPanel(bool* p_open) {
	UNREFERENCED_PARAMETER(p_open);

	ImGui::Begin("Animation Editor");
	static std::string selectedAnimationFile;//tracks the selected animation file
	static std::string spriteSheetID;//tracks the selected sprite sheet
	static entityID entitySpriteSheet;//tracks the entity id of the sprite sheet 


	//if the animation panel is first toggle,
	if (UiLayer::GetInstance().animationPanelFirstToggle == true) {
		//create the sprite sheet 
		entitySpriteSheet = ecs.createEntityFromFile
		("Resources/PresetEntities/animationSpriteSheet.json");
		ecs.getComponent<transform2D>(entitySpriteSheet).scale.x = 1000;//set default scale to 1000
		ecs.getComponent<transform2D>(entitySpriteSheet).scale.y = 1000;//set default scale to 1000
		ecs.renameEntity(entitySpriteSheet, "Sprite Sheet");//renaming the entity
		//shifting the object to the newly created entity
		UiLayer::GetInstance().animationPanelFirstToggle = false;//reset 
	}

	//if save new animation is opened 
	if (UiLayer::GetInstance().saveNewAnimationOpen == true)
	{
		UiLayer::GetInstance().ShowSaveAnimationDialog(&(UiLayer::GetInstance().saveNewAnimationOpen));
	}


	//new or old animation json, select the animation json file 
	//animation json text 
	ImGui::Text("Animation File: %s", selectedAnimationFile.c_str());
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("The file location of the Animation");
		ImGui::EndTooltip();
	}

	//button for adding new animation file 
	if (ImGui::Button("Add New##Editor")) {
		//creating the new file with the selected name 
		ImGui::SetWindowPos("Save Animation", ImVec2{ WindowsManager.getWidthF() / 4.0f, WindowsManager.getHeightF() / 4.0f });
		ImGui::SetWindowSize("Save Animation", ImVec2{ WindowsManager.getWidthF() / 2.0f, WindowsManager.getHeightF() / 2.0f });
		saveNewAnimationOpen = true;

	}
	ImGui::SameLine();
	//button for changing the animation file 
	if (ImGui::Button("Change Animation File##Editor"))
		ImGui::OpenPopup("Animation File Selector");
	if (ImGui::BeginPopup("Animation File Selector"))
	{
		for (auto& sha : AssetManager::GetInstance().animationLoaded)
		{
			if (ImGui::Selectable(sha.c_str(), false))
			{
				selectedAnimationFile = sha;
			}
		}
		ImGui::EndPopup();
	}

	//selecting sprite sheet 
	//Get the component's data
	auto& spriteSheetEntity = ecs.getComponent<spriteCom>(entitySpriteSheet);
	auto& spriteSheetEntityTransform = ecs.getComponent<transform2D>(entitySpriteSheet);

	//Texture Path
	ImGui::Text("Sprite Sheet File: %s", spriteSheetEntity.textureId.c_str());
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("The file location of the sprite sheet");
		ImGui::EndTooltip();
	}

	//button for changing the animation sprite
	if (ImGui::Button("Change Sprite Sheet##texture")) {
		ImGui::OpenPopup("Sprite Sheet Selector");
	}
	if (ImGui::BeginPopup("Sprite Sheet Selector"))
	{
		for (auto& tex : AssetManager::GetInstance().textureMap)
			if (ImGui::Selectable(tex.first.c_str(), false))
				spriteSheetEntity.textureId = tex.first;
		ImGui::EndPopup();
	}

	//adding frames do in the scene editor 

	//adding actions 
	static std::map<std::string, std::vector<size_t>> actions;//map to store the actions
	static std::string selectedAction; //indicates the selected action 
	ImGui::Text("Selected Action : %s", selectedAction.c_str());
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("The Frame Group");
		ImGui::EndTooltip();
	}
	ImGui::SameLine();
	//adding a new action 
	if (ImGui::Button("Attach Frame##Selector")) {
		ImGui::OpenPopup("Attach Frame Selector");
	}
	if (ImGui::BeginPopup("Attach Frame Selector"))
	{
		for (auto& entities : ecs.getEntityNames()) {
			if (entities.second.find("Frame") != std::string::npos) {
				if (ImGui::Selectable(entities.second.c_str(), false)) {
					//getting the number after the frame 
					int number = stoi(entities.second.substr(entities.second.find_first_of("01234567890"),entities.second.size()));

					//check if the selected action is within the map 
					if (actions.find(selectedAction) != actions.end()) {
						//if it's selected, you add into the actions
						actions.find(selectedAction)->second.emplace_back(number);
					}
				}
			}
		}
		ImGui::EndPopup();
	}


	//button for changing the animation sprite
	if (ImGui::Button("Select Actions##Selector")) {
		ImGui::OpenPopup("Action Selector");
	}
	if (ImGui::BeginPopup("Action Selector"))
	{
		for (auto& actionsScope : actions) {
			if (ImGui::Selectable(actionsScope.first.c_str(), false)) {
				selectedAction = actionsScope.first;//replace the selected action 
			}
		}
		ImGui::EndPopup();
	}
	static char newActionName[256];
	ImGui::InputText("##nameinput", newActionName, 256);
	ImGui::SameLine();
	//adding a new action 
	if (ImGui::Button("Add New Action##Editor")) {
		//creates a new action with the name 
		actions.emplace(std::make_pair<std::string, std::vector<size_t>>(newActionName, {}));
		memset(newActionName, 0, sizeof(newActionName));//reset 
	}

	//display all the columns and rows of actions with frames 
	ImGui::Columns(2, "Actionns Table");
	ImGui::Separator();
	ImGui::Text("Actions"); ImGui::NextColumn();
	ImGui::Text("Frames Attached"); ImGui::NextColumn();
	ImGui::Separator();

	//loop through all the actions
	for (auto actionMap : actions) {
		//ACTIONS
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		//action name
		std::string actionMapName = actionMap.first;
		ImGui::Text("%s", actionMapName.c_str());//showing the action name 
		ImGui::NextColumn();
		//FRAMES ATTACHED
		std::string framesAttached;

		//getting all the frames attached 
		for (size_t frameNumber : actionMap.second) {
			framesAttached = (" | " + std::to_string(frameNumber));
			ImGui::Text("%s", framesAttached.c_str());
			ImGui::SameLine();
		}
		//ImGui::Text("%s", framesAttached);
		ImGui::NextColumn();
		ImGui::PopStyleColor();//popping
	}

	//serialize
	//convert all the frames to uv coords & serialize them 
	if (ImGui::Button("Save Animation##Selector")) {
		//temp containers to store all the data 
		std::vector<std::array<Mathf::Vector2, 4>> tempFrameContainer;//storing of all the frames 
		//std::unordered_map<std::string, std::vector<size_t>> tempActionsContainer;//storing of all the actions and frame number

		/**
		Serializing the frames 
		**/
		
		//looping through the frames
		for (auto frames : ecs.getEntityNames()) {
			//if the entity has the name frame in it
			if (frames.second.find("Frame") != std::string::npos) {
				//calculate the UV frame points , TODO : CHECK FOR THE BOUNDARY & DO NOT INCLUDE FRAME POINTS NOT WITHIN THE SPRITESHEET 				
				tempFrameContainer.emplace_back(CalculateFrameUV(spriteSheetEntityTransform, ecs.getComponent<transform2D>(frames.first)));//emplace the uv frame points to the container
			}
		}

		//serializing into the animation serializer 
		SerializeManager sm;
		AnimationSerializer as;
		Log logger;
		
		std::copy(std::begin(tempFrameContainer), std::end(tempFrameContainer), std::back_inserter(as.uvFramePoints));//inserting into the uv framepoints container
		as.aniData.insert(actions.begin(), actions.end());//coping the map data in
		//sm.SerializeToFile("Resources/Animations/NewPlayerAnimationOutput2.json", as);
		selectedAnimationFile = "Resources/Animations/" + selectedAnimationFile;
		sm.SerializeToFile(selectedAnimationFile, as);
		logger.LogToConsole("Animation Saved!");//logging 
	}

	ImGui::End();
}



//TO SAVE A LEVEL
void UiLayer::ShowSaveAnimationDialog(bool* p_open)
{
	static std::filesystem::path p{ "Resources/Animations" };
	ImGui::Begin("Save Animation", p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking);
	ImGui::Text("Current Path: %s", p.string().c_str());
	if (p.has_parent_path())
		if (ImGui::Button("Up One Level"))
			p = p.parent_path(); //Go to parent file
	ImGui::Separator();
	ImGui::TextColored(ImVec4{ 0.5f, 1.0f, 0.5f, 1.0f }, "Save as NEW in here (May overwrite)");
	static char levelName[256];
	ImGui::InputText(".json##FileSaveInput", levelName, 256);
	if (ImGui::Button("Save as new"))
	{
		std::string targetFileName{ p.string() };
		targetFileName += "/";
		targetFileName += levelName;
		targetFileName += ".json";
		//SAVE DONE, REVERT IMGUI STATE AND CLOSE
		ecs.saveLevel(targetFileName);
		ImGui::End();
		*p_open = false;
		AssetManager::GetInstance().LoadAnimations();//load the animations
		return;
	}

	ImGui::Separator();
	ImGui::TextColored(ImVec4{ 1.0f, 0.5f, 0.5f, 1.0f }, "OR OVERWRITE an existing \"*Level.json\" file");

	ImGui::Columns(3, "Save Level Columns");
	ImGui::Separator();
	ImGui::Text("Filename"); ImGui::NextColumn();
	ImGui::Text("Size (Bytes)"); ImGui::NextColumn();
	ImGui::Text("Last Modified"); ImGui::NextColumn();
	ImGui::Separator();
	for (auto const& dir_entry : std::filesystem::directory_iterator{ p })
	{
		//Directories
		if (dir_entry.is_directory())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//NAME
			std::string directoryText = "<DIR> " + dir_entry.path().filename().string();
			if (ImGui::Selectable(directoryText.c_str(), false))
				p = dir_entry.path(); //Go to child directory
			ImGui::NextColumn();
			//SIZE
			size_t totalSize{ 0 };
			for (auto const& subEntry : std::filesystem::recursive_directory_iterator{ dir_entry.path() })
				totalSize += std::filesystem::file_size(subEntry);
			ImGui::Text("%d", totalSize);
			ImGui::NextColumn();
		}
		//Relevant files
		else if (stringEndsWithAnimation(dir_entry.path().string(), ".json"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
			//FILE NAME (*Level.json)
			std::string overwritePopupString = "ConfirmOverwrite" + dir_entry.path().string();
			std::string overwritePopupSelectableString = "OK##" + overwritePopupString;
			if (ImGui::Selectable(dir_entry.path().filename().string().c_str(), false))
				ImGui::OpenPopup(overwritePopupString.c_str());
			if (ImGui::BeginPopup(overwritePopupString.c_str()))
			{
				ImGui::Text("Confirm Overwrite?");
				ImGui::Separator();
				if (ImGui::Selectable(overwritePopupSelectableString.c_str()))
				{
					//OVERWRITE DONE, REVERT IMGUI STATE AND CLOSE
					ecs.saveLevel(dir_entry.path().string());
					ImGui::EndPopup();
					ImGui::PopStyleColor();
					ImGui::Columns(1);
					ImGui::End();
					*p_open = false;
					return;
				}
				ImGui::EndPopup();
			}
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		else //Irrelevant file
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
			//FILE NAME (*Level.json)
			ImGui::Text("%s", dir_entry.path().filename().string().c_str());
			ImGui::NextColumn();
			//SIZE
			ImGui::Text("%d", std::filesystem::file_size(dir_entry.path()));
			ImGui::NextColumn();
		}
		//TIME MODIFIED
		fileTimeAnimation(dir_entry.path());
		ImGui::NextColumn();
		ImGui::PopStyleColor();
	}

	ImGui::Columns(1); //Reset columning
	ImGui::Separator();
	assessCurrentDirectoryAnimation(p);
	ImGui::End();
}