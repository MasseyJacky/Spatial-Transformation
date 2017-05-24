#include <opencv2\opencv.hpp>

using namespace cv;

Mat source, source_rotated, destination, image_blur, image_gray, draw_on_image;
Rect boundrec;
std::vector<Point2f> destination_rotation(4), source_rotation(4), rotation(4),result_d(4),result_s(4),result(4);

int main(int argc, char* args[]) {

	//load the input image
	//"Image X.jpg" Where X could be anynumber form 1-5
	Mat image = imread("Image 2.jpg", 1);

	//convert to grayscale
	cvtColor(image, image_gray, CV_BGR2GRAY);

	//blur image
	GaussianBlur(image_gray, image_blur, Size(7, 7), 0, 0);

	//apply threshold to locate source letter and its destination
	threshold(image_blur, source, 20, 255, THRESH_BINARY_INV); //letter

	threshold(image_blur, destination, 252, 255, THRESH_BINARY); // destination

	//find the position of the letter and the its destination
	draw_on_image = destination;
	boundrec = boundingRect(draw_on_image);
	//rectangle(draw_on_image, boundrec.tl(), boundrec.br(), Scalar(255));
	int translation_x_destination = boundrec.x+(boundrec.width/2);
	int translation_y_destination = boundrec.y+(boundrec.height/2);
	
	rotation[0] = Point2f(boundrec.x, boundrec.y); rotation[1] = Point2f((boundrec.x + boundrec.width), boundrec.y);
	rotation[2] = Point2f(boundrec.x, (boundrec.y + boundrec.height)); rotation[3] = Point2f((boundrec.x + boundrec.width), (boundrec.y + boundrec.height));
	destination_rotation = rotation;

	//make result frame
	result[0] = Point2f(250 - (boundrec.width / 2), 250 - (boundrec.height / 2)); result[1] = Point2f(250 + (boundrec.width / 2), 250 - (boundrec.height / 2));
	result[2] = Point2f(250 - (boundrec.width / 2), 250 + (boundrec.height / 2)); result[3] = Point2f(250 + (boundrec.width / 2), 250 + (boundrec.height / 2));
	result_s = result;
	
	Mat persptrans_d = getPerspectiveTransform(rotation, result);
	warpPerspective(destination,destination,persptrans_d, Size(500, 500));

	draw_on_image = source;
	boundrec = boundingRect(draw_on_image);
	//rectangle(draw_on_image, boundrec.tl(), boundrec.br(), Scalar(255));
	int translation_x_source = boundrec.x  + (boundrec.width / 2);
	int translation_y_source = boundrec.y + (boundrec.height / 2);

	rotation[0] = Point2f(boundrec.x, boundrec.y); rotation[1] = Point2f((boundrec.x + boundrec.width), boundrec.y);
	rotation[2] = Point2f(boundrec.x, (boundrec.y + boundrec.height)); rotation[3] = Point2f((boundrec.x + boundrec.width), (boundrec.y + boundrec.height));
	source_rotation = rotation;

	//make result frame
	result[0] = Point2f(250 - (boundrec.width / 2), 250 - (boundrec.height / 2)); result[1] = Point2f(250 + (boundrec.width / 2), 250 - (boundrec.height / 2));
	result[2] = Point2f(250 - (boundrec.width / 2), 250 + (boundrec.height / 2)); result[3] = Point2f(250 + (boundrec.width / 2), 250 + (boundrec.height / 2));
	result_s = result;

	Mat persptrans_s = getPerspectiveTransform(source_rotation, result_s);
	warpPerspective(source, source, persptrans_s, Size(500, 500));

	//calculate translation in x axis and y axis
	int translation_x = translation_x_destination - translation_x_source;
	int translation_y = translation_y_destination - translation_y_source;

	//calculate rotation about z-axis
	double match, minimum=1;
	Mat overlapped_letters;
	int angle=0;

	//rotate the letter image
	for (int i = 0; i < 360; i++) {
		Mat M = getRotationMatrix2D(Point2f(250, 250), i, 1);
		warpAffine(source, source_rotated, M, Size(500, 500));

		draw_on_image = source_rotated;
		boundrec = boundingRect(draw_on_image);

		rotation[0] = Point2f(boundrec.x, boundrec.y); rotation[1] = Point2f((boundrec.x + boundrec.width), boundrec.y);
		rotation[2] = Point2f(boundrec.x, (boundrec.y + boundrec.height)); rotation[3] = Point2f((boundrec.x + boundrec.width), (boundrec.y + boundrec.height));
		source_rotation = rotation;

		//make result frame
		result[0] = Point2f(250 - (boundrec.width / 2), 250 - (boundrec.height / 2)); result[1] = Point2f(250 + (boundrec.width / 2), 250 - (boundrec.height / 2));
		result[2] = Point2f(250 - (boundrec.width / 2), 250 + (boundrec.height / 2)); result[3] = Point2f(250 + (boundrec.width / 2), 250 + (boundrec.height / 2));
		result_s = result;

		Mat persptrans_sr = getPerspectiveTransform(source_rotation, result_s);
		warpPerspective(source_rotated, source_rotated, persptrans_sr, Size(500, 500));

		addWeighted(source_rotated, 1, destination, 1, 0, overlapped_letters);

		match = matchShapes(source_rotated, overlapped_letters, CV_CONTOURS_MATCH_I1,0);
		std::cout << i << " " << match << " " << minimum << " " << angle << std::endl;

		if (minimum > match) {
			std::cout << "that is the one " << i << std::endl;
			minimum = match;
			angle = i;
		}
	}
	

	//print out the transforamtion matrix
	std::cout << "The transformation matrix is: " << std::endl;
	std::cout << "[ " << cos(angle) << ", " << -sin(angle) << ", " << translation_x << "]" << std::endl;
	std::cout << "[ " << sin(angle) << ", " << cos(angle) << ", " << translation_y << "]" << std::endl;
	std::cout << "[ 0, 0, " << "  1" << "]" << std::endl;
	
	
	namedWindow("Letter image", CV_WINDOW_AUTOSIZE);
	imshow("Letter image", overlapped_letters);//overlapped_letter

	namedWindow("Letter1 image", CV_WINDOW_AUTOSIZE);
	imshow("Letter1 image", source_rotated);

	while (1) {
		int key = waitKey();
		if (key == 27) {
			break;
		}
	}
}

