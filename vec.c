#include <stddef.h>
#include <raylib.h>
#include <string.h>

#include "vec.h"

const size_t VEC_INIT_CAPACITY = 256;

void vec_init(struct vec **vp) {
	struct vec *v = MemAlloc(sizeof(struct vec));

        v->size = 0;
        v->data = MemAlloc(v->capacity = VEC_INIT_CAPACITY);
        *vp = v;
}

void vec_push(struct vec *vp, char c) {
#ifdef DEBUG
	assert(vp != NULL);
#endif
        if (vp->size >= vp->capacity - 1) {
		char *newdata = MemAlloc(vp->capacity *= 2);
                memcpy(newdata, vp->data, vp->size * sizeof(*newdata));
                MemFree(vp->data);
                vp->data = newdata;
        }

        vp->data[vp->size++] = c;
}

size_t vec_len(struct vec *vp) {
#ifdef DEBUG
	assert(vp != NULL);
#endif
        return vp->size;
}

void vec_set(struct vec *vp, size_t index, char c) {
#ifdef DEBUG
	assert(vp != NULL);
#endif
        if (index < vp->size)
		vp->data[index] = c;
}

void vec_free(struct vec *vp) {
	MemFree(vp->data);
	MemFree(vp);
}
