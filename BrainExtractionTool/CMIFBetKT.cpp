/*
 * CMIFBet.cpp
 *
 *  Created on: Nov 5, 2010
 *      Author: kevin
 */

#include <errno.h>   // errno definition
#include <unistd.h>  // getcwd and chdir functions

#include <climits>   // PATH_MAX constant for this system
#include <cstring>   // includes errno string
#include <limits>
#include <fstream>

#include "CProcessInfo.hpp"
#include "util_std.hpp"
#include "CMIFName.hpp"
#include "cparseopt.hpp"
#include "cimage.hpp"
#include "CSlice.hpp"
#include "CChannel.hpp"
#include "CMIF.hpp"
using namespace MSMRI;

#include "ConfigFile.h"

#include "CMIFBet.hpp"
#include "dbh.h"

CMIFBet::CMIFBet(int argc, char **argv)
{
  parseCommand(argc, argv);
  loadConfigFile();

  try
  {
    cout << "Processing " << getInputFile() << ":" << endl << endl;

    {
      // Create temporary directory.  Replace tempDir with actual working directory,
      // So that runBET knows where it is.
      CTempDir tempDir(getTempDir());
      setTempDir(tempDir.getPath());

      processMIF();
    }

    cout << "OK." << endl << endl;
  }
  catch (exception& e)
  {
    printErrorExit(e.what(), 1);
  }

  cout << "MIFBet completed." << endl;
}

void CMIFBet::parseCommand(int argc, char **argv)
{
  // Set up parser
  CParseOpt cmdParser;
  cmdParser.addBoolOption  ('h', "help",    "Print help message and exit",        false);
  cmdParser.addBoolOption  ('v', "version", "Print version information and exit", false);
  cmdParser.addStringOption('i', "inifile", "Specify configuration file, default to MIFBET_INI environment variable.", "");
  cmdParser.addStringOption('o', "outfile", "Specify output file name.", "");

  // First handle empty command-line.
  if (argc==1)
  {
    cmdParser.showUsage(std::cout, getUsageString());
    exit(0);
  }

  // Parse and handle errors.
  CParseOpt::ErrorCode parseError = cmdParser.parseCommandLine(argc, argv);

  switch(parseError)
  {
    case CParseOpt::OK:
    case CParseOpt::OK_OPTION_REPEATED:
      break;
    case CParseOpt::OPTION_NOT_RECOGNIZED:
      throw CEXCEPTION << cmdParser.getErrorOption() << " is an invalid program option.";
      break;
    case CParseOpt::ARGUMENT_FORMAT_ERROR:
      throw CEXCEPTION << cmdParser.getErrorOption()
                       << " option cannot take the argument "
                       << cmdParser.getErrorArgument() << ".";
      break;
    case CParseOpt::ARGUMENT_MISSING:
      throw CEXCEPTION << cmdParser.getErrorOption()
                       << " option must take an argument.";
      break;
    default:
      throw CEXCEPTION << "There is an error on the command-line.";
      break;
  }

  // Handle options given by user
  if(cmdParser.value("h")->isAssigned())
  {
    cmdParser.showUsage(std::cout, getUsageString());
    exit(0);
  }
  if(cmdParser.value("v")->isAssigned())
  {
    // Version info is always printed, so do nothing and exit.
    exit(0);
  }

  if(cmdParser.value("o")->isAssigned())
  {
    setOutputFile( cmdParser.value("o")->stringVal() );
  }
  else
  {
    throw CEXCEPTION << "Output file was not specified.";
  }

  if(cmdParser.value("i")->isAssigned())
  {
    setConfigFile( cmdParser.value("i")->stringVal() );
  }
  else
  {
    char* iniPath = getenv("MIFBET_INI");

    if(!iniPath)
      throw CEXCEPTION << "Configuration file was not specified.";

    setConfigFile(iniPath);
  }

  // input output files
  if(cmdParser.getCommandArguments().size() == 1)
  {
    setInputFile( cmdParser.getCommandArguments()[0] );
  }
  else
  {
    throw CEXCEPTION << "Input MIF must be given on the command-line.";
  }
}

void CMIFBet::loadConfigFile()
{
  // assert that this function is called after parseCommand
  assert(!getConfigFile().empty());

  try
  {
    ConfigFile config(getConfigFile());
    setBetPath( config.read<string>("BETPATH") );
    setBetOptions( config.read<string>("BETFLAGS") );
    setBetSuffix( config.read<string>("BETSUFFIX") );
    setTempDir( config.read<string>("TEMPDIR") );  // Adds final / to path.
  }
  catch (ConfigFile::file_not_found& e)
  {
    throw CEXCEPTION << "The config file \"" << e.filename << "\" is not found.";
  }
  catch (ConfigFile::key_not_found& e)
  {
    throw CEXCEPTION << "The config option \"" << e.key << "\" has not been defined.";
  }
}

