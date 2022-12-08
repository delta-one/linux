#ifndef IOU_ALLOC_CACHE_H
#define IOU_ALLOC_CACHE_H

/*
 * Don't allow the cache to grow beyond this size.
 */
#define IO_ALLOC_CACHE_MAX	512

struct io_cache_entry {
<<<<<<< HEAD
	struct io_wq_work_node node;
=======
	struct hlist_node	node;
>>>>>>> b7ba80a49124 (Commit)
};

static inline bool io_alloc_cache_put(struct io_alloc_cache *cache,
				      struct io_cache_entry *entry)
{
	if (cache->nr_cached < IO_ALLOC_CACHE_MAX) {
		cache->nr_cached++;
<<<<<<< HEAD
		wq_stack_add_head(&entry->node, &cache->list);
		/* KASAN poisons object */
		kasan_slab_free_mempool(entry);
=======
		hlist_add_head(&entry->node, &cache->list);
>>>>>>> b7ba80a49124 (Commit)
		return true;
	}
	return false;
}

static inline struct io_cache_entry *io_alloc_cache_get(struct io_alloc_cache *cache)
{
<<<<<<< HEAD
	if (cache->list.next) {
		struct io_cache_entry *entry;

		entry = container_of(cache->list.next, struct io_cache_entry, node);
		kasan_unpoison_range(entry, cache->elem_size);
		cache->list.next = cache->list.next->next;
		return entry;
=======
	if (!hlist_empty(&cache->list)) {
		struct hlist_node *node = cache->list.first;

		hlist_del(node);
		return container_of(node, struct io_cache_entry, node);
>>>>>>> b7ba80a49124 (Commit)
	}

	return NULL;
}

<<<<<<< HEAD
static inline void io_alloc_cache_init(struct io_alloc_cache *cache, size_t size)
{
	cache->list.next = NULL;
	cache->nr_cached = 0;
	cache->elem_size = size;
=======
static inline void io_alloc_cache_init(struct io_alloc_cache *cache)
{
	INIT_HLIST_HEAD(&cache->list);
	cache->nr_cached = 0;
>>>>>>> b7ba80a49124 (Commit)
}

static inline void io_alloc_cache_free(struct io_alloc_cache *cache,
					void (*free)(struct io_cache_entry *))
{
<<<<<<< HEAD
	while (1) {
		struct io_cache_entry *entry = io_alloc_cache_get(cache);

		if (!entry)
			break;
		free(entry);
=======
	while (!hlist_empty(&cache->list)) {
		struct hlist_node *node = cache->list.first;

		hlist_del(node);
		free(container_of(node, struct io_cache_entry, node));
>>>>>>> b7ba80a49124 (Commit)
	}
	cache->nr_cached = 0;
}
#endif
