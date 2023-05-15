#pragma once
#include "../General/Util.h"
#include "../3rdparty/picojson/picojson.h"
#include <vector>
#include <fstream>

namespace PersistentStorage {

	template<typename T>
	class Option {
	public:
		bool isValid;
		T value;

		Option() : isValid(false) {}
		Option(T val) : isValid(true), value(val) {}
		Option(Option &that) : isValid(that.isValid), value(that.value) {}
	};

	class ClassStorage {
	private:
		picojson::object data;	//all the json data
		std::wstring file;	//file with its path starting at AA2_PLAY\ 
		static std::map<std::wstring, ClassStorage> allStorages;	//collection of all the loaded storages

		Option<picojson::object*> getCardRecord(CharInstData* character, std::wstring key);
	public:
		std::wstring className;

		picojson::value get(std::string key);
		ClassStorage* set(std::string key, picojson::value value);

		ClassStorage* storeClassBool(std::wstring key, bool value);
		ClassStorage* storeClassInt(std::wstring key, int value);
		ClassStorage* storeClassFloat(std::wstring key, float value);
		ClassStorage* storeClassString(std::wstring key, std::string value);
					
		ClassStorage* storeCardBool(CharInstData* character, std::wstring key, bool value);
		ClassStorage* storeCardInt(CharInstData* character, std::wstring key, int value);
		ClassStorage* storeCardFloat(CharInstData* character, std::wstring key, float value);
		ClassStorage* storeCardString(CharInstData* character, std::wstring key, std::string value);
		ClassStorage* storeCardObject(CharInstData* character, std::wstring key, picojson::object value);
					
		ClassStorage* storeCardAAUDataBool(CharInstData* character, std::wstring key, bool value);
		ClassStorage* storeCardAAUDataInt(CharInstData* character, std::wstring key, int value);
		ClassStorage* storeCardAAUDataFloat(CharInstData* character, std::wstring key, float value);
		ClassStorage* storeCardAAUDataString(CharInstData* character, std::wstring key, std::string value);
		ClassStorage* storeCardAAUDataObject(CharInstData* character, std::wstring key, picojson::object value);

		Option<int> getClassInt(std::wstring key);
		Option<float> getClassFloat(std::wstring key);
		Option<bool> getClassBool(std::wstring key);
		Option<std::string> getClassString(std::wstring key);

		Option<bool> getCardBool(CharInstData* character, std::wstring key);
		Option<int> getCardInt(CharInstData* character, std::wstring key);
		Option<float> getCardFloat(CharInstData* character, std::wstring key);
		Option<std::string> getCardString(CharInstData* character, std::wstring key);
		Option<picojson::object*> getCardObject(CharInstData* character, std::wstring key);

		picojson::value getCardAAUDataValue(CharInstData* character, std::wstring key);

		static ClassStorage* getStorage(std::wstring file);
		static ClassStorage* getCurrentClassStorage();
		static void reset(std::wstring file);
		ClassStorage* save();
		
		ClassStorage() {}
		ClassStorage(ClassStorage &that)
		{
			this->data = that.data;
			this->file = that.file;
			this->className = that.className;
		}
		ClassStorage(std::wstring className) : className(className)
		{
			picojson::value json;
			this->file = CLASS_SAVES_PATH + className + L".json";
			std::ifstream in(this->file);

			in >> json;

			if (picojson::get_last_error().empty() && json.is<picojson::object>())	//storage file loaded correctly			
			{
				//load the data in memory
				data = json.get<picojson::object>();
			}
			else	//file not found or invalid	
			{
				this->set("class_name", picojson::value(General::CastToString(className)));
			}

			ClassStorage::allStorages[className] = *this;
		}
	};

/*
#define LUA_CLASS PersistentStorage::ClassStorage
	static inline void bindLua() {
		LUA_NAME;
		LUA_METHOD(getCurrentClassStorage, { return _gl.push(ClassStorage::getCurrentClassStorage()).one; });
		LUA_METHOD(getCardBool, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(2);
			return _gl.push(_self->getCardBool(character, key)).one;
			});
		LUA_METHOD(getCardInt, {
			CharInstData * character = _gl.get(2);
			char* k = _gl.get(2);
			std::wstring key = std::wstring(k);
			return _gl.push(_self->getCardInt(character, key)).one;
			});
		LUA_METHOD(getCardFloat, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(2);
			return _gl.push(_self->getCardFloat(character, key)).one;
			});
		LUA_METHOD(getCardString, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(2);
			return _gl.push(_self->getCardString(character, key)).one;
			});
		LUA_METHOD(storeCardBool, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(3);
			bool value = _gl.get(4);
			_self->storeCardBool(character, key, value);
			});
		LUA_METHOD(storeCardInt, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(3);
			int value = _gl.get(4);
			_self->storeCardInt(character, key, value);
			});
		LUA_METHOD(storeCardFloat, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(3);
			float value = _gl.get(4);
			_self->storeCardFloat(character, key, value);
			});
		LUA_METHOD(storeCardString, {
			CharInstData * character = _gl.get(2);
			std::wstring key = _gl.get(3);
			std::string value = _gl.get(4);
			_self->storeCardString(character, key, value);
			});
	}
#undef LUA_CLASS
*/
	ClassStorage* current();
}