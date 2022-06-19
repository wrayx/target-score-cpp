# target-scores

Device & Environment requirements have been specified in `CMakeList.txt`.

Build program
```
mkdir build && cd build
cmake ..
make
```

Run program

```
./main
```

## Stage 1: Align Image taken from the camera

- `ImageAlignment.hpp`
- `ImageAlignment.cpp`

Input images:

|      Image taken       |     Reference target board     |
| :--------------------: | :----------------------------: |
| ![](output/shot_5.JPG) | ![](output/aligned_shot_0.JPG) |

### Extract matching features
|          Matches          |          Good Matches          |
| :-----------------------: | :----------------------------: |
|  `matches.size() = 500`   |   `good_matches.size() = 38`   |
| ![](output/match_img.png) | ![](output/good_match_img.png) |


### Warp Perspective

After the warpping, the output is given as below.

<img src="output/warped_img.png" alt="output" width="40%"/>

## Stage 2: Detecting information from the target board

- `ShootingScore.hpp`
- `ShootingScore.cpp`

Output: 

<img src="output/input.JPG" alt="output" width="50%"/><img src="output/output.png" alt="output" width="50%"/>