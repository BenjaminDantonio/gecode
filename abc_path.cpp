//
//  abc_path.cpp
//  
//
//  Created by Daiwei Wang on 8/11/13.
//
//

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <algorithm>
#include <map>

using namespace Gecode;
using namespace std;


class abc_path : public IntMaximizeScript {
protected:
    IntVarArray l;
    IntVar objective;
    
public:
    enum {
        MODEL_ONE,
        MODEL_TWO
    };
    abc_path(const SizeOptions& opt) : l(*this, opt.size()*opt.size(), 1, opt.size()*opt.size()), objective(*this, 0, 150) {
        
        // construct the variables
        const int n = opt.size();
        
        // constraint of all difference
        distinct(*this, l);
        
        // store the bool value of the objective function
        IntVarArray offsetCount = IntVarArray(*this, n*n, 0, 1);
        
        // constraint of adjacent cells, two models
        switch (opt.model()) {
            case MODEL_ONE: {
                // the variable representation in model one
                Matrix<IntVarArray> mat(l, n, n);
                for (int row = 0; row < n; ++row) {
                    for (int col = 0; col < n; ++col) {
                        // find the adjacent cells
                        int adjfc = max(0, col - 1);
                        int adjtc = min(n, col + 2);
                        int adjfr = max(0, row - 1);
                        int adjtr = min(n, row + 2);
                        IntVarArgs adjacentArgs = mat.slice(adjfc, adjtc, adjfr, adjtr);
                        
                        // ensure adjacent cell's value is mat(col,row)-1
                        IntVar adjCons = expr(*this, mat(col, row) - 1);
                        // test whether the cell mat(col,row) has value 1
                        IntVar isNotOne = expr(*this, (mat(col, row) + n*n - 2)/(n*n));
                        count(*this, adjacentArgs, adjCons, IRT_EQ, isNotOne);
                        
                        rel(*this, offsetCount[n*row + col] == expr(*this, mat(col, row) - (n*(row + 1) + (col + 1) - n) == 0));
                    }
                }
                
                // define objective function
                rel(*this, objective == sum(offsetCount));
                
                break;
            }
                
            case MODEL_TWO: {
                // l[sub+1] stands for xi, i==sub
                for (int sub = 1; sub < n*n; ++sub) {
                    IntVar subtraction = expr(*this, abs(l[sub] - l[sub - 1]));
                    rel(*this, subtraction <= n + 1);
                    rel(*this, subtraction >= 1);
                    rel(*this, abs(((l[sub] - 1)%n) - ((l[sub - 1] - 1)%n)) <= 1);
                    
                    // if xi == i, then set offsetCount as 1
                    rel(*this, offsetCount[sub] == expr(*this, l[sub] - (sub + 1) == 0));
                }
                rel(*this, offsetCount[0] == expr(*this, l[0] - 1 == 0));
                
                // define objective function
                rel(*this, objective == sum(offsetCount));
                
                break;
            }
                
            default:
                break;
        }
        
        
        rel(*this, l[0] == 1);
        
        // post branching
        branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    }
    
    abc_path(bool share, abc_path& s): IntMaximizeScript(share, s) {
        l.update(*this, share, s.l);
        objective.update(*this, share, s.objective);
    }
    
    virtual Space* copy(bool share) {
        return new abc_path(share, *this);
    }
    
    void print(std::ostream& os) const {
        const int n = (int)sqrt(l.size());
        for (int index = 0; index < l.size(); ++index) {
            if ((index % n) == 0) {
                os << endl;
            }
            os << l[index] << " ";
        }
        
        os << endl;
        os << "objective:" << objective << endl;
        os << endl;
    }
    
    virtual IntVar cost(void) const {
        return objective;
    }
    
};

int main(int argc, char* argv[]) {
    SizeOptions opt("ABC Path");
    opt.model(abc_path::MODEL_ONE,
              "1", "original model");
    opt.model(abc_path::MODEL_TWO,
              "2", "new model");
    opt.model(abc_path::MODEL_ONE);
    opt.size(4);
    opt.solutions(0);
    opt.parse(argc, argv);
    Script::run<abc_path, BAB, SizeOptions>(opt);
    return 0;
}

