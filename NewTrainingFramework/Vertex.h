#pragma once
#include "Math.h"

struct Vertex 
{
	Vector3 pos, color, norm, binorm, tgt;
	Vector2 uv, uv2;

	Vertex(const Vector3& pos_, const Vector3& norm_,const Vector3& binorm_,const Vector3& tgt_,const Vector2& uv_) 
		: pos(pos_), norm(norm_), binorm(binorm_), tgt(tgt_), uv(uv_) {}
	Vertex() {}
	Vertex(const Vector3& pos_, const Vector3& norm_, const Vector3& binorm_, const Vector3& tgt_, const Vector2& uv_, const Vector2& uv2_)
		: pos(pos_), norm(norm_), binorm(binorm_), tgt(tgt_), uv(uv_), uv2(uv2_)	 {}
	Vertex(const Vector3& pos_, const Vector3& col_) : pos(pos_), color(col_) {}
};