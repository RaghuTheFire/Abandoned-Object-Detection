#include <cmath>
#include <map>
#include <vector>
/*****************************************************************************/
class AbandonedObjectTracker 
{
private:
    std::map<int, std::pair<double, double>> center_points;
    int id_count;
    std::map<int, int> abandoned_temp;

public:
    AbandonedObjectTracker() 
    {
        id_count = 0;
    }

    std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> update(std::vector<std::vector<int>> objects_rect) 
    {
        //Objects boxes and ids
        std::vector<std::vector<int>> objects_bbs_ids;
        std::vector<std::vector<int>> abandoned_object;

        for (auto rect : objects_rect) 
        {
            int x = rect[0];
            int y = rect[1];
            int w = rect[2];
            int h = rect[3];
            //Get center point of new object
            double cx = (x + x + w) / 2.0;
            double cy = (y + y + h) / 2.0;
             
            //Find out if that object was detected already
            bool same_object_detected = false;
            for (auto& [id, pt] : center_points) 
            {
                double distance = std::hypot(cx - pt.first, cy - pt.second);
                if (distance < 25) 
                {
                    center_points[id] = std::make_pair(cx, cy);
                    objects_bbs_ids.push_back({x, y, w, h, id, distance});
                    same_object_detected = true;

                    //Add same object to the abandoned_temp dictionary. if the object is
                    //still in the temp dictionary for certain threshold count then
                    //the object will be considered as abandoned object 
                    if (abandoned_temp.count(id) > 0) 
                    {
                        if (distance < 1) 
                        {
                            if (abandoned_temp[id] > 100) 
                            {
                                abandoned_object.push_back({id, x, y, w, h, distance});
                            } 
                            else 
                            {
                                abandoned_temp[id] += 1;
                            }
                        }
                    }
                    break;
                }
            }
            //If new object is detected then assign the ID to that object
            if (!same_object_detected) 
            {
                center_points[id_count] = std::make_pair(cx, cy);
                abandoned_temp[id_count] = 1;
                objects_bbs_ids.push_back({x, y, w, h, id_count, -1});
                id_count += 1;
            }
        }

        //Clean the dictionary by center points to remove IDS not used anymore
        std::map<int, std::pair<double, double>> new_center_points;
        std::map<int, int> abandoned_temp_2;
        for (auto obj_bb_id : objects_bbs_ids) 
        {
            int object_id = obj_bb_id[4];
            std::pair<double, double> center = center_points[object_id];
            new_center_points[object_id] = center;
            if (abandoned_temp.count(object_id) > 0) 
            {
                int counts = abandoned_temp[object_id];
                abandoned_temp_2[object_id] = counts;
            }
        }

        // Update dictionary with IDs not used removed
        center_points = new_center_points;
        abandoned_temp = abandoned_temp_2;
        return std::make_pair(objects_bbs_ids, abandoned_object);
    }
};
/*****************************************************************************/