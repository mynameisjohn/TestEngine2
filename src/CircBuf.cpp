#include "CircBuf.h"
#include "Pose.h"
#include <iostream>
using namespace std;
template <class T>
CircBuf<T>::CircBuf(int dim){
	this->dim=dim;
	data.clear();
	data.resize(dim);
	idx=-1;
}

template <class T>
CircBuf<T>::CircBuf(std::vector<T> in)
: CircBuf(in.size()){
	for (int i=0;i<in.size();i++)
		add(in[i]);
	cout << idx << endl;
}

template <class T>
void CircBuf<T>::add(T x){
	idx=(idx+1)%dim;
	data[idx]=x;
}

template <class T>
void CircBuf<T>::cycle(){
	idx=(idx+1)%dim;
}
/*
template <class T>
vector<T>::iterator CircBuf<T>::begin(){
	return data.begin();
}

template <class T>
vector<T>::iterator CircBuf<T>::end(){
	return data.end();
}
*/
template <class T>
T CircBuf<T>::last(){
	return data[idx];
}

template <class T>
T CircBuf<T>::get(int i){
	return data[i%dim];
}

template <class T>
unsigned int CircBuf<T>::size(){
	return data.size();
}

template class CircBuf<int>;
template class CircBuf<Pose>;
