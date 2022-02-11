{
 "cells": [
  {
   "cell_type": "code",
   "execution_count": null,
   "metadata": {},
   "outputs": [],
   "source": [
    "\n",
    "\n",
    "#undistort image\n",
    "def undistort(image):\n",
    "    \n",
    "    return cv2.undistort(image, mtx,dist, None, mtx)\n",
    "\n",
    "#convert to grayscale\n",
    "def to_gray(image):\n",
    "    \n",
    "    return cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)\n",
    "\n",
    "#Soble gradient\n",
    "def sobel_thresh(image,thresh):\n",
    "    \n",
    "    gray = to_gray(image)\n",
    "    sob_x = cv2.Sobel(gray, cv2.CV_64F,1,0)\n",
    "    sob_y = cv2.Sobel(gray, cv2.CV_64F,0,1)\n",
    "    \n",
    "    sob = np.absolute(sob_x)#np.sqrt(np.square(sob_x)+ np.square(sob_y))\n",
    "    sob_scaled = np.uint8(255*sob/np.max(sob))\n",
    "    \n",
    "    sob_binary = np.zeros_like(sob_scaled)\n",
    "    sob_binary[(sob_scaled>=thresh[0])&(sob_scaled<=thresh[1])] = 1\n",
    "    \n",
    "    return sob_binary\n",
    "\n",
    "#convert image to hsv\n",
    "def to_hsv(image, thresh):\n",
    "    hsv = cv2.cvtColor(image,cv2.COLOR_RGB2HSV)\n",
    "    v = hsv[:,:,1]\n",
    "    v_binary = np.zeros_like(v)\n",
    "    v_binary[(v>=thresh[0])&(v<=thresh[1])] = 1\n",
    "    \n",
    "    return v_binary\n",
    "\n",
    "#convert image to hls\n",
    "def to_hls(image, thresh):\n",
    "    hls = cv2.cvtColor(image,cv2.COLOR_RGB2HLS)\n",
    "    s = hls[:,:,2]\n",
    "    s_binary = np.zeros_like(s)\n",
    "    s_binary[(s>=thresh[0])&(s<=thresh[1])] = 1\n",
    "    return s_binary\n",
    "\n",
    "#convert image to bird's eyeview\n",
    "def warp_image(image):\n",
    "    src = np.float32([[570,465], [710,465], [1070, 700],[240, 700]])\n",
    "    dst = np.float32([[350,0], [930,0], [930,700], [350,700]])\n",
    "    \n",
    "    M = cv2.getPerspectiveTransform(src,dst)\n",
    "    M_inv = cv2.getPerspectiveTransform(dst,src)\n",
    "    img_warped = cv2.warpPerspective(image, M,(image.shape[1],image.shape[0]),cv2.INTER_LINEAR)\n",
    "    return img_warped"
   ]
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "Python 3",
   "language": "python",
   "name": "python3"
  },
  "language_info": {
   "codemirror_mode": {
    "name": "ipython",
    "version": 3
   },
   "file_extension": ".py",
   "mimetype": "text/x-python",
   "name": "python",
   "nbconvert_exporter": "python",
   "pygments_lexer": "ipython3",
   "version": "3.6.4"
  }
 },
 "nbformat": 4,
 "nbformat_minor": 2
}
