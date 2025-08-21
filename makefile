find_duplicates: find_duplicates.o dir.o sorted_list.o
	cc -o find_duplicates find_duplicates.o dir.o sorted_list.o

find_duplicates.o: find_duplicates.c dir.h sorted_list.h
	cc -c find_duplicates.c

dir.o: dir.c dir.h
	cc -c dir.c

sorted_list.o: sorted_list.c sorted_list.h
	cc -c sorted_list.c
