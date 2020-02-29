#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
//#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "caffe/util/benchmark.hpp"
//#define custom_class
#include<sys/time.h>
#include<iostream>
//#define  CPU_ONLY


char* CLASSES[5] = { "__background__",
"person","plate"
};

/*char* CLASSES[5] = { "__background__","plate"
};*/
using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;
using boost::shared_ptr;

uint64_t current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    return te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
}

class Detector {
 public:
  Detector(const string& model_file,
           const string& weights_file,
           const string& mean_value,
		       const float confidence_threshold,
		       const float normalize_value,
		       const string& cpu_mode,
		       const int resize);

  std::vector<vector<float> > Detect(cv::Mat& img);

 private:
  void SetMean( const string& mean_value);

  void WrapInputLayer(std::vector<cv::Mat>* input_channels);

  cv::Mat Preprocess(const cv::Mat& img,
                  std::vector<cv::Mat>* input_channels);
  cv::Mat Preprocess(const cv::Mat& img,
	  std::vector<cv::Mat>* input_channels,double normalize_value);
  cv::Mat LetterBoxResize(cv::Mat img, int w, int h);
 private:
  boost::shared_ptr<Net<float> > net_;
  cv::Size input_geometry_;
  int num_channels_;
  cv::Mat mean_;
  float nor_val = 1.0;
  int resize_mode = 0;
public:
  float w_scale = 1;
  float h_scale = 1;
};

Detector::Detector(const string& model_file,
                   const string& weights_file,
                   const string& mean_value,
				   const float confidence_threshold,
				   const float normalize_value,
				   const string& cpu_mode,
				   const int resize) {
  if(cpu_mode == "cpu") {
	  Caffe::set_mode(Caffe::CPU);
  }
  else {
#ifdef CPU_ONLY
	  Caffe::set_mode(Caffe::CPU);
#else
	  Caffe::set_mode(Caffe::GPU);
    Caffe::SetDevice(0);
#endif // CPU_ONLY  
  }
  /* Load the network. */
  net_.reset(new Net<float>(model_file, TEST));
  net_->CopyTrainedLayersFrom(weights_file);

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();
  CHECK(num_channels_ == 3 || num_channels_ == 1)
    << "Input layer should have 1 or 3 channels.";
  input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

  /* Load the binaryproto mean file. */
  SetMean( mean_value);
  nor_val = normalize_value;
  resize_mode = resize;
}
float sec(clock_t clocks)
{
	return (float)clocks / CLOCKS_PER_SEC;
}
std::vector<vector<float> > Detector::Detect(cv::Mat& img) {
  Blob<float>* input_layer = net_->input_blobs()[0];
  //input_layer->Reshape(1, num_channels_,
  //                     input_geometry_.height, input_geometry_.width);
  /* Forward dimension change to all layers. */
  //net_->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);
  if (nor_val != 1.0) {
    uint64_t start5_time=current_timestamp();
	  img = Preprocess(img, &input_channels, nor_val);
    uint64_t end5_time=current_timestamp();
    std::cout << "process :" << end5_time - start5_time <<"ms" <<std::endl;
    std::cout << "nor_val :" <<nor_val <<std::endl;
  }
  else {
    uint64_t start4_time=current_timestamp();
	  img = Preprocess(img, &input_channels);
    uint64_t end4_time=current_timestamp();
    std::cout << "process :" << end4_time - start4_time <<"ms" <<std::endl;
    std::cout << "nor_val :" <<nor_val <<std::endl;
  }
	clock_t time;
	time = clock();
  uint64_t start3_time=current_timestamp();
	net_->Forward();
  uint64_t end3_time=current_timestamp();
  std::cout << "forward :" << end3_time - start3_time <<"ms" <<std::endl; 
	printf("Predicted in %f seconds.\n",  sec(clock() - time));
  /* Copy the output layer to a std::vector */
  Blob<float>* result_blob = net_->output_blobs()[0];
  const float* result = result_blob->cpu_data();
  const int num_det = result_blob->height();
  vector<vector<float> > detections;
  for (int k = 0; k < num_det; ++k) {
    if (result[0] == -1) {
      // Skip invalid detection.
      result += 7;
      continue;
    }
    vector<float> detection(result, result + 7);
    detections.push_back(detection);
    result += 7;
  }
  return detections;
}

