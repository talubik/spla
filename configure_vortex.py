import argparse
import subprocess


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--vortex-dir", help="path to vortex directory")
    parser.add_argument("--driver", help="vortex driver")
    parser.add_argument("--warps", help="count of warps", type=int, default=4)
    parser.add_argument("--cores", help="count of cores")
    parser.add_argument("--threads", help="count of threads", type=int, default=4)
    parser.add_argument("--clusters", help="count of clusters")
    parser.add_argument("--l2cache", help="l2cache")
    parser.add_argument("--l3cache", help="l3cache")
    args = parser.parse_args()
    vortex_config_args = [
        f"{args.vortex_dir}/build/ci/blackbox.sh",
        "--app=opencl/vecadd",
        "--perf=1"
    ]
    if args.driver:
        vortex_config_args += [f"--driver={args.driver}"]
    if args.warps:
        vortex_config_args += [f"--warps={args.warps}"]
    if args.cores:
        vortex_config_args += [f"--cores={args.cores}"]
    if args.threads:
        vortex_config_args += [f"--threads={args.threads}"]
    if args.clusters:
        vortex_config_args += [f"--clusters={args.clusters}"]
    if args.l2cache:
        if int(args.l2cache) == 1:
            vortex_config_args += [f"--l2cache"]
    if args.l3cache:
        if int(args.l3cache) == 1:
            vortex_config_args += [f"--l3cache"]
    result = subprocess.run(
        vortex_config_args, capture_output=True, text=True, timeout=600.0
    )
    export_line = None
    if result.returncode != 0:
        return
    for line in result.stdout.split("\n"):
        if line.startswith("LD_LIBRARY_PATH="):
            export_line = line
            break
        if line.startswith("SCOPE_JSON_PATH="):
            export_line = line
            break

    if export_line is None:
        return

    print(f"export {' '.join(export_line.split()[:-2])}")


if __name__ == "__main__":
    main()
