./main
mv gmon.out gmon.sum
./main
gprof -s main gmon.out gmon.sum
./main
gprof -s main gmon.out gmon.sum
./main
gprof -s main gmon.out gmon.sum
./main
gprof -s main gmon.out gmon.sum
gprof main gmon.sum > profile.txt