/* Load the mean file in binaryproto format. */
void Detector::SetMean( const string& mean_value) {
  cv::Scalar channel_mean;
  if (!mean_value.empty()) {
    stringstream ss(mean_value);
    vector<float> values;
    string item;
    while (getline(ss, item, ',')) {
      float value = std::atof(item.c_str());
      values.push_back(value);
    }
    CHECK(values.size() == 1 || values.size() == num_channels_) <<
      "Specify either 1 mean_value or as many as channels: " << num_channels_;

    std::vector<cv::Mat> channels;
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(input_geometry_.height, input_geometry_.width, CV_32FC1,
          cv::Scalar(values[i]));
      channels.push_back(channel);
    }
    cv::merge(channels, mean_);
  }
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void Detector::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
  Blob<float>* input_layer = net_->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

cv::Mat Detector::LetterBoxResize(cv::Mat img, int w, int h)
{
	cv::Mat intermediateImg, outputImg;
	int delta_w, delta_h, top, left, bottom, right;
	int new_w = img.size().width;
	int new_h = img.size().height;

	if (((float)w / img.size().width) < ((float)h / img.size().height)) {
		new_w = w;
		new_h = (img.size().height * w) / img.size().width;
	}
	else {
		new_h = h;
		new_w = (img.size().width * h) / img.size().height;
	}
	cv::resize(img, intermediateImg, cv::Size(new_w, new_h));
	w_scale = w / (float)new_w;
	h_scale = h / (float)new_h;
	delta_w = w - new_w;
	delta_h = h - new_h;
	top = floor(delta_h / 2);
	bottom = delta_h - floor(delta_h / 2);
	left = floor(delta_w / 2);
	right = delta_w - floor(delta_w / 2);
	cv::copyMakeBorder(intermediateImg, outputImg, top, bottom, left, right, cv::BORDER_CONSTANT, (0, 0, 0));

	return outputImg;
}
cv::Mat Detector::Preprocess(const cv::Mat& img,
	std::vector<cv::Mat>* input_channels) {
	/* Convert the input image to the input image format of the network. */
	cv::Mat sample,resized_img;
	if (img.channels() == 3 && num_channels_ == 1)
		cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
	else if (img.channels() == 4 && num_channels_ == 1)
		cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
	else if (img.channels() == 4 && num_channels_ == 3)
		cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
	else if (img.channels() == 1 && num_channels_ == 3)
		cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
	else
		sample = img;

	cv::Mat sample_resized;
	if (sample.size() != input_geometry_) {
		if (resize_mode == 1) {
			sample_resized = LetterBoxResize(sample, input_geometry_.width, input_geometry_.height);
			resized_img = sample_resized;
		}
		else {
			cv::resize(sample, sample_resized, input_geometry_);
		}
		
	}
	else
		sample_resized = sample;

	cv::Mat sample_float;
	if (num_channels_ == 3)
		sample_resized.convertTo(sample_float, CV_32FC3);
	else
		sample_resized.convertTo(sample_float, CV_32FC1);

	cv::Mat sample_normalized;
	cv::subtract(sample_float, mean_, sample_normalized);

	/* This operation will write the separate BGR planes directly to the
	* input layer of the network because it is wrapped by the cv::Mat
	* objects in input_channels. */
	cv::split(sample_normalized, *input_channels);

	CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
		== net_->input_blobs()[0]->cpu_data())
		<< "Input channels are not wrapping the input layer of the network.";
	if(resize_mode == 1)
		return resized_img;
	else 
		return img;
}

