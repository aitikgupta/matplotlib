/* -*- mode: c++; c-basic-offset: 4 -*- */

/* A python interface to FreeType */
#ifndef MPL_FT2FONT_H
#define MPL_FT2FONT_H
#include <vector>
#include <stdint.h>

extern "C" {
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_SFNT_NAMES_H
#include FT_TYPE1_TABLES_H
#include FT_TRUETYPE_TABLES_H
}

#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*
 By definition, FT_FIXED as 2 16bit values stored in a single long.
 */
#define FIXED_MAJOR(val) (signed short)((val & 0xffff0000) >> 16)
#define FIXED_MINOR(val) (unsigned short)(val & 0xffff)

// the FreeType string rendered into a width, height buffer
class FT2Image
{
  public:
    FT2Image();
    FT2Image(unsigned long width, unsigned long height);
    virtual ~FT2Image();

    void resize(long width, long height);
    void draw_bitmap(FT_Bitmap *bitmap, FT_Int x, FT_Int y);
    void write_bitmap(FILE *fp) const;
    void draw_rect(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1);
    void draw_rect_filled(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1);

    unsigned char *get_buffer()
    {
        return m_buffer;
    }
    unsigned long get_width()
    {
        return m_width;
    }
    unsigned long get_height()
    {
        return m_height;
    }

  private:
    bool m_dirty;
    unsigned char *m_buffer;
    unsigned long m_width;
    unsigned long m_height;

    // prevent copying
    FT2Image(const FT2Image &);
    FT2Image &operator=(const FT2Image &);
};

extern FT_Library _ft2Library;

class FT2Font
{

  public:
    FT2Font(FT_Open_Args &open_args, long hinting_factor, std::vector<FT2Font *> fallbacks);
    virtual ~FT2Font();
    void clear();
    void set_size(double ptsize, double dpi);
    void set_fallbacks(std::vector<FT2Font *> fallbacks);
    void set_charmap(int i);
    void select_charmap(unsigned long i);
    void set_text(
        size_t N, uint32_t *codepoints, double angle, FT_Int32 flags, std::vector<double> &xys);

    void fill_glyphs(size_t N,
                     uint32_t *codepoints,
                     FT_Int32 &flags,
                     std::vector<double> &xys,
                     FT_Matrix &matrix,
                     FT_UInt &previous);

    void ft_set_face_range(uint start,
                           uint end,
                           std::vector<FT_UInt> glyphs_indexes,
                           FT_Face current_face,
                           FT_Int32 &flags,
                           std::vector<double> &xys,
                           FT_Matrix &matrix,
                           FT_UInt &previous);

    int get_kerning(FT_UInt left, FT_UInt right, FT_UInt mode);
    void set_kerning_factor(int factor);
    void load_char(long charcode, FT_Int32 flags);
    void load_glyph(FT_UInt glyph_index, FT_Int32 flags);
    void get_width_height(long *width, long *height);
    void get_bitmap_offset(long *x, long *y);
    long get_descent();
    // TODO: Since we know the size of the array upfront, we probably don't
    // need to dynamically allocate like this
    void get_xys(bool antialiased, std::vector<double> &xys);
    void draw_glyphs_to_bitmap(bool antialiased);
    void draw_glyph_to_bitmap(FT2Image &im, int x, int y, size_t glyphInd, bool antialiased);
    void get_glyph_name(unsigned int glyph_number, char *buffer);
    long get_name_index(char *name);
    PyObject* get_path();

    FT_Face &get_face()
    {
        // TODO: return the first for now
        printf("Getting face, please don't segfault.\n");
        FT_Face does_this_work = faces[0];
        FT_Long numface = does_this_work->num_glyphs;
        printf("No segfault in getting face! Numface: %lu\n", numface);
        return faces[0];
    }
    std::vector<FT_Face> &get_faces()
    {
        return faces;
    }
    FT2Image &get_image()
    {
        return image;
    }
    FT_Glyph &get_last_glyph()
    {
        return glyphs.back();
    }
    size_t get_last_glyph_index()
    {
        return glyphs.size() - 1;
    }
    size_t get_num_glyphs()
    {
        printf("Get num glyphs is called! ");
        return glyphs.size();
    }
    long get_hinting_factor()
    {
        return hinting_factor;
    }

  private:
    FT2Image image;
    std::vector<FT_Face> faces;
    FT_Vector pen;    /* untransformed origin  */
    std::vector<FT_Glyph> glyphs;
    std::vector<FT2Font *> fallbacks;
    FT_BBox bbox;
    FT_Pos advance;
    long hinting_factor;
    int kerning_factor;

    // prevent copying
    FT2Font(const FT2Font &);
    FT2Font &operator=(const FT2Font &);
};

#endif
