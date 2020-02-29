#!/bin/bash
LOG=log/test-`date +%Y-%m-%d-%H-%M-%S`.log
../build/tools/caffe time -gpu 1 -model mobilenet_yolov3_lite_deploy.prototxt --gpu=1 2>&1 | tee $LOG
#-model examples/merge_bn/trt.prototxt   
