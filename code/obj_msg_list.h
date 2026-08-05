#pragma once

enum objMsgId : int {
	objMsgId_Null,
	objMsgId_Debug_TrackerPrint,
	objMsgId_General_Activate,
	objMsgId_General_Deactivate,
	objMsgId_General_Reset,
	objMsgId_General_Open,
	objMsgId_Banjo_Avatar_InContainer = 0xC6,
	objMsgId_Banjo_Avatar_SetIndicatorStatus = 0xD1
};