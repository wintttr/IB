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
		return ptr[x * n + y];
	}

	T operator()(intex x, intex y)&& {
		if (x >= n || y >= m)
			throw;
		return move(ptr[x * n + y]);
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
		ptr[x * n + y] = value;
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

	T operator()(intex x, intex y)&& {
		return move((*inst_ptr)(x, y));
	}

	void set(intex x, intex y, const T& value) {
		if ((*this)(x, y) == value)
			return;
		if (inst_ptr.unique())
			inst_ptr->set(x, y, value);
		else {
			inst_ptr = make_shared<matrix_instance<T>>(*inst_ptr);
			inst_ptr->set(x, y, value);
		}
	}

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
};

