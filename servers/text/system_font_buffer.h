/**************************************************************************/
/*  system_font_buffer.h                                                  */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "core/io/file_access.h"
#include "core/object/ref_counted.h"

class SystemFontBuffer : public RefCounted {
	const uint8_t *data = nullptr;
	size_t size = 0;

#if defined(WINDOWS_ENABLED)
	void *file_handle = (void *)-1;
	void *mapping_handle = nullptr;
	void *mapping_view = nullptr;
#elif defined(UNIX_ENABLED)
	void *mmap_ptr = nullptr;
	size_t mmap_size = 0;
#endif

	PackedByteArray fallback_data;

public:
	_FORCE_INLINE_ const uint8_t *get_data() const { return data; }
	_FORCE_INLINE_ size_t get_size() const { return size; }
	_FORCE_INLINE_ bool is_valid() const { return data != nullptr && size > 0; }

	static Ref<SystemFontBuffer> create(const String &p_path);

	~SystemFontBuffer();
};
