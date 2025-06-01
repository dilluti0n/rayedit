#include <string.h>

#include "editor.h"
#include "redr.h"
#include "mem.h"

#include "vector.h"		/* children */

#define MIN(a, b) ((a) < (b))? (a) : (b)

struct view;			/* to define vector first */

DEFINE_VECTOR(Vec_sviewp, struct view *);

struct view_ops {
	void (*draw)(struct view *);
	void (*destroy)(struct view *);
};

enum view_type {
	VIEW_NONE,
	VIEW_ROOT,
	VIEW_EB
};

struct view {
	enum view_type type;
	struct redr_ctx *window;	/* context of window */

	/* tree */
	struct view *parent;
	Vec_sviewp *children;

	/* geometry */
	float posx;
	float posy;
	float w;
	float h;

	/* style */
	float font_size;
	float font_spacing;

	/* action */
	const struct view_ops *ops;

	/* flag */
	/* TODO: int isdirty:1; */
	int is_posx_frac:1;
	int is_posy_frac:1;
	int is_width_frac:1;	/* is width absolute or fraction of parant's width */
	int is_height_frac:1;	/* is height absolute or fraction of parant's width */
};

static inline void draw_textn(struct redr_ctx *window, int x, int y, int size,
			      struct redr_color c,
			      const char *text, size_t len) {
	char line[4096];
	size_t rlen;
	if ((rlen = len) > sizeof line - 1)
		rlen = sizeof line - 1;
	memcpy(line, text, len);
	line[rlen] = '\0';
	redr_draw_text(window, line, x, y, size, c);
}

static void draw_root(struct view *v) {
	redr_clear_background(v->window, WHITE);
}

const struct view_ops root_op = {
	.draw = draw_root,
	.destroy = NULL,
};

void view_init_root(struct view **root, struct redr_ctx *window) {
	struct view *nroot = mem_malloc(sizeof(struct view));

	nroot->type = VIEW_ROOT;
	nroot->window = window;

	nroot->parent = NULL;
	Vec_sviewp_init(&nroot->children);

	nroot->posx = 0;
	nroot->posy = 0;
	nroot->w = redr_width(window);
	nroot->h = redr_height(window);

	nroot->font_size = 0;
	nroot->font_spacing = 0;

	nroot->ops = &root_op;

	nroot->is_width_frac = 0;
	nroot->is_height_frac = 0;

	*root = nroot;
}

size_t view_child_num(struct view *vw) {
	return Vec_sviewp_len(vw->children);
}

void view_free_under(struct view *root) {
	Vec_sviewp *s1, *s2;
	Vec_sviewp_init(&s1);
	Vec_sviewp_init(&s2);

	/* DFS search */
	Vec_sviewp_push(s1, root);
	size_t len;
	while ((len = Vec_sviewp_len(s1)) > 0) {
		struct view *vw;

		vw = Vec_sviewp_get(s1, len - 1);
		Vec_sviewp_pop(s1);
		Vec_sviewp_push(s2, vw);

		size_t child_num = view_child_num(vw);
		for (size_t i = 0; i < child_num; i++)
			Vec_sviewp_push(s1, Vec_sviewp_get(vw->children, i));
	}
	Vec_sviewp_free(s1);

	/* free with reverse order of DFS */
	while ((len = Vec_sviewp_len(s2)) > 0) {
		struct view *vw;

		vw = Vec_sviewp_get(s2, len - 1);
		Vec_sviewp_pop(s2);

		if (vw->ops && vw->ops->destroy)
			vw->ops->destroy(vw);

		Vec_sviewp_free(vw->children);
		mem_free(vw);
	}
	Vec_sviewp_free(s2);
}

void view_draw_under(struct view *root) {
	Vec_sviewp *stack;
	Vec_sviewp_init(&stack);
	Vec_sviewp_push(stack, root);

	size_t len;
	while ((len = Vec_sviewp_len(stack)) > 0) {
		struct view *vw;

		vw = Vec_sviewp_get(stack, len - 1);
		Vec_sviewp_pop(stack);

		if (vw->ops && vw->ops->draw)
			vw->ops->draw(vw);

		size_t child_num = view_child_num(vw);
		for (size_t i = 0; i < child_num; i++)
			Vec_sviewp_push(stack, Vec_sviewp_get(vw->children, i));
	}
	Vec_sviewp_free(stack);
}

void view_resize(struct view *vw, float w, float h) {
	vw->w = w;
	vw->h = h;
}

