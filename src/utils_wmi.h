/*
 * collectd - src/utils_wmi.h
 * Copyright (c) 2018  Google LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef UTILS_WMI_H
#define UTILS_WMI_H

#include <oaidl.h>

#define LIST_NODE_DECL(node_type) node_type _node
#define LIST_NEXT_DECL(node_type) node_type *_next
#define LIST_NODE(nodeptr) (nodeptr->_node)
#define LIST_NEXT(nodeptr) (nodeptr->_next)
#define LIST_HEAD(list) (list)

#define LIST_TYPE(type) list_##type##_t

#define LIST_DECL_TYPE(node_type)                                              \
  struct list_##node_type##_s;                                                 \
  typedef struct list_##node_type##_s LIST_TYPE(node_type)

#define LIST_DEF_TYPE(node_type)                                               \
  LIST_DECL_TYPE(node_type);                                                   \
  struct list_##node_type##_s {                                                \
    LIST_NODE_DECL(node_type *);                                               \
    LIST_NEXT_DECL(LIST_TYPE(node_type));                                      \
  }

#define LIST_INSERT_FRONT(list, new_node)                                      \
  do {                                                                         \
    __typeof__(list) _n = malloc(sizeof(__typeof__(*list)));                   \
    LIST_NEXT(_n) = list;                                                      \
    LIST_NODE(_n) = new_node;                                                  \
    list = _n;                                                                 \
  } while (0)

#define LIST_FREE(list, node_free)                                             \
  do {                                                                         \
    __typeof__(list) _head = list;                                             \
    while (_head != NULL) {                                                    \
      __typeof__(_head) _next = LIST_NEXT(_head);                              \
      node_free(LIST_NODE(_head));                                             \
      free(_head);                                                             \
      _head = _next;                                                           \
    }                                                                          \
  } while (0)

#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))

typedef struct wmi_connection_s { IDispatch *dispatcher; } wmi_connection_t;

typedef struct wmi_result_list_s {
  IDispatch *results;

  int count;
  int last_result;
} wmi_result_list_t;

typedef struct wmi_result_s { IDispatch *result; } wmi_result_t;

char *wstrtostr(const wchar_t *source);

double variant_get_double(VARIANT *v);
uint64_t variant_get_uint64(VARIANT *v);
int64_t variant_get_int64(VARIANT *v);
const char *variant_get_string(VARIANT *v);

HRESULT wmi_invoke_method(IDispatch *dispatcher, const wchar_t *method_name, DISPPARAMS *params, VARIANT *result);
HRESULT wmi_get_property(IDispatch *dispatcher, const wchar_t *property_name, VARIANT *result);
wmi_result_list_t *wmi_query(wmi_connection_t *connection, const char *query);
void wmi_result_list_release(wmi_result_list_t *results);
wmi_result_t *wmi_get_next_result(wmi_result_list_t *results);
void wmi_result_release(wmi_result_t *result);
int wmi_result_get_value(const wmi_result_t *result, const char *name, VARIANT *value);
void wmi_release(wmi_connection_t *connection);
wmi_connection_t *wmi_connect(void);

#endif