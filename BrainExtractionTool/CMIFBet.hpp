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
date	2011.03.11.20.05.11;	author kevin;	state Exp;
branches;
next	1.1;
commitid	G1L5yKod2j4U5tbv;

1.1
date	2010.11.26.20.24.45;	author kevin;	state Exp;
branches
	1.1.1.1;
next	;
commitid	zaS4054DuXpyzYXu;

1.1.1.1
date	2010.11.26.20.24.45;	author kevin;	state Exp;
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
 * CMIFBet.hpp
 *
 *  Created on: Nov 5, 2010
 *      Author: kevin
 */

#ifndef CMIFBET_HPP_
#define CMIFBET_HPP_

#include <string>
#include <vector>
#include <fstream>
using namespace std;

#include "CTempFile.hpp"

namespace MSMRI
{
namespace MIF
{
  class CMIF;
}
}

class CMIFBet
{
  public:

    /*
     * This constructor does everything!  All one needs to do is to call this
     * constructor with command-line parameters.
     *
     * Constructor takes program arguments, initializes, and then processes
     * input files.  Calls parseCommand().
     */
    CMIFBet(int argc, char** argv);

    /*
     * Default constructor. User may want to construct the object, parse
     * command-line, and process manually.  This is very useful for testing.
     */
    CMIFBet() {};

    /*
     * Initializes program, sets necessary operational parameters.
     */
    void parseCommand(int argc, char** argv);
    void loadConfigFile();

    /*
     * Main processing function.  Calls other functions below.
     */
    void processMIF() const;

    /*
     * This function exports MIF data as an ANALYZE 7.5 file pair.
     *
     * ANALYZE 7.5 consists of an .img file, which is a 2-byte pixel array,
     * and a .hdr file, which contains the header that describes the pixels.
     *
     * It uses two helper functions that handle writing the individual files.
     */
    void mifToAnalyze75(const MSMRI::MIF::CMIF& mif,
                        CTempFile& hdrStream, CTempFile& imgStream) const;
    void mifToAnalyzeHdr(const MSMRI::MIF::CMIF& mif,
                         CTempFile& hdrStream) const;
    void mifToAnalyzeImg(const MSMRI::MIF::CMIF& mif,
                         CTempFile& imgStream) const;

    /*
     * This function handles the actual brain extraction by calling the BET tool
     * through a system call.
     */
    void runBET(const string analyzeInName, const string analyzeOutName) const;

    /*
     * This function replaces slices of inputMIF with the output brain mask
     * saved in an ANALYZE file.
     */
    void mifSetAnalyzeImg(MSMRI::MIF::CMIF& mif,
                          CTempFile& imgStream) const;

    /*
     * This function cleans up temporary files
     */

    /*
     * Prints customized error message, and then exits.
     *
     * Static function allows main() to call this to get identical error
     * reporting behaviour, even before object construction, if the need arises.
     */
    static void printErrorExit(const string& errorMessage,
                               const int exitValue);

    /*
     * Prints version string to ostream.
     *
     * Static function allows main() to call this without an object.
     */
    static void printVersion(ostream& os);

    /*
     * Getters/Setters
     */
    void setInputFile(const string fileName);
    string getInputFile() const;

    void setOutputFile(const string fileName);
    string getOutputFile() const;

    void setConfigFile(const string fileName);
    string getConfigFile() const;

    void setTempDir(const string dir);
    string getTempDir() const;

    void setBetPath(const string betPath);
    string getBetPath() const;

    void setBetOptions(const string optionString);
    string getBetOptions() const;

    void setBetSuffix(const string betSuffix);
    string getBetSuffix() const;

  private:

    /*
     * Helper function that encapsulates formatting of the usage text.
     */
    string getUsageString() const;

    /*
     * Member variables.
     */
    string m_inputFile;
    string m_outputFile;

    string m_configFile;
    string m_tempDir;
    string m_betPath;
    string m_betOptions;
    string m_betSuffix;
};

#endif /* CMIFBET_HPP_ */
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
