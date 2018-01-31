#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex>
#include <iostream>
#include <valarray>

// const double PI = 3.141592653589793238460;


// typedef std::complex<double> Complex;
// typedef std::valarray<Complex> CArray;
typedef std::complex<float> FFTComplex;
typedef std::valarray<FFTComplex> FFTArray;



class JZMath
{
public:
	JZMath();
	~JZMath();


	/***********************/
	/*       ��������       */
	/***********************/
public:
	template <class T> static void QuickSort(T* data, int lo, int hi);
private:
	template <class T> static int partition(T* data, int lo, int hi);
	template <class T> static void swap(T* a, T* b);



	/***********************/
	/*         FFT         */
	/***********************/
public:
	// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
	// Better optimized but less intuitive
	static void fft(FFTArray& x);

	// inverse fft (in-place)
	static void ifft(FFTArray& x);
};







/***********************/
/*       ��������       */
/***********************/

template <class T>
void JZMath::QuickSort(T* data, int lo, int hi)
{
	if (lo < hi)
	{
		int k = partition(data, lo, hi);
		QuickSort(data, lo, k - 1);
		QuickSort(data, k + 1, hi);
	}
}

template <class T>
int JZMath::partition(T* data, int lo, int hi)
{
	T key = data[hi];  //�����һ��Ԫ�أ�data[hi]Ϊ��Ԫ
	int i = lo - 1;
	for (int j = lo; j < hi; j++)   //ע��j��pָ�����r-1������r��
	{
		if (data[j] <= key)
		{
			i = i + 1;
			swap(&data[i], &data[j]);
		}
	}
	swap(&data[i + 1], &data[hi]);
	return i + 1;
}

template <class T>
void JZMath::swap(T* a, T* b)
{
	T temp = *a;
	*a = *b;
	*b = temp;
}
