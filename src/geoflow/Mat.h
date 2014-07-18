///  @file    Mat.h
///  @author  Andrew Stevens
///  @brief Wrapper class for 2d and 3d arrays
///  @ingroup Geoflow
///  @version $Id$
///  @attention
///  @verbatim
///
/// APBS -- Adaptive Poisson-Boltzmann Solver
///
///  Nathan A. Baker (nathan.baker@pnnl.gov)
///  Pacific Northwest National Laboratory
///
///  Additional contributing authors listed in the code documentation.
///
/// Copyright (c) 2010-2014 Battelle Memorial Institute. Developed at the
/// Pacific Northwest National Laboratory, operated by Battelle Memorial
/// Institute, Pacific Northwest Division for the U.S. Department of Energy.
///
/// Portions Copyright (c) 2002-2010, Washington University in St. Louis.
/// Portions Copyright (c) 2002-2010, Nathan A. Baker.
/// Portions Copyright (c) 1999-2002, The Regents of the University of
/// California.
/// Portions Copyright (c) 1995, Michael Holst.
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// Redistributions of source code must retain the above copyright notice, this
/// list of conditions and the following disclaimer.
///
/// Redistributions in binary form must reproduce the above copyright notice,
/// this list of conditions and the following disclaimer in the documentation
/// and/or other materials provided with the distribution.
///
/// Neither the name of the developer nor the names of its contributors may be
/// used to endorse or promote products derived from this software without
/// specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
/// THE POSSIBILITY OF SUCH DAMAGE.
///
/// @endverbatim

#pragma once

#include <cmath>
#include <vector>

#include <Eigen/Core>

// The following is from modules.h, which includes this file, Mat.h.  We need
// the declaration here because we use it below in the deriv method.  If we
// don't declare it, clang and gcc > 4.4.7-4 will complain about not being able
// to resolve the function.
double dot(double x, double y, double z);

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Nota Bene!  Caveat Emptor!  Ad Nauseam!
// The geometric flow code was originally done in Fortran.  These templates are
// part of a port to C++ that was done quite literally.  The net result is that
// in _these_ templates, counting starts at '1' NOT at '0', as the creator,
// EWD intended (https://www.cs.utexas.edu/users/EWD/ewd08xx/EWD831.PDF).
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

template <typename T> struct Stencil;

template <typename T = double> class Mat;

template <typename T>
class Mat {
private:
	friend struct Stencil<T>;

private:
	size_t _nx,_ny,_nz;
public:
	size_t nx()const{return _nx;}
	size_t ny()const{return _ny;}
	size_t nz()const{return _nz;}

private:
	double _hx,_hy,_hz;

public:
	double hx()const{return _hx;}
	double hy()const{return _hy;}
	double hz()const{return _hz;}

private:
	Eigen::Matrix<T,Eigen::Dynamic,1> vec;

	Mat();

public:
	// hx, hy, and hz are the grid spacing, in Angstroms (presumably)
	Mat(size_t nx, size_t ny, size_t nz=1, T a=0):
		_nx(nx), _ny(ny), _nz(nz), _hx(1), _hy(1), _hz(1), vec(nx*ny*nz)
		{ vec.fill(a); }
	Mat(size_t nx, size_t ny, size_t nz, double hx, double hy, double hz, T a=0):
		_nx(nx), _ny(ny), _nz(nz), _hx(hx), _hy(hy), _hz(hz), vec(nx*ny*nz)
		{ vec.fill(a); }

	//Mat(const Mat&) default, vec is copyable

	~Mat() {};

	friend void swap(Mat<T> &a, Mat<T> &b) throw() {
		using std::swap;
		a.vec.swap(b.vec);
		swap(a._nx, b._nx);
		swap(a._ny, b._ny);
		swap(a._nz, b._nz);
		swap(a._hx, b._hx);
		swap(a._hy, b._hy);
		swap(a._hz, b._hz);
	}

	Eigen::Matrix<T,Eigen::Dynamic,1>& baseInterface()
	{
		return vec;
	}

	const Eigen::Matrix<T,Eigen::Dynamic,1>& baseInterface() const
	{
		return vec;
	}

