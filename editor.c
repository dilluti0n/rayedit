#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "editor.h"
#include "line.h"
#include "vector.h"
#include "log.h"

#include "config.h"

DEFINE_VECTOR(Vec_slinep, struct line *);

struct ed_buf {
	/* editor */
	Vec_slinep *line_vec;
	size_t cur_row;
	size_t cur_col;
	size_t scroll_row;

	/* file */
	const char *file_name;	/* absolute path for the file */
	const char *raw;
	size_t raw_size;
};

void eb_init(struct ed_buf **eb) {
	struct ed_buf *neb = mem_malloc(sizeof(struct ed_buf));

	Vec_slinep_init(&neb->line_vec);
	neb->cur_row = 0;
	neb->cur_col = 0;
	neb->scroll_row = 0;
	neb->file_name = NULL;
	neb->raw = NULL;
	neb->raw_size = 0;

	*eb = neb;
}

void eb_free(struct ed_buf *eb) {
	size_t len = Vec_slinep_len(eb->line_vec);

	for (size_t i = 0; i < len; i++) {
		struct line *curr;

		if ((curr = Vec_slinep_get(eb->line_vec, i)) != NULL)
			line_free(curr);
	}
	Vec_slinep_free(eb->line_vec);
	if (eb->raw != NULL)
		munmap((void *)eb->raw, eb->raw_size);
	if (eb->file_name != NULL)
		mem_free((void *)eb->file_name);
	mem_free(eb);
}

static inline struct line *eb_get_line(const struct ed_buf *eb, size_t index) {
	if (index == Vec_slinep_len(eb->line_vec))
		return NULL;
	return Vec_slinep_get(eb->line_vec, index);
}

#ifdef DEBUG
static void print_eb(struct ed_buf *eb) {
	log_printf(RED_LOG_DEBUG, "eb->line_vec: %p\n\
eb->cur_row: %lu\n\
eb->cur_col: %lu\n\
eb->scroll_row: %lu\n\
eb->file_name: %s\n\
eb->raw: %p\n\
eb->raw_size: %lu\n",
	       eb->line_vec,
	       eb->cur_row,
	       eb->cur_col,
	       eb->scroll_row,
	       eb->file_name,
	       eb->raw,
	       eb->raw_size);
}


static void print_eb_vec(struct ed_buf *eb) {
	if (eb->line_vec != NULL) {
		size_t len = Vec_slinep_len(eb->line_vec);
		for (size_t i = 0; i < len; i++) {
			log_printf(RED_LOG_DEBUG,
				   "line %u, %p\n",
				   i, Vec_slinep_get(eb->line_vec, i));
		}
	}
}
#endif

#ifdef DEBUG
#define PRINT_EB(eb) print_eb(eb)
#define PRINT_EB_VEC(eb) print_eb_vec(eb)
#else
#define PRINT_EB(eb) ((void)0)
#define PRINT_EB_VEC(eb) ((void)0)
#endif

static inline struct line *ensure_line(struct ed_buf *eb, size_t row) {
	ASSERT(row <= Vec_slinep_len(eb->line_vec));
	PRINT_EB(eb);
	struct line *li;
	if (row == Vec_slinep_len(eb->line_vec)) {
		line_init(&li);
		Vec_slinep_push(eb->line_vec, li);
		return li;
	}

	li = Vec_slinep_get(eb->line_vec, row);
	if (li == NULL) {
		line_init(&li);

		struct line *oldli = Vec_slinep_get(eb->line_vec, row);

		if (oldli != NULL)
			line_free(oldli);

		Vec_slinep_set(eb->line_vec, row, li);
	}
	return li;
}

/* insert ch to cursor */
void eb_insert(struct ed_buf *eb, int ch) {
	ASSERT(eb != NULL);
	struct line *line = ensure_line(eb, eb->cur_row);
	line_insert(line, eb->cur_col++, ch);
}

void eb_delete_line(struct ed_buf *eb, size_t pos) {
	ASSERT(pos <= Vec_slinep_len(eb->line_vec));
	PRINT_EB_VEC(eb);
	if (pos < Vec_slinep_len(eb->line_vec)) {
		struct line *li;
		if ((li = Vec_slinep_get(eb->line_vec, pos)) != NULL)
			line_free(li);
		Vec_slinep_delete(eb->line_vec, pos);
	}
}

void eb_backspace(struct ed_buf *eb) {
	ASSERT(eb != NULL);
	struct line *curr = eb_get_line(eb, eb->cur_row);
	if (eb->cur_col == 0) {	      /* backspace to upper line */
		if (eb->cur_row == 0) /* nothing to remove */
			return;

		size_t upper_index = eb->cur_row - 1;
		struct line *upper = eb_get_line(eb, upper_index);

		/* cat to upper line and delete curr; if curr is null, cat and free are no needed */
		if (upper != NULL) {
			const size_t upper_last = line_get_last(upper);
			if (line_get_last(curr) > 0) {
				line_cat(upper, curr);
			}
			eb_delete_line(eb, eb->cur_row);
			eb->cur_col = upper_last;
		} else { /* upper line is NULL; just delete it! */
			eb_delete_line(eb, eb->cur_row);
			eb->cur_col = 0;
		}

		--eb->cur_row;
	} else {		/* backspace curr line */
		line_delete(curr, --eb->cur_col);
	}

}

void eb_newline(struct ed_buf *eb) {
	struct line *curr_line = eb_get_line(eb, eb->cur_row);
	struct line *newline = NULL;

	if (curr_line != NULL && eb->cur_col < line_get_last(curr_line)) {
		line_split(curr_line, eb->cur_col, &newline);
	} else {
		newline = NULL;
	}

	Vec_slinep_insert(eb->line_vec, ++eb->cur_row, newline);
	eb->cur_col = 0;
}

