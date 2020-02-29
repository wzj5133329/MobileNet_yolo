# !/bin/bash

# This file is used to create the list of train and test files for training and testing procedures. After the run trainval.txt, test.txt and test_name_size.txt files will be generated. These files map each image to its label file.

# Modify the 'data_root_dir' to the location where your Images and Labels folders exist.
data_root_dir=/media/asd/DATA/PLATE/FAR/  # Modify the path with your folder having Images and Labels directories.
for ((DIRid=1;DIRid<15;DIRid++))
do
	current_dir=`pwd`
	echo "current_dir: "${current_dir}
	dst_all_tmp=${data_root_dir}"/all_tmp"${DIRid}".txt"
	dst_file_trainval=${data_root_dir}"/plate_trainval"${DIRid}".txt"
	dst_file_test=${data_root_dir}"/plate_test"${DIRid}".txt"
	dst_file_test_name_size=${data_root_dir}"/plate_train_name_size"${DIRid}".txt"

	length_imgs=`ls -l ${data_root_dir}/plate${DIRid}|grep '^-'|wc -l`
	length_labels=`ls -l ${data_root_dir}/xml${DIRid}|grep '^-'|wc -l`
	echo "all JPEGImages count: "${length_imgs}
	echo "all labels count: "${length_labels}
	if [ ${length_imgs} != ${length_labels} ]; then
		echo "JPEGImages and Labels not equal count. JPEGImages and Labels must be same count!"
	else
	    j=0
		for img in `ls ${data_root_dir}/plate${DIRid}|sort -h`
		do
			img_list[${j}]=${img}
			((j++))
		done

		k=0
		for label in `ls ${data_root_dir}/xml${DIRid}|sort -h`
		do
			label_list[${k}]=${label}
			((k++))
		done
		
		for ((i=1;i<${length_imgs};i++))
		do
			left=${img_list[i]}
			right=${label_list[i]}
			content="plate"${DIRid}"/"${left}" xml"${DIRid}"/"${right}
			echo ${content} >> ${dst_all_tmp}
			
		done
	fi

	# random shuffle the lines in all images
	arr=(`seq ${length_imgs}`)
	for ((i=0;i<10000;i++))
	do
		let "a=$RANDOM%${length_imgs}"
		let "b=$RANDOM%${length_imgs}"
		tmp=${arr[$a]}
		arr[$a]=${arr[$b]}
		arr[$b]=$tmp
	done

	# change this value to split trainval and test, default is 0.8
	split_ratio=0.9
	boundry=`echo | awk "{print int(${length_imgs}*${split_ratio})}"`
	echo "trainval count: "${boundry}
	for i in ${arr[@]:0:${boundry}}
	do
		sed -n "${i}p" ${dst_all_tmp} >> ${dst_file_trainval}
	done

	# generate test.txt and test_name_size.txt
	for i in ${arr[@]:${boundry}:((${length_imgs}-${boundry}))}
	do
		line=`sed -n -e "${i}p" ${dst_all_tmp}|cut -d ' ' -f 1`
		size=`identify ${data_root_dir}${line}|cut -d ' ' -f 3|sed -e "s/x/ /"`
		echo ${line}
		name=`basename ${line} .jpg`
		echo ${name}" "${size} >> ${dst_file_test_name_size}
		sed -n "${i}p" ${dst_all_tmp} >> ${dst_file_test}
	done

	rm -f ${dst_all_tmp}
done

echo "Done!"


