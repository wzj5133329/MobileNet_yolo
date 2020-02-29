# MobileNet-YOLO Caffe

## MobileNet-YOLO ONLY ON PERSON

### HOW TO TRAIN ON DATASET

1.制作LMDB数据集 [教程](https://github.com/wzj5133329/MobileNet_yolo/tree/master/create_lmdb)

2.生成训练、测试需要的与数据集匹配的锚点框  [教程](https://github.com/wzj5133329/MobileNet_yolo/tree/master/create_lmdb)

3.训练 (修改prototxt中的锚点框)

> cd ./train/train_voc

> bash train_mobile_yolov3_lite.sh




### Build , Run and Training

See [wiki](https://github.com/eric612/MobileNet-YOLO/wiki)

在安装caffe的时候也可以采用修改Makefile.config文件直接make 编译

## Reference

> https://github.com/eric612/MobileNet-YOLO
