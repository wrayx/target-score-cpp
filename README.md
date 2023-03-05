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

## Project Set up

The current set up is as follows:

1. An iPhone camera to take photo of the target board
2. The target board is placed inside a black box to maintain a clean background and consistent lighting condition

<p style="text-align:center;">
<img src="output/project_set_up.jpg" alt="output" width="500px"/>
<img src="output/camera_set_up.jpg" alt="output" width="500px"/>
</p>

## Stage 1: Align Image taken from the camera

- `ImageAlignment.hpp`
- `ImageAlignment.cpp`

### Method 1 - Outline Shape Detection

**Input:** Image taken from the camera
<p style="text-align:center;">
<img src="output/shot_1.JPG" alt="output" width="500px"/>
</p>

 1. Find polygon contours with 4 corners (quadrilaterals).
    <p style="text-align:center;">
    <img src="output/outline.png" alt="output" width="500px"/>
    </p>

1. Extract the edge of the target board by their **hierarchies**. 

    <p style="text-align:center;">
    <img src="output/outline_selected.png" alt="output" width="500px"/>
    </p>

2. Selected contour seen from the input image

    <p style="text-align:center;">
    <img src="output/outline_selected_input_img.png" alt="output" width="500px"/>
    </p>

3. Find homography and warp perspective of the selected quadrilateral to a 1080x1080 px image, which gives an output as follows:

    <p style="text-align:center;">
    <img src="output/warped_img.png" alt="output" width="300px"/>
    </p>

---

### Method 2 - Interest Points Matching

**Input:**

- Image taken from the camera

    <p style="text-align:center;">
    <img src="output/shot_1.JPG" alt="output" width="500px"/>
    </p>

- Reference board Image

    <p style="text-align:center;">
    <img src="output/aligned_shot_0.JPG" alt="output" width="300px"/>
    </p>

#### Steps:

1. Matching interest points
   
    |          Matches          |          Good Matches          |
    | :-----------------------: | :----------------------------: |
    |  `matches.size() = 500`   |   `good_matches.size() = 38`   |
    | ![](output/match_img.png) | ![](output/good_match_img.png) |


2. Warp Perspective. After the warpping, the output is given as below.

    <p style="text-align:center;">
    <img src="output/warped_img.png" alt="output" width="300px"/>
    </p>

## Stage 2: Detecting information from the target board

- `ShootingScore.hpp`
- `ShootingScore.cpp`

After the original image has been warpped the processed. Now we can extract and calculate the shots and scoring information from it. 
The score is approximated from the differences in the image from the next one: 

|         Before the shot       |          After the shot        |
| :---------------------------: | :----------------------------: |
| ![](output/last_img_blur.png) | ![](output/src_img_blur.png)   |
    
The differences from the above two images are as follows:
<p style="text-align:center;">
<img src="output/img_diff.png" alt="output" width="350px"/>
</p>

From there we calculate the scores and other infos: 
<p style="text-align:center;">
<img src="output/shot_contour.png" alt="output" width="350px"/>
<img src="output/shot_location.png" alt="output" width="350px"/>
</p>

including the target centre by detecting the outermost circle from the image:

<p style="text-align:center;">
<img src="output/target_circle.png" alt="output" width="350px"/>
</p>

Final Output: 
<p style="text-align:center;">
<img src="output/output.png" alt="output" width="350px"/>
</p>

## Additionally

The project itself and the complete set of documentation is still in development. The draft of the doc can be viewed from here: [wrayx.uk - The Development of the Target Scoring System](https://wrayx.uk/posts/target-scoring-system/)

### TODO

- [ ] Sound detection.
- [ ] Image capturing with raspi cam module.
- [ ] Shot sample image collection and learning.
- [ ] The detection system is sensitive to lighting conditions, so how to automatic tuning the image processing parameters relative to the image contrast and brightnesses.
