#include "Challenge.h"
#include "CommonReader.h"

void ChallengeNullDef::ParseHeader(char* data) {
	int idVar = 0;
	int unk1Var = 0;
	int taskIDVar = 0;
	int sizeVar = 0;

	memcpy_s(&idVar, 4, data, 4);
	memcpy_s(&unk1Var, 4, data + 4, 4);
	memcpy_s(&taskIDVar, 4, data + 8, 4);
	memcpy_s(&sizeVar, 4, data + 0xC, 4);

	header.reqId = (dbChallengeRequirement_e)flipEndian(idVar);
	header.unk1 = flipEndian(unk1Var);
	header.taskID = flipEndian(taskIDVar);
	header.size = flipEndian(sizeVar);
}

void ChallengeCommonDef::ParseCommon(char* data) {
	// Clear all the tag, flag and counter areas.
	memset(worldNameTag, 0, 0x20);
	memset(challengeTag, 0, 0x20);
	memset(gameStyleTag, 0, 0x20);

	memset(seenObjectivesFlag, 0, 0x40);
	memset(gameStyleFlag, 0, 0x40);
	memset(notesOnlyFlag, 0, 0x40);
	memset(beatenFlag, 0, 0x40);
	memset(beatenCPlusFlag, 0, 0x40);
	memset(jiggiesWonCounter, 0, 0x40);
	memset(highScoreCounter, 0, 0x40);
	memset(specialObjectTag01, 0, 0x40);
	memset(specialObjectIndicator01, 0, 0x40);
	memset(specialObjectTag02, 0, 0x40);
	memset(specialObjectIndicator02, 0, 0x40);

	strcpy_s(worldNameTag, 0x20, data + 0x10);
	strcpy_s(challengeTag, 0x20, data + 0x30);
	strcpy_s(gameStyleTag, 0x20, data + 0x50);


}

void challengeStayInVehicle::ParseStayInVehicle(char* data) {
	int canPlayersGetOutVar = 0; // 0x10
	int canBaddiesGetOutVar = 0; // 0x14

	memcpy_s(&canPlayersGetOutVar, 4, data + 0x10, 4);
	memcpy_s(&canBaddiesGetOutVar, 4, data + 0x14, 4);

	canPlayersGetOut = flipEndian(canPlayersGetOutVar);
	canBaddiesGetOut = flipEndian(canBaddiesGetOutVar);
}