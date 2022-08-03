# Target Scoring System

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

More specific information about the environment set up can be found here: [wrayx.uk](https://wrayx.uk/posts/configure-vscode-env-for-cpp-on-macos/)

## Stage 1: Align Image taken from the camera

- `ImageAlignment.hpp`
- `ImageAlignment.cpp`

### Method 1 - Outline Shape Detection

**Input:** Image taken from the camera

<img src="output/shot_1.JPG" alt="output" width="500px"/>

 1. Find polygon contours with 4 corners (quadrilaterals).

    <img src="output/outline.png" alt="output" width="500px"/>

1. Extract the edge of the target board by their **hierarchies**. 

    <img src="output/outline_selected.png" alt="output" width="500px"/>

2. Selected contour seen from the input image

    <img src="output/outline_selected_input_img.png" alt="output" width="500px"/>

3. Find homography and warp perspective of the selected quadrilateral to a 1080x1080 px image, which gives an output as follows:

    <img src="output/warped_img.png" alt="output" width="300px"/>

---

### Method 2 - Interest Points Matching

**Input:**

- Image taken from the camera

    <img src="output/shot_1.JPG" alt="output" width="500px"/>

- Reference board Image

    <img src="output/aligned_shot_0.JPG" alt="output" width="300px"/>

#### Steps:

1. Matching interest points
   
    |          Matches          |          Good Matches          |
    | :-----------------------: | :----------------------------: |
    |  `matches.size() = 500`   |   `good_matches.size() = 38`   |
    | ![](output/match_img.png) | ![](output/good_match_img.png) |


2. Warp Perspective. After the warpping, the output is given as below.

    <img src="output/warped_img.png" alt="output" width="300px"/>

## Stage 2: Detecting information from the target board

- `ShootingScore.hpp`
- `ShootingScore.cpp`

Output: 

<img src="output/output.png" alt="output" width="300px"/>