.PHONY: all test clean deps tags 

CXX=g++
CXXFLAGS += -g -Wall -O -std=c++11                 

OPENCVLIBS = `pkg-config opencv --cflags --libs`

DEPS_INCLUDE_PATH= $(DLIB_PATH)  -I /usr/local/cuda-10.1/include/ -I /media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/include



TARGET = mobile_yolo

LIBS=  -lboost_system -lcaffe -lglog  -lprotobuf -lcudart  -lgflags 

OBJS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))     

  
$(TARGET): $(OBJS) 
	$(CXX)  -o $@  $^ $(LIBS)  $(OPENCVLIBS)  $(DEPS_LIB_PATH) 


%.o:%.cpp
	$(CXX) -c $(CXXFLAGS)  $< $(DEPS_INCLUDE_PATH) 


clean:
	rm -f *.o $(TARGET)
