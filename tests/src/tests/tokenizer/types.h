#pragma once

#include "utils.h"

namespace tests_tokenizer_types {

inline suite _ = [] {
  "tokenizer_types"_test = [] {
    {
      static_assert(int(Token::Type::LastEnumValue) == 56);

      constexpr auto kSource = R"(
&
&&
!
!=
block
{
}
[
]
break
^
:
,
continue
def
.
elif
else
=
==
false
0.0
>
>>
>>>
>=
test
if
0
<
<<
<=
-
noop
null
(
)
%
|
||
+
print
return
/
//
*
**
""
~
true
var
while
)";

      std::vector<Token::Type> expected;
      for (int i = 0; i < int(Token::Type::LastEnumValue); ++i) {
        const auto type = Token::Type(i);
        switch (type) {
          case Token::Type::Dedent:
          case Token::Type::Eof:
          case Token::Type::Indent:
          case Token::Type::NewLine:
            break;
          default:
            expected.push_back(type);
            expected.push_back(Token::Type::NewLine);
            break;
        }
      }
      expected.push_back(Token::Type::Eof);
      tokenize(kSource, expected);
    }
    {
      constexpr auto kSource = "&&&";
      tokenize(kSource, {
        Token::Type::And2,
        Token::Type::And,
        Token::Type::NewLine,
        Token::Type::Eof
      });
    }
  };
};

}  // namespace tests_tokenizer_types
