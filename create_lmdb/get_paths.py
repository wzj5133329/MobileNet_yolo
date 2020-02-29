'''
将filename1下的文件以绝对路经的形式保存在filename2中
'''
import os
from os import listdir, getcwd
from os.path import join
if __name__ == '__main__':

	
	filename1 = '/media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/data/VOC0712/labels'
	filename2 = '/media/xiaosa/xiaosa/学习/深度学习/目标检测/一阶段网络/YOLO/MobileNet-YOLO-master/data/VOC0712/voc_lables.txt'
	file_list=os.listdir(filename1)       
	train_file=open(filename2,'a')
	for file_obj in file_list:                
		file_path=os.path.join(filename1,file_obj) 
	
		file_name,file_extend=os.path.splitext(file_obj)                             
		train_file.write(file_path+'\n')    
	train_file.close()

