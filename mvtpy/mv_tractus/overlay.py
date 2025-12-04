from __future__ import annotations

import json
import math
import sys
from pathlib import Path
from typing import Iterable, List, Tuple

try:
    from PIL import Image, ImageDraw
except ModuleNotFoundError as exc:  # pragma: no cover - dependency guard
    raise ImportError("Pillow is required for overlay generation. Install with `pip install pillow`.") from exc

Color = Tuple[int, int, int]


def _draw_arrow(draw: ImageDraw.ImageDraw, start: Tuple[int, int], end: Tuple[int, int], color: Color, width: int = 2, tip_length: float = 0.25) -> None:
    """Draw an arrow (line + head) between two points."""
    draw.line([start, end], fill=color, width=width)
    dx, dy = end[0] - start[0], end[1] - start[1]
    length = math.hypot(dx, dy)
    if length == 0:
        return
    ux, uy = dx / length, dy / length
    tip_size = tip_length * length
    left = (end[0] - ux * tip_size - uy * tip_size * 0.5, end[1] - uy * tip_size + ux * tip_size * 0.5)
    right = (end[0] - ux * tip_size + uy * tip_size * 0.5, end[1] - uy * tip_size - ux * tip_size * 0.5)
    draw.polygon([end, left, right], fill=color)


def overlay_motion_vectors(
    frames_dir: Path | str = "output/frames",
    mv_dir: Path | str = "output/mv",
    out_dir: Path | str = "output/overlay",
    scale: float = 1.0,
    thickness: int = 2,
    color_p: Color = (0, 255, 0),
    color_b: Color = (255, 0, 0),
    stride: int = 1,
    max_vectors_per_frame: int | None = None,
) -> List[Path]:
    """
    Overlay motion vectors onto saved frames and write PNGs.

    Args:
        frames_dir: Directory containing frame<idx>.ppm files.
        mv_dir: Directory containing <idx>.json motion-vector files.
        out_dir: Destination directory for overlay PNGs.
        scale: Multiplier for dx/dy to improve visibility.
        thickness: Arrow line thickness.
        color_p: RGB color for P-frames (source >= 0).
        color_b: RGB color for B-frames (source < 0).
        stride: draw every Nth vector to reduce workload (default 1 = all).
        max_vectors_per_frame: cap the number of vectors drawn per frame (None for unlimited).

    Returns:
        List of written overlay file paths.
    """
    frames_path = Path(frames_dir)
    mv_path = Path(mv_dir)
    out_path = Path(out_dir)
    out_path.mkdir(parents=True, exist_ok=True)

    written: List[Path] = []
    json_files: Iterable[Path] = sorted(mv_path.glob("*.json"), key=lambda p: int(p.stem))
    if not json_files:
        raise FileNotFoundError(f"No motion-vector JSON files found under {mv_path.resolve()}")

    missing_frames = 0
    for json_file in json_files:
        idx = int(json_file.stem)
        candidates = [
            frames_path / f"frame{idx}.ppm",
            frames_path / f"{idx}.ppm",
        ]
        frame_file = next((c for c in candidates if c.exists()), None)
        if frame_file is None:
            missing_frames += 1
            continue

        frame = Image.open(frame_file).convert("RGB")
        draw = ImageDraw.Draw(frame)

        with json_file.open() as f:
            mvs = json.load(f)

        drawn = 0
        for idx_mv, mv in enumerate(mvs):
            if stride > 1 and (idx_mv % stride):
                continue
            if max_vectors_per_frame is not None and drawn >= max_vectors_per_frame:
                break
            x, y = int(mv["src_x"]), int(mv["src_y"])
            dx, dy = mv["dx"], mv["dy"]
            end = (int(x + dx * scale), int(y + dy * scale))
            color = color_b if mv.get("source", 0) < 0 else color_p
            _draw_arrow(draw, (x, y), end, color=color, width=thickness, tip_length=0.25)
            drawn += 1

        overlay_file = out_path / f"overlay_{idx}.png"
        frame.save(overlay_file)
        written.append(overlay_file)

    if missing_frames:
        sys.stderr.write(f"Skipped {missing_frames} frames missing from {frames_path.resolve()}\n")

    return written
