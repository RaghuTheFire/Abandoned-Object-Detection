#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
/*****************************************************************************************************************************************************/
int main() 
{
    Rect roi(1, 1, 480, 720);
    int maxNumObj = 200;
    int alarmCount = 75;
    int maxConsecutiveMiss = 7;
    VideoCapture cap;
    cap.open("video.mp4");
    Mat frame;
    cap >> frame;
    Mat OutIm = frame(roi);
    Mat YCbCr;
    cvtColor(OutIm, YCbCr, COLOR_BGR2YCrCb);
    Mat CbCr;
    vector<Mat> channels;
    split(YCbCr, channels);
    CbCr = channels[1] - channels[2];
    Mat BkgY, BkgCbCr;
    bool firsttime = true;
    vector<vector<Point>> allBlobList;
    int frame_no = 0;
    while (!frame.empty()) 
	{
        frame_no++;
        OutIm = frame(roi);
        cvtColor(OutIm, YCbCr, COLOR_BGR2YCrCb);
        split(YCbCr, channels);
        CbCr = channels[1] - channels[2];
        
		if (firsttime) 
		{
            firsttime = false;
            BkgY = channels[0].clone();
            BkgCbCr = CbCr.clone();
        }
        Mat SegY = abs(channels[0] - BkgY);
        Mat SegCbCr = abs(CbCr - BkgCbCr) > 0.05;
        Mat Segmented = SegY | SegCbCr;
        vector<vector<Point>> contours;
        findContours(Segmented, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        vector<Rect> BBox;
        for (int i = 0; i < contours.size(); i++) 
		{
            Rect bbox = boundingRect(contours[i]);
            BBox.push_back(bbox);
        }
        for (int i = 0; i < BBox.size(); i++) 
		{
            Point centroid = (BBox[i].tl() + BBox[i].br()) / 2;
            int roundX = centroid.x - centroid.x % 5;
            int roundY = centroid.y - centroid.y % 5;
            bool found = false;
            for (int j = 0; j < allBlobList.size(); j++) 
			{
                if (allBlobList[j][0].x == roundX && allBlobList[j][0].y == roundY) 
				{
                    allBlobList[j][2].x++;
                    found = true;
                    allBlobList[j][3].x = frame_no;
                    allBlobList[j][4].x = i;
                    allBlobList[j][6].x = 0;
                }
            }
            if (!found) 
			{
                vector<Point> blob;
                blob.push_back(Point(roundX, roundY));
                blob.push_back(Point(1, frame_no));
                blob.push_back(Point(i, frame_no));
                blob.push_back(Point(0, 0));
                allBlobList.push_back(blob);
            }
        }
        vector<int> rowToRemove;
        for (int i = 0; i < allBlobList.size(); i++) 
		{
            if (allBlobList[i][2].x > alarmCount && allBlobList[i][6].x <= maxConsecutiveMiss && allBlobList[i][3].x == frame_no) 
			{
                rectangle(frame, BBox[allBlobList[i][4].x], Scalar(0, 0, 255), FILLED);
                // Play sound
            } 
			else 
			{
                allBlobList[i][6].x++;
                if (allBlobList[i][6].x > maxConsecutiveMiss) 
				{
                    rowToRemove.push_back(i);
                }
            }
        }
        for (int i = rowToRemove.size() - 1; i >= 0; i--) 
		{
            allBlobList.erase(allBlobList.begin() + rowToRemove[i]);
        }
        imshow("Abandoned Objects", frame);
        cap >> frame;
        waitKey(1);
    }
    cap.release();
    destroyAllWindows();
    return 0;
}
/*****************************************************************************************************************************************************/


