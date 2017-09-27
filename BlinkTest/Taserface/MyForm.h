#pragma once
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <msclr\marshal_cppstd.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>


using namespace std;
using namespace cv;

using namespace System;
using namespace System::Windows::Forms;


clock_t startingMoving = 0;
int cnt = 0;
int c_flag = 0;
int blink = 0;
int face_on = 0;
int eye_work = 0;
clock_t before;
double result;
int keep = 0;

string text = "Concentrate The Ironman!";
string text2 = "Stop";
int fontFace = 1;
double fontScale = 2;
int thickness = 10;
Point textOrg(120, 200);


string hero = "ironman.png";
int idx = 0;
int p[12] = { 12,600,12,600, 12, 600, 12, 600, 230, 370, 350, 270 };
int q[12] = { 12,400,400,12, 250, 140, 155, 215, 12, 435, 12, 435 };

extern boolean startingVideo = FALSE;
std::string stream_name;
string face_cascade_name = "haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);


void image_overlap(Mat frame)
{
	double time;
	waitKey(1000);
	idx++;
	if (idx > 11) { idx = 0; }
	Mat logo = imread(hero);

	Mat imageROI = frame(Rect(p[idx], q[idx], logo.rows, logo.cols));
	Mat mask = imread(hero, 0);
	logo.copyTo(imageROI, mask);

	putText(frame, text, textOrg, fontFace, fontScale, Scalar(0, 255, 0));

	//namedWindow("imageROI");
	imshow(window_name, frame);

	time = (double)(clock() - startingMoving) / CLOCKS_PER_SEC;
	if (time > 20.0) {
		//putText(frame, text2, textOrg, fontFace, fontScale, Scalar(0, 255, 0)); 
		eye_work = 0;
		startingMoving = 0;
		keep = 0;
	}
}


void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	//Mat faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	face_on = 0;////////////


				//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		face_on = 1;////////////////////
					//putText(frame, "Play", textOrg, fontFace, fontScale, Scalar(0, 255, 0));
		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		c_flag = 0;
		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
			c_flag = 1;//눈 인식한 걸 의미한다.
			blink = 0;
			//printf("%3d,%3d\n", eyes[j].x, eyes[j].y);//좌우구분 없이 눈을 인식한다.
		}
		if (c_flag == 0 && blink == 0) {
			cnt += 1;//눈 감는 횟수
			blink = 1;//눈 한 번 감으면 cnt가 한 번만 올라가도록한다.
			printf("%3d\n", cnt);

			result = (double)(clock() - before) / CLOCKS_PER_SEC;
			printf("걸린시간은 %5.2f 입니다.\n", result);
			before = clock();
			if (result > 12.0) { eye_work = 1; }



		}
	}



	//-- Show what you got
	imshow(window_name, frame);
}


void init() {


	CvCapture* capture;
	CvCapture* capture2;
	Mat frame;
	IplImage *frame2;

	//-- 1. Load the cascades
	while (true) {
		if (!face_cascade.load("haarcascade_frontalface_alt.xml")) { printf("--(!)Error loading\n"); };
		if (!eyes_cascade.load("haarcascade_eye_tree_eyeglasses.xml")) { printf("--(!)Error loading\n"); };
		break;
	}




	//-- 2. Read the video stream
	if (startingVideo == TRUE) {
		capture2 = cvCaptureFromFile(stream_name.c_str());


		//cvNamedWindow("Test", 1);

		capture = cvCaptureFromCAM(0);



		if (capture)
		{

			while (true)
			{
				frame = cvQueryFrame(capture);

				//-- 3. Apply the classifier to the frame
				if (!frame.empty())
				{
					detectAndDisplay(frame);
					if (eye_work == 1 || keep == 1) {

						if (startingMoving == 0) {
							startingMoving = clock();
							keep = 1;
							putText(frame, text, textOrg, fontFace, fontScale, Scalar(0, 255, 0));
							//waitKey(33);
						}
						image_overlap(frame);
					}

					if (face_on == 0) { putText(frame, "stop", textOrg, fontFace, fontScale, Scalar(0, 255, 0)); }

				}
				else
				{
					printf(" --(!) No captured frame -- Break!"); break;
				}

				int c = waitKey(10);
				if ((char)c == 'c') { break; }


				if (face_on == 1) {

					frame2 = cvQueryFrame(capture2);
					cvShowImage(stream_name.c_str(), frame2);
					//if (cvWaitKey(0) == 27);//영상재생
				}


				//printf("%5d,%5d", frame.rows, frame.cols);
				//cout << frame3.rows << " " << frame3.cols << endl;

			}

		}
	}
	else {
		capture = cvCaptureFromCAM(0);
		if (capture)
		{
			while (true)
			{
				frame = cvQueryFrame(capture);

				//-- 3. Apply the classifier to the frame
				if (!frame.empty())
				{
					detectAndDisplay(frame);
					if (eye_work == 1 || keep == 1) {
						if (startingMoving == 0) {
							startingMoving = clock();
							keep = 1;

							//waitKey(33); 
						}
						image_overlap(frame);
					}

				}
				else
				{
					printf(" --(!) No captured frame -- Break!"); break;
				}

				int c = waitKey(10);
				if ((char)c == 'c') { break; }

				if (face_on == 1) {
					//frame2 = cvQueryFrame(capture2);
					//cvShowImage("Test", frame2);
					if (cvWaitKey(33) == 27);//영상재생
				}
				//printf("%5d,%5d", frame.rows, frame.cols);

			}
		}

	}
}


//__declspec(dllexport) void deallocVector(std::vector<std::string> &x);

//void deallocVector(std::vector<std::string> &x) {
//std::vector<std::string> tmp;
//v.swap(tmp);
//}





namespace Taserface {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// MyForm에 대한 요약입니다.
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: 생성자 코드를 여기에 추가합니다.
			//
		}

	protected:
		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel2;
	protected:

	private:
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다. 
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(63, 251);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(188, 25);
			this->textBox1->TabIndex = 0;
			this->textBox1->Text = L"영상제목.확장자";
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &MyForm::textBox1_TextChanged);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(289, 253);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"start";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// panel1
			// 
			this->panel1->Location = System::Drawing::Point(23, 28);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(159, 163);
			this->panel1->TabIndex = 2;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::panel1_Paint);
			// 
			// panel2
			// 
			this->panel2->Location = System::Drawing::Point(235, 28);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(160, 163);
			this->panel2->TabIndex = 0;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(422, 310);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->textBox1);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		init();
	}
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		stream_name = msclr::interop::marshal_as<std::string>(textBox1->Text);




		//cvNamedWindow("Stream", 1);

		startingVideo = TRUE;
		init();
	}
};
}
