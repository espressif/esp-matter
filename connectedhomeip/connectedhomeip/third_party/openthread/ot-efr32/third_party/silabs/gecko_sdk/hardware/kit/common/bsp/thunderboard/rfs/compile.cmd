gcc -c -I../../.. -I../.. -I.. rfs_create.c -o rfs_create.o
gcc -o rfs_create rfs_create.o

REM rfs_create rfs_files.h testfiles/test.txt testfiles/1.bmp testfiles/2.bmp testfiles/3.bmp testfiles/4.bmp
rfs_create rfs_files.h testfiles/test.txt

gcc -c -I../../.. -I../.. -I.. rfs.c -o rfs.o
gcc -c -I../../.. -I../.. -I.. rfs_test.c -o rfs_test.o
gcc -o rfs_test rfs.o rfs_test.o


del *.o

