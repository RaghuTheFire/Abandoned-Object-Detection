#include <iostream>
#include <opencv2/opencv.hpp>
#include "AbandonedObjectTracker.hpp"
/*****************************************************************************/
int main() 
{
    //Initialize Tracker
    AbandonedObjectTracker tracker;
    //location of first frame 
    cv::Mat firstframe;
    std::string firstframe_path = "Frame.png";
    firstframe = cv::imread(firstframe_path);

    cv::Mat firstframe_gray;
    cv::cvtColor(firstframe, firstframe_gray, cv::COLOR_BGR2GRAY);
    cv::Mat firstframe_blur;
    cv::GaussianBlur(firstframe_gray, firstframe_blur, cv::Size(3, 3), 0);

    std::string file_path = "cut.mp4";
    cv::VideoCapture cap(file_path);
    if (!cap.isOpened()) 
    {
        std::cout << "Error opening video file" << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (cap.read(frame)) 
    {
        int frame_height = frame.rows;
        int frame_width = frame.cols;
        cv::Mat frame_gray;
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
        cv::Mat frame_blur;
        cv::GaussianBlur(frame_gray, frame_blur, cv::Size(3, 3), 0);

        //find diffrence between first frame and current frame
        cv::Mat frame_diff;
        cv::absdiff(firstframe_blur, frame_blur, frame_diff);
        //Canny Edge Detection
        cv::Mat edged;
        cv::Canny(frame_diff, edged, 5, 200);
        //higher the kernel, eg (20,20), more will be eroded or dilated
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
        cv::Mat thresh;
        cv::morphologyEx(edged, thresh, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 2);
        //find contours of all detected objects
        std::vector<std::vector<cv::Point>> cnts;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(thresh, cnts, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::vector<std::vector<int>> detections;
        int count = 0;
        for (const auto& c : cnts) 
        {
            double contourArea = cv::contourArea(c);
            if (contourArea > 50 && contourArea < 10000) 
			{
                count += 1;
                cv::Rect rect = cv::boundingRect(c);
                detections.push_back({rect.x, rect.y, rect.width, rect.height});
            }
        }

        // Track all the abandoned objects
        std::vector<std::vector<int>> abandoned_objects;
        tracker.update(detections, abandoned_objects);

        //Draw rectangle and id over all abandoned objects
        for (const auto& objects : abandoned_objects) 
        {
            int x2 = objects[1];
            int y2 = objects[2];
            int w2 = objects[3];
            int h2 = objects[4];
            cv::putText(frame, "Suspicious object detected", cv::Point(x2, y2 - 10), cv::FONT_HERSHEY_PLAIN, 1.2, cv::Scalar(0, 0, 255), 2);
            cv::rectangle(frame, cv::Point(x2, y2), cv::Point(x2 + w2, y2 + h2), cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("AbandonedObject Detector", frame);
        if (cv::waitKey(15) == 'q') 
        {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
/*****************************************************************************/