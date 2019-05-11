#CORRECT
3 < 5

3 <= 5
5 <= 5

4 == 4
4 != 3

CONSTRAIN(x) => if (x > 0) return 1 else return 0
INVERT_FROM_ZERO(x) => return 1 - x

3 < 5
5 - 3 = 2 CONSTRAIN(2)=1

3 <= 5
5 + 1 - 3 = 3 CONSTRAIN(3)=1

5 <= 5
5 + 1 - 5 = 1 CONSTRAIN(1)=1

4 == 4
4 - 4 = 0 ABS(0)=0 CONSTRAIN(0)=0 INVERT_FROM_ZERO(0)=1

4 != 3
4 - 3 = 1 ABS(1)=1 CONSTRAIN(1)=1

#INCORRECT
5 < 5

5 <= 3
5 <= 4

3 == 4
4 != 4


5 < 5
5 - 5 = 0 CONSTRAIN(0)=0

5 <= 3
3 + 1 - 5 = -1 CONSTRAIN(-1)=0

5 <= 4
4 + 1 - 5 = 0 CONSTRAIN(0)=0

3 == 4
3 - 4 = -1 ABS(-1)=1 CONSTRAIN(1)=1 INVERT_FROM_ZERO(1)=0

4 != 4
4 - 4 = 0 ABS(0)=0 CONSTRAIN(0)=0

# NEGATIVE CASES CORRECT
-5 < 5

-5 <= 3
-5 <= -4

-4 == -4
4 != -4

-5 < 5
5 - (-5) = 10 CONSTRAIN(10)=1

-5 <= 3
3 + 1 - (-5) = 9 CONSTRAIN(9)=1

-5 <= -4
-4 + 1 - (-5) = 2 CONSTRAIN(2)=1

-4 == -4
(-4) - (-4) = 0 ABS(0)=0 CONSTRAIN(0)=0 INVERT_FROM_ZERO(0)=1

4 != -4
4 - (-4) = 8 CONSTRAIN(8)=1

# NEGATIVE CASES INCORRECT
5 < -5

5 <= -3
-4 <= -5

4 == -4
-4 != -4

5 < -5
(-5) - 5 = -10 CONSTRAIN(-10)=0

5 <= -3
-3 + 1 - 5 = -7 CONSTRAIN(-7)=0

-4 <= -5
-5 + 1 - (-4) = 0 CONSTRAIN(0)=0

4 == -4
4 - (-4) = 8 ABS(8)=8 CONSTRAIN(8)=1 INVERT_FROM_ZERO(1)=0

-4 != -4
(-4) - (-4) = 0 ABS(0)=0 CONSTRAIN(0)=0

