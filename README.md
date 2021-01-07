# MV-Tractus
A simple tool to extract motion vectors from H264 encoded video sources.

**NOTE**: This was a side project and never intended to be open sourced which is basically the reason behind the inappropriate coding style. This was just an experiment to build a tool to extract motion-vectors by building a wrapper around FFmpeg. This tool only supports motion-vector extraction. If you are intending to use it in any other context then I am afraid this is not the right tool that you are looking for. FFmpeg core libraries have not been altered. 

**Disclaimer**: The authors don't claim anything regarding the performance of the tool.

## If you use this tool, please cite.

Jishnu P. (2018, October 21). MV-Tractus:  A simple tool to extract motion vectors from H264 encoded video sources. 
http://doi.org/10.5281/zenodo.1467851

```bibtex
@article{jishnu_mvtractus,
  title={MV-Tractus:  A simple tool to extract motion vectors from H264 encoded video sources.},
  author={Jishnu P, Praneet Singh},
  journal={GitHub. Note: https://github.com/jishnujayakumar/MV-Tractus},
  volume={1},
  year={2018},
  url = { https://github.com/jishnujayakumar/MV-Tractus }
}
```

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
[![Watch the video](https://i.ytimg.com/vi/-e02hCdQ0_w/mq1.jpg)](https://www.youtube.com/embed/-e02hCdQ0_w)

#### Python Usage (extract_mvs_with_frames)
Coming Soon...

#### NOTE: Due to the lack of time, I am not able to maintain this project. I would like to request the users and contributors to help each other in all possible ways by joining [Slack](https://join.slack.com/t/mv-tractus/shared_invite/zt-jnyh53qo-o6xMN_AdZ1DyeusCERVMOA).
