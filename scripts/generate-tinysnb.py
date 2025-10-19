import os
import shutil
import subprocess
import sys

RYU_ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
# Datasets can only be copied from the root since copy.schema contains relative paths
os.chdir(RYU_ROOT)

# Define the build type from input
if len(sys.argv) > 1 and sys.argv[1].lower() == "release":
    build_type = "release"
else:
    build_type = "relwithdebinfo"

# Change the current working directory
if os.path.exists(f"{RYU_ROOT}/dataset/databases/tinysnb"):
    shutil.rmtree(f"{RYU_ROOT}/dataset/databases/tinysnb")
if sys.platform == "win32":
    ryu_shell_path = f"{RYU_ROOT}/build/{build_type}/src/ryu_shell"
else:
    ryu_shell_path = f"{RYU_ROOT}/build/{build_type}/tools/shell/ryu"
subprocess.check_call(
    [
        "python3",
        f"{RYU_ROOT}/benchmark/serializer.py",
        "TinySNB",
        f"{RYU_ROOT}/dataset/tinysnb",
        f"{RYU_ROOT}/dataset/databases/tinysnb",
        "--single-thread",
        "--ryu-shell",
        ryu_shell_path,
    ]
)
