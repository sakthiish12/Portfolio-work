#include "pch.h"
#include "BananaSerialisation.h"
#include <Maths/Quaternion.h>
#include <Maths/Vector3D.h>
#include <Debugger/StopWatch.h>
#include <Systems/RendererSystem/RendererSystem.h>
#include "Systems/UIManager/UIManager.h"
static const std::string S_EntityID = "EntityID";
using namespace rapidjson;
using namespace rttr;
namespace BE
{
	namespace Serialisation
	{
		inline void LoadArray(const rapidjson::Value& arrayValue, variant& variantArray)
		{
			if (!arrayValue.IsArray())
			{
				return;
			}
			auto sequentialView = variantArray.create_sequential_view();
			const auto& currentArrayType = sequentialView.get_value_type();
			if (sequentialView.is_dynamic())
				sequentialView.set_size(arrayValue.GetArray().Capacity());
			rttr::type arrType = currentArrayType.is_wrapper() ? currentArrayType.get_wrapped_type() : currentArrayType;
			uint32_t counter = 0;
			for (const auto& arrayMember : arrayValue.GetArray())
			{
				if (arrType.is_arithmetic())
					sequentialView.set_value(counter, LoadArimethicTypes(arrayMember, arrType));
				else if (arrType == type::get<std::string>())
				{
					//does this work? im not sure
					std::string tempStr = arrayMember.GetString();
					rttr::variant temp = rttr::variant(tempStr);
					sequentialView.set_value(counter, temp);
				}
				else if (arrType.is_sequential_container())
				{
					rttr::variant temp = sequentialView.get_value(counter).extract_wrapped_value();
					LoadArray(arrayMember, temp);
					sequentialView.set_value(counter, temp);
				}
				else if (arrType.get_properties().size() != 0)
				{
					rttr::variant temp = sequentialView.get_value(counter).extract_wrapped_value();
					LoadObjectRecur(arrayMember, arrType, BEID(0), temp);
					sequentialView.set_value(counter, temp);
				}
				//need to add array of arrays
				counter++;
			}
		}
		void SerializeToValueBase(const rttr::instance& inst, rapidjson::Value& v, rapidjson::Document& d);
		rttr::variant LoadArimethicTypes(const rapidjson::Value& curr, const rttr::type& givenType)
		{
			if (givenType == rttr::type::get<bool>())
			{
				return rttr::variant(curr.GetBool());
			}
			else if (givenType == rttr::type::get<float>())
			{
				return rttr::variant(curr.GetFloat());
			}
			else if (givenType == rttr::type::get<double>())
			{
				return rttr::variant(curr.GetDouble());
			}
			else if (givenType == rttr::type::get<int8_t>()
				|| givenType == rttr::type::get<int16_t>()
				|| givenType == rttr::type::get<int32_t>())
			{
				return rttr::variant(curr.GetInt());
			}
			else if (givenType == rttr::type::get<uint8_t>()
				|| givenType == rttr::type::get<uint16_t>()
				|| givenType == rttr::type::get<uint32_t>())
			{
				return rttr::variant(curr.GetUint());
			}
			else if (givenType == rttr::type::get<uint64_t>())
			{
				return rttr::variant(curr.GetUint64());
			}
			else if (givenType == rttr::type::get<int64_t>())
			{
				return rttr::variant(curr.GetInt64());
			}
			else
				AssertCore(false, "Arthmetic type encountered that is not currently implemented named %s",givenType.get_name().to_string().c_str());
			return rttr::variant();
		}
		rttr::variant LoadStringValue (const rapidjson::Value& curr)
		{
			return rttr::variant(curr.GetString());
		}
		inline void LoadObjectRecur(const rapidjson::Value& curr, const rttr::type& componentType, const BEID& id, const rttr::instance& inst)
		{
			//std::cout << inst.get_type().get_name() << "\n";
			for (const auto& prop : componentType.get_properties())
			{
				if (prop.get_metadata(MetaDataKey::Serialize).is_valid()
					&& prop.get_metadata(MetaDataKey::Serialize) == false)
				{
					continue;
				}
				const std::string& propName = prop.get_name().to_string();
				if (!curr.HasMember(propName.c_str()))
				{
					LogCoreError("Loading Entity id %u, component %s , did not contain member %s", *id, componentType.get_name().to_string().c_str(), propName.c_str());
				}
				else
				{
					const rttr::type& memberType = prop.get_type();
					if (memberType.is_arithmetic())
					{
						prop.set_value(inst, LoadArimethicTypes(curr[propName.c_str()], memberType));
					}
					else if (memberType.is_sequential_container())
					{
						rttr::variant variantArray = prop.get_value(inst);
						LoadArray(curr[propName.c_str()], variantArray);
						prop.set_value(inst, variantArray);
					}
					else if (memberType.is_associative_container())
					{
						const rapidjson::Value& arrayValue = curr[propName.c_str()];
						if (!arrayValue.IsArray())
						{
							LogCoreError("Property is array but value is not an array when loading entity %ull when loading component %s with property %s", *id, componentType.get_name().to_string().c_str(), prop.get_name().to_string().c_str());
							continue;
						}
						auto variantMap = prop.get_value(inst);
						auto associativeView = variantMap.create_associative_view();
						const rttr::type& keyType = associativeView.get_key_type().is_wrapper() ? associativeView.get_key_type().get_wrapped_type() : associativeView.get_key_type();
						const rttr::type& valueType = associativeView.get_value_type().is_wrapper() ? associativeView.get_value_type().get_wrapped_type() : associativeView.get_value_type();
						uint32_t counter = 0;
						for (const auto& arrayMember : arrayValue.GetArray())
						{
							variant key, value;
							const Value& keyValue = arrayMember["Key"];
							const Value& valueValue = arrayMember["Value"];
							if (keyType.is_arithmetic())
								key = LoadArimethicTypes(keyValue, keyType);
							else if (keyType == type::get<std::string>())
							{
								//does this work? im not sure
								key = rttr::variant(std::string(keyValue.GetString()));
							}
							else if (keyType.get_properties().size() != 0)
							{
								key = keyType.create();
								//key.co
								LoadObjectRecur(keyValue, keyType, id, key);
							}
							else
							{
								AssertCore(false, "This key type is not supported %s", keyType.get_name().to_string());
							}
							if (valueType.is_arithmetic())
								value = LoadArimethicTypes(valueValue, valueType);
							else if (valueType == type::get<std::string>())
							{
								//does this work? im not sure
								value = rttr::variant(std::string(valueValue.GetString()));
							}
							else if (valueType == type::get<IScript*>() && keyType == type::get<std::string>())
							{
								//Create a shared pointer the the correct type
								value = rttr::type::get_by_name(keyValue.GetString()).create();
								std::string temp(keyValue.GetString());
								if (!value.is_valid())
								{
									LogError("Cannot create script of type %s", temp.c_str());
									continue;
								}
								//Some dangerous casting to get it to cast to the correct type for insertion into the map
								std::shared_ptr<IScript> temp2 = rttr::variant_cast<std::shared_ptr<IScript>>(value);
								//Copy the default value, only set the value after inserting into the map
								value = temp2;
							}
							else if (valueType.get_properties().size() != 0)
							{
								value = valueType.create();
								LoadObjectRecur(valueValue, valueType, id, value);
							}
							else
							{
								AssertCore(false, "This value type is not supported %s", valueType.get_name().to_string());
							}
							//need to add array of arrays
							counter++;
							auto tempItr = associativeView.insert(key, value);
							//If its a script, set the value here
							if (valueType == type::get<IScript*>() && keyType == type::get<std::string>())
							{
								auto viewResult = associativeView.equal_range(std::string(keyValue.GetString()));
								if (viewResult.first != associativeView.end())
								{
									std::shared_ptr<IScript> ptrrr2 = viewResult.first.get_value().get_wrapped_value<std::shared_ptr<IScript>>(); 
									const rttr::type scriptName = rttr::type::get_by_name(keyValue.GetString());
									LoadObjectRecur(valueValue, scriptName, id, *ptrrr2);
								}
							}
						}
						auto tempView = variantMap.create_associative_view();
						auto viewResult = tempView.equal_range(std::string("CharacterScript"));
						if (viewResult.first != tempView.end())
						{
							std::shared_ptr<IScript> ptrrr = viewResult.first.get_value().get_wrapped_value<std::shared_ptr<IScript>>();
						}
						prop.set_value(inst, variantMap);
					}
					else if (memberType == type::get<std::string>())
					{
						const std::string tempStr = curr[propName.c_str()].GetString();
						prop.set_value(inst, rttr::variant(tempStr));
					}
					else if (memberType.is_enumeration())
					{
						rttr::enumeration tempEnum = memberType.get_enumeration();
						rttr::variant tempVar = tempEnum.name_to_value(curr[propName.c_str()].GetString());
						prop.set_value(inst, tempVar);
					}
					else if (memberType.is_valid())
					{
						rttr::variant temp = prop.get_value(inst);
						LoadObjectRecur(curr[propName.c_str()], memberType, id, temp);
						prop.set_value(inst, temp);
					}
					else
						LogCoreError("Failed to find type");
				}
			}
		}
		inline void LoadObjectRecur(const rapidjson::Value& curr, const char* type, const BEID& id, const rttr::instance& inst)
		{
			LoadObjectRecur(curr, rttr::type::get_by_name(type), id, inst);
		}
		inline void LoadEntity(const rapidjson::Value& curr)
		{
			if (!curr.HasMember(S_EntityID.c_str()))
				LogCoreError("Entity does not have id attached");
			const rapidjson::Value& idMember = curr[S_EntityID.c_str()];
			if (!idMember.IsUint64())
				LogCoreError("Entity does not have id attached");
			BEID id = ECS->CreateEntity(BEID(idMember.GetUint64()));
			const auto& map = ECS->GetAllComponentTypes();
			//For each registered component check if the entity has that component
			for (const auto& component : map)
			{
				if (curr.HasMember(component.second.c_str()))
				{
					if(component.first != ECS->GetComponentID<Transform>() && component.first != ECS->GetComponentID<EntityInfo>())
						ECS->AddComponent(component.second, id);
					rttr::instance inst = ECS->GetComponent(component.second.c_str(), id);
					const rapidjson::Value& componentValue = curr[component.second.c_str()];
					LoadObjectRecur(componentValue, component.second.c_str(), id, inst);
				}
			}
		}

