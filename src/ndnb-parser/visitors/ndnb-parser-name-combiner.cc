/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "print-helper.h"
#include "ns3/ndnb-parser-name-combiner.h"

#include "ns3/ndnb-parser-blob.h"
#include "ns3/ndnb-parser-udata.h"
#include "ns3/ndnb-parser-dtag.h"

#include <iostream>
#include <sstream>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace std;

namespace ns3 {
namespace NdnbParser {


using namespace boost::archive::iterators;
typedef base64_from_binary<transform_width<string::const_iterator, 6, 8> > base64_t;


//////////////////////////////////////////////////////////////////////

void
NameCombiner::visit (Blob &n, boost::any param)
{
  // Buffer n.m_blob;
  ostream &os = *(boost::any_cast<ostream*> (param));

  if (n.m_blob.get () != 0)
    {
      PrintHelper::print_percent_escaped (os, (unsigned char*)n.m_blob.get (), n.m_blobSize);
    }
  else
    {
      os << "...";
    }
}
 
void
NameCombiner::visit (Udata &n, boost::any param)
{
  // std::string n.m_udata;
  *(boost::any_cast<ostream*> (param)) << n.m_udata;
}

void
NameCombiner::visit (Dtag &n, boost::any param)
{
  switch (n.m_dtag)
    {
    case NDN_DTAG_Name: // process only name dtag
      {
        ostream &os = *(boost::any_cast<ostream*> (param));

        if (n.m_nestedTags.size()==0)
          os << "/";
        
        BOOST_FOREACH (Ptr<Block> nested, n.m_nestedTags)
          {
            os << "/";
            nested->accept (*this, param);
          }
        break;
      }
    case NDN_DTAG_Interest:
    case NDN_DTAG_Data:
    case NDN_DTAG_Component:
      // cout << "preved \n";
      VoidDepthFirstVisitor::visit (n, param);
      break;
    default:
      break;
    }
}

}
}
