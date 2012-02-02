/*
 VFP math library for the iPhone / iPod touch
 
 Copyright (c) 2007-2008 Wolfgang Engel and Matthias Grundmann
 http://code.google.com/p/vfpmathlibrary/
 
 This software is provided 'as-is', without any express or implied warranty.
 In no event will the authors be held liable for any damages arising
 from the use of this software.
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it freely,
 subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must
 not claim that you wrote the original software. If you use this
 software in a product, an acknowledgment in the product documentation
 would be appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must
 not be misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source distribution.
 */

#import "MatrixTest.h"
#import "../../matrix_impl.h"

@implementation MatrixTest

- (id)initWithTextView:(UITextView*)output {
  if (self = [super initWithTextView:output]) {
    [self registerTest:@selector(testMatrix4Mul) showDescription:@"Matrix4Mul"];
    [self registerTest:@selector(testMatrix4Vector4Mul) showDescription:@"Matrix4Vector4Mul"];

    [self registerTest:@selector(testMatrix4Vector3MulUnit) 
       showDescription:@"Matrix4Vector3Mul"];

    [self registerTest:@selector(testMatrix4Vector3Mul) 
       showDescription:@"Matrix4Vector3Mul Unit"];
    
    [self registerTest:@selector(testMatrix4Vector3ArrayMul) 
       showDescription:@"Matrix4Vector3ArrayMul"];
    
    [self registerTest:@selector(testMatrix4Vector3ArrayMulUnit) 
       showDescription:@"Matrix4Vector3ArrayMul Unit"];
    
    [self registerTest:@selector(testMatrix4Invert)
       showDescription:@"Matrix4Invert"];
    
    // Setup test matrices.
    float matrix_a_data[16] = {  -45,   -91,   -81,    64,
                                  38,   -37,    90,   -94,
                                 -13,   -24,    53,    59,
                                 -63,    -3,   -11,    29 };
    memcpy(matrix_a_, matrix_a_data, 16 * sizeof(float));
    
    float matrix_b_data[16] = {  -30,    66,    17,     9,
                                  83,   -43,    51,    50,
                                 -24,    13,   -85,   -90,
                                   6,    55,    86,   -75};
    
    memcpy(matrix_b_, matrix_b_data, 16 * sizeof(float));
    
    float vec4_data[4] = { 31, -68, -77, -1 };
    memcpy(vec4_, vec4_data, 4 * sizeof(float) );
    
    float vec3_data[3] = { -49,  1, 39 };
    memcpy(vec3_, vec3_data, 3 * sizeof(float) );

  }
  
  return self;
}

- (void) dealloc {
  [super dealloc]; 
}

- (BOOL) testMatrix4Mul {
  Matrix4Mul(matrix_a_, matrix_b_, result_);
  
  float result_data[16] = {  3070,        -147,        9172,       -6860,
                            -9182,       -7336,       -8440,       13813,
                             8349,        4013,        -401,      -10383,
                             5427,       -4420,        9847,       -1887 };
  
  return memcmp(result_, result_data, 16 * sizeof(float)) == 0;
}

- (BOOL) testMatrix4Vector4Mul {
  Matrix4Vector4Mul(matrix_a_, vec4_, result_);
  float result_data[4] = { -2915, 1546, -12701, 3804 };
  
  return memcmp(result_, result_data, 4 * sizeof(float)) == 0;
}

- (BOOL) testMatrix4Vector3Mul {
  Matrix4Vector3Mul(matrix_a_, vec3_, 13.0, result_);
  float result_data[4] = { 917, 3447, 5983, -552 };
  return memcmp(result_, result_data, 4 * sizeof(float)) == 0;
}

- (BOOL) testMatrix4Vector3MulUnit {
  Matrix4Vector3Mul(matrix_a_, vec3_, result_);
  float result_data[4] = { 1673, 3483, 6115, -900 };
  
  return memcmp(result_, result_data, 4 * sizeof(float)) == 0;
}

- (BOOL) testMatrix4Vector3ArrayMul {
  Matrix4Vector3ArrayMul(4, matrix_a_, 13.0,  4 * sizeof(float), 
                         matrix_b_, 4 * sizeof(float), result_);
  
  float result_data[16] = { 2818,        -159,        9128,       -6744,
                           -6851,       -7225,       -8033,       12740,
                            1860,        3704,       -1534,       -7396,
                            -117,       -4684,        8879,         665};
  return memcmp(result_, result_data, 16 * sizeof(float)) == 0;
}


- (BOOL) testMatrix4Vector3ArrayMulUnit {
  Matrix4Vector3ArrayMul(4, matrix_a_, 4 * sizeof(float), 
                         matrix_b_, 4 * sizeof(float), result_);
  
  float result_data[16] = {   3574,        -123,        9260,       -7092,
                             -6095,       -7189,       -7901,       12392,
                              2616,        3740,       -1402,       -7744,
                               639,       -4648,        9011,         317 };  
  return memcmp(result_, result_data, 16 * sizeof(float)) == 0;
}

- (BOOL) testMatrix4Invert {
  Matrix4Invert(matrix_a_, result_);
  
  float result_data[16] = { 1.1089385e-03, -3.2081879e-03,  3.1474421e-03, -1.9249683e-02,
                           -8.1720734e-03, -5.9248884e-03, -1.8780583e-03,  2.6509888e-03,
                           -3.6345657e-03,  3.4951195e-03,  8.0113579e-03,  3.0511483e-03,
                            1.8505794e-04, -6.2566968e-03,  9.6820556e-03, -5.9039490e-03 };
    
  /*
  for (int i = 0; i < 16; ++i) {
    NSString* elem = [NSString stringWithFormat:@" %f ", result_[i]];
    output_.text = [output_.text stringByAppendingString:elem];
  }
  */
  
  BOOL ret_val = YES;
  for (int i = 0; i < 16; ++i)
    if (fabs(result_data[i] - result_[i]) / fabs(result_data[i]) > 1e-6)
      ret_val = NO;
  
  return ret_val;
}

@end
