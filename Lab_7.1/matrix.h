#pragma once
#include<iostream>
#include<memory>
#include<immintrin.h>

using namespace std;

using intex = uint32_t;

inline unsigned int lzc(unsigned int x) {
	return 32 - _lzcnt_u32(x);
}

template<typename T>
class matrix_instance {
	intex n, m;
	T* ptr;

public:
	matrix_instance(intex x, intex y) : n(x), m(y) {
		ptr = new T[x * y];
	}

	matrix_instance(intex x, intex y, const T& alloc) : matrix_instance(x, y) {
		for (int i = 0; i < x; i++)
			for (int j = 0; j < y; j++)
				set(i, j, alloc);
	}

	matrix_instance(const matrix_instance& m) : matrix_instance(m.n, m.m) {
		for (intex i = 0; i < m.n; i++)
			for (intex j = 0; j < m.m; j++)
				set(i, j, m(i, j));
	}

	~matrix_instance() {
		delete[] ptr;
	}

	intex getn() const { return n; }
	intex getm() const { return m; }

	const T& operator()(intex x, intex y) const& {
		if (x >= n || y >= m)
			throw;
		return ptr[x * m + y];
	}

	T operator()(intex x, intex y)&& {
		if (x >= n || y >= m)
			throw;
		return move(ptr[x * m + y]);
	}

	bool operator==(const matrix_instance& op) const {
		if (getn() != op.getn() || getm() != op.getm())
			throw;

		for (intex i = 0; i < getn(); i++)
			for (intex j = 0; j < getm(); j++)
				if ((*this)(i, j) != op(i, j))
					return false;
		return true;
	}

	bool operator!=(const matrix_instance& op) const {
		return !((*this) == op);
	}

	void set(intex x, intex y, const T& value) {
		if (x >= n || y >= m)
			throw;
		ptr[x * m + y] = value;
	}
};

template<typename T>
class matrix {
	shared_ptr<matrix_instance<T>> inst_ptr;
public:
	intex getn() const { return inst_ptr->getn(); }
	intex getm() const { return inst_ptr->getm(); }

	int dbg_get_count() {
		return inst_ptr.use_count();
	}

	matrix(intex x, intex y) : inst_ptr(make_shared<matrix_instance<T>>(x, y)) {}

	matrix(intex x) : matrix(x, x) {}

	matrix(const matrix& m) {
		inst_ptr = m.inst_ptr;
	}

	matrix(matrix&& m) noexcept {
		swap(inst_ptr, m.inst_ptr);
	}

	~matrix() {
		inst_ptr.reset();
	}

	const T& operator()(intex x, intex y) const& {
		return (*inst_ptr)(x, y);
	}

	T operator()(intex x, intex y) && {
		return move((*inst_ptr)(x, y));
	}

	void set(intex x, intex y, const T& value);
	matrix degree(int k) const;

	matrix operator=(const matrix& m) {
		this->inst_ptr = m.inst_ptr;
		return *this;
	}

	matrix operator=(matrix&& m) {
		swap(this->inst_ptr, m.inst_ptr);
		return *this;
	}

	matrix operator*(const matrix& op) const;

	bool operator==(const matrix& op) const {
		return *(this->inst_ptr) == *(op.inst_ptr);
	}

	bool operator!=(const matrix& op) const {
		return !((*this) == op);
	}

	void resize(intex newn, intex newm) {
		matrix_instance<T>* temp_ptr = new matrix_instance<T>(newn, newm);

		for (int i = 0; i < newn; i++) {
			for (int j = 0; j < newm; j++)
				cout << &(temp_ptr->operator()(i, j)) << " ";
			cout << endl;
		}

		for (int i = 0; i < getn(); i++)
			for (int j = 0; j < getm(); j++)
				temp_ptr->set(i, j, (*this)(i, j));

		inst_ptr = shared_ptr<matrix_instance<T>>(temp_ptr);
	}
};


template<typename T>
void matrix<T>::set(intex x, intex y, const T& value) {
	if ((*this)(x, y) == value)
		return;
	if (inst_ptr.unique())
		inst_ptr->set(x, y, value);
	else {
		inst_ptr = make_shared<matrix_instance<T>>(*inst_ptr);
		inst_ptr->set(x, y, value);
	}
}

template<typename T>
matrix<T> matrix<T>::operator*(const matrix<T>& op) const {
	const matrix<T>& f = *this;
	const matrix<T>& s = op;

	if (f.getm() != s.getn())
		throw;

	intex n = f.getn(), m = s.getm(), v = f.getm();
	matrix<T> temp(n, m);

	for (intex i = 0; i < n; i++) {
		for (intex j = 0; j < m; j++) {
			T sum = 0;
			for (intex k = 0; k < v; k++)
				sum += f(i, k) * s(k, j);
			temp.set(i, j, sum);
		}
	}
	return temp;
}

template<typename T>
matrix<T> matrix<T>::degree(int k) const {
	int last = lzc(k);
	k &= ~(1 << (last - 1));
	matrix<T> temp(*this);

	while (k != 0) {
		for (int i = 0; i < (last - lzc(k)); i++)
			temp = temp * temp;
		temp = temp * (*this);

		last = lzc(k);
		k &= ~(1 << (last - 1));
	}
	return temp;
}