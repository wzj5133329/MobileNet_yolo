## HOW TO CREATE LMDB FOR ONLY PEOPLE

1.执行get_personxml.py  得到只有人的xml

2.执行get_peosonimage.py将对应的图片移动（剪切）到相应的地方

3.执行creat_list.sh  生成 trian.txt ,test.txt 

4.执行creat_data.sh 生成 lmdb

## HOW TO GET OWN ANCHORS

1.执行xml2txt.py    将xml文件全部转为txt文件

2.执行get_paths.py  

3.执行gen_anchors.py 

