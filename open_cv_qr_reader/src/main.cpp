 // ZED includes
#include <sl/Camera.hpp>

// OpenCV includes
#include <opencv2/opencv.hpp>
// OpenCV dep
#include <opencv2/cvconfig.h>

// #include <opencv2/core_detect.hpp>
// #include <opencv2/core.hpp>
// #include <opencv2/highgui.hpp>
// #include <opencv2/imgproc.hpp>

// OpenCV wechat qr code detector
#include <opencv2/wechat_qrcode.hpp>


#include <string>

using namespace sl;
using namespace std;

// using namespace cv;

using reader = cv::wechat_qrcode::WeChatQRCode;
string models_path = "../wechat_models";

// display a qr code in an image
void display(const cv::Mat &image, const cv::Mat &bbox) {
    // TODO: change this function to a simplified display function
    for (int i = 0; i < bbox.rows; i++) {
        cv::line(
            image, 
            // draw a line between two subsequent bounding box points
            cv::Point2i(bbox.at<float>(i, 0), bbox.at<float>(i, 1)), 
            // use % to loop back to the first point if we overflow
            cv::Point2i(bbox.at<float>((i+1) % bbox.rows, 0), bbox.at<float>((i+1) % bbox.rows, 1)), 
            // draw a green line (r,g,b)
            cv::Scalar(0, 255, 0), 
            // line thickness
            3);

        cv::imshow("Image", image);
    }
}

// Mapping between MAT_TYPE and CV_TYPE -> used in slMat2cvMat
int getOCVtype(sl::MAT_TYPE type) {
    int cv_type = -1;
    switch (type) {
        case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
        case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
        case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
        case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
        case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
        case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
        case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
        case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
        default: break;
    }
    return cv_type;
}

/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::Mat slMat2cvMat(Mat& input) {
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), getOCVtype(input.getDataType()), input.getPtr<sl::uchar1>(MEM::CPU), input.getStepBytes(sl::MEM::CPU));
}


int main(int argc, char **argv) {
    // instantiate wechat qr code reader using open cv smart pointer
    cv::Ptr<reader> detector = cv::makePtr<reader>(
        models_path + "/detect.prototxt", 
        models_path + "/detect.caffemodel",
        models_path + "/sr.prototxt",
        models_path + "/sr.caffemodel"
    );
    // create zed camera
    Camera zed;      

    // Set configuration parameters
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION::HD1080;
    // init_params.depth_mode = DEPTH_MODE::ULTRA;
    init_params.camera_fps = 30;
    init_params.coordinate_units = UNIT::METER;

    // open camera
    auto camera_return = zed.open(init_params);
    if (camera_return != ERROR_CODE::SUCCESS) {
        printf("%s\n", toString(camera_return).c_str());
        zed.close();
        return 1;
    }
    // get camera resolution details
    Resolution zed_size = zed.getCameraInformation().camera_resolution;

    // setup sl mat
    // TODO: investiage the different with an sl image being 3 vs 4 channels
    sl::Mat image(zed_size.width, zed_size.height, MAT_TYPE::U8_C4);
    cv::Mat image_open_cv = slMat2cvMat(image);
    // store bounding box coordinates of points
    std::vector<cv::Mat> bounding_points;
    while (true) {
        // get new img
        camera_return = zed.grab();

        if (camera_return != ERROR_CODE::SUCCESS) {
        printf("%s\n", toString(camera_return).c_str());
        zed.close();
        return 1;
        } else {
            // TODO: change to a better view, just left view for now
            zed.retrieveImage(image, VIEW::LEFT);
            // cout<<"Image resolution: "<< image.getWidth()<<"x"<<image.getHeight() <<" || Image timestamp: "<<image.timestamp.data_ns<<endl;

            // apply qr code reader to image
            auto read_code = detector->detectAndDecode(image_open_cv, bounding_points);

            if (read_code.size() > 0) {
                // show the string points
                for (const auto read_val: read_code) {
                    std::cout << read_val << std::endl;
                }
                // TODO: add points into bounding box in new order
                // TODO: display bounding box on image to see results

            } else {
                std::cout << "No QR code detected" << std::endl;
            }
        }

        cv::imshow("Image window", image_open_cv);

        // handle key event -> make cv2 show a new image on the window every 5ms
        cv::waitKey(5);
    }
    zed.close();
    return 1;
}