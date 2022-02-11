## Writeup

---

**Vehicle Detection Project**

The goals / steps of this project are the following:

* Perform a Histogram of Oriented Gradients (HOG) feature extraction on a labeled training set of images and train a classifier Linear SVM classifier
* Apply a color transform and append binned color features, as well as histograms of color, to the HOG feature vector. 
* Implement a sliding-window technique and use a trained classifier to search for vehicles in images.
* Run a pipeline on a video stream (test_video.mp4 and project_video.mp4) and create a heat map of recurring detections frame by frame to reject outliers and follow detected vehicles.
* Estimate a bounding box for vehicles detected.

[//]: # (Image References)
[sample_car_not_car]: ./output_images/sample_car_not_car.png
[hog_img_car]: ./output_images/hog_features_car.png
[hog_img_notcar]: ./output_images/hog_features_notcar.png
[search_windows]: ./output_images/search_windows.png
[result_0]: ./output_images/result_0.png
[result_1]: ./output_images/result_1.png
[result_2]: ./output_images/result_2.png
[frame_heatmap_1]:./output_images/frame_heatmap_1.png
[frame_heatmap_2]:./output_images/frame_heatmap_2.png
[frame_heatmap_3]:./output_images/frame_heatmap_3.png
[frame_heatmap_4]:./output_images/frame_heatmap_4.png
[frame_heatmap_5]:./output_images/frame_heatmap_5.png
[heatmap_bbox_5]:./output_images/heatmap_bbox_5.png
[project_video_out]: ./output_video.avi

## [Rubric](https://review.udacity.com/#!/rubrics/513/view) Points
### Here I will consider the rubric points individually and describe how I addressed each point in my implementation.  

---
### Writeup / README


### Histogram of Oriented Gradients (HOG)

#### 1. Explain how (and identify where in your code) you extracted HOG features from the training images.

> The code for this step is contained in the third code cell of the Jupyter notebook (cell #3 ).  

> I started by reading in all the `vehicle` and `non-vehicle` images. It appears that the number of vehicle images is slightly less than the non-vehicle images. To avoid bias by the classifier later, I selected images randomly from other folders and placed it in 'Additional' folder. The number of vehicle and non-vehicle images is 8968 each. Here is an example of one of each of the `vehicle` and `non-vehicle` classes:

> ![alt text][sample_car_not_car]

> I then explored different color spaces and different `skimage.hog()` parameters (`orientations`, `pixels_per_cell`, and `cells_per_block`).  I grabbed random images from each of the two classes and displayed them to get a feel for what the `skimage.hog()` output looks like.

> Here is an example using the `YCrCb` color space and HOG parameters of `orientations=8`, `pixels_per_cell=(8, 8)` and `cells_per_block=(2, 2)`:


![alt text][hog_img_car]
![alt text][hog_img_notcar]

#### 2. Explain how you settled on your final choice of HOG parameters.

> I tried various combinations of parameters and color spaces; three different values for 'orientation' parameter (5,8,9), two values for pixels_per_cell (8 and 16) and five color spaces (YCrCb, RGB, HSV, LUV, HLS). I got better result with 'YCrCb' color space and HOG parameters of 'orientation=8','pixels_per_cell=(8,8)' and 'cells_per_block=(2,2)'. The combination of different HOG parameters and color spaces produce slightly different training accuracies (about 98% on average) and various training times (8 - 20 seconds). I picked the combination that results in the highest training accuracy (99%) and relatively faster training time (17 s) as shown below.

> Effect of HOG parameter changes on training accuracy and YCrCb color space
>
 |orientation   |  pix_per_cell  | cells_per_block  |training accuracy| training time |
 |:------------:|:--------------:|:----------------:|:---------------:|:-------------:|
 |   9          |   8            |  2               | 98.98           | 16.93 s       |
 |   8          |   8            |  2               | 98.86           | 17.35 s       |
 |   8          |  16            |  2               | 97.99           |  7.96 s       |
 |   5          |  16            |  2               | 97.98           | 8.53 s        |
 |   5          |   8            |  2               | 98.83           | 13.75 s       |
              
> Color spaces
>
 | Colorspace | Training accuracy| Training time|
 |:----------:|:----------------:|:-------------|
 | YCrCb      | 99.02            |  16.78 s     |
 |  RGB       | 98.13            |  20.88 s     |
 | HSV        | 98.33            |  18.73 s     |
 | LUV        | 98.72            |  16.51 s     |
 | HLS        | 98.41            |  20.30 s     |



#### 3. Describe how (and identify where in your code) you trained a classifier using your selected HOG features (and color features if you used them).

> The classifier I used is linear SVM and the feature vector contains concatenated spatially binned color feature, histogram of colors and HOG features. All of those feature vectors are stacked and normalized using the `StandardScalar` function from sklearn. Then, the normalized data is shuffled and divided in to training (80%) and testing(20%) sets using sklearn's `train_test_split()` function. Finally, the `linearSVC()` classifier is trained, tested, and saved in pickle form. This process can be found in cell #5 of the jupyter notebook (vehicle_detection.ipynb).


### Sliding Window Search

#### 1. Describe how (and identify where in your code) you implemented a sliding window search.  How did you decide what scales to search and how much to overlap windows?

> I tried the sliding window approach first with window sizes of 64x64, 96x96 and 128x128 and 50% window overlapping. It has reasonable performance. However, the amount of time it takes to make a single prediction is large. Therefore, I adopted the sub-sampling window search, as presented in the course material, which performs better in terms of classification accuracy and speed. I chose four scales to cover wide range of distances from the camera. To catch cars far from the camera, a scale of 0.5 is choosen (32x32 window) and for the closer cars a scale of 2.0 (128x128) is chosen. Two more scales 1.0 (64x64) and 1.5 (96x96) are used to accomodate cars in various ranges from the camera position. The search windows are shown below. 
Scales:  [0.5, 1, 1.5, 2.0], ystart = [400, 400, 400, 400], ystop = [480, 600, 680, 680]


![alt text][search_windows]

#### 2. Show some examples of test images to demonstrate how your pipeline is working.  What did you do to optimize the performance of your classifier?

> Ultimately I searched on four scales using YCrCb 3-channel HOG features plus spatially binned color and histograms of color in the feature vector, which provided a nice result. The reason that I used four scales is to improve detection accuracy at the cost of prediction speed per image frame. Here are some example images:

![alt text][result_0]
![alt text][result_1]
![alt text][result_2]

---

### Video Implementation

#### 1. Provide a link to your final video output.  Your pipeline should perform reasonably well on the entire project video (somewhat wobbly or unstable bounding boxes are ok as long as you are identifying the vehicles most of the time with minimal false positives.)

> Here's a [project_video_out](./output_video.avi). In this video there are about 6 false positives and few misses.


#### 2. Describe how (and identify where in your code) you implemented some kind of filter for false positives and some method for combining overlapping bounding boxes.

> I recorded the positions of positive detections in each frame of the video.  From the positive detections I created a heatmap and then thresholded that map to identify vehicle positions.  I then used `scipy.ndimage.measurements.label()` to identify individual blobs in the heatmap. I then assumed each blob corresponded to a vehicle.  I constructed bounding boxes to cover the area of each blob detected. The code blocks can be found in the `common_functions.py` file starting from line #38. The functions are called (used) in the `extraction_functions.py` file, `predict_vehicle()` function line #166. A threshold of 5 counts is used to minimize false positives. 

> Here's an example result showing the heatmap from a series of frames of video, the result of `scipy.ndimage.measurements.label()` and the bounding boxes then overlaid on the last frame of video:

#### Here are five frames and their corresponding heatmaps:
> 
  ![alt text][frame_heatmap_1]
  ![alt text][frame_heatmap_2]
  ![alt text][frame_heatmap_3]
  ![alt text][frame_heatmap_4]
  ![alt text][frame_heatmap_5]


#### Here the resulting bounding boxes are drawn onto one of the frames in the series:
> ![alt text][heatmap_bbox_5]

---

### Discussion

#### 1. Briefly discuss any problems / issues you faced in your implementation of this project.  Where will your pipeline likely fail?  What could you do to make it more robust?

> When multiple scales of smaller window are chosen, the number of search windows increase. As a result, the prediction accuracy gets better. However, the time to predict for a single image frame gets larger. With one or two scales, the number of search windows is lower. As a result, I got high number of false positives with faster prediction time. That is one of the issues. In addition, the pipeline sometimes fails to detect the car when the car in the image appear somehow far from the camera. Further, there are false detections when the image contains some shadow and metal barriers.

> Using additional features and some form of tracking system, as presented in the course material, could make the detection more robust.

