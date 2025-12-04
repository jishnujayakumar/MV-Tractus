from pathlib import Path

from setuptools import find_packages, setup

HERE = Path(__file__).resolve().parent
README = (HERE / "README.md").read_text(encoding="utf-8")


setup(
    name="mv-tractus",
    version="1.0.8",
    description="Extract motion vectors from H264/MPEG video streams",
    long_description=README,
    long_description_content_type="text/markdown",
    author="Jishnu Jaykumar P",
    author_email="jishnu.jayakumar182@gmail.com",
    url="https://github.com/jishnujayakumar/MV-Tractus",
    project_urls={
        "Source": "https://github.com/jishnujayakumar/MV-Tractus",
        "Issues": "https://github.com/jishnujayakumar/MV-Tractus/issues",
    },
    packages=find_packages(),
    package_data={"mv_tractus": ["extract_mvs.so", "extract_mvs_with_frames.so"]},
    include_package_data=True,
    python_requires=">=3.8",
    keywords=["motion-vectors", "video compression", "opticalflow", "computer vision"],
    install_requires=["Pillow>=8"],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: POSIX :: Linux",
        "Topic :: Multimedia :: Video",
    ],
)
