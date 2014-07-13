flowshop-with-time-Delay
========================
    This project is a research project about flowshop with time delays. 
    At first, I use "Max-Plus" to make a model for this problem. and 
the model consist a  problem TSP"Travelling Salesman Problem", this problem 
is solved by Fortran, I can use it direcment. And then, I use Branch and Bound
to solve this problem, my search strategy is DFS. For each node, I calculate 
its lower bound and compare with upper bound. 
    After solving this problem, I have proposed a very effecient strategy about 
how to calculate the lower bound. It works good.
