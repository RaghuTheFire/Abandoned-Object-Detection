#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <vector>
#include <ctime>

using namespace cv;
using namespace dnn;
using namespace std;

const vector < string > CLASSES = 
{
  "background",
  "aeroplane",
  "bicycle",
  "bird",
  "boat",
  "bottle",
  "bus",
  "car",
  "cat",
  "chair",
  "cow",
  "diningtable",
  "dog",
  "horse",
  "motorbike",
  "person",
  "pottedplant",
  "sheep",
  "sofa",
  "train",
  "tvmonitor"
};

int main() 
{
  vector < Scalar > COLORS;
  for (int i = 0; i < CLASSES.size(); i++) 
  {
    COLORS.push_back(Scalar(rand() % 256, rand() % 256, rand() % 256));
  }

  Net net = readNetFromCaffe("MobileNetSSD_deploy.prototxt.txt", "MobileNetSSD_deploy.caffemodel");

  VideoCapture cap(2);
  if (!cap.isOpened()) 
  {
    cerr << "Error opening video stream" << endl;
    return -1;
  }

  int c = 0;
  vector < string > detect;
  double start, end, dif, stime;

  VideoWriter out("output1.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), 20.0, Size(640, 480));

  while (true) 
  {
    Mat frame;
    cap >> frame;
    if (frame.empty()) 
	{
      break;
    }

    resize(frame, frame, Size(800, 600));
    Mat blob = blobFromImage(frame, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), true, false);

    net.setInput(blob);
    vector < Mat > detections = net.forward();

    Mat det_conf = detections[0].reshape(1, detections[0].total() / 7);
    vector < double > top_conf;
    vector < int > top_indices;
    for (int i = 0; i < det_conf.total(); i++) 
	{
      if (det_conf.at < float > (i) >= 0.2) 
	  {
        top_conf.push_back(det_conf.at < float > (i));
        top_indices.push_back(i);
      }
    }

    Mat det_indx = detections[0].reshape(1, detections[0].total() / 7).col(1);
    vector < int > top_label_indices;
    for (int i = 0; i < top_indices.size(); i++) 
	{
      top_label_indices.push_back(static_cast < int > (det_indx.at < float > (top_indices[i])));
    }

    if (c == 0) 
	{
      for (int i = 0; i < detections[0].total() / 7; i++) 
	  {
        float confidence = detections[0].at < float > (i * 7 + 2);
        if (confidence > 0.99) 
		{
          int idx = static_cast < int > (detections[0].at < float > (i * 7 + 1));
          int startX = static_cast < int > (detections[0].at < float > (i * 7 + 3) * frame.cols);
          int startY = static_cast < int > (detections[0].at < float > (i * 7 + 4) * frame.rows);
          int endX = static_cast < int > (detections[0].at < float > (i * 7 + 5) * frame.cols);
          int endY = static_cast < int > (detections[0].at < float > (i * 7 + 6) * frame.rows);
          string label = CLASSES[idx];
          if (label == "bottle" && c == 0) 
		  {
            for (int j = 0; j < detections[0].total() / 7; j++) 
			{
              idx = static_cast < int > (detections[0].at < float > (j * 7 + 1));
              label = CLASSES[idx];
              if (label != "person") 
			  {
                start = static_cast < double > (clock()) / CLOCKS_PER_SEC;
                cout << "Timer Started" << endl;
                c = 1;
                break;
              }
            }
          }
          rectangle(frame, Point(startX, startY), Point(endX, endY), COLORS[idx], 2);
          int y = startY - 15 > 15 ? startY - 15 : startY + 15;
          putText(frame, label, Point(startX, y), FONT_HERSHEY_SIMPLEX, 0.5, COLORS[idx], 2);
        }
      }
    }

    if (c == 1) 
	{
      for (int i = 0; i < detections[0].total() / 7; i++) 
	  {
        float confidence = detections[0].at < float > (i * 7 + 2);
        if (confidence > 0.40) 
		{
          int idx = static_cast < int > (detections[0].at < float > (i * 7 + 1));
          int startX = static_cast < int > (detections[0].at < float > (i * 7 + 3) * frame.cols);
          int startY = static_cast < int > (detections[0].at < float > (i * 7 + 4) * frame.rows);
          int endX = static_cast < int > (detections[0].at < float > (i * 7 + 5) * frame.cols);
          int endY = static_cast < int > (detections[0].at < float > (i * 7 + 6) * frame.rows);
          string label = CLASSES[idx];
          rectangle(frame, Point(startX, startY), Point(endX, endY), COLORS[idx], 2);
          int y = startY - 15 > 15 ? startY - 15 : startY + 15;
          if (label == "bottle") 
		  {
            putText(frame, "Target Object", Point(startX, y), FONT_HERSHEY_SIMPLEX, 1, COLORS[idx], 2);
          } 
		  else 
		  {
            putText(frame, label, Point(startX, y), FONT_HERSHEY_SIMPLEX, 0.5, COLORS[idx], 2);
          }
        }
      }
      detect.push_back(CLASSES[static_cast < int > (detections[0].at < float > (0 * 7 + 1))]);
      int count1 = count(detect.begin(), detect.end(), "person");
      cout << "Detected objects: ";
      for (const auto & obj: detect) 
	  {
        cout << obj << " ";
      }
      cout << endl;
      cout << "Number of persons: " << count1 << endl;
      end = static_cast < double > (clock()) / CLOCKS_PER_SEC;
      dif = end - start;
      cout << "Time elapsed: " << dif << " seconds" << endl;
      stime = static_cast < double > (clock()) / CLOCKS_PER_SEC;
      if (dif > 15 && count1 < 50) 
	  {
        c = 2;
      }
      if (dif > 20 && count1 > 100) 
	  {
        detect.clear();
        c = 0;
      }
    }

    if (c == 2) 
	{
      for (int i = 0; i < detections[0].total() / 7; i++) {
        float confidence = detections[0].at < float > (i * 7 + 2);
        if (confidence > 0.40) {
          int idx = static_cast < int > (detections[0].at < float > (i * 7 + 1));
          int startX = static_cast < int > (detections[0].at < float > (i * 7 + 3) * frame.cols);
          int startY = static_cast < int > (detections[0].at < float > (i * 7 + 4) * frame.rows);
          int endX = static_cast < int > (detections[0].at < float > (i * 7 + 5) * frame.cols);
          int endY = static_cast < int > (detections[0].at < float > (i * 7 + 6) * frame.rows);
          string label = CLASSES[idx];
          rectangle(frame, Point(startX, startY), Point(endX, endY), COLORS[idx], 2);
          int y = startY - 15 > 15 ? startY - 15 : startY + 15;
          if (label == "bottle") 
		  {
            putText(frame, "Warning", Point(startX, y), FONT_HERSHEY_SIMPLEX, 1, COLORS[idx], 2);
          } 
		  else 
		  {
            putText(frame, label, Point(startX, y), FONT_HERSHEY_SIMPLEX, 0.5, COLORS[idx], 2);
          }
        }
      }
    }

    out.write(frame);
    imshow("Frame", frame);
    char key = static_cast < char > (waitKey(1));
    if (key == 'q') 
	{
      break;
    }
  }

  cap.release();
  out.release();
  destroyAllWindows();

  return 0;
}