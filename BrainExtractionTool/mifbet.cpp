head	1.2;
access;
symbols
	mifbet_v1_1:1.2
	mifbet_v1_0:1.2
	start:1.1.1.1
	msmri:1.1.1;
locks; strict;
comment	@// @;


1.2
date	2011.03.11.20.05.12;	author kevin;	state Exp;
branches;
next	1.1;
commitid	G1L5yKod2j4U5tbv;

1.1
date	2010.11.26.20.24.44;	author kevin;	state Exp;
branches
	1.1.1.1;
next	;
commitid	zaS4054DuXpyzYXu;

1.1.1.1
date	2010.11.26.20.24.44;	author kevin;	state Exp;
branches;
next	;
commitid	zaS4054DuXpyzYXu;


desc
@@


1.2
log
@Made a mistake in the previous check-in command; now forcing checking in
all project files.  This check-in is for v1.0 release.
@
text
@/*
 * mifbet.cpp
 *
 * Brain Extraction Tool.  C++ wrapper that handles conversion between CMIF
 * and ANALYZE 7.5 format, the format that is used by BET from FMRIB,
 * University of Oxford, UK.
 *
 *  Created on: Nov 3, 2010
 *      Author: kevin
 */

#include <iostream>

#include "CProcessInfo.hpp"
#include "CMIFBet.hpp"

int main(int argc, char** argv)
{
  // Get process info for CMIF.
  MSMRI::CProcessInfo::getInstance().getCommandLine(argc, argv);

  // Always print version in the beginning.
  CMIFBet::printVersion(std::cout);

  // Initialize a MIFBet object, which does brain extraction.
  try
  {
    CMIFBet mifbet(argc, argv);
  }
  catch (std::exception& e)
  {
    CMIFBet::printErrorExit(e.what(), 1);
  }

  // DONE
  return 0;
}
@


1.1
log
@Initial revision
@
text
@@


1.1.1.1
log
@Initial import of the production MIFBet project.  MIFBet calls the Brain
Extraction Tool.  MIFBet is a Debian 5.0 C++ project, using CMIF v5.3.
@
text
@@
