//
//  queens.cpp
//  
//
//  Created by Daiwei Wang on 6/11/13.
//
//

#include <gecode/int.hh>
#include <gecode/set.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <set>
#include <map>

using namespace Gecode;
using namespace std;

class queens : public Script {
protected:
    IntVarArray q;
    
public:
    enum {
        MODEL_ONE,
        MODEL_TWO,
        MODEL_THREE
    };
    queens(const SizeOptions& opt) : q(*this, 2*opt.size() - 1, -opt.size(), 2*opt.size() - 1) {
        // construct the variables
        const int n = opt.size();
        const int varSize = 2*n - 1;
        
        // stores all possible values for the determined key
        map< int, set<int> > allPossibleValueMap;
        
        // set the domains for the variables
        for (int index = 0; index < varSize; ++index) {
            int i = 1 - n + index;
            int end = varSize - abs(i);
            
            // the set stores all the possible values for q[index]
            set<int> possibleValueSet;
            possibleValueSet.insert(-n);
            for (int element = abs(i) + 1; element <= end; element += 2) {
                possibleValueSet.insert(element);
            }
            allPossibleValueMap.insert(make_pair(index, possibleValueSet));
            
            for (int position = -n + 1; position <= 2*n - 1; ++position) {
                if (!possibleValueSet.count(position)) {
                    rel(*this, q[index] != position);
                }
            }
        }
        
        // the first and second constraints
        // the number of (yi != -n) is n,
        // which equals that the number of (yi == -n) is n - 1
        count(*this, q, -n, IRT_EQ, n - 1);
        for (int j = 1; j <= varSize; ++j) {
            count(*this, q, j, IRT_LQ, 1);
        }
        
        // the third constraint, use three models
        switch (opt.model()) {
            // the arithmetic constraint
            case MODEL_ONE: {
                for (int indexI = 0; indexI < varSize; ++indexI) {
                    for (int indexJ = indexI + 1; indexJ < varSize; ++indexJ) {
                        rel(*this, abs(q[indexI] - q[indexJ]) != abs(indexI - indexJ));
                    }
                }
                break;
            }
            
            // the tuple sets constraint
            case MODEL_TWO: {
                // initialize the iterator map to iteratively add tuples
                map<int, set<int>::const_iterator> setIterator;
                map<int, set<int>::const_iterator> iteEnd;
                for (int index = 0; index < varSize; ++index) {
                    set<int>::const_iterator ite = allPossibleValueMap[index].begin();
                    setIterator.insert(make_pair(index, ite));
                    
                    set<int>::const_iterator endIte = allPossibleValueMap[index].end();
                    iteEnd.insert(make_pair(index, endIte));
                }
                
                // stores all the tuples to be added
                TupleSet allTupleSet;
                
                // this iterator is used to detect end conditions
                set<int>::const_iterator firstIteEnd = iteEnd[0];
                const int lastIndex = varSize - 1;
                // test all the possible tuples, then add the valid ones which satisfy the third constraint into tuple set
                while (setIterator[0] != firstIteEnd) {
                    // stores each tuple
                    IntArgs tuple;
                    
                    // use to detect whether a tuple is valid
                    vector<int> tupleVec;
                    bool validFlag = true;
                    for (int index = 0; index < varSize; ++index) {
                        set<int>::const_iterator ite = setIterator[index];
                        int value = *ite;
                        if (tupleVec.empty()) {
                            tupleVec.push_back(value);
                            tuple << value;
                        } else {
                            // using the third constraint, if a new value doesn't satisfy the constraint, skip this tuple
                            for (int vecIndex = 0; vecIndex < tupleVec.size(); ++vecIndex) {
                                if (abs(index - vecIndex) == abs(tuple[vecIndex] -  value)) {
                                    validFlag = false;
                                    break;
                                }
                            }
                            if (validFlag == true) {
                                tupleVec.push_back(value);
                                tuple << value;
                            } else {
                                break;
                            }
                        }
                    }
                    
                    // if it is valid then add this tuple
                    if (validFlag == true) {
                        allTupleSet.add(tuple);
                    }
                    
                    // try next possible tuple
                    ++setIterator[lastIndex];
                    if (setIterator[lastIndex] == iteEnd[lastIndex]) {
                        setIterator[lastIndex] = allPossibleValueMap[lastIndex].begin();
                        for (int index = lastIndex - 1; index >= 0; --index) {
                            ++setIterator[index];
                            if (setIterator[index] != iteEnd[index]) {
                                break;
                            } else if (index != 0) {
                                setIterator[index] = allPossibleValueMap[index].begin();
                            }
                        }
                    }
                }
                allTupleSet.finalize();
                
                extensional(*this, q, allTupleSet);
                
                break;
            }
                
            // the DFAs constraint
            case MODEL_THREE: {
                // use the third constraint to test every pair of two variables
                for (int index = 0; index < varSize; ++index) {
                    // all the possible value for the first variable
                    set<int> valueSet1 = allPossibleValueMap[index];
                    for (int subIndex = index + 1; subIndex < varSize; ++subIndex) {
                        // use for testing
                        cout << index << endl;
                        cout << subIndex << endl;
                        
                        // stores the transition status.
                        DFA::Transition *t = new DFA::Transition[2*n*n];
                        
                        // use this index to set the transition values
                        int transitionIndex = 0;
                        
                        // key: transition state, value: the first variable's value
                        map<int, int> transitionMap;
                        
                        // get transtion states {0, value1, 2}, {0, value2, 3}...
                        for (set<int>::const_iterator set1Ite = valueSet1.begin(); set1Ite != valueSet1.end(); ++set1Ite) {
                            t[transitionIndex] = {0, *set1Ite, transitionIndex + 2};
                            transitionMap.insert(make_pair(transitionIndex + 2, *set1Ite));
                            ++transitionIndex;
                        }
                        
                        //all the possible value for the second variable
                        set<int> valueSet2 = allPossibleValueMap[subIndex];
                        
                        // tests every state, if the second variable satisfy the third constraint, then add the variable into the transition
                        for (map<int, int>::const_iterator mapIte = transitionMap.begin(); mapIte != transitionMap.end(); ++mapIte) {
                            for (set<int>::const_iterator set2Ite = valueSet2.begin(); set2Ite != valueSet2.end(); ++set2Ite) {
                                if (abs(index - subIndex) != abs(mapIte->second -  *set2Ite)) {
                                    t[transitionIndex] = {mapIte->first, *set2Ite, 1};
                                    ++transitionIndex;
                                }
                            }
                        }
                        t[transitionIndex] = {-1, 0, 0};
                        
                        // use 1 as the final state
                        int f[] = {1, -1};
                        DFA d(0, t, f);
                        cout << d << endl;
                        IntVarArgs x;
                        x << q[index] << q[subIndex];
                        extensional(*this, x, d);
                        delete[] t;
                    }
                }
                
                break;
            }
                
            default:
                break;
        }
        
        // post branching
        branch(*this, q, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
    }
    
    queens(bool share, queens& s) : Script(share, s) {
        q.update(*this, share, s.q);
    }
    
    virtual Script* copy(bool share) {
        return new queens(share, *this);
    }
    
    void print(std::ostream& os) const {
        for (int index = 0; index < q.size(); ++index) {
            os << q[index] << " ";
        }
        os << endl << endl;
    }
};

int main(int argc, char* argv[]) {
    SizeOptions opt("queens");
    opt.model(queens::MODEL_ONE, "1", "use arithmetic constraint");
    opt.model(queens::MODEL_TWO, "2", "use tuple sets constraint");
    opt.model(queens::MODEL_THREE, "3", "use DFAs constraint");
    opt.model(queens::MODEL_ONE);
    opt.solutions(0);
    opt.size(4);
    opt.parse(argc, argv);
    Script::run<queens, DFS, SizeOptions>(opt);
    return 0;
}