#include <stddef.h>
#include <raylib.h>
#include <string.h>

struct vec {
	char *data;
        size_t size;
        size_t capacity;
};

const size_t VEC_INIT_SIZE = 256;

void vec_init(struct vec **vp) {
	struct vec *v = MemAlloc(sizeof(struct vec));

        v->data = MemAlloc(v->size = v->capacity = VEC_INIT_SIZE);
        *vp = v;
}

void vec_push(struct vec *vp, char c) {
#ifdef DEBUG
	assert(vp != NULL);
#endif
        if (vp->size - 1 >= vp->capacity) {
		char *newdata = MemAlloc(vp->capacity *= 2);
                memcpy(newdata, vp->data, vp->size * sizeof(*newdata));
                MemFree(vp->data);
                vp->data = newdata;
        }

        vp->data[vp->size++] = c; 
}
