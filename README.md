# MV-Tractus
A simple tool for motion vector extraction from mpeg videos.

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

## Installation and bootstrapping

1. Install FFmpeg using the link [here](http://embedonix.com/articles/linux/installing-ffmpeg-from-source-on-ubuntu-14-0-4/).
2. Use the compile_command.txt to generate the binary file.
3. Create a directory and a sub-directory within the current directory. ('./output/mv/') for the first time.
4. `./extract_mvs <video-file-path>`.
5. And there you go. The output will be saved in './output/mv/'.

`pip install mv-tractus` (Under construction).
