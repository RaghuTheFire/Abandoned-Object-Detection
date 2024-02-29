# Abandoned Object Detection

Detects abandoned objects in a video, particularly useful for identifying suspicious abandoned luggage in railway stations and bus stands. 
This is a project developed in C++, which is used to detect abandoned objects automatically from a video, particularly useful for identifying suspicious abandoned luggage at busy places like railway stations and bus stands.

This task is done the followinf steps:-
1. First frame of the video is assumed as the background image.
2. The video is converted to frames of images and then each frame is subtracted from the background image
3. If an object remains static at one place for a fixed number of frames, then it is declared as an abandoned object
4. An alarm is raised for an abandoned object .

This C++ code is designed to process a video file and detect abandoned objects using OpenCV, a popular computer vision library. 
Here's a simplified explanation of what each part of the code does: 
1. **Main Function Setup**: The `main()` function is the entry point of the program. It initializes several variables and objects: 
- `roi` is a rectangle that defines a region of interest in the video frames. 
- `maxNumObj`, `alarmCount`, and `maxConsecutiveMiss` are integers used for controlling the object detection logic. 
- `VideoCapture cap` is an object for capturing video from a file named "video.mp4". 
- `Mat frame` is a matrix to store the current video frame. 
- `Mat OutIm` is a matrix to store the region of interest from the current frame. 
- `Mat YCbCr` and `Mat CbCr` are matrices to store color-converted frames. 
- `vector<vector<Point>> allBlobList` is a list to keep track of detected objects (blobs). 
2. **Video Processing Loop**: The code enters a loop that continues as long as there are frames to process in the video. 
Inside this loop: 
- The current frame is cropped to the region of interest and converted to the YCbCr color space. - The Cb and Cr channels are subtracted to create a `CbCr` matrix, which helps in distinguishing objects from the background. 
- During the first iteration, the background is captured for later comparison. 
- The absolute difference between the current frame and the background is calculated for both the Y channel and the `CbCr` matrix to segment the frame and identify changes. 
- Contours are found in the segmented frame, and bounding boxes are created for each contour. 
- Each bounding box's centroid is calculated, and objects are tracked by comparing centroids across frames. 
- If an object has been detected in the same location for more than `alarmCount` frames but less than `maxConsecutiveMiss` frames, it is considered an abandoned object, and its bounding box is drawn on the frame in red. 
- Objects that have not been detected for more than `maxConsecutiveMiss` frames are removed from tracking. 
3. **Display and Cleanup**: The processed frame is displayed in a window titled "Abandoned Objects". 
The loop then fetches the next frame and repeats the process until no more frames are available. After processing all frames,
the video capture is released, and all created windows are destroyed. 
4. **Program Termination**: The program returns 0, indicating successful execution. 
This code is a basic example of video processing for object detection and tracking, 
specifically aimed at identifying abandoned objects by analyzing changes in video frames over time.

# Data Set Used for Testing:
Videos at https://github.com/kevinlin311tw/ABODA

# Compilation Command
g++ yoloX.cpp -o Aban pkg-config --cflags --libs opencv4
