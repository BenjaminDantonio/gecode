Gecode
======

Some c++ code based on Gecode to solve CSP problems.

See [Gecode](http://www.gecode.org/) for installing Gecode environment and compiling the code. If you are using Mac OSX 10.9 or higher, please add a command to compile the code after you have installed the environment. For example, for compiling sudoku.cpp:

	g++ --mmacosx-version-min=10.8 -framework gecode -o sudoku sudoku.cpp
	
This may results from the fact that Gecode doesn't support latest Mac framwork.

The problem definitions are posted in my blog. Please refer it for help.

[Sudoku Problem](http://wangdaiwei.github.io/posts/2014/Feb/20/sudoku-problem-definition/)

[N-Queens Problem](http://wangdaiwei.github.io/posts/2014/Feb/20/n-queens-problem-definition/)

