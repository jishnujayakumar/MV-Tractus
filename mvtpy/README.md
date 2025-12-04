# MV-Tractus
A simple tool for motion vector extraction from mpeg videos.

## Python usage

```bash
python -m pip install -e mvtpy --upgrade
```

```python
from mv_tractus import MVTractus

mvt = MVTractus("/path/to/video.mp4")  # outputs to ./output/mv by default
mvt.get_motion_vectors()
```

If you also want extracted RGB frames alongside motion vectors, use:

```python
from mv_tractus import MVTractusWithFrames

mvt = MVTractusWithFrames("/path/to/video.mp4")
mvt.get_motion_vectors_and_frames()  # writes JSON to ./output/mv and PPM frames to ./output/frames
```

Overlay vectors on saved frames (uses Pillow):

```python
from mv_tractus import overlay_motion_vectors

overlay_motion_vectors(
    frames_dir="output/frames",
    mv_dir="output/mv",
    out_dir="output/overlay",
    scale=1.0,
    thickness=2,
    stride=2,                   # draw every 2nd vector for speed
    max_vectors_per_frame=500,  # cap arrows per frame for speed; use None for all vectors
)
```

For all vectors (slow): set `stride=1` and `max_vectors_per_frame=None`.

Native/ffmpeg visualization (fast, all vectors, no Python):
```bash
ffmpeg -flags2 +export_mvs -debug vis_mv=pf+bf -i input.mp4 -c:v libx264 -crf 18 -preset fast output_with_mv.mp4
```

## If you use this tool, please cite.

> @misc{j_padalunkal_2018_216985,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;author = {J. Padalunkal},  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;title = {MV-Tractus},  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;month = apr,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;year = 2018,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;doi = {10.5072/zenodo.216985},  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;url = {https://doi.org/10.5072/zenodo.216985}
}

### [![DOI](https://sandbox.zenodo.org/badge/DOI/10.5072/zenodo.216985.svg)](https://doi.org/10.5072/zenodo.216985)
