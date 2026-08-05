#pragma once

class MLVec {
public:
	float x = 0;
	float y = 0;
	float z = 0;

	MLVec() {};

	MLVec(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Add(MLVec other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	void Sub(MLVec other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	void Multiply(MLVec other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;
	}

	//Static Functions

	static MLVec Zero() {
		MLVec ret;
		ret.x = 0;
		ret.y = 0;
		ret.z = 0;
		return ret;
	}

	static MLVec One() {
		MLVec ret;
		ret.x = 1;
		ret.y = 1;
		ret.z = 1;
		return ret;
	}
};

struct MLRot {
public:
	float p = 0;
	float y = 0;
	float r = 0;

	MLRot() {};

	MLRot(float p, float y, float r) {
		this->p = p;
		this->y = y;
		this->r = r;
	}

	void Add(MLRot other) {
		p += other.p;
		y += other.y;
		r += other.r;
	}

	void Sub(MLRot other) {
		p -= other.p;
		y -= other.y;
		r -= other.r;
	}

	void Multiply(MLRot other) {
		p *= other.p;
		y *= other.y;
		r *= other.r;
	}

	//Static Functions

	static MLRot Zero() {
		MLRot ret;
		ret.p = 0;
		ret.y = 0;
		ret.r = 0;
		return ret;
	}

	static MLRot One() {
		MLRot ret;
		ret.p = 1;
		ret.y = 1;
		ret.r = 1;
		return ret;
	}
};