cv::Mat Detector::Preprocess(const cv::Mat& img,
                            std::vector<cv::Mat>* input_channels, double normalize_value) {
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample, resized_img;
  if (img.channels() == 3 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry_) {
	  if (resize_mode == 1) {
		  sample_resized = LetterBoxResize(sample, input_geometry_.width, input_geometry_.height);
		  resized_img = sample_resized;
	  }
	  else {
		  cv::resize(sample, sample_resized, input_geometry_);
	  }
  }
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3, normalize_value);
  else
    sample_resized.convertTo(sample_float, CV_32FC1, normalize_value);

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
  if (resize_mode == 1)
	  return resized_img;
  else
	  return img;
}

int main(int argc,char **argv)
{
  const string& model_file = argv[1];              
  const string& weights_file = argv[2];
  const string& mean_value = "1.0,1.0,1.0";           //代替均值文件
  const float& confidence_threshold = 0.5;
  const float& normalize_value = 0.007843;                 // 影响检测的准确率与速度 0~1之间 0.007843
  const int& resize_mode = 0;    //0:WARP , 1:FIT_LARGE_SIZE_AND_PAD
  const string& cpu_mode = "gpu"; 
  Detector detector(model_file, weights_file, mean_value, confidence_threshold, normalize_value, cpu_mode, resize_mode);
  
  //string haikang = "rtsp://admin:123456jy@192.168.3.67:554/MPEG-4/ch1/main/av_stream";
  //string haikang2 = "rtsp://admin:123456jy@192.168.3.67/Streaming/Channels/1";
  cv::VideoCapture video_ncc;
  video_ncc.open(0);
  cv::Mat img ,img2;
  cv::Mat imagesave;
  string imagesavename;
  int frame_count = 0;
  int freame_count=0;
  while (true) 
  {
    ++freame_count;
    uint64_t whole_time1=current_timestamp();
	  bool success = video_ncc.read(img);
	  if (!success) 
    {
		  cout  << "Process  failed :" << frame_count << " frames from " <<endl;
		  break;
	  }
	  CHECK(!img.empty()) << "Error when read frame";
    uint64_t start1_time=current_timestamp();
	  std::vector<vector<float> > detections = detector.Detect(img);
    uint64_t end1_time=current_timestamp();
    cout<<"detections.size() :"<<detections.size()<<endl;
	  for (int i = 0; i < detections.size(); ++i) 
    {
		  const vector<float>& d = detections[i];
		  // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
		  CHECK_EQ(d.size(), 7);
		  const float score = d[2];
		  const int label_id = d[1];
      cout << "score  :" <<d[2] <<endl;
  		if ((score >= confidence_threshold && label_id==1) )
      {
  			  cv::Point pt1, pt2;
  				pt1.x = (img.cols*d[3]);
  				pt1.y = (img.rows*d[4]);
  				pt2.x = (img.cols*d[5]);
  				pt2.y = (img.rows*d[6]);
  		    cout<< "x1 :" <<pt1.x<<"y1 :"<<pt1.y<<"x2 :"<<pt2.x<<"y2 :"<<pt2.y<<endl;
  			  int index = static_cast<int>(d[1]);
  			  int green = 255 * ((index + 1) % 3);
  			  int blue = 255 * (index % 3);
  			  int red = 255 * ((index + 1) % 2);
  			  cv::rectangle(img, pt1, pt2, cvScalar(red, green, blue), 3, 8, 0);
  			  char label[100];
  			  sprintf(label, "%s,%f", CLASSES[static_cast<int>(d[1])], score);
  			  int baseline;
  			  cv::Size size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 0, &baseline);
  			  cv::Point pt3;
  			  pt3.x = pt1.x + size.width;
  			  pt3.y = pt1.y - size.height;
  		}
  	}
    uint64_t whole_time2=current_timestamp();
    std::cout<<"whole time :" << whole_time2-whole_time1<< "ms"<<std::endl;
    cv::namedWindow("show",CV_WINDOW_NORMAL);
    cv::resizeWindow("show",960,540);
	  cv::imshow("show", img);
	  cv::waitKey(1);
	}	  
	
  
  return 0;
}

