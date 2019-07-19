/* 
 * CS:APP Data Lab 
 * 
 * Maor Shutman.
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  return ~((~((~x) & y)) & (~(x & (~y)))); // 8
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  return (!!(~x)) & (!(((x + 1) + x) + 1)); // 8
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int mask = (((((0xAA << 8) + 0xAA) << 8) + 0xAA) << 8) + 0xAA; // 6
  return !((mask & x) ^ mask); // 3
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (~x) + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int left_26_bits = !(((~0) << 6) & x); // 4
  int ones_56th_bits = !((x >> 4) ^ 3); // 3
  int first_4 = (x & 0xF); // 1
  int minus_first_4 = (~first_4) + 1; // 2
  int is_0_to_9 = !((9 + minus_first_4) >> 31); // 3
  return left_26_bits & ones_56th_bits & is_0_to_9; // 2
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  int x_01 = !!x; // 2
  int y_mask = (~x_01) + 1; // 1
  return (y & y_mask) | (z & (~y_mask)); // 4
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int equal = !(x ^ y); // 2
  
  int y_is_pos = !((y >> 31) & 1); // 3
  int x_is_pos = !((x >> 31) & 1); // 3

  int y_pos_x_neg = y_is_pos & (x_is_pos ^ 1); // 2
  int x_pos_y_neg = x_is_pos & (y_is_pos ^ 1); // 2

  int diff = y + ((~x) + 1); // 3
  int diff_is_pos = !((diff >> 31) & 1); // 3

  int y_gt_x = (y_pos_x_neg | diff_is_pos) & (!x_pos_y_neg); // 3
  return equal | y_gt_x; // 1
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  int msb = (x >> 31) & 1; // 2
  int v = ((~x) + 1) ^ x; // 3
  return (~msb) & (((v >> 31) + 1) & 1); // 5
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  // special case for tmin
  int is_x_tmin = !(x ^ (1 << 31)); // 3
  int mask_tmin = (~is_x_tmin) + 1; // 2

  int is_neg = (x >> 31) & 1; // 2

  // if negative and not TMin, take abs
  int neg_mask = ~(is_neg & (!is_x_tmin)) + 1; // 4
  
  int r;
  int shift;
  int out;
  int is_pow_2;
  int one_extra; 

  x = (x & (~neg_mask)) | (((~x) + 1) & neg_mask); // 6

  // is power of 2 for positives
  is_pow_2 = !((x + (~0)) & x); // 4

  one_extra = ~((is_pow_2 & is_neg) | !(x)) + 1; // 5

  // Modified from: https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
  // Section: Find the log base 2 of an N-bit integer in O(lg(N)) operations
  r =     (((((~x) + 1) + 0xFF + (0xFF << 8)) >> 31) & 1) << 4; x >>= r;                  // 9
  shift = (((((~x) + 1) + 0xFF) >> 31) & 1) << 3;               x >>= shift; r |= shift;  // 8
  shift = (((((~x) + 1) + 0xF) >> 31) & 1) << 2;                x >>= shift; r |= shift;  // 8
  shift = (((((~x) + 1) + 0x3) >> 31) & 1) << 1;                x >>= shift; r |= shift;  // 8
                                                                r |= (x >> 1);            // 2
  out = (32 & mask_tmin) |
         ((r + (1 & one_extra) + (2 & (~one_extra))) &
         (~mask_tmin)); // 9

  return out;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  int exp_uf;
  int exp_uf_two;
  unsigned frac_mask = 0x7FFFFF;
  unsigned sign_bit;
  unsigned uf_two;
  int new_exp;
  unsigned uf_frac;

  // if 0 return early
  if ((uf & 0x7FFFFFFF) == 0) return uf; // 2

  // s1 ^ s2, does not change
  sign_bit = uf & (1 << 31); // 2
  uf_two = 1 << 30; // 2.0f, 1 op

  exp_uf = (uf >> 23) & 0xFF; // 2
  exp_uf_two = (uf_two >> 23) & 0xFF; // 128, 2 ops

  uf_frac = frac_mask & uf; // 1

  new_exp = 0;

  // is uf NaN ?
  if ((exp_uf == 0xFF) && (frac_mask != 0)) return uf; // 3

  if (exp_uf != 0) { // normalized case, don't touch M, 1 op
    new_exp = exp_uf + exp_uf_two - 0x7F; // 2
    if (new_exp >= 0xFF) { // overflow, 1 op
      new_exp = 0xFF;
      uf_frac = 0; // +/- INF special case
    } else if (new_exp <= 0) { // overflow, 1 op
      uf_frac = 0;
    }  
  } else { // denormzlied case
    uf_frac <<= 1; // 1
    if (uf_frac >> 24) { // move from denorm to norm, 1 op
      new_exp = 127;
    }
  }
  
  return sign_bit | (new_exp << 23) | uf_frac; // 3
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  unsigned sign_bit;
  int exp_uf;
  unsigned frac_mask = 0x7FFFFF;
  unsigned uf_frac;
  int e;
  int shifted;

  // if 0 return early
  if ((uf & 0x7FFFFFFF) == 0) return 0;

  sign_bit = uf & (1 << 31);
  exp_uf = (uf >> 23) & 0xFF;
  
  // is uf NaN or +/-INF ?
  if (exp_uf == 0xFF) return 0x80000000u;
  if (exp_uf == 0) return 0; // denorm state is always 0

  uf_frac = frac_mask & uf;

  e = (exp_uf - 127);
  if (e < 0) return 0;

  uf_frac = uf_frac + (1 << 23);
  if (e > 31) {
    return 0x80000000u;
  } else {
    if (e < 23) {
      shifted = uf_frac >> (23 - e);
    } else {
      shifted = uf_frac << (e - 23);
    }
  }
  
  if (sign_bit) shifted *= -1;

  return shifted;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  unsigned exp = 0; 
  unsigned frac = 0; 
  
  if (x > 128) return 0x7F800000; // +INF
  if (x < (-126-23)) return 0;

  if ((x > -149) && (x < -126)) { // denorm
    frac = 1 << (23 - (126 - x));
  } else { // norm
    exp = x + 127;
  }

  return (exp << 23) | frac;
}
