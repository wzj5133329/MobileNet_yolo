import numpy as np
import os
import shutil

label_path='../data/VOC0712/xmls'
image_path_1 = '../data/VOC2007/JPEGImages'
image_path_2='../data/VOC2012/JPEGImages'
image_pathnew='../data/VOC0712/images'
xml_list = os.listdir(label_path)
image_list1 = os.listdir(image_path_1)
image_list2 = os.listdir(image_path_2)
image_list = image_list1 + image_list2
for axml in xml_list:
    (axmlname, xmlextension) = os.path.splitext(axml)
    for aimage in image_list:
        (aimagename, imageextension) = os.path.splitext(aimage)
        if(axmlname==aimagename):
            if(os.path.exists(image_path_1+'/'+aimage)):
                shutil.move(image_path_1+'/'+aimage, image_pathnew+'/'+aimage)          #移动文件
                #shutil.copyfile("oldfile","newfile")                                 #复制文件
                break
            elif(os.path.exists(image_path_2+'/'+aimage)):
                shutil.move(image_path_2+'/'+aimage, image_pathnew+'/'+aimage)          #移动文件
                #shutil.copyfile("oldfile","newfile")                                 #复制文件
                break
        
    #print axmlname
    #print extension