struct view_eb {
	struct view base;
	struct ed_buf *eb;
	float scroll;
};

static float real_posx(struct view *veb) {
	if (veb->is_posx_frac)
		return veb->parent->w * veb->posx;
	return veb->posx;
}

static float real_posy(struct view *veb) {
	if (veb->is_posy_frac)
		return veb->parent->h * veb->posy;
	return veb->posy;
}

static float real_width(struct view *veb) {
	if (veb->is_width_frac)
		return veb->parent->w * veb->w;
	return veb->w;
}

static float real_height(struct view *veb) {
	if (veb->is_height_frac)
		return veb->parent->h * veb->h;
	return veb->h;
}

static void view_eb_draw(struct view *veb) {
	struct ed_buf *eb = ((struct view_eb *)veb)->eb;
	float scroll = ((struct view_eb *)veb)->scroll;

	float font_size = veb->font_size;
	float spacing = veb->font_spacing;
	struct redr_vector2 fontsize = redr_measure_text(veb->window, "M", font_size, spacing);
	struct redr_ctx *window =  veb->window;
	const size_t linenum_to_draw = MIN(eb_get_line_num(eb), real_height(veb) / fontsize.y);
	const float padding = 10.f;

#ifdef CONFIG_DEBUG
	log_printf(RED_LOG_DEBUG,
		   "[draw]: %lu lines ------\n", linenum_to_draw);
	log_printf(RED_LOG_DEBUG,
		   "[draw]: cursor: (%lu, %lu) ------\n",
		   eb_get_cur_col(eb), eb_get_cur_row(eb));
#endif

	for (size_t i = scroll; i <= linenum_to_draw + scroll; i++) {
		struct slice sl = {};
		eb_get_line_slice(eb, i, &sl);

#ifdef CONFIG_DEBUG
		log_printf(RED_LOG_DEBUG,
			   "[draw]: line %lu, ptr=%p, len=%lu\n",
			   i, sl.ptr, sl.len);
#endif
		float text_posx = real_posx(veb) + padding;
		float text_posy = real_posy(veb) + padding + font_size * i;
		size_t len = MIN((size_t) (real_width(veb) / fontsize.x), sl.len);

		if (i != eb_get_cur_row(eb)) {
			draw_textn(veb->window,
				   text_posx, text_posy,
				   font_size, BLACK, sl.ptr, len);
		} else { /* draw cursor */

#define BUFSIZE 4096
			char buf[BUFSIZE];
			size_t cur_col = eb_get_cur_col(eb);
			strncpy(buf, sl.ptr == NULL? "" : sl.ptr, MIN(sl.len, BUFSIZE));
			if (sl.len < 4096)
				buf[sl.len] = '\0';
			if (buf[cur_col] == '\0')
				buf[cur_col + 1] = '\0';
			buf[cur_col] = '_';
			draw_textn(window, text_posx, text_posy, font_size, BLACK, buf, len);
		}
	}
}

const struct view_ops view_eb_ops = {
	.draw = view_eb_draw,
	.destroy = NULL
};

struct view_eb *view_eb_create_under(struct view *upper,
				     struct ed_buf *eb,
				     float posx,
				     bool is_posx_frac,
				     float posy,
				     bool is_posy_frac,
				     float w,
				     bool is_w_frac,
				     float h,
				     bool is_h_frac,
				     float font_size,
				     float font_spacing,
				     float scroll) {
	struct view_eb *veb = mem_malloc(sizeof *veb);

	((struct view *)veb)->type = VIEW_EB;
	((struct view *)veb)->window = upper->window;

	((struct view *)veb)->parent = upper;
	Vec_sviewp_init(&((struct view *)veb)->children);

	((struct view *)veb)->posx = posx;
	((struct view *)veb)->posy = posy;
	((struct view *)veb)->w = w;
	((struct view *)veb)->h = h;

	((struct view *)veb)->font_size = font_size;
	((struct view *)veb)->font_spacing = font_spacing;

	((struct view *)veb)->ops = &view_eb_ops;

	((struct view *)veb)->is_width_frac = (int)is_w_frac;
	((struct view *)veb)->is_height_frac = (int)is_h_frac;
	((struct view *)veb)->is_posx_frac = (int)is_posx_frac;
	((struct view *)veb)->is_posy_frac = (int)is_posy_frac;

	veb->eb = eb;
	veb->scroll = scroll;

	Vec_sviewp_push(upper->children, (struct view *)veb);

	return veb;
}
