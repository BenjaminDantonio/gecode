N Queens Problem
===========

N-Queens problem is defined as: place n queens on an $$$n × n$$$ chess board so that the queens do not attack each other. Now, we rotate the board clockwisely by 45 degrees as shown in Figure 1, and use a variable to denote the queen partition (if any) for each row (a diagonal in the original board). Thus the variables are $$$y\_{1-n}, y\_{2-n},..., y\_{0},..., y\_{n-2}, y\_{n-1}.$$$ While each variable takes values according to the new coordinate system, we use the value $$$−n$$$ to denote that there are no queens on a particular row. The domain of each variable $$$y_i$$$ is thus $$$ D_i = $$$ { $$$|i|+1,|i|+3,...,2n−|i|−1$$$ } $$$∪$$$ {$$$−n$$$} for $$$i ∈ [1−n,n−1] $$$. The set of constraints are given as follows:

- for $$$i∈[1−n,n−1]$$$: $$$\sum_i(y_i \ne−n)=n$$$
- for $$$i∈[1−n,n−1]$$$ and $$$j∈[1,2n−1]: \sum_i(y_i =j)≤1$$$
- for $$$i,j∈[1−n,n−1]$$$ and $$$i \ne j:|y_i−y_j| \ne |i−j|$$$

The model is modeled in three ways: arithmetic constraint, tuple sets and DFA.