import subprocess
import argparse
import os
import sys

def run_command(command, cwd=None):
    print(f"Running command: {command}")
    result = subprocess.run(command, shell=True, cwd=cwd, executable='/bin/bash')
    if result.returncode != 0:
        print(f"Command failed with return code {result.returncode}")
        sys.exit(result.returncode)

def main():
    project_root = os.path.dirname(os.path.abspath(__file__))
    

    vortex_source_dir = os.path.join(project_root, "deps/vortex")
    pocl_source_dir = os.path.join(project_root, "deps/pocl")
    
    vortex_build_dir = os.path.join(vortex_source_dir, "build")
    tool_dir = os.path.join(project_root, "vortex-tools")

   
    os.makedirs(vortex_build_dir, exist_ok=True)
    os.makedirs(tool_dir, exist_ok=True)

    
    vortex_source_rel_to_build = os.path.relpath(vortex_source_dir, vortex_build_dir)
    pocl_source_rel_to_build = os.path.relpath(pocl_source_dir, vortex_build_dir)
    tool_dir_rel_to_build = os.path.relpath(tool_dir, vortex_build_dir)

    
    configure_cmd = (
        f"{vortex_source_rel_to_build}/configure --xlen=64 --tooldir={tool_dir} && "
        f"TOOLDIR={tool_dir_rel_to_build} ./ci/toolchain_install.sh --all"
    )
    run_command(configure_cmd, cwd=vortex_build_dir)

   
    build_cmd = (
        f"source {vortex_build_dir}/ci/toolchain_env.sh && make -s"
    )
    run_command(build_cmd, cwd=vortex_build_dir)

   
    pocl_build_script_path = os.path.join(vortex_build_dir, "build_pocl.sh")
    with open(pocl_build_script_path, "w") as f:
        f.write(f"""
#!/bin/bash
set -ex
echo "--- Building POCL for Vortex ---"
cd "{pocl_source_rel_to_build}"
mkdir -p build && cd build
export POCL_PATH="{tool_dir}/pocl"
export VORTEX_PREFIX="{vortex_source_dir}"
export LLVM_PREFIX="{tool_dir_rel_to_build}/llvm-vortex"
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$POCL_PATH \\
      -DWITH_LLVM_CONFIG=$LLVM_PREFIX/bin/llvm-config -DVORTEX_PREFIX=$VORTEX_PREFIX -DENABLE_VORTEX=ON \\
      -DKERNEL_CACHE_DEFAULT=OFF -DENABLE_HOST_CPU_DEVICES=OFF -DENABLE_TESTS=OFF -DENABLE_ICD=OFF ..
make -j$(nproc)
make install
cp -r ../include "$POCL_PATH"
echo "--- POCL build finished ---"
        """)
    
    os.chmod(pocl_build_script_path, 0o755)
    run_command("./build_pocl.sh", cwd=vortex_build_dir)

    print("--- Vortex and POCL build finished successfully! ---")

if __name__ == "__main__":
    main()
