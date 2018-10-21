# MV-Tractus
A simple tool to extract motion vectors from mpeg videos.

## If you use this tool, please cite.

Jishnu P. (2018, October 21). jishnujayakumar/MV-Tractus: This is the first stable release. (Version 1.0). Zenodo. http://doi.org/10.5281/zenodo.1467851

> @misc{jishnu_p_2018_1467851,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;author = {Jishnu P},  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;title = {{jishnujayakumar/MV-Tractus: This is the first stable release.}},  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;month = oct,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;year = 2018,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;doi = {10.5281/zenodo.1467851},  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;url = {https://doi.org/10.5281/zenodo.1467851}
}

### [![DOI](https://zenodo.org/badge/131159983.svg)](https://zenodo.org/badge/latestdoi/131159983)

## Installation and bootstrapping

1. `git clone https://github.com/jishnujayakumar/MV-Tractus.git` 
2. `cd MV-Tractus`
3. `./install_ffmpeg.sh` (You can also install FFmpeg using the link [here](http://embedonix.com/articles/linux/installing-ffmpeg-from-source-on-ubuntu-14-0-4/))
4. `sudo cp ffmpeg.conf /etc/ld.so.conf.d/ffmpeg.conf && sudo ldconfig`
5. `./compile` OR You can use the compile_command.txt to generate the binary file.
6. mkdir -p output/mv/
7. `./extract_mvs <video-file-path>`.
8. And there you go. The output will be saved in './output/mv/'.

`pip install mv-tractus` (Coming Soon).
