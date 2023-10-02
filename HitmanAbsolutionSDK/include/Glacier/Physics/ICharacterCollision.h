#pragma once

class HitmanAbsolutionSDK_API ICharacterCollision : public IComponentInterface
{
public:
	virtual ~ICharacterCollision() = default;
	virtual SMatrix GetCollisionCheckedMatPos(const SMatrix& matrix) = 0;
};
