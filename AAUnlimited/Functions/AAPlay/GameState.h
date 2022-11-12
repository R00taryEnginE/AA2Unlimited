#pragma once

#include "External\ExternalVariables\AAPlay\GameGlobals.h"
#include "Functions\AAPlay\Globals.h"

namespace Shared {
	namespace GameState {

		void reset();

		void setIsClothesScreen(bool value);
		bool getIsClothesScreen();

		void setIsPcConversation(bool value);
		bool getIsPcConversation();
		void setIsSaving(bool value);
		bool getIsSaving();
		void setIsOverridingDialogue(bool value);
		bool getIsOverridingDialogue();
		void setIsOverriding(bool value);
		bool getIsOverriding();
		void updateIsOverriding();

		void setIsInMainMenu(bool value);
		bool getIsDrawingShadow();
		void setIsDrawingShadow(bool value);
		bool getIsInMainMenu();

		void setIsHighPolyLoaded(bool value);
		bool getIsHighPolyLoaded();

		void setIsPeeping(bool value);
		bool getIsPeeping();

		void setH_AI(bool value);
		bool getH_AI();
		void setTalkingName(std::wstring value);
		std::wstring getTalkingName();
		void setTalkAboutName(std::wstring value);
		std::wstring getTalkAboutName();
		void setLockedInH(bool value);
		bool getLockedInH();
		void setRemovedSeat(int value);
		int getRemovedSeat();

		void setIsInH(bool value);
		bool getIsInH();

		void setVoyeur(ExtClass::CharacterStruct * voyeur);
		ExtClass::CharacterStruct * getVoyeur();

		void setHInfo(ExtClass::HInfo * h_info);
		ExtClass::HInfo * getHInfo();

		void setVoyeurTarget(ExtClass::NpcData * target);
		ExtClass::NpcData * getVoyeurTarget();

		void setPCConversationState(DWORD value);
		DWORD getPCConversationState();

		void setNPCLineState(DWORD value);
		DWORD getNPCLineState();

		void setInterrupt(BYTE value);
		DWORD getInterrupt();

		DWORD getHPosition();
		void setHPosition(DWORD value);

		void SetRoomNumber(int seat, int room);
		int GetRoomNumber(int seat);

		void AddDelayedEvent(Shared::Triggers::DelayedEventData);
		std::list<Shared::Triggers::DelayedEventData>* GetDelayedEvents();

		void kickCard(int s);
		void addCard(std::wstring cardName, bool gender, int s);
		DWORD getClubType(BYTE clubID);
		void addConversationCharacter(ExtClass::CharacterStruct * chara);
		ExtClass::CharacterStruct * getConversationCharacter(int idx);
		void setConversationCharacter(ExtClass::CharacterStruct * chara, int idx);
		void clearConversationCharacter(int idx);
		void clearConversationCharacterBySeat(int seat);

		inline CharInstData* getPlayerCharacter()
		{
			ExtClass::CharacterStruct* pcPtr = *(ExtVars::AAPlay::PlayerCharacterPtr());
			if (pcPtr != nullptr) {
				for (int i = 0; i< 25; i++)
				{
					if (AAPlay::g_characters[i].m_char == pcPtr) return &AAPlay::g_characters[i];
				}
				return NULL;
			}
			else return NULL;
			
		}
		void setPlayerCharacter(int seat);

		inline ExtClass::PcConversationStruct * getPlayerConversation()
		{
			return ExtVars::AAPlay::PlayerConversationPtr();
		}

		std::wstring getCurrentClassSaveName();
	}
}