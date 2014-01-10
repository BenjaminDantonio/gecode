//
//  ABC.cpp
//  
//
//  Created by Daiwei Wang on 6/10/13.
//
//

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <algorithm>

using namespace Gecode;
using namespace std;

class ABC : public IntMaximizeScript {
protected:
    IntVarArray l;
    IntVar objective;
    
public:
    enum {
        MODEL_ONE,
        MODEL_TWO
    };
    ABC(const Options& opt) : l(*this, 4*4, 1, 16), objective(*this, 0, 150) {
        // construct the variables
        Matrix<IntVarArray> mat(l, 4, 4);
        
        // constraint of all difference
        distinct(*this, l);
        
        // store the bool value of the objective function
        IntVarArray offsetCount = IntVarArray(*this, 4*4, 0, 1);
        
        // constraint of adjacent cells
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                // find the adjacent cells
                int adjfc = max(0, col - 1);
                int adjtc = min(4, col + 2);
                int adjfr = max(0, row - 1);
                int adjtr = min(4, row + 2);
                IntVarArgs adjacentArgs = mat.slice(adjfc, adjtc, adjfr, adjtr);
                
                if (opt.model() == MODEL_ONE) {
                    IntVar adjCons = expr(*this, mat(col, row) - 1);
                    IntVar isNotOne = expr(*this, (mat(col, row) + 14)/16);
                    count(*this, adjacentArgs, adjCons, IRT_EQ, isNotOne);
                } else {
                    IntVar adjCons = expr(*this, mat(col, row) + 1);
                    IntVar isNotMax = expr(*this, 1 - mat(col, row)/16);
                    count(*this, adjacentArgs, adjCons, IRT_EQ, isNotMax);
                }
                
                rel(*this, offsetCount[4*row + col] == expr(*this, mat(col, row) - (4*(row + 1) + (col + 1) - 4) == 0));
            }
        }
        
        rel(*this, l[0] == 1);
        rel(*this, l[11] == 5);
        
        // define objective function
        rel(*this, objective == sum(offsetCount));
        
        // post branching
        branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    }
    
    ABC(bool share, ABC& s): IntMaximizeScript(share, s) {
        l.update(*this, share, s.l);
        objective.update(*this, share, s.objective);
    }
    
    virtual Space* copy(bool share) {
        return new ABC(share, *this);
    }
    
    void print(std::ostream& os) const {
        for (int index = 0; index < 4*4; ++index) {
            if ((index % 4) == 0) {
                os << endl;
            }
            os << l[index] << " ";
        }
        os << endl;
        os << endl;
    }
    
    virtual IntVar cost(void) const {
        return objective;
    }
};

int main(int argc, char* argv[]) {
    Options opt("ABC Path");
    opt.model(ABC::MODEL_ONE,
              "1", "start from first");
    opt.model(ABC::MODEL_TWO,
              "2", "start from last");
    opt.model(ABC::MODEL_ONE);
    opt.solutions(0);
    opt.parse(argc, argv);
    Script::run<ABC, BAB, Options>(opt);
    return 0;
}
