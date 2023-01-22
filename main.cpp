#include "matrix.h"

using namespace std;
//krasiv main :)
int main(int argc, char** argv) {
    Matrix matrix = Matrix(argv[1]);
    matrix.print_result();
    return 0;
}
