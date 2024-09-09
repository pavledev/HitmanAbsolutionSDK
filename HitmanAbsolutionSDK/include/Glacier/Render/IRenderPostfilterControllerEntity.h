#pragma once

class IRenderPostfilterParametersEntity;

class IRenderPostfilterControllerEntity : public IComponentInterface
{
public:
	virtual TEntityRef<IRenderPostfilterParametersEntity> GetCurrentParametersEntity() const = 0;
	virtual TEntityRef<IRenderPostfilterParametersEntity> GetParametersEntity() const = 0;
};
