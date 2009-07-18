// TODO:
// first match from left
// first match from right
// longest match
// split on longest match ?
// deallocations on clear

#include "tst.h"

#include <Python.h>

typedef struct {
    PyObject_HEAD
	tst *theTree;
	unsigned int length;
} TernaryStateTree;

static void
TernaryStateTree_dealloc(TernaryStateTree* self)
{
	tst_cleanup(self->theTree);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
TernaryStateTree_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    TernaryStateTree *self;

    self = (TernaryStateTree *)type->tp_alloc(type, 0);
    if (self != NULL) {
		self->theTree = tst_init(100);
    }

    return (PyObject *)self;
}

static PyObject*
TernaryStateTree_clear(TernaryStateTree *self, PyObject *args, PyObject *kwds)
{
	tst_cleanup(self->theTree);
	self->theTree = tst_init(100);
	self->length = 0;
	
	Py_INCREF(Py_None);
	return Py_None;
}

Py_ssize_t TernaryStateTree_Size(PyObject* self)
{
	return ((TernaryStateTree*)self)->length;
}

int TernaryStateTree_SetItem(PyObject *self, PyObject *key, PyObject *v)
{
	PyObject* exist_ptr = NULL;
	PyObject* keyStr = NULL;

	const unsigned char* attr_name;

	if (PyString_Check(key))
	{
		attr_name = (const unsigned char*)PyString_AsString(key);
	}
	else if (PyUnicode_Check(key))
	{
		keyStr = PyUnicode_AsUTF8String(key);
		if (keyStr == NULL) return -1;
	}
	else
	{
		keyStr = PyObject_Str(key);
		if (keyStr == NULL) return -1;
	}

	if (keyStr != NULL)
	{
		attr_name = (const unsigned char*)PyString_AsString(keyStr);
	}

	if (v != NULL)
	{
		int result = tst_insert(attr_name, (void*)v,
		                        ((TernaryStateTree*)self)->theTree,
		                        TST_REPLACE, (void**)&exist_ptr);

		Py_XDECREF(keyStr);

		if (result == TST_OK)
		{
			Py_INCREF(v);
			if (exist_ptr != NULL)
			{
				Py_DECREF(exist_ptr);
			}
			else
			{
				++((TernaryStateTree*)self)->length;
			}
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		PyObject* deleted =
			(PyObject*)tst_delete((const unsigned char*)attr_name,
					              ((TernaryStateTree*)self)->theTree);

		Py_XDECREF(keyStr);

		if (deleted == NULL)
		{
			PyErr_SetObject(PyExc_KeyError, key);
			return -1;
		}
		else
		{
			Py_DECREF(deleted);
			--((TernaryStateTree*)self)->length;
			return 0;
		}
	}
}

PyObject* TernaryStateTree_doSearch(TernaryStateTree *self, PyObject *key, int mode)
{
	unsigned int match_len;

	PyObject* keyStr = NULL;

	const unsigned char* attr_name;

	if (PyString_Check(key))
	{
		attr_name = (const unsigned char*)PyString_AsString(key);
	}
	else if (PyUnicode_Check(key))
	{
		keyStr = PyUnicode_AsUTF8String(key);
		if (keyStr == NULL) return NULL;
	}
	else
	{
		keyStr    = PyObject_Str(key);
		if (keyStr == NULL) return NULL;
	}

	if (keyStr != NULL)
	{
		attr_name = (const unsigned char*)PyString_AsString(keyStr);
	}

	PyObject* result =
		(PyObject*)tst_search(attr_name, self->theTree,
				              mode, &match_len);

	Py_XDECREF(keyStr);
	Py_XINCREF(result);

	if (result == NULL)
	{
		PyErr_SetObject(PyExc_KeyError, key);
	}

	if (mode == TST_SUBSTRING_MATCH)
	{
		return Py_BuildValue("OO", PyString_FromStringAndSize((const char*)attr_name, match_len),
				                   result);
	}
	else
	{
		return result;
	}
}

PyObject* TernaryStateTree_GetItem(PyObject *self, PyObject *key)
{
	return TernaryStateTree_doSearch((TernaryStateTree*)self, key, 0);
}

static PyObject*
TernaryStateTree_search(TernaryStateTree *self, PyObject *search)
{
	return TernaryStateTree_doSearch(self, search, TST_SUBSTRING_MATCH);
}

static PyMappingMethods TernaryStateTreeMappingMethods = {
	TernaryStateTree_Size,
	TernaryStateTree_GetItem,
	TernaryStateTree_SetItem
};

static PyMethodDef TernaryStateTree_methods[] = {
	{"search", (PyCFunction)TernaryStateTree_search, METH_O, "Search for a substring match"},
	{"clear",  (PyCFunction)TernaryStateTree_clear,  METH_NOARGS, "Empty the TST"},
    {NULL}  /* Sentinel */
};

static PyTypeObject TernaryStateTreeType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "tst.TernaryStateTree",             /*tp_name*/
    sizeof(TernaryStateTree),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)TernaryStateTree_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &TernaryStateTreeMappingMethods, /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "TernaryStateTree objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    TernaryStateTree_methods,  /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    TernaryStateTree_new,      /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
inittst(void) 
{
    PyObject* m;

    if (PyType_Ready(&TernaryStateTreeType) < 0)
        return;

    m = Py_InitModule3("tst", module_methods,
                       "Example module that creates an extension type.");

    if (m == NULL)
      return;

    Py_INCREF(&TernaryStateTreeType);
    PyModule_AddObject(m, "TernaryStateTree", (PyObject *)&TernaryStateTreeType);
}