void eb_set_cur_prev_line(struct ed_buf *eb) {
	if (eb->cur_row > 0) {
		--eb->cur_row;
		eb->cur_col = 0;
	}
}

static inline size_t eb_get_last(struct ed_buf *eb, size_t col) {
	struct line *li = eb_get_line(eb, col);

	return li != NULL? line_get_last(li) : 0;
}

void eb_set_cur_backward(struct ed_buf *eb) {
	if (eb->cur_col > 0) {
		--eb->cur_col;
	} else if (eb->cur_row > 0) {
		eb_set_cur_prev_line(eb);
		eb->cur_col = eb_get_last(eb, eb->cur_row);
	}
}

static inline bool is_cur_col_last(struct ed_buf *eb) {
	struct line *li = eb_get_line(eb, eb->cur_row);
	if (li == NULL)
		return true;
	return eb->cur_col == line_get_last(li);
}

void eb_set_cur_next_line(struct ed_buf *eb) {
	if (eb->cur_row < eb_get_line_num(eb)) {
		eb->cur_row++;
		eb->cur_col = 0;
	}
}

void eb_set_cur_forward(struct ed_buf *eb) {
	if (is_cur_col_last(eb)) {
		eb_set_cur_next_line(eb);
	} else {
		++eb->cur_col;
	}
}

size_t eb_get_cur_col(struct ed_buf *eb) {
	return eb->cur_col;
}

size_t eb_get_cur_row(struct ed_buf *eb) {
	return eb->cur_row;
}

size_t eb_get_line_num(struct ed_buf *eb) {
	return Vec_slinep_len(eb->line_vec);
}

const char *eb_get_line_string(struct ed_buf *eb, size_t pos) {
	struct line *li = eb_get_line(eb, pos);
	return li == NULL? "" : line_get_string(li);
}

void eb_get_line_slice(const struct ed_buf *eb, size_t pos, struct slice *sl) {
	struct line *li = eb_get_line(eb, pos);
	if (li == NULL) {
		sl->ptr = NULL;
		sl->len = 0;
	} else {
		line_get_slice(li, sl);
	}
}

void eb_bind(struct ed_buf *eb, const char *path) {
	char *resolved = mem_malloc(PATH_MAX);

	eb->file_name = resolved;

	if (realpath(path, resolved) != NULL)
		return;

	if (path[0] == '/') {	/* path is absolute but file not exists */
		strcpy(resolved, path);
		return;
	}

	/* path is not absolute and file not exists */
	char cwd[PATH_MAX];

	if (getcwd(cwd, PATH_MAX) != NULL &&
	    strlen(cwd) + strlen(path) < PATH_MAX) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
		snprintf(resolved, PATH_MAX, "%s/%s", cwd, path);
#pragma GCC diagnostic pop
		return;
	}

	if (errno != ENAMETOOLONG) { /* getcwd fail with other reasons */
		log_printf(RED_LOG_WARNING, "getcwd: %s\n", strerror(errno));
	} else {		/* getcwd fail with too long cwd or cwd + path too long */
		log_printf(RED_LOG_WARNING, "absolute path of file too long (>= %d)\n",
			   PATH_MAX);
	}

	log_printf(RED_LOG_WARNING, "%s not binded to buffer\n", path);
	mem_free(resolved);
	eb->file_name = NULL;
}

void eb_load_file(struct ed_buf *eb) {
	if (eb->file_name == NULL)
		return;

	int fd = open(eb->file_name, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return;
	}
	struct stat statbuf = {};
	if (fstat(fd, &statbuf) == -1) {
		close(fd);
		perror("fstat");
		return;
	}

	const off_t filesize = statbuf.st_size;
	if (filesize == 0) {
		close(fd);
		return;
	}
	const char *raw = mmap(NULL, filesize, PROT_READ,
			       MAP_PRIVATE, fd, 0);
	close(fd);
	if (raw == MAP_FAILED) {
		perror("mmap");
		return;
	}

	const char *start = raw;
	const char *end = raw + filesize;

	while (start < end) {
		const char *tmp = memchr(start, '\n', end - start);
		const char *newline = tmp == NULL? end : tmp;

		/* handling CRLF */
		size_t len = (size_t)(newline - start);
		if (newline > start && start[len - 1] == '\r')
			len--;

		struct line *li;
		line_lazy_init(&li, start, len);
		Vec_slinep_push(eb->line_vec, li);

		start = newline + 1;
	}

	eb->raw = raw;
	eb->raw_size = filesize;
}

void eb_save_file(struct ed_buf *eb) {
	if (eb->file_name == NULL)
		return;

	char tmp_path[1024];
	snprintf(tmp_path, sizeof(tmp_path), "%s.tmpXXXXXX", eb->file_name);

	int tmp_fd = mkstemp(tmp_path);
	if (tmp_fd == -1) {
		perror("mktemp");
		return;
	}

	FILE *fp = fdopen(tmp_fd, "w");
	if (fp == NULL) {
		perror("fdopen");
		close(tmp_fd);
		unlink(tmp_path);
		return;
	}

	const size_t vec_len = Vec_slinep_len(eb->line_vec);

	for (size_t i = 0; i < vec_len; i++) {
		struct slice sl = {};
		eb_get_line_slice(eb, i, &sl);

		fwrite(sl.ptr, 1, sl.len, fp);
		fputc('\n', fp);
	}
	fclose(fp);

	if (rename(tmp_path, eb->file_name) == -1) {
		perror("rename");
		unlink(tmp_path);
	}
	log_printf(RED_LOG_INFO, "saved to: %s\n", eb->file_name);
}
