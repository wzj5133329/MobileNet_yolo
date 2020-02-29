import numpy as np
import os
import shutil

label_path='/home/asd/Project/NCC_WZJ/YOLO/train/NCC/xml'
image_path = '/home/asd/Project/NCC_WZJ/YOLO/train/NCC/image2'
image_pathnew='/home/asd/Project/NCC_WZJ/YOLO/train/NCC/image'
xml_list = os.listdir(label_path)
image_list = os.listdir(image_path)
for axml in xml_list:
    (axmlname, xmlextension) = os.path.splitext(axml)
    for aimage in image_list:
        (aimagename, imageextension) = os.path.splitext(aimage)
        if(axmlname==aimagename):
            shutil.move(image_path+'/'+aimage, image_pathnew+'/'+aimage)
            break
    #print axmlname
    #print extension