void CMIFBet::processMIF() const
{
  // Make sure input will not get overwritten.
  string realInputFile  = Utilities::realPath(getInputFile());
  string realOutputFile = Utilities::realPath(getOutputFile());

  if( realOutputFile != "" )  // Exists
  {
    if( realOutputFile == realInputFile )
      throw CEXCEPTION << "Output MIF will overwrite input MIF.";
  }

  // Construct output file names
  string inMifName  = getFileName(getInputFile());
  string outMifName = getFileName(getOutputFile());

  MIF::CMIF inputMIF(getInputFile());
  if(inputMIF.getChannelCount() != 1)
  {
    throw CEXCEPTION << "Input MIF has more than one channels.";
  }

  // Write ANALYZE file.
  string inHdrPath = getTempDir() + inMifName + ".hdr";
  string inImgPath = getTempDir() + inMifName + ".img";
  CTempFile inHdrStream(inHdrPath, ios_base::out, CTempFile::DELETE);
  CTempFile inImgStream(inImgPath, ios_base::out, CTempFile::DELETE);

  mifToAnalyze75(inputMIF, inHdrStream, inImgStream);

  // run BET tool.
  runBET(inMifName, outMifName);

  // Read mask file in ANALYZE format
  // Must create mask stream objects AFTER runBET, so that the temp files exist.
  string outHdrPath = getTempDir() + outMifName + getBetSuffix() + ".hdr";
  string outImgPath = getTempDir() + outMifName + getBetSuffix() + ".img";
  CTempFile outHdrStream(outHdrPath, ios_base::in, CTempFile::DELETE); //unused
  CTempFile outImgStream(outImgPath, ios_base::in, CTempFile::DELETE);

  mifSetAnalyzeImg(inputMIF, outImgStream);

  // Writing out the MIF.  Overwriting existing file is allowed.
  // If there are any extra logging, they can be passed to this function as
  // a parameter. See CMIF header.
  inputMIF.writeToFile(getOutputFile(), true);

  // Clean up is not necessary, as the CTempDir and CTempFile classes handle
  // auto-deletion.
}

void CMIFBet::mifToAnalyze75(const MIF::CMIF& mif,
                             CTempFile& hdrStream,
                             CTempFile& imgStream) const
{

  // Write ANALYZE .hdr and .img files
  try
  {
    mifToAnalyzeHdr(mif, hdrStream);
  }
  catch (exception& e)
  {
    throw CEXCEPTION << "Could not write intermediate file \"" << hdrStream.getPath() << "\".";
  }

  try
  {
    mifToAnalyzeImg(mif, imgStream);
  }
  catch (exception& e)
  {
    throw CEXCEPTION << "Could not write intermediate file \"" << imgStream.getPath() << "\".";
  }
}

void CMIFBet::mifToAnalyzeHdr(const MIF::CMIF& mif,
                              CTempFile& hdrStream) const
{
  string name = getFileName(hdrStream.getPath());

  /*
   * The dsr structure consists only of fixed-size members.  The size may be
   * reliably calculated using sizeof() function.  Because of this, it is safe
   * to use memset() to initialize the entire structure to null bytes and only
   * set values of interest.  The entire header can also be written to file
   * using fstream::write().
   */
  dsr header;
  memset(&header, 0, sizeof(dsr));

  header.hk.sizeof_hdr = sizeof(dsr);
  header.hk.extents = 16384;
  header.hk.regular = 'r';

  header.dime.dim[0] = 4;
  header.dime.dim[1] = short(mif.getColumnCount(1));
  header.dime.dim[2] = short(mif.getRowCount(1));
  header.dime.dim[3] = short(mif.getSliceCount(1));
  header.dime.dim[4] = 1;
  header.dime.datatype = DT_SIGNED_INT;
  header.dime.bitpix = 32;

  // Convert from micrometer to meter
  header.dime.pixdim[1] = float(mif.getChannel(1).getPixelSizeX())/1000.0;
  header.dime.pixdim[2] = float(mif.getChannel(1).getPixelSizeY())/1000.0;
  header.dime.pixdim[3] = float(mif.getChannel(1).getSliceThickness())/1000.0;

  header.dime.glmax = numeric_limits<int>::max();
  header.dime.glmin = 0;

  header.hist.orient = 3;

  string description = string("Converted from ") + mif.getMIFName();
  strcpy(header.hist.descrip, description.substr(0, 80).c_str());
  strcpy(header.hk.db_name, name.substr(0,18).c_str());

  hdrStream.write((const char*)(&header), sizeof(dsr));
  hdrStream.close();
}

void CMIFBet::mifToAnalyzeImg(const MIF::CMIF& mif,
                              CTempFile& imgStream) const
{
  long pixelCount = mif.getColumnCount(1) * mif.getRowCount(1);

  for(int s = 1; s <= mif.getSliceCount(1); ++s)
  {
    // Convert MIF 16-bit data to ANALYZE 32-bit data through a CImage object.
    CImage<unsigned short> cimage16u(mif.getColumnCount(1), mif.getRowCount(1));
    cimage16u.set(mif.getRawData(1)[s][0], pixelCount);

    int array32s[pixelCount];
    cimage16u.toInt().getData(array32s, pixelCount);

    // See CMIF design doc for raw data access using pixel pointer table.
    imgStream.write((const char*) array32s, pixelCount * sizeof(int));
  }

  imgStream.close();
}

