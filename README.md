# mklifs
LIFS creation tool.

## About LIFS
LIFS (List File System) is a file system for FSIX operating system project.
See [this link](LIFS.md) for more information about LIFS.

## About this tool
This is a cross-platform tool for creating disk images with LIFS. 
It supports LIFS partitioning, converting files to LIFS units and adding 
SXB0 (boot sector and second sector of [SXB](https://github.com/eastev/sxb)).
After creating a partition you need a directory with something you want to
write on this partition. Adding SXB0 is necessary for system disks, but you 
may add no bootloaders for other (not system) disks.

For more information about LIFS creation process use: `mklifs -h`

## Install this tool
To install and assemble this tool you need: `gcc`, `make` and `git`

Follow these simple steps:
* Clone this repo: `git clone https://github.com/eastev/mklifs`
* Move to `mklifs` directory: `cd mklifs`
* Assemble this tool: `make`
* Install this tool to your system: `sudo make install`

## Uninstall this tool
To uninstall this tool use: `sudo make uninstall` or `sudo rm /bin/mklifs`

## Other
This tool is a part of [TSIX](https://github.com/eastev/tsix) project.


Copyright © 2020 Alexander (eastev) E. <https://github.com/eastev>
For any questions write on `eastevrud31@gmail.com`
