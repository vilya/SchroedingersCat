#ifndef cat_image_h
#define cat_image_h

#include <cstdlib>

namespace cat {

  //
  // Types
  //

  class Image {
  public:
    Image(int width, int height, int channels);
    Image(const Image& img);
    ~Image();

    const Image& operator = (const Image& img);

    int width() const;
    int height() const;
    int channels() const;

    int elements() const; // The number of pixels * the number of channels.
    int rowSize() const;  // The width * the number of channels.
    int pixels() const;   // The width * height.

    float get(int x, int y, int c) const;
    void set(int x, int y, int c, float val);

    const float* data() const;
    float* data();

    const float* row(int index) const;
    float* row(int index);

  private:
    void init();
    void destroy();

  private:
    int _width;       // Width of the image, in pixels.
    int _height;      // Height of the image, in pixels.
    int _channels;    // Number of floats per pixel.
    float** _data;
  };


  //
  // Functions
  //

  Image* LoadJPEG(const char* filename);

} // namespace cat

#endif // cat_image_h

