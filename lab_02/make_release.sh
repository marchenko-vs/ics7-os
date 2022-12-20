for src in *.c
do
	gcc -std=c99 -Wall -Werror ${src} -o ${src//.c/.out}
done

cd program_1
gcc -std=c99 -Wall -Werror main.c -o ../program_1.out
cd ../program_2
make

