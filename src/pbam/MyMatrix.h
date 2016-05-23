//
//  MyMatrix.h
//  pb_solvers_code
//
/*
 Copyright (c) 2015, Teresa Head-Gordon, Lisa Felberg, Enghui Yap, David Brookes
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of UC Berkeley nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MyMatrix_h
#define MyMatrix_h

#include <stdio.h>
#include <vector>
#include <sstream>

using namespace std;


class MatrixAccessException: public exception
{
protected:
int i_, j_;
int nrows_, ncols_;

public:
MatrixAccessException(const int i, const int j,
                    const int nrows, const int ncols)
:i_(i), j_(j), nrows_(nrows), ncols_(ncols)
{
}

virtual const char* what() const throw()
{
ostringstream ss;
ss << "Cannot access point [" << i_ << "," <<  j_ <<
"] in matrix of size (" << nrows_ << "," << ncols_ << ")" << endl;
return ss.str().c_str();
}
};


enum ArithmeticType { ADDITION, MULTIPLICATION, INNER_PRODUCT };


class MatrixArithmeticException: public exception
{
protected:
  int nrows1_, ncols1_;
  int nrows2_, ncols2_;
  ArithmeticType type_;
  
public:
  
  MatrixArithmeticException(ArithmeticType type, const int nrows1,
                            const int ncols1, const int nrows2,
                            const int ncols2)
  :nrows1_(nrows1), ncols1_(ncols1), nrows2_(nrows2),
  ncols2_(ncols2), type_(type)
  {
  }
  
  virtual const char* what() const throw()
  {
    ostringstream ss;
    string start;
    if (type_ == ADDITION) start = "Cannot add matrices of sizes (";
    else if (type_ == MULTIPLICATION)
      start = "Cannot multiply matrices of size (";
    else if (type_ == INNER_PRODUCT)
      start = "Cannot find inner product of vectors of sizes (";
    else start = "Unknown arithmetic error with matrices of sizes (";
    ss << start << nrows1_ << "," << ncols1_ << ") and (" <<
    nrows2_ << "," << ncols2_ << ")" << endl;
    return ss.str().c_str();
  }
  
};


template <typename T>
class MyMatrix
{
protected:
  int                 nrows_;
  int                 ncols_;
  vector< vector<T> >  vals_; //Len of 1st vector is # of rows,
  //  length of second is ncols
public:
  
  /*
   Initialize an empty matrix (call default constructors of T)
   Default is 1 x 1 matrix
   */
  MyMatrix(const int nrows=1, const int ncols=1)
  : nrows_(nrows), ncols_(ncols), vals_ (nrows, vector<T>(ncols))
  {
  }
  
  MyMatrix(vector< vector<T> > vals)
  : vals_(vals), nrows_( (int) vals.size()), ncols_( (int) vals[0].size())
  {
  }
  
  /*
   Fill with a default value (good for initializing memory)
   */
  MyMatrix(const int nrows, const int ncols, T default_val)
  :nrows_(nrows), ncols_(ncols), vals_(nrows, vector<T> (ncols, default_val))
  {
    int i, j;
    for (i = 0; i < nrows; i++)
    {
      for (j = 0; j < ncols; j++)
      {
//        set_val(i, j, default_val);
        vals_[i][j] = default_val;
      }
    }
  }

  /*
   Set the value of a coordinate in this matrix given the position (i, j)
   */
  void set_val(const int i, const int j, T val)
  {
    vals_[i][j] = val;
  }
  
  /*
   Element access operator given position (i, j)
   */
  T& operator()(const int i, const int j)
  {
    if (i < 0 || j < 0 || i > nrows_ || j > ncols_)
    {
      throw MatrixAccessException(i, j, nrows_, ncols_);
    }
    else
    {
      return vals_[i][j];
    }
  }
  
  /*
   Addition operator returns new matrix
   */
  MyMatrix<T> operator+(MyMatrix<T>& rhs)
  {
    if (ncols_ != rhs.ncols_ || nrows_ != rhs.nrows_)
    {
      throw MatrixArithmeticException(ADDITION, nrows_, ncols_, rhs.nrows_,
                                      rhs.ncols_);
    }
    
    MyMatrix<T> result = MyMatrix<T>(nrows_, ncols_);
    int i, j;
    for (i = 0; i < nrows_; i++)
    {
      for (j= 0; j < ncols_; j++)
      {
        result.set_val(i, j, vals_[i][j] + rhs(i, j));
      }
    }
    return result;
  }
  
  /*
   summation operator adds to existing matrix
   */
  MyMatrix<T>& operator+=(MyMatrix<T>& rhs)
  {
    if (ncols_ != rhs.ncols_ || nrows_ != rhs.nrows_)
    {
      throw MatrixArithmeticException(ADDITION, nrows_, ncols_, rhs.nrows_,
                                      rhs.ncols_);
    }
    int i, j;
    for (i = 0; i < nrows_; i++)
    {
      for (j= 0; j < ncols_; j++)
      {
        set_val(i, j, vals_[i][j] + rhs(i, j));
      }
    }
    return *this;
  }
  
  
  /*
   Matrix multiplication. If this is size n x m, then rhs must be size m x p
   */
  MyMatrix<T> operator*(MyMatrix<T>& rhs)
  {
    if (ncols_ != rhs.nrows_)
    {
      throw MatrixArithmeticException(MULTIPLICATION, nrows_,
                                      ncols_, rhs.nrows_, rhs.ncols_);
    }
    
    int n, m, p;
    n = nrows_;
    m = ncols_;
    p = rhs.ncols_;
    
    MyMatrix<T> result = MyMatrix<T>(n, p);
    int i, j, k;
    T inner_sum;
    for (i = 0; i < n; i++)
    {
      for (j= 0; j < p; j++)
      {
        inner_sum = T();  // default constructor should be equivalent to zero
        for (k = 0; k < m; k++)
        {
          inner_sum += vals_[i][k] * rhs(k, j);
        }
        result.set_val(i, j, inner_sum);
      }
    }
    return result;
  }
  
  const int get_nrows() { return nrows_; }
  const int get_ncols() { return ncols_; }
  
};


