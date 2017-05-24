#include <opencv2\opencv.hpp>

using namespace cv;

Mat source, destination, image_blur,image_gray, draw_on_image;
Rect boundrec;
Mat translation, rotation;

int main(int argc, char* args[]) {

	//load the input image
	//"Image X.jpg" Where X could be anynumber form 1-5
	Mat image = imread("Image 1.jpg", 1);

	cvtColor(image, image_gray, CV_BGR2GRAY);

	//blur image
	GaussianBlur(image_gray, image_blur, Size(7, 7), 0, 0);

	//apply threshold to locate source letter and its destination
	threshold(image_blur, source, 20, 255, THRESH_BINARY_INV); //letter

	threshold(image_blur, destination, 252, 255, THRESH_BINARY); // destination

	//find the position of the letter and the its destination
	draw_on_image = destination;
	boundrec = boundingRect(draw_on_image);
	rectangle(draw_on_image, boundrec.tl(), boundrec.br(), Scalar(255));
	int translation_x_destination = boundrec.x;
	int translation_y_destination = boundrec.y;

	draw_on_image = source;
	boundrec = boundingRect(draw_on_image);		
	rectangle(draw_on_image, boundrec.tl(), boundrec.br(), Scalar(255));
	int translation_x_source = boundrec.x;
	int translation_y_source = boundrec.y;

	//calculate translation in x axis and y axis
	int translation_x = translation_x_destination - translation_x_source;
	int translation_y = translation_y_destination - translation_y_source;

	//print out the transforamtion matrix
	std::cout << "The transformation matrix is: " << std::endl;
	std::cout << "[ 1, 0, " << translation_x << "]" << std::endl;
	std::cout << "[ 0, 1, " << translation_y << "]" << std::endl;
	std::cout << "[ 0, 0, " << "  1"		 << "]" << std::endl;

	namedWindow("Letter image", CV_WINDOW_NORMAL);
	imshow("Letter image", destination);

	namedWindow("Letter1 image", CV_WINDOW_NORMAL);
	imshow("Letter1 image", source);

	while (1) {
		int key = waitKey();
		if (key == 27) {
			break;
		}
	}
}

/*abandont code
Mat pst_destination = (Mat_<Point2f>(3, 1) <<
	boundrec.tl(), (boundrec.x + boundrec.width, boundrec.y), boundrec.br());//Point2f pst_destination = boundrec.tl();

translation = getAffineTransform(pst_source, pst_destination);
*/