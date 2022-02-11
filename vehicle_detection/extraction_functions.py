## FEATURE EXTRACTION FUNCTIONS
import numpy as np
import cv2
import time
from skimage.feature import hog
from scipy.ndimage.measurements import label
from common_functions import *
# A function to return HOG features and visualization
def get_hog_features(img, orient, pix_per_cell, cell_per_block, vis=False, feature_vec=True):
    # Call with two outputs if vis==True
    if vis == True:
        features, hog_image = hog(img, orientations=orient,pixels_per_cell=(pix_per_cell, pix_per_cell),
                                  block_norm='L2-Hys', cells_per_block=(cell_per_block, cell_per_block),
                                  transform_sqrt=True, visualise=vis, feature_vector=feature_vec)
        return features, hog_image
    # Otherwise call with one output
    else:      
        features = hog(img, orientations=orient,pixels_per_cell=(pix_per_cell, pix_per_cell),
                       cells_per_block=(cell_per_block, cell_per_block), block_norm='L2-Hys',
                       transform_sqrt=True, visualise=vis, feature_vector=feature_vec)
        return features

# A function to extract features from a list of images
# this function calls single_img_features() function
def extract_features(imgs, color_space='RGB', spatial_size=(32, 32), hist_bins=32, orient=9, 
                     pix_per_cell=8, cell_per_block=2, hog_channel=0):
    # Create a list to append feature vectors to
    features = []
    # Iterate through the list of images
    for file in imgs:
        # Read in each image using cv2.imread->convert it to RGB color space
        image = cv2.cvtColor(cv2.imread(file),cv2.COLOR_BGR2RGB)
        # collect features for each image
        features.append(single_img_features(image, color_space, spatial_size,hist_bins, orient,
                                            pix_per_cell, cell_per_block, hog_channel))
    return features
 
# A function to extract features from a single image window
def single_img_features(image, color_space='RGB', spatial_size=(32, 32), hist_bins=32, orient=9,
                        pix_per_cell=8, cell_per_block=2, hog_channel=0):
    # Create a list to append feature vectors to
    features = []
    # apply color conversion if other than 'RGB'
    feature_image = convert_color(image,conv=color_space)
    # Call get_hog_features() with vis=False, feature_vec=False
    if hog_channel == 'ALL':
        hog_features = []
        for channel in range(feature_image.shape[2]):
            hog_features.append(get_hog_features(feature_image[:, :, channel],
                                                 orient, pix_per_cell, cell_per_block, vis=False,
                                                 feature_vec=False))
        hog_features = np.ravel(hog_features)
    else:
        hog_features = get_hog_features(feature_image[:, :, hog_channel], orient,
                                        pix_per_cell, cell_per_block, vis=False, feature_vec=False)
        hog_features = np.ravel(hog_features)

    # Get the spatial binned features
    spatial_features = bin_spatial(feature_image, size=spatial_size)
    # Get the color histograms
    hist_features = color_hist(feature_image, nbins=hist_bins)
    # Put all features together
    features= np.concatenate((hog_features,hist_features,spatial_features))

    return features


