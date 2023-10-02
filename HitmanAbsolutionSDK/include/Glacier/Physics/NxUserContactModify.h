#pragma once

class NxShape;

class NxUserContactModify
{
public:
	struct NxContactCallbackData;

	virtual bool onContactConstraint(unsigned int& param1, const NxShape* nxShape, const NxShape* nxShape3, const unsigned int param4, const unsigned int param5, NxContactCallbackData& nxContactCallbackData) = 0;
	virtual ~NxUserContactModify() = default;
};
