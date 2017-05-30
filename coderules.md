#OpenRhythm C++ Code Guidelines

There is not to many rules yet, but new ones will likely be added as issues come up.
If you see a case where something does not follow this please correct it :)

###Naming Rules
* Function/Method names `underscore_case`
* Variable names `camelCase`
* member variable names `m_camelCase`
* Static class member `sm_camelCase`
* class names `UpperCamelCase`

###Other Rules
* No raw new/delete, use smart pointers instead with make_unique or make_shared.
* No methods inside class defintions.

###Style Rules
* The order of methods in cpp files should generally be the same as the order they were defined in the class definition. (There are some exceptions however)
* Classes should be in the order of `public`, `protected`, `private`
* Always put a space after keywords (`for`, `do`, `while`, `switch`, `if`, etc) except keywords generate used like functions(`sizeof`, etc)
* Always use curly brackets even when there is just a single line in the block.
* No spaces directly inside parentheses please.
* Curly brackets always on a new line.
* Spaces, no tabs please. Most modern editors can convert tabs to spaces, so please do so.

