#include "image.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <jpeglib.h>  // Required for jpeg support.

namespace cat {

  //
  // Image public methods
  //

  Image::Image(int width, int height, int channels) :
    _width(width),
    _height(height),
    _channels(channels)
  {
    init();
    memset(_data[0], 0, elements() * sizeof(float));
  }


  Image::Image(const Image& img) :
    _width(img.width()),
    _height(img.height()),
    _channels(img.channels())
  {
    init();
    memcpy(_data[0], img._data[0], elements() * sizeof(float));
  }


  Image::~Image()
  {
    destroy();
  }


  const Image& Image::operator = (const Image& img)
  {
    destroy();

    _width = img.width();
    _height = img.height();
    _channels = img.channels();

    init();
    memcpy(_data[0], img._data[0], elements() * sizeof(float));

    return *this;
  }


  int Image::width() const
  {
    return _width;
  }


  int Image::height() const
  {
    return _height;
  }


  int Image::channels() const
  {
    return _channels;
  }


  int Image::elements() const
  {
    return _width * _height * _channels;
  }


  int Image::rowSize() const
  {
    return _width * _channels;
  }


  int Image::pixels() const
  {
    return _width * _height;
  }


  float Image::get(int x, int y, int c) const
  {
    return _data[y][x * _channels + c];
  }


  void Image::set(int x, int y, int c, float val)
  {
    _data[y][x * _channels + c] = val;
  }


  const float* Image::data() const
  {
    return _data[0];
  }


  float* Image::data()
  {
    return _data[0];
  }


  const float* Image::row(int index) const
  {
    return _data[index];
  }


  float* Image::row(int index)
  {
    return _data[index];
  }


  //
  // Image private functions
  //

  void Image::init()
  {
    const int kPixels = pixels();
    const int kRemainder = (4 - (kPixels % 4)) % 4;
    const int kAllocCount = (kPixels + kRemainder) * _channels;

    if (posix_memalign((void**)&_data, 16, _height * sizeof(float*)) != 0)
      abort();

    if (posix_memalign((void**)&_data[0], 16, kAllocCount * sizeof(float)) != 0)
      abort();

    const int kRowSize = rowSize();
    for (int i = 1; i < _height; ++i)
      _data[i] = _data[0] + (i * kRowSize);
  }


  void Image::destroy()
  {
    if (_data != NULL) {
      if (_data[0] != NULL)
        free(_data[0]);
      free(_data);
      _data = NULL;
    }
  }

  
  //
  // Functions
  //

  Image* LoadJPEG(const char* filename)
  {
    FILE* file = fopen(filename, "rb");
    if (!file)
      return NULL;

    Image* img = NULL;

    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, file);

    bool ok = jpeg_read_header(&cinfo, TRUE) == JPEG_HEADER_OK;

    // Set decompression parameters in cinfo here. (optional)

    if (ok)
      ok = jpeg_start_decompress(&cinfo);

    if (ok) {
      const JSAMPLE kMaxVal = ~0;
      const float kScale = 1.0f / kMaxVal;

      int width = cinfo.output_width;
      int height = cinfo.output_height;
      int channels = cinfo.output_components;

      img = new Image(width, height, channels);

      const int kRowSize = img->rowSize();

      JSAMPLE* p = new JSAMPLE[kRowSize];

      while (cinfo.output_scanline < cinfo.output_height) {
        int y = cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, &p, 1);

        float* dst = img->row(y);
        for (int i = 0; i < kRowSize; ++i)
          dst[i] = float(p[i]) * kScale;
      }

      delete[] p;
      ok = jpeg_finish_decompress(&cinfo);
    }

    jpeg_destroy_decompress(&cinfo);
    if (!ok) {
      delete img;
      img = NULL;
    }
    return img;
  }

} // namespace cat

