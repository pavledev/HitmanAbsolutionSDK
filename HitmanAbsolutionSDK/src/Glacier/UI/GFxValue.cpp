#include <Glacier/UI/GFxValue.h>

#include <Function.h>
#include <Global.h>

bool GFxValue::GetBool() const
{
	return Value.BValue;
}

void GFxValue::SetBool(const bool value)
{
	Value.BValue = value;
}

bool GFxValue::GetMember(const char* name, GFxValue* pval) const
{
	return Function::CallMethodAndReturn<bool, const GFxValue*, const char*, GFxValue*>(BaseAddress + 0x19D5C0, this, name, pval);
}

bool GFxValue::SetMember(const char* name, const GFxValue& val)
{
	return Function::CallMethodAndReturn<bool, const GFxValue*, const char*, const GFxValue&>(BaseAddress + 0x1E5720, this, name, val);
}
