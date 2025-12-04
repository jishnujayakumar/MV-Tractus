#!/bin/bash
mkdir v2
cd v2
mkdir output
cd output
mkdir frames mv
cd ../..
python scripts/extract_mvs_with_frames.py "$1"
