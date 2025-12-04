from __future__ import annotations

import sys
from ctypes import CDLL
from pathlib import Path


def main() -> int:
    if len(sys.argv) < 2:
        sys.stderr.write("Usage: python scripts/extract_mvs_with_frames.py <video_path>\n")
        return 1

    video = sys.argv[1]
    project_root = Path(__file__).resolve().parent.parent
    so_path = project_root / "bin" / "extract_mvs_with_frames.so"
    if not so_path.exists():
        sys.stderr.write(f"Shared library not found at {so_path}\n")
        return 1

    emv = CDLL(str(so_path))
    print(video)
    emv.extract_motion_vectors(video.encode())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
