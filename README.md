# MV-Tractus 🚀
Extract motion vectors from H264/MPEG streams. CLI tools wrap FFmpeg; the Python package (`mv-tractus`) gives you the same power with optional frame extraction and overlays.

**Authors:** Jishnu Jaykumar Padalunkal, Praneet Singh  
**Note:** FFmpeg is unmodified; performance is best effort.

## Quick Start (CLI) ⚡
1. `git clone https://github.com/jishnujayakumar/MV-Tractus.git && cd MV-Tractus`
2. Build or point to FFmpeg; ensure libs are discoverable (see `install_ffmpeg.sh`, `ffmpeg.conf`).
3. `./compile` (outputs to `bin/`)
4. `mkdir -p output/mv output/frames`
5. Run:
   - Vectors: `./bin/extract_mvs <video> [output_dir]`
   - Vectors + frames: `./bin/extract_mvs_with_frames <video> [output_dir]`
   - Default outputs: JSON → `output/mv`, PPM frames → `output/frames` when `output_dir` is omitted.

Fast native visualization (all vectors, no Python):
```bash
ffmpeg -flags2 +export_mvs -debug vis_mv=pf+bf -i input.mp4 -c:v libx264 -crf 18 -preset fast output_with_mv.mp4
```

## Python Usage 🐍
Install:
```bash
pip install mv-tractus
```

- Motion vectors:
  ```python
  from mv_tractus import MVTractus
  mvt = MVTractus("/path/to/video.mp4")
  mvt.get_motion_vectors()  # JSON -> ./output/mv
  ```
- Motion vectors + frames:
  ```python
  from mv_tractus import MVTractusWithFrames
  mvt = MVTractusWithFrames("/path/to/video.mp4")
  mvt.get_motion_vectors_and_frames()  # JSON -> ./output/mv, frames -> ./output/frames
  ```
- Overlay vectors on saved frames (Pillow):
  ```python
  from mv_tractus import overlay_motion_vectors

  overlay_motion_vectors(
      frames_dir="output/frames",
      mv_dir="output/mv",
      out_dir="output/overlay",
      scale=1.0,
      thickness=2,
      stride=2,                   # draw every 2nd vector for speed
      max_vectors_per_frame=500,  # cap per frame; set to None for all vectors
  )
  ```
  For all vectors, use `stride=1` and `max_vectors_per_frame=None`.

## Repository Layout 📂
- `src/c` – FFmpeg-based C sources (`extract_mvs.c`, `extract_mvs_with_frames.c`, helpers)
- `src/cpp` – C++ OpenCV/FFmpeg experiments (`efmvs.cpp`, etc.)
- `scripts` – Utility scripts
- `mvtpy` – Python package source for `mv-tractus`
- `include` – Vendored FFmpeg headers
- `output` – Default location for extracted data

## Community 🤝
Need help or want to contribute? Join the Slack: https://join.slack.com/t/mv-tractus/shared_invite/zt-lmjczfaf-TX_JYYkKIlBeySkwPcX3xg

## Citation 📚
If you use this tool, please cite:

Jishnu P, & Singh, Praneet. (2018, October 21). MV-Tractus: A simple tool to extract motion vectors from H264 encoded video sources (Version 2.0). Zenodo.  
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4422613.svg)](https://doi.org/10.5281/zenodo.4422613)

```bibtex
@software{jishnu_p_2018_4422613,
  author       = {Jishnu P and
                  Singh, Praneet},
  title        = {{MV-Tractus: A simple tool to extract motion 
                   vectors from H264 encoded video sources}},
  month        = oct,
  year         = 2018,
  publisher    = {Zenodo},
  version      = {2.0},
  doi          = {10.5281/zenodo.4422613},
  url          = {https://doi.org/10.5281/zenodo.4422613}
}
```

## Tutorials 🎥
- Installation, setup, and usage (extract_mvs): [YouTube](https://www.youtube.com/embed/qpwTdxsBebk)
- Usage (extract_mvs_with_frames): [YouTube](https://www.youtube.com/embed/-e02hCdQ0_w)
