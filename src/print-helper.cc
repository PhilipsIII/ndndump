/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include "dtag-dict.h"
#include <iostream>
#include "print-helper.h"

#include <ctype.h>
#include <iomanip>
using namespace std;

const char Base64[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char *
PrintHelper::dict_name_from_number (int ndx, const struct ndn_dict_entry *dict, int n)
{
  int i;
  for (i = 0; i < n; i++)
    if (ndx == dict[i].index)
      return (dict[i].name);

  throw UnknownDtag ();
}

const char PrintHelper::Base64[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int
PrintHelper::is_text_encodable(const unsigned char *p, size_t start, size_t length)
{
  size_t i;

  if (length == 0) return (0);
  for (i = 0; i < length; i++)
    {
      char c = p[start + i];
      if (c < ' ' || c > '~') return (0);
      if (c == '<' || c == '>' || c == '&') return (0);
    }
  return (1);
}

/* see ndn_uri_append_percentescaped */
void
PrintHelper::print_percent_escaped(ostream& os, const unsigned char *data, size_t size)
{
  size_t i;
  unsigned char ch;
  for (i = 0; i < size && data[i] == '.'; i++)
    continue;
  /* For a component that consists solely of zero or more dots, add 3 more */
  if (i == size)
    os << "...";
  for (i = 0; i < size; i++)
    {
      ch = data[i];
      /*
       * Leave unescaped only the generic URI unreserved characters.
       * See RFC 3986. Here we assume the compiler uses ASCII.
       */
      if (('a' <= ch && ch <= 'z') ||
          ('A' <= ch && ch <= 'Z') ||
          ('0' <= ch && ch <= '9') ||
          ch == '-' || ch == '.' || ch == '_' || ch == '~')
        os << (char) ch;
      else
        os << "%" << hex << setfill('0') << setw(2) << (unsigned)ch;
    }
}

/*
* print data in rows of 16 bytes: offset   hex   ascii
*
* 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
*/
void
PrintHelper::print_hex_ascii_line (ostream &os, const unsigned char *payload, int len, int offset)
{

  int i;
  int gap;
  const unsigned char *ch;

  /* offset */
  os << setw(5) << offset << "   ";

  /* hex */
  ch = payload;
  for(i = 0; i < len; i++)
    {
      os << " " << setfill('0') << setw(2) << hex << (unsigned)*ch;
      ch++;
      /* print extra space after 8th byte for visual aid */
      if (i == 7)
        os << " ";
    }
  /* print space to handle line less than 8 bytes */
  if (len < 8)
    os << " ";

  /* fill hex gap with spaces if not full line */
  if (len < 16)
    {
      gap = 16 - len;
      for (i = 0; i < gap; i++)
        {
          os << "   ";
		}
	}
  os << "   ";

  /* ascii (if printable) */
  ch = payload;
  for(i = 0; i < len; i++)
    {
      if (isprint(*ch))
        os << *ch;
      else
        os << ".";
      ch++;
	}
  os << "\n";
}

/*
 * print packet payload data (avoid printing binary data)
 */
void
PrintHelper::print_payload (ostream &os, const unsigned char *payload, int len)
{
  int len_rem = len;
  int line_width = 16;			/* number of bytes per line */
  int line_len;
  int offset = 0;					/* zero-based offset counter */
  const unsigned char *ch = payload;

  if (len <= 0)
    return;

  /* data fits on one line */
  if (len <= line_width)
    {
      print_hex_ascii_line(os, ch, len, offset);
      return;
	}

  /* data spans multiple lines */
  for ( ;; )
    {
      /* compute current line length */
      line_len = line_width % len_rem;
      /* print line */
      print_hex_ascii_line(os, ch, line_len, offset);
      /* compute total remaining */
      len_rem = len_rem - line_len;
      /* shift pointer to remaining bytes to print */
      ch = ch + line_len;
      /* add offset */
      offset = offset + line_width;
      /* check if we have line width chars or less */
      if (len_rem <= line_width)
        {
          /* print last line and get out */
          print_hex_ascii_line(os, ch, len_rem, offset);
          break;
		}
	}
}
