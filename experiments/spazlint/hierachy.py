from pygccxml import utils
from pygccxml import declarations
from pygccxml import parser

generator_path, generator_name = utils.find_xml_generator()

xml_generator_config = parser.xml_generator_configuration_t(
    xml_generator_path=generator_path,
    xml_generator=generator_name)

decls = parser.parse_string("""
namespace nc{
    int a = 2;
}                            
""", xml_generator_config)
global_namespace = declarations.get_global_namespace(decls)
ns = global_namespace.namespace("ns")

for var in ns.variables():
    if var.name == "c":
        print("c found")