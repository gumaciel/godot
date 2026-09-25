/**************************************************************************/
/*  system_font_buffer.cpp                                                */
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

#include "system_font_buffer.h"

#if defined(WINDOWS_ENABLED)
#include <windows.h>
#elif defined(UNIX_ENABLED)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

Ref<SystemFontBuffer> SystemFontBuffer::create(const String &p_path) {
	if (p_path.is_empty()) {
		return Ref<SystemFontBuffer>();
	}

#if defined(UNIX_ENABLED)
	int fd = open(p_path.utf8().get_data(), O_RDONLY);
	if (fd != -1) {
		struct stat st;
		if (fstat(fd, &st) == 0 && st.st_size > 0) {
			void *ptr = mmap(nullptr, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
			if (ptr != MAP_FAILED) {
				Ref<SystemFontBuffer> buf;
				buf.instantiate();
				buf->data = (const uint8_t *)ptr;
				buf->size = (size_t)st.st_size;
				buf->mmap_ptr = ptr;
				buf->mmap_size = (size_t)st.st_size;
				close(fd);
				return buf;
			}
		}
		close(fd);
	}
#elif defined(WINDOWS_ENABLED)
	HANDLE file = CreateFileW((LPCWSTR)p_path.utf16().get_data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (file != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER file_size;
		if (GetFileSizeEx(file, &file_size) && file_size.QuadPart > 0) {
			HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (mapping != NULL) {
				void *view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
				if (view != nullptr) {
					Ref<SystemFontBuffer> buf;
					buf.instantiate();
					buf->data = (const uint8_t *)view;
					buf->size = (size_t)file_size.QuadPart;
					buf->file_handle = (void *)file;
					buf->mapping_handle = (void *)mapping;
					buf->mapping_view = view;
					return buf;
				}
				CloseHandle(mapping);
			}
		}
		CloseHandle(file);
	}
#endif

	PackedByteArray file_bytes = FileAccess::get_file_as_bytes(p_path);
	if (!file_bytes.is_empty()) {
		Ref<SystemFontBuffer> buf;
		buf.instantiate();
		buf->fallback_data = file_bytes;
		buf->data = buf->fallback_data.ptr();
		buf->size = (size_t)buf->fallback_data.size();
		return buf;
	}

	return Ref<SystemFontBuffer>();
}

SystemFontBuffer::~SystemFontBuffer() {
#if defined(WINDOWS_ENABLED)
	if (mapping_view != nullptr) {
		UnmapViewOfFile(mapping_view);
		mapping_view = nullptr;
	}
	if (mapping_handle != nullptr) {
		CloseHandle((HANDLE)mapping_handle);
		mapping_handle = nullptr;
	}
	if (file_handle != nullptr && file_handle != (void *)-1) {
		CloseHandle((HANDLE)file_handle);
		file_handle = (void *)-1;
	}
#elif defined(UNIX_ENABLED)
	if (mmap_ptr != nullptr && mmap_size > 0) {
		munmap(mmap_ptr, mmap_size);
		mmap_ptr = nullptr;
		mmap_size = 0;
	}
#endif
}
