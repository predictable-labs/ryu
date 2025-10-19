import sys
from pathlib import Path

RYU_ROOT = Path(__file__).parent.parent.parent.parent

if sys.platform == "win32":
    # \ in paths is not supported by ryu's parser
    RYU_ROOT = str(RYU_ROOT).replace("\\", "/")
