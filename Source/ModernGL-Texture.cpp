#include "ModernGL.hpp"

#include "OpenGL.hpp"

PyObject * NewTexture(PyObject * self, PyObject * args, PyObject * kwargs) {
	int width;
	int height;
	const void * data;
	int components = 3;
	int size;
	bool floats = false;

	static const char * kwlist[] = {"width", "height", "data", "components", "floats", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iiy#|ip:NewTexture", (char **)kwlist, &width, &height, &data, &size, &components, &floats)) {
		return 0;
	}

	if (width <= 0 || height <= 0 || components < 1 || components > 4) {
		PyErr_Format(ModuleRangeError, "NewTexture() width = %d height = %d components = %d", width, height, components);
	}

	int expected_size = floats ? (width * height * 4) : (height * ((width * components + 3) & ~3));

	if (size != expected_size) {
		PyErr_Format(ModuleRangeError, "NewTexture() expected size is %d, not %d", expected_size, size);
		return 0;
	}

	const int formats[] = {0, OpenGL::GL_RED, OpenGL::GL_RG, OpenGL::GL_RGB, OpenGL::GL_RGBA};

	int pixel_type = floats ? OpenGL::GL_FLOAT : OpenGL::GL_UNSIGNED_BYTE;
	int format = formats[components];

	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + defaultTextureUnit);

	int texture = 0;
	OpenGL::glGenTextures(1, (OpenGL::GLuint *)&texture);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MIN_FILTER, OpenGL::GL_LINEAR);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MAG_FILTER, OpenGL::GL_LINEAR);
	OpenGL::glTexImage2D(OpenGL::GL_TEXTURE_2D, 0, format, width, height, 0, format, pixel_type, data);
	return CreateTextureType(texture, width, height, components, floats);
}

PyObject * DeleteTexture(PyObject * self, PyObject * args) {
	Texture * texture;

	if (!PyArg_ParseTuple(args, "O!:DeleteTexture", &TextureType, &texture)) {
		return 0;
	}

	OpenGL::glDeleteTextures(1, (OpenGL::GLuint *)&texture->texture);
	Py_RETURN_NONE;
}

PyObject * UpdateTexture(PyObject * self, PyObject * args, PyObject * kwargs) {
	Texture * texture;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	const void * data;
	int size;

	static const char * kwlist[] = {"texture", "data", "x", "y", "width", "height", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!y#|iiii:UpdateTexture", (char **)kwlist, &TextureType, &texture, &data, &size, &x, &y, &width, &height)) {
		return 0;
	}

	if (width == 0) {
		width = texture->width;
	}

	if (height == 0) {
		height = texture->height;
	}

	if (x < 0 || y < 0 || width < 0 || height < 0 || x + width > texture->width || y + height > texture->height) {
		PyErr_Format(ModuleRangeError, "UpdateTexture() x = %d y = %d width = %d height = %d", x, y, width, height);
	}
	
	const int formats[] = {0, OpenGL::GL_RED, OpenGL::GL_RG, OpenGL::GL_RGB, OpenGL::GL_RGBA};

	int pixel_type = texture->floats ? OpenGL::GL_FLOAT : OpenGL::GL_UNSIGNED_BYTE;
	int format = formats[texture->components];
	
	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + defaultTextureUnit);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture->texture);
	OpenGL::glTexSubImage2D(OpenGL::GL_TEXTURE_2D, 0, x, y, width, height, format, pixel_type, data);
	Py_RETURN_NONE;
}

PyObject * UseTexture(PyObject * self, PyObject * args) {
	Texture * texture;
	int location = 0;

	if (!PyArg_ParseTuple(args, "O!|i:UseTexture", &TextureType, &texture, &location)) {
		return 0;
	}

	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + location);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture->texture);
	Py_RETURN_NONE;
}

PyObject * SetTexturePixelated(PyObject * self, PyObject * args) {
	Texture * texture;
	
	if (!PyArg_ParseTuple(args, "O!:SetTexturePixelated", &TextureType, &texture)) {
		return 0;
	}

	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + defaultTextureUnit);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture->texture);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MIN_FILTER, OpenGL::GL_NEAREST);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MAG_FILTER, OpenGL::GL_NEAREST);
	Py_RETURN_NONE;
}

PyObject * SetTextureFiltered(PyObject * self, PyObject * args) {
	Texture * texture;
	
	if (!PyArg_ParseTuple(args, "O!:SetTextureFiltered", &TextureType, &texture)) {
		return 0;
	}

	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + defaultTextureUnit);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture->texture);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MIN_FILTER, OpenGL::GL_LINEAR);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MAG_FILTER, OpenGL::GL_LINEAR);
	Py_RETURN_NONE;
}

PyObject * SetTextureMipmapped(PyObject * self, PyObject * args) {
	Texture * texture;
	
	if (!PyArg_ParseTuple(args, "O!:SetTextureMipmapped", &TextureType, &texture)) {
		return 0;
	}

	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + defaultTextureUnit);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture->texture);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MIN_FILTER, OpenGL::GL_LINEAR_MIPMAP_LINEAR);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MAG_FILTER, OpenGL::GL_LINEAR);
	Py_RETURN_NONE;
}

PyObject * BuildMipmaps(PyObject * self, PyObject * args, PyObject * kwargs) {
	Texture * texture;
	int base = 0;
	int max = 1000;

	static const char * kwlist[] = {"texture", "base", "max", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|ii:BuildMipmaps", (char **)kwlist, &TextureType, &texture, &base, &max)) {
		return 0;
	}

	OpenGL::glActiveTexture(OpenGL::GL_TEXTURE0 + defaultTextureUnit);
	OpenGL::glBindTexture(OpenGL::GL_TEXTURE_2D, texture->texture);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_BASE_LEVEL, base);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MAX_LEVEL, max);
	OpenGL::glGenerateMipmap(OpenGL::GL_TEXTURE_2D);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MIN_FILTER, OpenGL::GL_LINEAR_MIPMAP_LINEAR);
	OpenGL::glTexParameteri(OpenGL::GL_TEXTURE_2D, OpenGL::GL_TEXTURE_MAG_FILTER, OpenGL::GL_LINEAR);
	Py_RETURN_NONE;
}

PyObject * UseTextureAsImage(PyObject * self, PyObject * args, PyObject * kwargs) {
	Texture * texture;
	int binding = 0;

	static const char * kwlist[] = {"texture", "binding", 0};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!|i:UseTextureAsImage", (char **)kwlist, &TextureType, &texture, &binding)) {
		return 0;
	}

	if (texture->floats) {
		const int formats[] = {0, OpenGL::GL_R8UI, OpenGL::GL_RG8UI, OpenGL::GL_RGB8UI, OpenGL::GL_RGBA8UI};
		int format = formats[texture->components];
		
		OpenGL::glBindImageTexture(binding, texture->texture, 0, false, 0, OpenGL::GL_READ_WRITE, format);
	} else {
		const int formats[] = {0, OpenGL::GL_R32F, OpenGL::GL_RG32F, OpenGL::GL_RGB32F, OpenGL::GL_RGBA32F};
		int format = formats[texture->components];
		
		OpenGL::glBindImageTexture(binding, texture->texture, 0, false, 0, OpenGL::GL_READ_WRITE, format);
	}
	
	Py_RETURN_NONE;
}
