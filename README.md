# markov-decision-process
1. Input graph data from either a .csv file, .txt file, or console screen.

2. Solve this problem using a dynamic programming method.

3. Output the solution policy to a .txt, .csv, or the console screen.

4. Clearly explain how the output is generated from the input.

It is a typical oval track with two straight-aways and two turns.  Each straight-away has 4 segments, and each segment has an inside and outside lane.  The turns, each consisting of one segment, also have inside and outside lanes.

In this problem, a single race car is performing time trials.  The goal is to minimize the time needed to traverse segments (to complete one lap, for example).  The race car begins in segment 1 in the inside lane, and moves in the counter-clockwise direction.  It can travel at 0, 20, 40, or 60 mph.  Its initial speed is 0.  

Two types of actions are possible for the car:  changing speed, and changing lane.  Speed-related actions are:
-	go 20 mph. faster (subject to maximum speed of 60 mph. constraint)
-	go 20 mph. slower (subject to minimum speed of 0 mph. constraint)
-	stay at same speed

Lane-related actions are:
-	switch lanes
-	stay in same lane

State is represented by the car’s
-	segment
-	lane
-	speed

For straight segments, the state transition function has the following characteristics:
-	If the action is to go 20 mph faster, and the speed in the current state is not already the maximum, the speed in the next state will be 20 mph faster with probability 0.9, or will not change with probability 0.1.  If the speed in the current state is already the maximum, the speed in the next state will be the same with probability 1.
-	If the action is to go 20 mph slower, and the speed in the current state is not already the minimum, the speed in the next state will be 20 mph slower with probability 0.9, or will not change with probability 0.1.  If the speed in the current state is already the minimum, the speed in the next state will be the same with probability 1.
-	If the action is to stay at the same speed, this will happen with probability 1.
-	If the action is to switch lanes, this will happen with probability 0.9;  the car will stay in the same lane with probability 0.1.
-	If the action is to stay in the same lane, this will happen with probability 1.

The probabilities model unforeseen circumstances like oil slicks, driver hesitation, etc.

The transition function for the turns is the same as for the straight segments, with the following addition.  The faster the car is going through the turn, the higher the likelihood it will crash.  The likelihood is even higher in the inside lane of the turn.  A crash is modeled here as resetting the speed to 0 (as is done in many video games).  Finally, the segment 8 turn is banked, so the likelihood of crash here is lower than for the segment 3 turn.