# Define a single function that can extract features using hog sub-sampling and make predictions
def find_cars(img, ystart, ystop, scale, svc, X_scaler, color_space,orient, pix_per_cell, cell_per_block, 
              spatial_size, hist_bins):
   
    draw_img = np.copy(img)
    
    window_list =[] # list to collect bounding box coordinates with positive prediction
    all_windows = [] # a list to collect all searching windows/boxes
    
    img_tosearch = img[ystart:ystop, :, :]
    ctrans_tosearch = convert_color(img_tosearch, conv=color_space)
    if scale != 1:
        imshape = ctrans_tosearch.shape
        ctrans_tosearch = cv2.resize(ctrans_tosearch, (np.int(imshape[1] / scale), np.int(imshape[0] / scale)))

    ch1 = ctrans_tosearch[:, :, 0]
    ch2 = ctrans_tosearch[:, :, 1]
    ch3 = ctrans_tosearch[:, :, 2]

    # Define blocks and steps as above
    nxblocks = (ch1.shape[1] // pix_per_cell) - cell_per_block + 1
    nyblocks = (ch1.shape[0] // pix_per_cell) - cell_per_block + 1
    nfeat_per_block = orient * cell_per_block ** 2

    # 64 was the orginal sampling rate, with 8 cells and 8 pix per cell
    window = 64
    nblocks_per_window = (window // pix_per_cell) - cell_per_block + 1
    cells_per_step = 2  # Instead of overlap, define how many cells to step
    nxsteps = (nxblocks - nblocks_per_window) // cells_per_step + 1
    nysteps = (nyblocks - nblocks_per_window) // cells_per_step + 1

    # Compute individual channel HOG features for the entire image
    hog1 = get_hog_features(ch1, orient, pix_per_cell, cell_per_block, feature_vec=False)
    hog2 = get_hog_features(ch2, orient, pix_per_cell, cell_per_block, feature_vec=False)
    hog3 = get_hog_features(ch3, orient, pix_per_cell, cell_per_block, feature_vec=False)

    for xb in range(nxsteps):
        for yb in range(nysteps):
            ypos = yb * cells_per_step
            xpos = xb * cells_per_step
            # Extract HOG for this patch
            hog_feat1 = hog1[ypos:ypos + nblocks_per_window, xpos:xpos + nblocks_per_window].ravel()
            hog_feat2 = hog2[ypos:ypos + nblocks_per_window, xpos:xpos + nblocks_per_window].ravel()
            hog_feat3 = hog3[ypos:ypos + nblocks_per_window, xpos:xpos + nblocks_per_window].ravel()
            hog_features = np.hstack((hog_feat1, hog_feat2, hog_feat3))

            xleft = xpos * pix_per_cell
            ytop = ypos * pix_per_cell
            
            xbox_left = np.int(xleft * scale)
            ytop_draw = np.int(ytop * scale)
            win_draw = np.int(window * scale)
            
            # Collect all the search windows/boxes
            all_windows.append(((xbox_left, ytop_draw + ystart), (xbox_left + win_draw, ytop_draw + win_draw + ystart)))
            
            # Extract the image patch
            subimg = cv2.resize(ctrans_tosearch[ytop:ytop + window, xleft:xleft + window], (64, 64))

            # Get color features
            spatial_features = bin_spatial(subimg, size=spatial_size)
            hist_features = color_hist(subimg, nbins=hist_bins)

            # Scale features and make a prediction
            test_features = X_scaler.transform(np.hstack((hog_features,hist_features,spatial_features)).reshape(1, -1))
            # test_features = X_scaler.transform(np.hstack((shape_feat, hist_feat,,spatial_features)).reshape(1, -1))
            test_prediction = svc.predict(test_features)

            if test_prediction == 1: 
                window_list.append(((xbox_left, ytop_draw + ystart), (xbox_left + win_draw, ytop_draw + win_draw + ystart)))
                
    return window_list,all_windows

def predict_vehicle(img, svc, X_scalar, scales, ystart, ystop, color_space,
                    orient, pix_per_cell, cell_per_block,spatial_size, hist_bins, heat_thresh):
    ## Convert the BGR image obtained from cv2.imread() to RGB color space
    image = cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
    
    windows =[] # collects all the bounding boxes where a car is estimated to be found
    bboxes =[] # temporary list to collect bounding boxes from each scale 
    all_windows = []
    par_windows = []
    ## Collect the bounding boxes containing cars for each scale
    for i in range(len(scales)):
        boxes, wins = find_cars(image, ystart[i], ystop[i], scales[i], svc, X_scalar, color_space,
                                orient, pix_per_cell, cell_per_block,spatial_size, hist_bins)
        bboxes.append(boxes)
        par_windows.append(wins)
    
    ## Draw the bounding boxes on the original image
    for box_list in bboxes:    
        for box in box_list:
            windows.append(box)
    for box_list in par_windows:    
        for box in box_list:
            all_windows.append(box)
    bbox_img = draw_boxes(img, windows, color=(20, 185, 200), thick=4)
    #Initialize heatmap image
    heat = np.zeros_like(image[:,:,0]).astype(np.float)
    heat = add_heat(heat,windows)
    heat_img = np.copy(heat)
    heat_th = apply_threshold(heat,heat_thresh)
    # Visualize the heatmap when displaying
    heatmap = np.clip(heat_th, 0, 255)
    # Find final boxes from heatmap using label function
    labels = label(heatmap)
    final_img = draw_labeled_bboxes(np.copy(img), labels)
    
    return final_img, bbox_img, heat_img,heatmap, all_windows