	bool equalSize(const Mat<T>& rhs) const
	{
		return (this->_nx == rhs._nx) && (this->_ny == rhs._ny) &&
				(this->_nz == rhs._nz);
	}

	bool equalSpacing(const Mat<T>& rhs) const
	{
		return (this->_hx == rhs._hx) && (this->_hy == rhs._hy) &&
				(this->_hz == rhs._hz);
	}

	bool operator==(const Mat<T>& rhs) const
	{
		return (data() == rhs.data()) && equalSize(rhs) && equalSpacing(rhs);
	}

	T& operator()(size_t x, size_t y, size_t z=1)
	{
		return vec[index(x,y,z)];
	}

	T operator()(size_t x, size_t y, size_t z=1) const
	{
		return vec[index(x,y,z)];
	}

	T& operator[](size_t i)
	{
		return vec(i);
	}

	T operator[](size_t i) const
	{
		return vec(i);
	}

	size_t index(size_t x, size_t y, size_t z) const
	{
		return x-1 + _nx*((y-1) + _ny*(z-1));
	}

	T* data()
	{
		return vec.data();
	}

	const T* data() const
	{
		return vec.data();
	}

	size_t size()
	{
		return vec.size();
	}

	//should be iterator...
	T* end()
	{
		return vec.data() + size();
	}

	Mat<T>& operator=(T a)
	{
		vec.fill(a); return *this;
	}

	Mat<T>& operator=(Mat a)
	{
		swap(*this, a);
		return *this;
	}

	Stencil<T> stencilBegin()
	{
		return Stencil<T>(*this, 2,2,2);
	}

	Stencil<T> stencilEnd()
	{
		return Stencil<T>(*this, _nx-1,_ny-1,_nz-1);
	}
};

template<typename T>
struct Stencil : public std::iterator<std::forward_iterator_tag, T>
{
	//this should be a const iterator :)
	Mat<T>& _mat;
	const T halfhx,h2x,qrth2x;
	const T halfhy,h2y,qrth2y;
	const T halfhz,h2z,qrth2z;
	size_t i;
	const size_t yStep, zStep;

	T *c;

	Stencil(Mat<T>& mat, size_t x, size_t y, size_t z) : _mat(mat),
		halfhx(0.5/mat.hx()),				// W1(1)
		h2x(1.0/(mat.hx()*mat.hx())),		// W2(1)
		qrth2x(0.25/(mat.hx()*mat.hx())),	// WXY(1,1)
		halfhy(0.5/mat.hy()),				// ...
		h2y(1.0/(mat.hy()*mat.hy())),		// ...
		qrth2y(0.25/(mat.hy()*mat.hy())),	// ...
		halfhz(0.5/mat.hz()),				// ...
		h2z(1.0/(mat.hz()*mat.hz())),		// ...
		qrth2z(0.25/(mat.hz()*mat.hz())),	// ...
		i(mat.index(x,y,z)),				// Current index
		yStep(mat.nx()),					// Offset for each y increment
		zStep(mat.nx()*mat.ny()),			// Offset for each z increment
		c(mat.data() + i)					// Current value
	{}

	bool operator==(const Stencil<T>& rhs) const
	{
		return (this->_mat == rhs._mat) && (this->c == rhs.c);
	}

	bool operator!=(const Stencil<T>& rhs) const
	{
		return !(*this == rhs);
	}

	T& operator*()
	{
		return *c;
	}

	const T* operator->() //???
	{
		return c;
	}

	Stencil& operator++()
	{
		next();
		return *this;
	}

	Stencil operator++ ( int )
	{
		Stencil<T> clone( *this );
		operator++();
		return clone;
	}

	void next()
	{
		ptrStep(1);
		if ((i+1) % yStep == 0) {
			if ((i+1+yStep) % zStep == 0 ) {
				// Skip the last row in this z, and the first in the next z.
				// Plus we need to skip the last anf first columns, as below.
				ptrStep(2*yStep + 2);
			} else {
				// Skip the last column in the current row, and the first
				// column in the next row.
				ptrStep(2);
			}
		}
	}

