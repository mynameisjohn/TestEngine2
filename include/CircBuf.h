#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <vector>

template <class T>
class CircBuf{
protected:
	std::vector<T> data;
	int idx;
	int dim;
public:
	CircBuf(int dim=1);
	CircBuf(std::vector<T> in);
	void add(T x);
	unsigned int size();
	T last();
	void cycle();
	T get(int i);
};

#endif