		void LoadEntities(const std::string& fileName)
		{
			StopWatch watch("LoadEntities");
			rapidjson::Document doc;
			//Try to open the file and read into a string steam
			std::ifstream ifs(fileName);
			if (ifs.fail())
				LogCoreError("Could not open file : %s", fileName.c_str());
			std::stringstream ss(std::string(std::istreambuf_iterator<char>(ifs), {}));
			if (ss.str().length() == 0) // New file detected
				return;
			//Parse the inputted string indo the rapidjson document
			doc.Parse(ss.str().c_str());
			//Validate the json
			ECS->DeserialiseAllTags(doc, doc);
			rapidjson::Value& v = doc["entities"];
			if (v.IsArray())
			{
				LogCoreError("Convert current level file to new save file type by saving file in editor : %s", fileName.c_str());
				//Call load entity for each entity found inside the json
				for (rapidjson::Value::ConstValueIterator curr = v.GetArray().Begin(); curr != v.GetArray().End(); ++curr)
				{
					if (!curr->IsObject())
						LogCoreError("Error loading entity in %s", fileName.c_str());;
					LoadEntity(*curr);
				}
			}
			else
			{
				for (auto curr = v.MemberBegin(); curr != v.MemberEnd(); ++curr)
				{
					if (!curr->value.IsObject())
						LogCoreError("Error loading entity in %s", fileName.c_str());;
					LoadEntity(curr->value);
				}
			}
			if(doc.HasMember("Canvases"))
			{
				rapidjson::Value& canvaseValues = doc["Canvases"];
				rttr::enumeration uiTypeEnum = rttr::type::get_by_name(RegistrationNameCreator<UIElementType>()).get_enumeration();
				for (rapidjson::Value::ConstValueIterator curr = canvaseValues.GetArray().Begin(); curr != canvaseValues.GetArray().End(); ++curr)
				{
					if (!curr->IsObject())
						LogCoreError("Error loading Canvas in %s", fileName.c_str());
					BEID canvasID = BEID(curr[0]["ID"].GetUint64());
					int canvasSortOrder = curr[0]["Sort Layer"].GetInt();
					UI->CreateCanvas(canvasSortOrder, canvasID);
					auto& canvasRef = *UI->GetCanvas(canvasID);
					LoadObjectRecur(*curr, rttr::type::get<UICanvas>(), NULL_ENTITY, canvasRef);
					const rapidjson::Value& uiElementArray = curr[0]["UI Elements"];
					for (rapidjson::Value::ConstValueIterator currUIElement = uiElementArray.GetArray().Begin(); currUIElement != uiElementArray.GetArray().End(); ++currUIElement)
					{
						BEID uiId = BEID(currUIElement[0]["ID"].GetUint64());
						std::string tempS = currUIElement[0]["Type Name"].GetString();
						rttr::variant temp = uiTypeEnum.name_to_value(tempS);
						UIElementType currUIElementType = temp.get_value<UIElementType>();
						canvasRef.CreateUI(currUIElementType, uiId);
						rttr::instance uiInstance = *canvasRef.GetUI(uiId);
						LoadObjectRecur(*currUIElement, rttr::type::get_by_name(currUIElement[0]["Type Name"].GetString()), NULL_ENTITY, uiInstance);
					}
					canvasRef.SortUIElements();
				}
				UI->SortUICanvases();
			}
			if (doc.HasMember("Global Illumination setting"))
			{
				rapidjson::Value& illuminationMemberValue = doc["Global Illumination setting"];
				RendererSystem::GetInstance()->getUBO().ambientLightColor[0] = illuminationMemberValue["x"].GetFloat();
				RendererSystem::GetInstance()->getUBO().ambientLightColor[1] = illuminationMemberValue["y"].GetFloat();
				RendererSystem::GetInstance()->getUBO().ambientLightColor[2] = illuminationMemberValue["z"].GetFloat();
				RendererSystem::GetInstance()->getUBO().ambientLightColor[3] = illuminationMemberValue["intentsity"].GetFloat();
			}
			else
			{
				RendererSystem::GetInstance()->getUBO().ambientLightColor[0] = 1.0f;
				RendererSystem::GetInstance()->getUBO().ambientLightColor[1] = 1.0f;
				RendererSystem::GetInstance()->getUBO().ambientLightColor[2] = 1.0f;
				RendererSystem::GetInstance()->getUBO().ambientLightColor[3] = 0.02f;
			}
		}

