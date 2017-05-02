// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_LGIF_FX_GIF_H_
#define CORE_FXCODEC_LGIF_FX_GIF_H_

#include <setjmp.h>
#include <vector>

#include "core/fxcrt/fx_basic.h"

#define GIF_SIGNATURE "GIF"
#define GIF_SIG_EXTENSION 0x21
#define GIF_SIG_IMAGE 0x2C
#define GIF_SIG_TRAILER 0x3B
#define GIF_BLOCK_GCE 0xF9
#define GIF_BLOCK_PTE 0x01
#define GIF_BLOCK_CE 0xFE
#define GIF_BLOCK_AE 0xFF
#define GIF_BLOCK_TERMINAL 0x00
#define GIF_MAX_LZW_CODE 4096
#define GIF_DATA_BLOCK 255
#define GIF_MAX_ERROR_SIZE 256
#define GIF_D_STATUS_SIG 0x01
#define GIF_D_STATUS_TAIL 0x02
#define GIF_D_STATUS_EXT 0x03
#define GIF_D_STATUS_EXT_AE 0x04
#define GIF_D_STATUS_EXT_CE 0x05
#define GIF_D_STATUS_EXT_GCE 0x06
#define GIF_D_STATUS_EXT_PTE 0x07
#define GIF_D_STATUS_EXT_UNE 0x08
#define GIF_D_STATUS_IMG_INFO 0x09
#define GIF_D_STATUS_IMG_DATA 0x0A
#pragma pack(1)
typedef struct tagGifGF {
  uint8_t pal_bits : 3;
  uint8_t sort_flag : 1;
  uint8_t color_resolution : 3;
  uint8_t global_pal : 1;
} GifGF;
typedef struct tagGifLF {
  uint8_t pal_bits : 3;
  uint8_t reserved : 2;
  uint8_t sort_flag : 1;
  uint8_t interlace : 1;
  uint8_t local_pal : 1;
} GifLF;
typedef struct tagGifHeader {
  char signature[3];
  char version[3];
} GifHeader;
typedef struct tagGifLSD {
  uint16_t width;
  uint16_t height;
  uint8_t global_flag;
  uint8_t bc_index;
  uint8_t pixel_aspect;
} GifLSD;
typedef struct tagGifImageInfo {
  uint16_t left;
  uint16_t top;
  uint16_t width;
  uint16_t height;

  uint8_t local_flag;
} GifImageInfo;
typedef struct tagGifCEF {
  uint8_t transparency : 1;
  uint8_t user_input : 1;
  uint8_t disposal_method : 3;
  uint8_t reserved : 3;
} GifCEF;
typedef struct tagGifGCE {
  uint8_t block_size;
  uint8_t gce_flag;
  uint16_t delay_time;
  uint8_t trans_index;
} GifGCE;
typedef struct tagGifPTE {
  uint8_t block_size;
  uint16_t grid_left;
  uint16_t grid_top;
  uint16_t grid_width;
  uint16_t grid_height;

  uint8_t char_width;
  uint8_t char_height;

  uint8_t fc_index;
  uint8_t bc_index;
} GifPTE;
typedef struct tagGifAE {
  uint8_t block_size;
  uint8_t app_identify[8];
  uint8_t app_authentication[3];
} GifAE;
typedef struct tagGifPalette { uint8_t r, g, b; } GifPalette;
#pragma pack()
typedef struct tagGifImage {
  GifGCE* image_gce_ptr;
  GifPalette* local_pal_ptr;
  GifImageInfo* image_info_ptr;
  uint8_t image_code_size;
  uint32_t image_data_pos;
  uint8_t* image_row_buf;
  int32_t image_row_num;
} GifImage;

typedef struct tagGifPlainText {
  GifGCE* gce_ptr;
  GifPTE* pte_ptr;
  CFX_ByteString* string_ptr;
} GifPlainText;

class CGifLZWDecoder {
 public:
  struct tag_Table {
    uint16_t prefix;
    uint8_t suffix;
  };