	void ptrStep(size_t numSteps)
	{
		i += numSteps;
		c += numSteps;
	}

	const T* xm() const { return c - 1; }		// PHI(IX-1,IY,IZ)
	const T* xp() const { return c + 1; }		// PHI(IX+1,IY,IZ)
	const T* ym() const { return c - yStep; }	// PHI(IX,IY-1,IZ)
	const T* yp() const { return c + yStep; }	// PHI(IX,IY+1,IZ)
	const T* zm() const { return c - zStep; }	// PHI(IX,IY,IZ-1)
	const T* zp() const { return c + zStep; }	// PHI(IX,IY,IZ+1)

	const T* xym() const { return xm() - yStep; } // PHI(IX-1,IY-1,IZ)
	const T* xyp() const { return xp() + yStep; } // PHI(IX+1,IY+1,IZ)
	const T* xzm() const { return xm() - zStep; } // PHI(IX-1,IY,IZ-1)
	const T* xzp() const { return xp() + zStep; } // PHI(IX+1,IY,IZ+1)
	const T* yzm() const { return zm() - yStep; } // PHI(IX,IY-1,IZ-1)
	const T* yzp() const { return zp() + yStep; } // PHI(IX,IY+1,IZ+1)

	const T* xm_yp() const { return xm() + yStep; } // PHI(IX-1,IY+1,IZ)
	const T* xp_ym() const { return xp() - yStep; } // PHI(IX+1,IY-1,IZ)
	const T* xm_zp() const { return xm() + zStep; } // PHI(IX-1,IY,IZ+1)
	const T* xp_zm() const { return xp() - zStep; } // PHI(IX+1,IY,IZ-1)
	const T* ym_zp() const { return ym() + zStep; } // PHI(IX,IY-1,IZ+1)
	const T* yp_zm() const { return yp() - zStep; } // PHI(IX,IY+1,IZ-1)

	T dx() const { return halfhx * (*xp() - *xm()); } // PHIX
	T dy() const { return halfhy * (*yp() - *ym()); } // PHIY
	T dz() const { return halfhz * (*zp() - *zm()); } // PHIZ

	T dxx() const { return h2x * (*xp() - 2.0*(*c) + *xm()); } // PHIXX
	T dyy() const { return h2y * (*yp() - 2.0*(*c) + *ym()); } // PHIYY
	T dzz() const { return h2z * (*zp() - 2.0*(*c) + *zm()); } // PHIZZ

	T dxy() const { return qrth2x * (*xyp() + *xym() - *xm_yp() - *xp_ym()); } // PHIXY
	T dxz() const { return qrth2y * (*xzp() + *xzm() - *xm_zp() - *xp_zm()); } // PHIXZ
	T dyz() const { return qrth2z * (*yzp() + *yzm() - *ym_zp() - *yp_zm()); } // PHIYZ

	T deriv(T tx) const {
// From the following Fortran...
// DPHI=(1.0D0+PHIX**2+PHIY**2)*PHIZZ+(1.0D0+PHIX**2+PHIZ**2)*PHIYY+  &
// 	(1.0D0+PHIY**2+PHIZ**2)*PHIXX
//   ...
// DPHI=DPHI-2.D0*(PHIXY*PHIX*PHIY+PHIXZ*PHIX*PHIZ+PHIYZ*PHIY*PHIZ)
		double dphi =
			( 1.0 + dx()*dx() + dy()*dy() )*dzz()
		  + ( 1.0 + dx()*dx() + dz()*dz() )*dyy()
		  + ( 1.0 + dy()*dy() + dz()*dz() )*dxx();

		dphi -= 2.0 * ( dx()*dy()*dxy() + dx()*dz()*dxz() + dy()*dz()*dyz() );

// GRAM=(1.D0+PHIX**2+PHIY**2+PHIZ**2)
		double gram = 1.0 + dot(dx(),dy(),dz());

// DPHI=DPHI/GRAM+SQRT(GRAM)*PHITOTX(IX,IY,IZ)
		double d = dphi/gram + sqrt(gram)*tx;
		return d;
	}

};
