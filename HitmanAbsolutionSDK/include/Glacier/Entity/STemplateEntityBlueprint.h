#pragma once

#include "../Templates/TArray.h"
#include "STemplateSubEntityBlueprint.h"
#include "SEntityTemplatePinConnection.h"

struct STemplateEntityBlueprint
{
	int rootEntityIndex;
	TArray<STemplateSubEntityBlueprint> entityTemplates;
	TArray<SEntityTemplatePinConnection> pinConnections;
	TArray<SEntityTemplatePinConnection> inputPinForwardings;
	TArray<SEntityTemplatePinConnection> outputPinForwardings;
};
