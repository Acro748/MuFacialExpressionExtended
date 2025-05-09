import sys
import os

def generate_header(header_file, hlsl_files):
    with open(header_file, 'w') as f:
        f.write('#pragma once\n\n')
        
        for hlsl_file in hlsl_files:
            shader_var_name = os.path.splitext(os.path.basename(hlsl_file))[0]
            f.write(f'const char* {shader_var_name} = R"(\n')
            
            with open(hlsl_file, 'r') as shader_file:
                for line in shader_file:
                    f.write(line)
            
            f.write(')";\n\n')

if __name__ == "__main__":
    header_file = sys.argv[1]
    hlsl_files = sys.argv[2:]
    generate_header(header_file, hlsl_files)