void CMIFBet::mifSetAnalyzeImg(MIF::CMIF& mif,
                               CTempFile& imgStream) const
{
  long pixelCount = mif.getColumnCount(1) * mif.getRowCount(1);

  try
  {
    for(int s = 1; s <= mif.getSliceCount(1); ++s)
    {
      int array32s[pixelCount];
      imgStream.read((char*) array32s, pixelCount * sizeof(int));

      // Convert ANALYZE 32-bit data to MIF 16-bit data through a CImage object.
      CImage<int> cimage32s(mif.getColumnCount(1), mif.getRowCount(1));
      cimage32s.set(array32s, pixelCount);

      unsigned short array16u[pixelCount];
      cimage32s.toUShort().getData(array16u, pixelCount);

      memcpy(mif.getRawData(1)[s][0], array16u,
             pixelCount * sizeof(unsigned short));
    }
  }
  catch (exception& e)
  {
    throw CEXCEPTION << "Could not read intermediate file \"" << imgStream.getPath() << "\".";
  }

  // Also set some MIF header to reflect changes here if needed.
  // Currently all relevant headers are set automatically when writing out.
}

void CMIFBet::runBET(const string analyzeInName, const string analyzeOutName) const
{
  string cwd = CProcessInfo::getInstance().getWorkingDirectory();

  if(chdir(getTempDir().c_str()) != 0)
  {
    int errsv = errno;
    throw CEXCEPTION << getTempDir() << ": " << strerror(errsv) << ".";
  }

  string sysCommand =
      getBetPath() + " " +
      analyzeInName + " " + analyzeOutName + " " +
      getBetOptions();

  if(system(sysCommand.c_str()) != 0)
  {
    throw CEXCEPTION << "Bet command \"" << sysCommand << "\" failed.";
  }

  if(chdir(cwd.c_str()) != 0)
  {
    int errsv = errno;
    throw CEXCEPTION << cwd << ": " << strerror(errsv) << ".";
  }

  cout << endl;
}

void CMIFBet::printErrorExit(const string & message, const int exitValue)
{
  cout << "MIFBet encountered an error:" << endl
       << "    " << message << endl
       << endl
       << "MIFBet aborted." << endl;
  exit(exitValue);
}

void CMIFBet::printVersion(std::ostream & os)
{
  os
  << "MIFBet v" << MIFBET_MAJ_VER << "." << MIFBET_MIN_VER << " "
  << CTime::fromGccFormat(__DATE__, __TIME__)
  << ", compiled on " << CProcessInfo::getInstance().getHost()
  << " running " << PLATFORM
  << " Linux using gcc " << __GNUC__ << "." << __GNUC_MINOR__ << "."
  << __GNUC_PATCHLEVEL__ << " and "
  << MIF::CMIF::getModuleVersion() << endl << endl;
}

void CMIFBet::setInputFile(const string fileName)
{
  m_inputFile = fileName;
}
string CMIFBet::getInputFile() const
{
  return m_inputFile;
}

void CMIFBet::setOutputFile(const string fileName)
{
  m_outputFile = fileName;
}
string CMIFBet::getOutputFile() const
{
  return m_outputFile;
}

void CMIFBet::setConfigFile(const string fileName)
{
  m_configFile = fileName;
}
string CMIFBet::getConfigFile() const
{
  return m_configFile;
}

void CMIFBet::setTempDir(const string dir)
{
  m_tempDir = dir;
  if(m_tempDir[m_tempDir.size()-1] != '/') m_tempDir += '/';
}
string CMIFBet::getTempDir() const
{
  return m_tempDir;
}

void CMIFBet::setBetPath(const string betPath)
{
  // Check existence of executable.
  struct stat filestat;
  std::string mifBETTRUE = "/ISILON" + betPath;
  printf(mifBETTRUE);
  int statReturn = stat(mifBETTRUE.c_str(), &filestat);
  if(statReturn == 0)
  {
    // Set bet path to absolute path, so that it can still be accessed after
    // changing directory.
    m_betPath = Utilities::realPath(betPath);
  }
  else
  {
    throw CEXCEPTION << "Bet executable \"" << betPath << "\" was not found.";
  }
}
string CMIFBet::getBetPath() const
{
  return m_betPath;
}

void CMIFBet::setBetOptions(const string optionString)
{
  m_betOptions = optionString;
}
string CMIFBet::getBetOptions() const
{
  return m_betOptions;
}

void CMIFBet::setBetSuffix(const string betSuffix)
{
  m_betSuffix = betSuffix;
}
string CMIFBet::getBetSuffix() const
{
  return m_betSuffix;
}

/*
 * Private functions
 */
string CMIFBet::getUsageString() const
{
  return
  "Extracts brain tissue.\n\nUsage: mifbet [-i CONFIG] INPUT -o OUTPUT";
}

/*
 * helper functions
 */