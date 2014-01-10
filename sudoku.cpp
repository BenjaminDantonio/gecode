//
//  sudoku.cpp
//  
//
//  Created by Daiwei Wang on 3/10/13.
//
//

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>

using namespace Gecode;
using namespace std;

class sudoku : public Script {
protected:
    IntVarArray l;
    
public:
    enum {
        MODEL_ONE
    };
    sudoku(const Options& _opt) : l(*this, 9*9, 1, 9) {
        // construct the variables
        Matrix<IntVarArray> mat(l, 9, 9);
        int sudokuArray[] = {
            0, 9, 0, 5, 0, 7, 0, 8, 0,
            8, 0, 0, 0, 1, 0, 0, 0, 7,
            0, 0, 0, 8, 0, 9, 0, 0, 0,
            5, 0, 2, 0, 0, 0, 4, 0, 6,
            4, 7, 0, 0, 0, 0, 0, 2, 8,
            6, 0, 9, 0, 0, 0, 1, 0, 5,
            0, 0, 0, 7, 0, 2, 0, 0, 0,
            9, 0, 0, 0, 4, 0, 0, 0, 2,
            0, 2, 0, 1, 8, 5, 0, 6, 0};
        
        // constraint of row and column
        for (int index = 0; index < 9; ++index) {
            distinct(*this, mat.row(index));
            distinct(*this, mat.col(index));
        }
        
        // constraint of each 3 * 3 box and the specific box
        for (int index1 = 0; index1 < 9*9; ++index1) {
            for (int index2 = index1 + 1; index2 < 9*9; ++index2) {
                // index/9 is the row number, index%9 is the column number
                if ((index1/(9*3) == index2/(9*3)) && ((index1%9)/3 == (index2%9)/3)) {
                    rel(*this, l[index1], IRT_NQ, l[index2]);
                }
            }
            if (sudokuArray[index1] != 0) {
                rel(*this, l[index1], IRT_EQ, sudokuArray[index1]);
            }
        }
        
        // post branching
        branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
    }
    
    sudoku(bool share, sudoku& s) : Script(share, s) {
        l.update(*this, share, s.l);
    }
    
    virtual Script* copy(bool share) {
        return new sudoku(share, *this);
    }
    
    void print(std::ostream& os) const {
        for (int index = 0; index < 9*9; ++index) {
            if ((index % 9) == 0) {
                os << endl;
            }
            os << l[index] << " ";
        }
        os << endl;
        os << endl;
    }
};

int main(int argc, char* argv[]) {
    Options opt("sudoku");
    opt.model(sudoku::MODEL_ONE);
    opt.solutions(0);
    opt.parse(argc, argv);
    Script::run<sudoku, DFS, Options>(opt);
    return 0;
}