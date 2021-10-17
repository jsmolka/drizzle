#include "dzclosure.h"

DzClosure::DzClosure(DzFunction& function)
    : DzObject(Type::Closure), function(function)
{
    upvalues.resize(function.upvalues);
    std::fill(upvalues.begin(), upvalues.end(), nullptr);
}
