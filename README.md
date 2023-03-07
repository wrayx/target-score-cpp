# Visual Based Shot Tracking System For Target Shooting

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

More specific information about the environment set up can be found here: [Configure the VSCode Environment for C++ on MacOS
](https://wrayx.uk/posts/configure-vscode-env-for-cpp-on-macos/)

### Notes

The project and documentation are still under development. The drafted doc can be viewed from here: [wrayx.uk - A Visual Based Shot Tracking System For Target Shooting](https://wrayx.uk/posts/shot-tracking-system/)

Currently, the detection algorithms are implemented using the openCV library. But we are working on implementing some of the methods from scratch so that the programme can run faster and be better tailored to this specific application scenario.

### TODOs

- [ ] Sound detection.
- [ ] Image capturing with raspi cam module.
- [ ] Shot sample image collection and learning.
- [ ] The detection system is sensitive to lighting conditions, so how do we automatically tune the image processing parameters relative to the image contrast and brightnesses?

## Set-up

The current setup is as follows:

1. An iPhone camera to take a photo of the target board
2. The target board is placed inside a black box to maintain a clean background and consistent lighting conditions

<p align="center">
<img src="output/project_set_up.jpg" alt="output" width="500px"/>
<img src="output/camera_set_up.jpg" alt="output" width="500px"/>
</p>

## Stage 1: Align the Image taken from the camera

- `ImageAlignment.hpp`
- `ImageAlignment.cpp`

Here two different methods are used: outline shape detection and interest point matching. The former approach is cleaner and faster to execute but fails on different lighting and background conditions; more improvements on this are needed. The latter one is more robust in different environments. So we'll keep both of them for now and decide later which one should be kept.

### Method 1 - Outline Shape Detection

**Input:** Image taken from the camera
<p align="center">
<img src="output/shot_1.JPG" alt="output" width="500px"/>
</p>

 1. Find polygon contours with 4 corners (quadrilaterals).
    <p align="center">
    <img src="output/outline.png" alt="output" width="500px"/>
    </p>

1. Extract the edge of the target board by their **hierarchies**. 

    <p align="center">
    <img src="output/outline_selected.png" alt="output" width="500px"/>
    </p>

2. Selected contour seen from the input image

    <p align="center">
    <img src="output/outline_selected_input_img.png" alt="output" width="500px"/>
    </p>

3. Find homography and warp perspective of the selected quadrilateral to a 1080x1080 px image, which gives an output as follows:

    <p align="center">
    <img src="output/warped_img.png" alt="output" width="300px"/>
    </p>


### Method 2 - Interest Points Matching

**Input:**

- Image taken from the camera

    <p align="center">
    <img src="output/shot_1.JPG" alt="output" width="500px"/>
    </p>

- Reference board Image

    <p align="center">
    <img src="output/aligned_shot_0.JPG" alt="output" width="300px"/>
    </p>

#### Steps:

1. Matching interest points
   
    |         All Matches       |         Good Matches           |
    | :-----------------------: | :----------------------------: |
    | ![](output/match_img.png) | ![](output/good_match_img.png) |


2. Warped Perspective. After the warping, the output is given as below.

    <p align="center">
    <img src="output/warped_img.png" alt="output" width="350px"/>
    </p>

## Stage 2: Detecting information from the target board

- `ShootingScore.hpp`
- `ShootingScore.cpp`

After the original image has been warped, it is processed. Now we can extract and calculate the shots and scoring information from it.
The score is approximated by the differences in the image from the next one:

|         Before the shot       |          After the shot        |
| :---------------------------: | :----------------------------: |
| ![](output/last_img_blur.png) | ![](output/src_img_blur.png)   |
    
The differences from the above two images are as follows, which indicate the new shot location.
There are a few issues with this, as the shots on paperboard are not always easily visible. Depending on the paper density, the shape of the shot can vary.

<p align="center">
<img src="output/img_diff.png" alt="output" width="350px"/>
</p>

From the diff image and the original shot image, shot location, contour, and other info can be calculated: 

<p align="center">
<img src="output/shot_contour.png" alt="output" width="350px"/>
<img src="output/shot_location.png" alt="output" width="350px"/>
</p>

Including the target center, which was calculated by detecting the outermost circle in the image:

<p align="center">
<img src="output/target_circle.png" alt="output" width="350px"/>
</p>

According to the shot location and centre location, a score is obtained, and we output the result image as follows.

Final Output: 

<p align="center">
<img src="output/output.png" alt="output" width="350px"/>
</p>
