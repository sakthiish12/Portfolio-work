#include "UiLayer.h"

#include "../../ECS/ECS.h"
#include "../../Sound/SoundManager.h"
#include"../../Debugging/Log.h"
Log logger;
void UiLayer::DrawSoundPanel(bool* p_open)
{
	UNREFERENCED_PARAMETER(p_open);
	ImGui::Begin("Sounds");
	


	if (ImGui::Button("Play Background"))
	{

	}

	//if(ImGui::ListBox)
	//	std::cout << SoundClass::getInstance()->sounds[i] << std::endl;
	//	//logger.Info(true,"Sound loaded");
	//for (int i = 0; i < SoundClass::getInstance()->sounds.size(); i++)
	//{
	//	std::cout << SoundClass::getInstance()->sounds.at(i) << ' ';
	//}
	//bool VectorOfStringGetter(void* data, int n, const char** out_text)
	//{
	//	const vector<string>* v = (vector<string>*)data;
	//	*out_text = v[n].c_str();
	//	return true;
	//}
	//ImGui::ListBox("Symbols", listbox_item_current, VectorOfStringGetter(, (void*)&symbols, (int)symbols.size());*/

	ImGui::End();
	//ImGui::Text("Texture File: %s");
	//if (ImGui::IsItemHovered())
	//{
	//	ImGui::BeginTooltip();
	//	ImGui::Text("The file location of the texture");
	//	ImGui::EndTooltip();
	//}
	//if (ImGui::Button("Play Background Sound"))
	//{
	//	SoundClass::getInstance()->CreateSound("BackgroundSound.mp3", true);
	//	int bg = SoundClass::getInstance()->CreateChannel(0, false);
	//	SoundClass::getInstance()->setVolume(bg, 0.3);
	//	float pos[] = { 0,0,0 };
	//	SoundClass::getInstance()->setArgsAndPlay(bg, pos);
	//}
	//
	//ImGui::PopItemWidth();
	//ImGui::End();
	


}