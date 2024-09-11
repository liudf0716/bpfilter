/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2022 Meta Platforms, Inc. and affiliates.
 */

#pragma once

#include <linux/bpf.h>

#include "core/dump.h"

#define BF_PIN_PATH_LEN 64

#define _cleanup_bf_bpf_map_ __attribute__((__cleanup__(bf_bpf_map_free)))

enum bf_bpf_map_type
{
    BF_BPF_MAP_TYPE_ARRAY,
    BF_BPF_MAP_TYPE_HASH,
    _BF_BPF_MAP_TYPE_MAX,
};

struct bf_bpf_map
{
    int fd;
    char name[BPF_OBJ_NAME_LEN];
    char path[BF_PIN_PATH_LEN];
    enum bf_bpf_map_type type;
    size_t key_size;
    size_t value_size;
    size_t n_elems;
};

struct bf_marsh;

/**
 * Allocates and initializes a new BPF map object.
 *
 * @note This function won't create a new BPF map, but a bpfilter-specific
 * object used to keep track of a BPF map on the system.
 *
 * @todo Can @c value_size be 0?
 *
 * @param map BPF map object to allocate and initialize. Can't be NULL. On
 *            success, @c *map points to a valid @ref bf_bpf_map . On failure,
 *            @c *map remain unchanged.
 * @param name_suffix Suffix to use for the map name. Usually specify the
 *                    hook, front-end, or program type the map is used for.
 *                    Can't be NULL.
 * @param type Map type. Not all BPF maps are supported, see
 *             @ref bf_bpf_map_type for the full list of supported types.
 * @param key_size Size (in bytes) of a key in the map. Can't be 0.
 * @param value_size Size (in bytes) of an element of the map.
 * @param n_elems Number of elemets to reserve room for in the map. Can't be 0.
 * @return 0 on success, or a negative errno value on error.
 */
int bf_bpf_map_new(struct bf_bpf_map **map, const char *name_suffix,
                   enum bf_bpf_map_type type, size_t key_size,
                   size_t value_size, size_t n_elems);

/**
 * Create a new BPF map object from serialized data.
 *
 * @note The new BPF map object will represent a BPF map from bpfilter's point
 * of view, but it's not a BPF map.
 *
 * @param map BPF map object to allocate and initialize from the serialized
 *            data. The caller will own the object. On success, @c *map points
 *            to a valid BPF object map. On failure, @c *map is unchanged.
 *            Can't be NULL.
 * @param marsh Serialized BPF map object data. Can't be NULL.
 * @return 0 on success, or a negative errno value on failure.
 */
int bf_bpf_map_new_from_marsh(struct bf_bpf_map **map,
                              const struct bf_marsh *marsh);

/**
 * Free a BPF map object.
 *
 * The BPF map's file descriptor contained in @c map is closed and set to
 * @c -1 . To prevent the BPF map from being destroyed, pin it beforehand.
 *
 * @param map @ref bf_bpf_map object to free. On success, @c *map is set to
 *            NULL. On failure, @c *map remain unchanged.
 */
void bf_bpf_map_free(struct bf_bpf_map **map);

/**
 * Serializes a BPF map object.
 *
 * @param map BPF map object to serialize. The object itself won't be modified.
 *            Can't be NULL.
 * @param marsh Marsh object, will be allocated by this function and owned by
 *              the caller. On success, @c *marsh will point to the BPF map's
 *              serialized data. On failure, @c *marsh is left unchanged. Can't
 *              be NULL.
 * @return 0 on success, or a negative errno value on error.
 */
int bf_bpf_map_marsh(const struct bf_bpf_map *map, struct bf_marsh **marsh);

/**
 * Dump a BPF map object.
 *
 * @param map BPF map object to dump. Can't be NULL.
 * @param prefix String to prefix each log with. If no prefix is needed, use
 *               @ref EMPTY_PREFIX . Can't be NULL.
 */
void bf_bpf_map_dump(const struct bf_bpf_map *map, prefix_t *prefix);

/**
 * Convert a @ref bf_bpf_map_type to a string.
 *
 * @param type Map type to convert to string. Must be a valid
 *             @ref bf_bpf_map_type (except for @ref _BF_BPF_MAP_TYPE_MAX ).
 * @return The map type, as a string.
 */
const char *bf_bpf_map_type_to_str(enum bf_bpf_map_type type);

/**
 * Convert a string into a @ref bf_bpf_map_type value.
 *
 * If the string is an invalid @ref bf_bpf_map_type string representation,
 * an error is returned.
 *
 * @param str String to convert to a @ref bf_bpf_map_type value. Can't be NULL.
 * @param type On success, contains the map type value. Unchanged on failure.
 *             Can't be NULL.
 * @return 0 on success, or a negative errno value on failure.
 */
int bf_bpf_map_type_from_str(const char *str, enum bf_bpf_map_type *type);