		void SaveEntities(const std::string& output)
		{
			StopWatch watch("SaveEntities");
			if ((serialiseQuat & serialiseVec) == false)
				return;
			//Create a output stream to override the output file
			std::ofstream ofs(output, std::ofstream::out | std::ofstream::trunc);
			using namespace rapidjson;
			//Create a document to store the json
			Document d;
			//Set the root of the json and create temporary variables
			Value& rootValue = d.SetObject();
			Value entityArray, entityObj, entityMember, entityMemberKey, entityBehaviourName, entitySystemName;
			
			const auto& componentTypes = ECS->GetAllComponentTypes();
			ECS->SerialiseAllTags(d, rootValue);
#ifdef DONT_USE_ARRAY_FOR_LEVELFILES
			entityArray.SetObject();
			std::stringstream ss;
			for (BEID id : ECS->GetAllEntities())
			{
				ss.str(std::string());
				entityObj.SetObject();
				entityMember.SetObject();
				entityMemberKey.SetString(S_EntityID.c_str(), static_cast<SizeType>(S_EntityID.length()));
				entityMember.SetUint64(id);
				entityObj.AddMember(entityMemberKey, entityMember, d.GetAllocator());
				ss << (*id);
				for (auto itr : componentTypes)
				{
					if (ECS->HasComponent(itr.second, id))
					{
						entityMember.SetObject();
						entityMemberKey.SetString(itr.second.c_str(), static_cast<SizeType>(itr.second.length()), d.GetAllocator());
						SerializeToValueBase(rttr::instance(ECS->GetComponent(itr.second, id)), entityMember, d);
						entityObj.AddMember(entityMemberKey, entityMember, d.GetAllocator());
					}
				}
				entityMember.SetString(ss.str().c_str(), static_cast<SizeType>(ss.str().length()), d.GetAllocator());
				entityArray.AddMember(entityMember,entityObj, d.GetAllocator());
			}
			rootValue.AddMember("entities", entityArray, d.GetAllocator());
#else
			entityArray.SetArray();
			for (BEID id : ECS->GetAllEntities())
			{
				entityObj.SetObject();
				entityMember.SetObject();
				entityMemberKey.SetString(S_EntityID.c_str(), static_cast<SizeType>(S_EntityID.length()));
				entityMember.SetUint64(id);
				entityObj.AddMember(entityMemberKey, entityMember, d.GetAllocator());
				for (auto itr : componentTypes)
				{
					if (ECS->HasComponent(itr.second, id))
					{
						entityMember.SetObject();
						entityMemberKey.SetString(itr.second.c_str(), static_cast<SizeType>(itr.second.length()), d.GetAllocator());
						SerializeToValueBase(rttr::instance(ECS->GetComponent(itr.second, id)), entityMember, d);
						entityObj.AddMember(entityMemberKey, entityMember, d.GetAllocator());
					}
				}
				entityArray.PushBack(entityObj, d.GetAllocator());
			}
			rootValue.AddMember("entities", entityArray, d.GetAllocator());
#endif // 

		
			rapidjson::Value canvaseArray,canvaseArrayMember,canvaseIDValue;
			const auto& canvases = UI->GetAllCanvases();
			canvaseArray.SetArray();
			for (const auto& canv : canvases)
			{
				canvaseArrayMember.SetObject();
				const auto& canvasRef = *UI->GetCanvas(canv);
				canvaseIDValue.SetUint64(*canv);
				canvaseArrayMember.AddMember("ID", canvaseIDValue, d.GetAllocator());
				SerializeToValueBase(canvasRef, canvaseArrayMember, d);
				const auto& uiElements = UI->GetCanvas(canv)->GetAllUIElements();
				rapidjson::Value uiElementArray, uiElementValue,uiBEIDValue,uiTypeValue;
				uiElementArray.SetArray();
				for(const auto& currentUIElement: uiElements)
				{
					uiElementValue.SetObject();
					uiBEIDValue.SetUint64(*currentUIElement);
					uiElementValue.AddMember("ID", uiBEIDValue, d.GetAllocator());
					rttr::instance uiElementInstance = *canvasRef.GetUI(currentUIElement);
					uiTypeValue.SetString(uiElementInstance.get_derived_type().get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(uiElementInstance.get_derived_type().get_name().length()), d.GetAllocator());
					uiElementValue.AddMember("Type Name", uiTypeValue, d.GetAllocator());
					SerializeToValueBase(*canvasRef.GetUI(currentUIElement), uiElementValue, d);
					uiElementArray.PushBack(uiElementValue,d.GetAllocator());
				}
				canvaseArrayMember.AddMember("UI Elements", uiElementArray, d.GetAllocator());
				
				canvaseArray.PushBack(canvaseArrayMember, d.GetAllocator());
			}
			rootValue.AddMember("Canvases", canvaseArray, d.GetAllocator());
			rapidjson::Value illuminationMemberValue,illuminationMemberFloat;
			illuminationMemberValue.SetObject();
			illuminationMemberFloat.SetFloat(RendererSystem::GetInstance()->getUBO().ambientLightColor[0]);
			illuminationMemberValue.AddMember("x", illuminationMemberFloat, d.GetAllocator());
			illuminationMemberFloat.SetFloat(RendererSystem::GetInstance()->getUBO().ambientLightColor[1]);
			illuminationMemberValue.AddMember("y", illuminationMemberFloat, d.GetAllocator());
			illuminationMemberFloat.SetFloat(RendererSystem::GetInstance()->getUBO().ambientLightColor[2]);
			illuminationMemberValue.AddMember("z", illuminationMemberFloat, d.GetAllocator());
			illuminationMemberFloat.SetFloat(RendererSystem::GetInstance()->getUBO().ambientLightColor[3]);
			illuminationMemberValue.AddMember("intentsity", illuminationMemberFloat, d.GetAllocator());
			rootValue.AddMember("Global Illumination setting", illuminationMemberValue, d.GetAllocator());
			OStreamWrapper osw(ofs);
			//Create a writer to write the document to the output stream
			PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
			d.Accept(writer);
			ofs.close();
		}
		//inline void VariantToValueEnum(const rttr::variant& var, rapidjson::Value& val)
		//{
		//	if (!var.to_uint64())
		//		AssertCore(false, "Cannot convert enum to uint");
		//	val.SetUint64(var.to_uint64());
		//}
		inline void VariantToValueArth(const rttr::variant& var, rapidjson::Value& val)
		{
			if (var.get_type() == type::get<bool>())
				val.SetBool(var.get_value<bool>());
			else if (var.get_type() == type::get<char>())
				val.SetInt(var.get_value<char>());
			else if (var.get_type() == type::get<int8_t>())
				val.SetInt(var.get_value<int8_t>());
			else if (var.get_type() == type::get<int16_t>())
				val.SetInt(var.get_value<int16_t>());
			else if (var.get_type() == type::get<int32_t>())
				val.SetInt(var.get_value<int16_t>());
			else if (var.get_type() == type::get<int64_t>())
				val.SetInt64(var.get_value<int64_t>());
			else if (var.get_type() == type::get<uint8_t>())
				val.SetUint(var.get_value<uint8_t>());
			else if (var.get_type() == type::get<uint16_t>())
				val.SetUint(var.get_value<uint16_t>());
			else if (var.get_type() == type::get<uint32_t>())
				val.SetUint(var.get_value<uint32_t>());
			else if (var.get_type() == type::get<uint64_t>())
				val.SetUint64(var.get_value<uint64_t>());
			else if (var.get_type() == type::get<float>())
				val.SetFloat(var.get_value<float>());
			else if (var.get_type() == type::get<double>())
				val.SetDouble(var.get_value<double>());
			else
				LogCoreError("Failed to find type");
		}
		//Need to handle array of strings
		inline void VariantToValueArr(const rttr::variant& var, rapidjson::Value& val, rapidjson::Document& d)
		{
			auto view = var.create_sequential_view();
			for (const auto& curr : view)
			{
				rapidjson::Value arrMember;
				rttr::variant currUnwrapped = curr.get_type().is_wrapper() ? curr.extract_wrapped_value() : curr;
				if (currUnwrapped.get_type().is_arithmetic())
					VariantToValueArth(currUnwrapped, arrMember);
				else if (currUnwrapped.get_type() == type::get<std::string>())
				{
					arrMember.SetString(currUnwrapped.get_value<std::string>().c_str(), static_cast<rapidjson::SizeType>(currUnwrapped.get_value<std::string>().length()), d.GetAllocator());
				}
				else if (currUnwrapped.get_type().is_sequential_container())
				{
					arrMember.SetArray();
					VariantToValueArr(currUnwrapped, arrMember, d);
				}
				else
				{
					arrMember.SetObject();
					SerializeToValueBase(currUnwrapped, arrMember, d);
				}
				val.PushBack(arrMember, d.GetAllocator());
			}
		}
		void VariantToValueMap(const rttr::variant& var, rapidjson::Value& val, rapidjson::Document& d)
		{
			auto view = var.create_associative_view();
			for (const auto& curr : view)
			{
				rapidjson::Value arrMember,memberKey,memberValue;
				arrMember.SetObject();
				rttr::variant keyUnwrapped = curr.first.get_type().is_wrapper() ? curr.first.extract_wrapped_value() : curr.first;
				if (keyUnwrapped.get_type().is_arithmetic())
					VariantToValueArth(keyUnwrapped, memberKey);
				else if (keyUnwrapped.get_type() == type::get<std::string>())
				{
					//LogCoreError("Why am i seeing a map that uses a string as a key?");
					memberKey.SetString(keyUnwrapped.get_value<std::string>().c_str(), static_cast<rapidjson::SizeType>(keyUnwrapped.get_value<std::string>().length()), d.GetAllocator());
				}
				else
				{
					memberKey.SetObject();
					SerializeToValueBase(keyUnwrapped, memberKey, d);
				}
				arrMember.AddMember("Key", memberKey, d.GetAllocator());
				rttr::variant valueUnwrapped = curr.second.get_type().is_wrapper() ? curr.second.extract_wrapped_value() : curr.second;
				if (valueUnwrapped.get_type().is_arithmetic())
					VariantToValueArth(valueUnwrapped, memberValue);
				else if (valueUnwrapped.get_type() == type::get<std::string>())
				{
					memberValue.SetString(valueUnwrapped.get_value<std::string>().c_str(), static_cast<rapidjson::SizeType>(valueUnwrapped.get_value<std::string>().length()), d.GetAllocator());
				}
				else
				{
					memberValue.SetObject();
					SerializeToValueBase(valueUnwrapped, memberValue, d);
				}
				arrMember.AddMember("Value", memberValue, d.GetAllocator());
				val.PushBack(arrMember, d.GetAllocator());
			}
		}
		//TODO:: Need to handle enumerations
		void SerializeToValueBase(const rttr::instance& inst, rapidjson::Value& v, rapidjson::Document& d)
		{
			rapidjson::Value componentMemberKey, componentMember;
			rttr::instance instUnwrapped = inst.get_type().get_raw_type().is_wrapper() ? inst.get_wrapped_instance() : inst;
			auto prop_list = instUnwrapped.get_derived_type().get_properties();
			rttr::type t = instUnwrapped.get_derived_type();
			if (t.get_properties().size() == 0)
				LogCoreError("Did not load any data for type %s", t.get_name().to_string().c_str());
			for (auto prop : t.get_properties())
			{
				if (prop.get_metadata(MetaDataKey::Serialize).is_valid()
					&& prop.get_metadata(MetaDataKey::Serialize) == false)
				{
					continue;
				}
				rttr::variant variant = prop.get_value(inst);
				if (variant.get_type().is_arithmetic())
				{
					componentMemberKey.SetString(prop.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(prop.get_name().length()), d.GetAllocator());
					VariantToValueArth(variant, componentMember);
					v.AddMember(componentMemberKey, componentMember, d.GetAllocator());
				}
				else if (variant.get_type().is_sequential_container())
				{
					componentMemberKey.SetString(prop.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(prop.get_name().length()), d.GetAllocator());
					componentMember.SetArray();
					VariantToValueArr(variant, componentMember, d);
					v.AddMember(componentMemberKey, componentMember, d.GetAllocator());
				}
				else if (variant.get_type().is_associative_container())
				{
					componentMemberKey.SetString(prop.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(prop.get_name().length()), d.GetAllocator());
					componentMember.SetArray();
					VariantToValueMap(variant, componentMember, d);
					v.AddMember(componentMemberKey, componentMember, d.GetAllocator());
				}
				else if (variant.is_type<std::string>())
				{
					componentMemberKey.SetString(prop.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(prop.get_name().length()), d.GetAllocator());
					componentMember.SetString(variant.get_value<std::string>().c_str(), static_cast<rapidjson::SizeType>(variant.get_value<std::string>().length()), d.GetAllocator());
					v.AddMember(componentMemberKey, componentMember, d.GetAllocator());
				}
				else if(variant.get_type().is_enumeration())
				{
					componentMemberKey.SetString(prop.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(prop.get_name().length()), d.GetAllocator());
					std::string temp = variant.to_string();
					componentMember.SetString(temp.c_str(), static_cast<rapidjson::SizeType>(temp.length()), d.GetAllocator());
					v.AddMember(componentMemberKey, componentMember, d.GetAllocator());
				}
				else if (variant.is_valid())
				{
					componentMemberKey.SetString(prop.get_name().to_string().c_str(), static_cast<rapidjson::SizeType>(prop.get_name().length()), d.GetAllocator());
					componentMember.SetObject();
					SerializeToValueBase(variant, componentMember, d);
					v.AddMember(componentMemberKey, componentMember, d.GetAllocator());
				}
				else
					LogCoreError("Failed to find type");

			}
		}
	}
}