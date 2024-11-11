import sys

args = sys.argv

if len(args) > 1:
    name = args[1]
else:
    name = ""
name_cap = name.upper()

with open(name + '.cpp', 'w') as file:
    file.write(f"#include \"{name}.hpp\"\n\n{name}::{name} ( void ) {{\n\tstd::cout << \"Default {name} constructor called\" << std::endl;\n}}\n\n")
    file.write(f"{name}::{name} ( const {name} &copy ) {{\n\tstd::cout << \"Copy {name} constructor called\" << std::endl;\n}}\n\n")
    file.write(f"{name}::~{name} ( void ) {{\n\tstd::cout << \"Destructor {name} called\" << std::endl;\n}}\n\n")
    file.write(f"{name}\t&{name}::operator=( const {name}& other ) {{\n\tstd::cout << \"Copy assignment operator {name} called\" << std::endl;\n\treturn *this;\n}}\n")

with open(name + '.hpp', 'w') as file:
    file.write(f"#ifndef {name_cap}_HPP\n# define {name_cap}_HPP\n\n# include <iostream>\n# include <string>\n\nclass {name} {{\n\n\tprivate:\n\n\tpublic:\n\t//	Canonical form\n\t\t{name}( void );\n\t\t~{name}();\n\t\t{name}( const {name}& copy);\n\t\t{name}\t&operator= ( const {name}& other );\n}};\n\n#endif")