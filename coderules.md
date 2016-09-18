#OpenRhythm C++ Code Guidelines

There is not to many rules yet, but new ones will likely be added as issues come up.

###Naming Rules
*Function/Method names `underscore_case`
*Variable names `camelCase`
*member variable names `m_camelCase`
*class names `UpperCamelCase`

###Other Rules
*No raw new/delete, use smart pointers instead with make_unique or make_shared.
*No methods inside class defintions.
*The order of methods in cpp files should generally be the same as the order they were defined in the class definition. (There are some exceptions however)
*Classes should be in the order of `public`, `protected`, `private`
