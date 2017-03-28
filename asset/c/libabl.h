#include <stddef.h>
#include <stdlib.h>
#include <math.h>

/*
 * Dynamic array
 */

typedef struct {
	void *values;
	size_t len;
	size_t cap;
} dyn_array;

#define DYN_ARRAY_CREATE_FIXED(elem_type, len) \
	dyn_array_create_fixed(sizeof(elem_type), len)

#define DYN_ARRAY_GET(ary, elem_type, idx) \
	(&((elem_type *) (ary)->values)[idx])

static inline dyn_array dyn_array_create_fixed(size_t elem_size, size_t len) {
	return (dyn_array) {
		.values = calloc(elem_size, len),
		.len = len,
		.cap = len,
	};
}

static inline void dyn_array_release(dyn_array *ary) {
	free(ary->values);
}

/*
 * float2
 */

typedef struct {
	float x;
	float y;
} float2;

static inline float2 float2_create(float x, float y) {
	return (float2) { x, y };
}
static inline float2 float2_fill(float x) {
	return (float2) { x, x };
}
static inline float2 float2_add(float2 a, float2 b) {
	return (float2) { a.x + b.x, a.y + b.y };
}
static inline float2 float2_sub(float2 a, float2 b) {
	return (float2) { a.x - b.x, a.y - b.y };
}
static inline float2 float2_mul_scalar(float2 a, float s) {
	return (float2) { a.x * s, a.y * s };
}
static inline float2 float2_div_scalar(float2 a, float s) {
	return (float2) { a.x / s, a.y / s };
}

/*
 * float3
 */

typedef struct {
	float x;
	float y;
	float z;
} float3;

static inline float3 float3_create(float x, float y, float z) {
	return (float3) { x, y, z };
}
static inline float3 float3_fill(float x) {
	return (float3) { x, x, x };
}
static inline float3 float3_add(float3 a, float3 b) {
	return (float3) { a.x + b.x, a.y + b.y, a.z + b.z };
}
static inline float3 float3_sub(float3 a, float3 b) {
	return (float3) { a.x - b.x, a.y - b.y, a.z - b.z };
}
static inline float3 float3_mul_scalar(float3 a, float s) {
	return (float3) { a.x * s, a.y * s, a.z * s };
}
static inline float3 float3_div_scalar(float3 a, float s) {
	return (float3) { a.x / s, a.y / s, a.z / s };
}

/*
 * Lengths and distances
 */

static inline float dot_float2(float2 a, float2 b) {
	return a.x * b.x + a.y * b.y;
}
static inline float dot_float3(float3 a, float3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float length_float2(float2 v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}
static inline float length_float3(float3 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline float dist_float2(float2 a, float2 b) {
	return length_float2(float2_sub(a, b));
}
static inline float dist_float3(float3 a, float3 b) {
	return length_float3(float3_sub(a, b));
}

/*
 * Random numbers
 */

float random_float(float min, float max);

static inline float2 random_float2(float2 min, float2 max) {
	return (float2) {
		random_float(min.x, max.x),
		random_float(min.y, max.y)
	};
}
static inline float3 random_float3(float3 min, float3 max) {
	return (float3) {
		random_float(min.x, max.x),
		random_float(min.y, max.y),
		random_float(min.z, max.z)
	};
}

/*
 * Runtime type information
 */

typedef enum {
	TYPE_END,
	TYPE_BOOL,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_FLOAT2,
	TYPE_FLOAT3,
} type_id;

typedef struct {
	type_id type;
	unsigned offset;
	const char *name;
} type_info;

void save(dyn_array *arr, const char *path, const type_info *info);
