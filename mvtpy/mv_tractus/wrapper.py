from __future__ import annotations

from ctypes import CDLL
from importlib import resources
from pathlib import Path
from typing import Optional, Union

_emv: Optional[CDLL] = None
_emv_frames: Optional[CDLL] = None


def _load_shared_library(filename: str) -> CDLL:
    """Load a bundled shared object from the installed package."""
    try:
        so_path = resources.files(__package__).joinpath(filename)
    except (AttributeError, ImportError):
        raise RuntimeError("Unable to locate shared library in mv_tractus package resources")

    if not so_path.exists():
        raise FileNotFoundError(f"Shared library not found at {so_path}")

    return CDLL(str(so_path))


def _get_emv() -> CDLL:
    global _emv
    if _emv is None:
        _emv = _load_shared_library("extract_mvs.so")
    return _emv


def _get_emv_frames() -> CDLL:
    global _emv_frames
    if _emv_frames is None:
        _emv_frames = _load_shared_library("extract_mvs_with_frames.so")
    return _emv_frames


class MVTractus:
    """Thin Python wrapper around the MV-Tractus FFmpeg shared library."""

    def __init__(self, videopath: Union[str, Path], output_dir: Union[str, Path] = "./output/mv") -> None:
        self.videopath = Path(videopath)
        self.output_dir = Path(output_dir)
        self._ensure_output_dir()

    def get_motion_vectors(self) -> None:
        """Extract motion vectors for the configured video path."""
        _get_emv().extract_motion_vectors(str(self.videopath).encode())

    def _ensure_output_dir(self) -> None:
        """Ensure the expected output directory exists on disk."""
        self.output_dir.mkdir(parents=True, exist_ok=True)


class MVTractusWithFrames:
    """Python wrapper that also dumps RGB frames alongside motion vectors."""

    def __init__(self, videopath: Union[str, Path], output_dir: Union[str, Path] = "./output/mv") -> None:
        self.videopath = Path(videopath)
        self.output_dir = Path(output_dir)
        self.frames_dir = Path("./output/frames")  # fixed in C implementation
        self._ensure_output_dirs()

    def get_motion_vectors_and_frames(self) -> None:
        """Extract motion vectors and frames for the configured video path."""
        _get_emv_frames().extract_motion_vectors(str(self.videopath).encode())

    def _ensure_output_dirs(self) -> None:
        """Ensure the expected output directories exist on disk."""
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.frames_dir.mkdir(parents=True, exist_ok=True)


__all__ = ["MVTractus", "MVTractusWithFrames"]
