while [ ! -n "$srcdir" -o ! -d "$srcdir" ]
do
	read -p "Please input the srcdir:" srcdir
done
while [ ! -n "$destdir" ]
do
	read -p "Please input the destdir:" destdir
done
if [ ! -d "$destdir" ]; then 
	mkdir $destdir
fi
for f in $(ls $srcdir | grep .c$)
do
	cp $srcdir/$f destdir
done
ls -S $destdir
read -p "Please input tarname(Enter to skip the step):" tarname
if [ -n "$tarname" ]; then
	while [ ! -n "$size" ]
	do
		read -p "Please input the most min size:[b/k/m]" size
		case ${size:0-1} in
			b|B) size=${size%?};;
			k|K) size=$[${size%?}*1024];;
			m|M) size=$[${size%?}*1024*1024];;
			*) size="" ;;
		esac
	done
	mkdir $destdir/$tarname 
	for f in $(ls $destdir)
	do
		if [ $size -gt $(du -b $destdir/$f | awk '{print $1}') ]; then
			cp $destdir/$f $destdir/$tarname		
		fi
	done
	tar	-zcf $destdir/$tarname.tar.gz $destdir/$tarname
	rm -rf $destdir/$tarname
fi
echo "Program has finished!"