/*
 Vector class is implemented as extension of matrix class but
 with only one column
 */
template<typename T>
class MyVector : MyMatrix<T>
{
public:
  
  /*
   Initialize empty vector given the size
   */
  MyVector(const int size=1)
  :MyMatrix<T>(size, 1)
  {
  }
  
  MyVector(vector<T> vals)
  :MyMatrix<T>((int) vals.size(), 1)
  {
    int i;
    for (i = 0; i < vals.size(); i++)
    {
      this->vals_[i][0] = vals[0];
    }
  }
  
  MyVector(const int size, T default_val)
  :MyMatrix<T>(size, 1)
  {
    int i;
    for (i = 0; i < size; i++)
    {
      this->vals_[i][0] = default_val;
    }
  }
  
  void set_val(const int i, T val)
  {
    MyMatrix<T>::set_val(i, 0, val);
  }
  
  /*
   Addition operator returns new vector
   */
  MyVector<T> operator+(MyVector<T>& rhs)
  {
    if (this->nrows_ != rhs.nrows_)
    {
      throw MatrixArithmeticException(ADDITION, this->nrows_, 1,
                                      rhs.nrows_, 1);
    }
    
    MyVector<T> result = MyVector<T>(this->nrows_);
    int i;
    for (i = 0; i < this->nrows_; i++)
    {
      result.set_val(i, this->vals_[i][0] + rhs[i]);
    }
    return result;
  }
  
  /*
   summation operator adds to existing vector
   */
  MyVector<T>& operator+=(MyVector<T>& rhs)
  {
    if (this->nrows_ != rhs.nrows_)
    {
      throw MatrixArithmeticException(ADDITION, this->nrows_, 1, rhs.nrows_,
                                      1);
    }
    int i;
    for (i = 0; i < this->nrows_; i++)
    {
        set_val(i, this->vals_[i][0] + rhs[i]);
    }
    return *this;
  }
  
  // scalar multiplication
  MyVector<T> operator*(T scal)
  {
    MyVector vout (get_nrows());
    for (int i = 0; i < get_nrows(); i++)
    {
      vout[i] = this->vals_[i][0] * scal;
    }
      
    return vout;
  }
  
  /*
   Access operator with brackets only requires one value
   */
  T& operator[](int i)
  {
    return this->vals_[i][0];
  }
  
  /*
   The multiplication operator now computes the inner product
   */
  T operator*(const MyVector<T>& rhs)
  {
    if (rhs->nrows_ != this->nrows)
    {
      throw MatrixArithmeticException(INNER_PRODUCT, this->nrows_,
                                      this->ncols_, rhs->nrows_, rhs->ncols_);
    }
    T out = T();
    int i;
    for(i = 0; i < this->nrows_; i++)
    {
      out = out + (this[i] * rhs[i]);
    }
    return out;
  }
  
  const int get_nrows() { return this->nrows_; }
  const int get_ncols() { return this->ncols_; }
};

/*
 Convenience classes for matrices of matrices, vectors of vectors
 and vectors of matrices.
 
 These are used by calling the name of the class and then ::type
 to retrieve the typedef that they contain.
 
 So to get a matrix of matrices containing ints you would write:
 
 MatofMats<int>::type my_mat;

 */
template <typename T>
struct MatOfMats
{
  typedef MyMatrix<MyMatrix<T> > type;
};

template <typename T>
struct VecOfVecs
{
  typedef MyVector<MyVector<T> > type;
};

template <typename T>
struct VecOfMats
{
  typedef MyVector<MyMatrix<T> > type;
};

#endif /* MyMatrix_h */
