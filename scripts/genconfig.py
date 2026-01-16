import kconfiglib
import sys
import os

def gen():

    kconf = kconfiglib.Kconfig("Kconfig")

    if os.path.exists(".config"):
        kconf.load_config(".config")

    print("#ifndef _AUTOCONF_H_")
    print("#define _AUTOCONF_H_")
    print()

    for sym in kconf.unique_defined_syms:
        
        if not sym.name:
            continue

        val = sym.str_value
        if not val:
            continue

        sym_name = f"CONFIG_{sym.name}"
    
        match sym.type:

            case kconfiglib.BOOL | kconfiglib.TRISTATE:
                print(f"#define {sym_name} {1 if val == "y" else 0}")
            
            case kconfiglib.INT | kconfiglib.HEX | kconfiglib.STRING:
                print(f"#define {sym_name} {val}")

    print()
    print("#endif")

if __name__ == "__main__":
    gen()    