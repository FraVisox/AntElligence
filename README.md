# AntElligence
Hive game engine developed by Francesco Visona' and Matteo Canton.



## Useful resources

### Changelog

All notable changes to this project will be documented in the file [CHANGELOG.md](CHANGELOG.md).
Useful links:
* [Why keep a changelog?](https://keepachangelog.com/en/1.0.0/)
* [Semantic versioning of changelog](https://semver.org/)

### Hive rules

* [Official rules for Hive](https://docs.google.com/document/u/1/d/e/2PACX-1vRncDO_yAcktXwer0OG46Tp_dqT6ubMcGb_l7nb7BAFkAcPqXZatTUbrsc_5srwoSAW3Xm7-HuBqjDM/pub)

## Interesting resources
* [Computational complexity of plane tickets](http://www.demarcken.org/carl/papers/ITA-software-travel-complexity/ITA-software-travel-complexity.pdf)



## 1st phase: UHP compliant engine

* [Introduction to UHP compliance (Mzinga)](https://github.com/jonthysell/Mzinga/wiki)
* [Riccardo's python engine](https://github.com/Crystal-Spider/hivemind)
* [Construct hexagonal grids](https://www.redblobgames.com/grids/hexagons/)

### Install Anaconda

Setting up the environment is pretty easy:

1. Install [Anaconda](https://www.anaconda.com/download/success).
2. Open the project root directory and run the following command:
   ```powershell
   conda env create -n <my_env> --file environment.yaml
   ```
   `<env>` can be any name you want.


### Usage

There are two ways to use this Hive engine:

1. Run [`engine.py`](/src/engine.py) from the command line or with VSCode and start using the console to interact with it.  
   The engine will be fully functional, but there won't be any graphical interface.
2. Use the included `AntElligenceEngine.exe` (or build it yourself) along with [MzingaViewer](https://github.com/jonthysell/Mzinga/wiki/MzingaViewer).  
   To do this, move `AntElligenceEngine.exe` into the same directory as `MzingaViewer.exe` and then follow the instructions [here](https://github.com/jonthysell/Mzinga/wiki/BuildingAnEngine), specifically `step 2 > iii`.

To install what I've made simply run the following:
```
pyinstaller ./src/engine.py --name AntElligenceEngine --noconsole --onefile
```


## 2nd phase: AI engine

* [Reinforcement Learning on Hive](https://research.spec.org/icpe_proceedings/2022/proceedings/p145.pdf)
* [Monte Carlo strategy for Hive](https://theses.liacs.nl/pdf/NilsOUT.pdf)