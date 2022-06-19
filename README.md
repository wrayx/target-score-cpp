# target-scores

Run program

```
cd build
./main
```

## Stage 1: Align Image taken from the camera

- `ImageAlignment.hpp`
- `ImageAlignment.cpp`

Input images:

1. Image taken 

    <img src="output/shot_5.JPG" alt="output" width="70%"/>

2. Reference target board imge

    <img src="output/aligned_shot_0.JPG" alt="output" width="40%"/>

### Extract matching features
```
matches.size() = 500
```
<img src="output/match_img.png" alt="output" width="80%"/>

Obtain good matching points with the ratio test
```
good_matches.size() = 38
```
<img src="output/good_match_img.png" alt="output" width="80%"/>

### Warp Perspective

<img src="output/shot_5.JPG" alt="output" width="53%"/>
<img src="output/warped_img.png" alt="output" width="40%"/>

## Stage 2: Detecting information from the target board

- `ShootingScore.hpp`
- `ShootingScore.cpp`

Output: 

<img src="output/input.JPG" alt="output" width="50%"/><img src="output/output.png" alt="output" width="50%"/>