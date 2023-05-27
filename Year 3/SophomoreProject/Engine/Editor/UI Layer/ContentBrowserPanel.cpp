// /******************************************************************************/
// /*!
// \file         ContentBrowserPanel.cpp
// \project name Engine
// \author(s)    Wilfred Ng Jun Hwee 
// \date         12th Nov, 2021 <Date of last SIGNIFICANT CHANGE>
// \brief        This file contains the function definitions for the content browser panel
// \lines        123 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
#include "UiLayer.h"
#include <filesystem>//file system 
#include "../../AssetManager/AssetManager.h"//asset manager 
#include "../../Graphics/Source/texture.h"//texture 
#include "../../WinMgr/WinMgr.h"//window manager 
#include "../../Sound/SoundManager.h"
#pragma warning(disable : 4312)
#pragma warning(disable : 4244)

void UiLayer::LoadConsole(bool* p_open) {
    static_cast<void>(p_open);//unreferenced variable 
    ImGui::Begin("Content Browser");

    if(DropManager::GetInstance().isDragging){
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern))	// we use an external source (i.e. not ImGui-created)
        {
            // replace "FILES" with whatever identifier you want - possibly dependant upon what type of files are being dragged
            // you can specify a payload here with parameter 2 and the sizeof(parameter) for parameter 3.
            // I store the payload within a vector of strings within the application itself so don't need it.
            std::wstring wstr(DropManager::GetInstance().szFile);
            ImGui::SetDragDropPayload("FILES", DropManager::GetInstance().szFile, (wcslen(DropManager::GetInstance().szFile) + 1)* sizeof(wchar_t));
            ImGui::BeginTooltip();
            ImGui::Text("FILES");
            ImGui::EndTooltip();
            ImGui::EndDragDropSource();
        }
    }

    //if the current file directory is not the starting directory 
    if (AssetManager::GetInstance().GetCurrentFileDirectory() != AssetManager::GetInstance().m_StartingFileDirectory) {
        if (ImGui::Button("Back")) {
            AssetManager::GetInstance().SetParentFileDirectory(AssetManager::GetInstance().GetCurrentFileDirectory().parent_path());
        }
    }

    static float padding = 16.0f;//padding of thumbnail
    static float thumbnailSize = 128.f;//size of thumbnail
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;//getting panel width 
    int columnCount = static_cast<int>(panelWidth / cellSize);//getting the number of columns 
    if (columnCount < 1) {
        columnCount = 1;//setting the minimum number of columns 
    }
    if (columnCount > 100) //Set a hard limit because ImGui::GetContentRegionAvail() might bug out when closing the editor
        columnCount = 100;
    ImGui::Columns(columnCount,0,false);//set the columns to 5, no border 

  

    //loop throught the assets directory 
    for (const auto& directoryEntry : std::filesystem::directory_iterator(AssetManager::GetInstance().GetCurrentFileDirectory())) {
        std::string path = directoryEntry.path().string();//getting the path of the assets

        ImGui::PushID(path.c_str());//pushing the id to differentiate the files 

        //if it is a directory 
        if (directoryEntry.is_directory()) {
            //make directory icon 
            ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find("folder.png")->second->GetRendererID()), {thumbnailSize,thumbnailSize}, {0,1}, {1,0});//setting the thumbnail size button, with the uv coords 
        }
        else {
            //if it's a texture that is loaded, show the texture 
            if (directoryEntry.path().parent_path().string().find("CurrentTexture") != std::string::npos && directoryEntry.path().string().find(".png") != std::string::npos) {
                //show the texture 
                std::string loadedTextureName = directoryEntry.path().string();
                loadedTextureName = loadedTextureName.substr(loadedTextureName.find_last_of("/\\"));
                loadedTextureName = loadedTextureName.substr(loadedTextureName.find_first_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));
                //check if the asset is already loaded
                if (AssetManager::GetInstance().textureMap.find(loadedTextureName) != AssetManager::GetInstance().textureMap.end()) {
                    ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().textureMap.find(loadedTextureName)->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
                }
                else {
                    //make file icon 
                    ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find("file.png")->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
                }
            }
            else if (directoryEntry.path().parent_path().string().find("Icons") != std::string::npos) {
                //show the texture 
                std::string loadedTextureName = directoryEntry.path().string();
                loadedTextureName = loadedTextureName.substr(loadedTextureName.find_last_of("/\\"));
                loadedTextureName = loadedTextureName.substr(loadedTextureName.find_first_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));
                //check if the asset is already loaded
                if (AssetManager::GetInstance().iconTextureMap.find(loadedTextureName) != AssetManager::GetInstance().iconTextureMap.end()) {
                    ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find(loadedTextureName)->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
                }
                else {
                    //make file icon 
                    ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find("file.png")->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
                }
            }
            else if (directoryEntry.path().parent_path().string().find("Fonts") != std::string::npos) {
                ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find("fonts.png")->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
            }
            else if (directoryEntry.path().parent_path().string().find("Sounds") != std::string::npos) {
                ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find("sounds.png")->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
            }
            else {
                //make file icon 
                ImGui::ImageButton((ImTextureID)(AssetManager::GetInstance().iconTextureMap.find("file.png")->second->GetRendererID()), { thumbnailSize,thumbnailSize }, { 0,1 }, { 1,0 });//setting the thumbnail size button, with the uv coords 
            }

           
        }
       // int counter = 0;
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("FILES"))  // or: const ImGuiPayload* payload = ... if you sent a payload in the block above
            {
                if(DropManager::GetInstance().isDropped){
                    const wchar_t* pathInner = (const wchar_t*)payLoad->Data;

                    
                    std::wstring wStr(pathInner);//converting to wide string 
                    std::string sourcePath(wStr.begin(), wStr.end());
                    std::string sourceFinalPath = sourcePath;//putting in the final path 
                    sourcePath = sourcePath.substr(sourcePath.find_last_of("/\\"));
                    sourcePath = sourcePath.substr(sourcePath.find_first_of("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"));
                    std::string target = directoryEntry.path().parent_path().string() + "\\" + sourcePath;
                    dropManager.copyLastDroppedFile(target.c_str(), sourceFinalPath.c_str());//copy function 

                    //if the file being dropped is a png, reload the textures 
                    if (target.find(".png") != std::string::npos) {
                        AssetManager::GetInstance().LoadSprites();//reload the textures 
                    }
                   /* if (target.find(".wav") != std::string::npos) {
                        SoundClass::getInstance()->CreateSound(target.c_str(), false);
                        
                        SoundClass::getInstance()->CreateChannel(counter, false);
                        counter++;
                    }
                    if (target.find(".mp3") != std::string::npos) {
                        SoundClass::getInstance()->CreateSound(target.c_str(), false);
                       
                        SoundClass::getInstance()->CreateChannel(counter, false);
                        counter++;
                    }
                    if (target.find(".ogg") != std::string::npos) {
                        SoundClass::getInstance()->CreateSound(target.c_str(), false);
                        
                        SoundClass::getInstance()->CreateChannel(counter, false);
                        counter++;
                    }*/
                    if (target.find(".aac") != std::string::npos) {
                        //error msg
                        Log logger;
                        logger.Error(true,"Saved however - Incorrect file type, Please use .ogg , .mp3 or .wav");
                    } 
                    if (target.find(".wma") != std::string::npos) {
                        //error msg
                        Log logger;
                        logger.Error(true, "Saved however - Incorrect file type, Please use .ogg , .mp3 or .wav");
                    } 
                    if (target.find(".flac") != std::string::npos) {
                        //error msg
                        Log logger;
                        logger.Error(true, "Saved however - Incorrect file type, Please use .ogg , .mp3 or .wav");
                    }
                    if (target.find(".m4a") != std::string::npos) {
                        //error msg
                        Log logger;
                        logger.Error(true, "Saved however - Incorrect file type, Please use .ogg , .mp3 or .wav");
                    }

                    DropManager::GetInstance().isDropped = false;
                    DropManager::GetInstance().isDragging = false;
                }
            }
            ImGui::EndDragDropTarget();
        }


        //DRAG & DROP
        if (ImGui::BeginDragDropSource()) {
            const wchar_t* itemPath = directoryEntry.path().c_str();//getting the relative path 
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath,(wcslen(itemPath) + 1)* sizeof(wchar_t));//setting the drag and drop pay load 
            ImGui::EndDragDropSource();//end 
        }


        //if mouse if hovered and is double clicked 
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (directoryEntry.is_directory()) {
                AssetManager::GetInstance().SetCurrentFileDirectory(directoryEntry.path().filename());//set the current directory to the inner directory 
            }
        }


        static bool deletingFile = false;
        static std::string fileToDelete;
        //if mouse if hovered and is clicked with mouse right click
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            //check if the file is a regular file and not a directory 
            if (directoryEntry.is_regular_file()) {
                fileToDelete = directoryEntry.path().string();
                deletingFile = true;
            }
        }

        ImGui::TextWrapped(path.c_str());//setting the wrapped text 

        if (fileToDelete == directoryEntry.path().string()) {
            if (deletingFile == true) {
                //confirmation message 
                ImGui::Text("Confirm Delete?");
                if (ImGui::Button("Yes")) {
                    DropManager::deleteFile(fileToDelete);//deleting the file 
                    fileToDelete = "";
                }
                ImGui::SameLine();
                if (ImGui::Button("No")) {
                    fileToDelete = "";//reseting the file to delete 
                }
            }
        }


        ImGui::NextColumn();//set next column

        ImGui::PopID();//popping the id to differentiate the files 

    }
    ImGui::Columns(1);//set it back to 1 
    ImGui::End();
}