  explicit CGifLZWDecoder(char* error_ptr);
  ~CGifLZWDecoder();

  void InitTable(uint8_t code_len);
  int32_t Decode(uint8_t* des_buf, uint32_t* des_size);
  void Input(uint8_t* src_buf, uint32_t src_size);
  uint32_t GetAvailInput();

 private:
  void ClearTable();
  void AddCode(uint16_t prefix_code, uint8_t append_char);
  void DecodeString(uint16_t code);

  uint8_t code_size;
  uint8_t code_size_cur;
  uint16_t code_clear;
  uint16_t code_end;
  uint16_t code_next;
  uint8_t code_first;
  uint8_t stack[GIF_MAX_LZW_CODE];
  uint16_t stack_size;
  tag_Table code_table[GIF_MAX_LZW_CODE];
  uint16_t code_old;

  uint8_t* next_in;
  uint32_t avail_in;

  uint8_t bits_left;
  uint32_t code_store;

  char* err_msg_ptr;
};

typedef struct tag_gif_decompress_struct gif_decompress_struct;
typedef gif_decompress_struct* gif_decompress_struct_p;
typedef gif_decompress_struct_p* gif_decompress_struct_pp;
static const int32_t s_gif_interlace_step[4] = {8, 8, 4, 2};
struct tag_gif_decompress_struct {
  jmp_buf jmpbuf;
  char* err_ptr;
  void (*gif_error_fn)(gif_decompress_struct_p gif_ptr, const char* err_msg);
  void* context_ptr;
  int width;
  int height;
  GifPalette* global_pal_ptr;
  int32_t global_pal_num;
  uint8_t global_sort_flag;
  uint8_t global_color_resolution;

  uint8_t bc_index;
  uint8_t pixel_aspect;
  CGifLZWDecoder* img_decoder_ptr;
  uint32_t img_row_offset;
  uint32_t img_row_avail_size;
  uint8_t img_pass_num;
  std::vector<GifImage*>* img_ptr_arr_ptr;
  uint8_t* (*gif_ask_buf_for_pal_fn)(gif_decompress_struct_p gif_ptr,
                                     int32_t pal_size);
  uint8_t* next_in;
  uint32_t avail_in;
  int32_t decode_status;
  uint32_t skip_size;
  void (*gif_record_current_position_fn)(gif_decompress_struct_p gif_ptr,
                                         uint32_t* cur_pos_ptr);
  void (*gif_get_row_fn)(gif_decompress_struct_p gif_ptr,
                         int32_t row_num,
                         uint8_t* row_buf);
  bool (*gif_get_record_position_fn)(gif_decompress_struct_p gif_ptr,
                                     uint32_t cur_pos,
                                     int32_t left,
                                     int32_t top,
                                     int32_t width,
                                     int32_t height,
                                     int32_t pal_num,
                                     void* pal_ptr,
                                     int32_t delay_time,
                                     bool user_input,
                                     int32_t trans_index,
                                     int32_t disposal_method,
                                     bool interlace);
  CFX_ByteString* cmt_data_ptr;
  GifGCE* gce_ptr;
  std::vector<GifPlainText*>* pt_ptr_arr_ptr;
};

gif_decompress_struct_p gif_create_decompress();
void gif_destroy_decompress(gif_decompress_struct_pp gif_ptr_ptr);
int32_t gif_read_header(gif_decompress_struct_p gif_ptr);
int32_t gif_get_frame(gif_decompress_struct_p gif_ptr);
int32_t gif_get_frame_num(gif_decompress_struct_p gif_ptr);
int32_t gif_load_frame(gif_decompress_struct_p gif_ptr, int32_t frame_num);
void gif_input_buffer(gif_decompress_struct_p gif_ptr,
                      uint8_t* src_buf,
                      uint32_t src_size);
uint32_t gif_get_avail_input(gif_decompress_struct_p gif_ptr,
                             uint8_t** avail_buf_ptr);

#endif  // CORE_FXCODEC_LGIF_FX_GIF_H_
