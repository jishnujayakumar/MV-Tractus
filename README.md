# MV-Tractus
A simple tool to extract motion vectors from mpeg videos.

## If you use this tool, please cite.

Jishnu P. (2018, October 21). MV-Tractus:  A simple and fast tool to extract motion vectors from H264 encoded video streams. (Version 1.0). Zenodo. http://doi.org/10.5281/zenodo.1467851

> @misc{jishnu_p_2018_1467851,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;author = { Jishnu P },  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;title = { MV-Tractus:  A simple and fast tool to extract motion vectors from H264 encoded video streams. },  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;month = oct,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;year = 2018,  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;doi = { 10.5281/zenodo.1467851 },  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;url = { https://doi.org/10.5281/zenodo.1467851 }
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

## Tutorials
#### Installation, Setup and Usage (extract_mvs)
[![Watch the video](https://i.ytimg.com/vi/qpwTdxsBebk/hqdefault.jpg)](https://www.youtube.com/embed/qpwTdxsBebk)

#### Usage (extract_mvs_with_frames)
[![Watch the video](https://i9.ytimg.com/vi/-e02hCdQ0_w/mq1.jpg?sqp=COi3mOkF&rs=AOn4CLDsQiHs5qBYcQm2bM4PKpPewUSPiA)](https://www.youtube.com/embed/e02hCdQ0_w)

#### Python Usage (extract_mvs_with_frames)
Coming Soon
