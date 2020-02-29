import xml.etree.cElementTree as ET
import os
path_root = ['/home/asd/Project/NCC_WZJ/YOLO/train/VOC/VOC2007/Annotations',
             '/home/asd/Project/NCC_WZJ/YOLO/train/VOC/VOC2012/Annotations']
 
CLASSES = [
            "person"
           ]
for anno_path in path_root:
    xml_list = os.listdir(anno_path)
    for axml in xml_list:
        project =0
        projectperson=0
        path_xml = os.path.join(anno_path, axml)
        tree = ET.parse(path_xml)
        root = tree.getroot()
 
        for child in root.findall('object'):
            project+=1
            
            name = child.find('name').text
            if not name in CLASSES:            #if project is not person ,remove it
                root.remove(child)
                projectperson+=1
        if(projectperson!=project):             #projectperson==project mean this xml not contain person
            tree.write(os.path.join('/home/asd/Project/NCC_WZJ/YOLO/train/VOC/VOC0712/Annotations', axml))

