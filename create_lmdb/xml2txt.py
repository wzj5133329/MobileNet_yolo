'''
将一个xml文件解析成对应的txt文件
'''
#coding=utf-8
import xml.etree.ElementTree as ET
import pickle
import os
from os import listdir, getcwd
from os.path import join


classes = ["person"]


def convert(size, box):
    dw = 1./size[0]
    dh = 1./size[1]
    x = (box[0] + box[1])/2.0
    y = (box[2] + box[3])/2.0
    w = box[1] - box[0]
    h = box[3] - box[2]
    x = x*dw
    w = w*dw
    y = y*dh
    h = h*dh
    return (x,y,w,h)

def convert_annotation( image_id):
    print(image_id)
    in_file = open('/media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/data/VOC0712/xmls/%s.xml'%(image_id))
    out_file = open('/media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/data/VOC0712/labels/%s.txt'%(image_id), 'w')
    tree=ET.parse(in_file)
    root = tree.getroot()
    size = root.find('size')
    w = int(size.find('width').text)
    h = int(size.find('height').text)

    for obj in root.iter('object'):
        #if obj.find('difficult').text :
        #    print "aaa"
        #else:
        #    continue
        #difficult = obj.find('difficult').text
        #if not obj.find('name').text:
        #    break
        #cls = obj.find('name').text
        #if cls not in classes or int(difficult) == 1:
        #    break        
        #cls_id = classes.index(cls)
        if not obj.find('bndbox'):
            break
        xmlbox = obj.find('bndbox')
        b = (float(xmlbox.find('xmin').text), float(xmlbox.find('xmax').text), float(xmlbox.find('ymin').text), float(xmlbox.find('ymax').text))
        bb = convert((w,h), b)
        #out_file.write(str(cls_id) + " " + " ".join([str(a) for a in bb]) + '\n')
        out_file.write(str("0") + " " + " ".join([str(a) for a in bb]) + '\n')		#txt 中 文件格式 0,x(中心),y(中心),w,h

#wd = getcwd()


file_name = os.listdir('/media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/data/VOC0712/xmls')    #xml所在的文件夹
for image_id in file_name:
    imageid=os.path.splitext(image_id)[0]#获取文件名
    convert_annotation(imageid)
