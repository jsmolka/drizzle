#include "dzupvalue.h"

#include "dzvalue.h"

DzUpvalue::DzUpvalue(DzValue& value)
    : DzObject(Type::Upvalue), value(value)
{

}

