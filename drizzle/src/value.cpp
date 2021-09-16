#include "value.h"

#include <shell/format.h>

Value::Value()
    : type(Type::Null) {}

Value::Value(bool boolean)
    : type(Type::Boolean), boolean(boolean) {}

Value::Value(s64 integer)
    : type(Type::Integer), integer(integer) {}

Value::Value(double rational)
    : type(Type::Rational), rational(rational) {}

void Value::print()
{
    switch (type)
    {
    case Type::Null:
        shell::print("null");
        break;

    case Type::Boolean:
        shell::print(boolean);
        break;

    case Type::Integer:
        shell::print(integer);
        break;

    case Type::Rational:
        shell::print(rational);
        break;
    }
}
