// CMIFBet.cpp (Refactored)
// Created: Nov 5, 2010 by Kevin
// Updated: Oct 2023 by Kiarash Taheri

#include <errno.h>
#include <unistd.h>
#include <climits>
#include <cstring>
#include <limits>
#include <fstream>
#include <sys/stat.h>

#include "CProcessInfo.hpp"
#include "util_std.hpp"
#include "CMIFName.hpp"
#include "cparseopt.hpp"
#include "cimage.hpp"
#include "CSlice.hpp"
#include "CChannel.hpp"
#include "CMIF.hpp"
#include "ConfigFile.h"
#include "CMIFBet.hpp"
#include "dbh.h"

using namespace MSMRI;

CMIFBet::CMIFBet(int argc, char **argv) {
  parseCommand(argc, argv);
  loadConfigFile();

  try {
    std::cout << "Processing " << getInputFile() << ":\n\n";
    CTempDir tempDir(getTempDir());
    setTempDir(tempDir.getPath());
    processMIF();
    std::cout << "OK.\n\n";
  } catch (const std::exception &e) {
    printErrorExit(e.what(), 1);
  }

  std::cout << "MIFBet completed." << std::endl;
}

void CMIFBet::parseCommand(int argc, char **argv) {
  CParseOpt parser;
  parser.addBoolOption('h', "help", "Show help", false);
  parser.addBoolOption('v', "version", "Show version info", false);
  parser.addStringOption('i', "inifile", "Configuration file", "");
  parser.addStringOption('o', "outfile", "Output file name", "");

  if (argc == 1) {
    parser.showUsage(std::cout, getUsageString());
    exit(0);
  }

  auto result = parser.parseCommandLine(argc, argv);
  if (result != CParseOpt::OK && result != CParseOpt::OK_OPTION_REPEATED) {
    throw CEXCEPTION << "Command line parsing error.";
  }

  if (parser.value("h")->isAssigned()) {
    parser.showUsage(std::cout, getUsageString());
    exit(0);
  }

  if (parser.value("v")->isAssigned()) exit(0);
  if (!parser.value("o")->isAssigned()) throw CEXCEPTION << "Output file not specified.";

  setOutputFile(parser.value("o")->stringVal());
  setInputFile(parser.getCommandArguments().at(0));

  if (parser.value("i")->isAssigned()) {
    setConfigFile(parser.value("i")->stringVal());
  } else if (char *envPath = getenv("MIFBET_INI")) {
    setConfigFile(envPath);
  } else {
    throw CEXCEPTION << "Config file not specified and MIFBET_INI not set.";
  }
}

void CMIFBet::loadConfigFile() {
  try {
    ConfigFile config(getConfigFile());
    setBetPath(config.read<std::string>("BETPATH"));
    setBetOptions(config.read<std::string>("BETFLAGS"));
    setBetSuffix(config.read<std::string>("BETSUFFIX"));
    setTempDir(config.read<std::string>("TEMPDIR"));
  } catch (ConfigFile::file_not_found &e) {
    throw CEXCEPTION << "Config file not found: " << e.filename;
  } catch (ConfigFile::key_not_found &e) {
    throw CEXCEPTION << "Missing config key: " << e.key;
  }
}

void CMIFBet::processMIF() const {
  auto realInput = Utilities::realPath(getInputFile());
  auto realOutput = Utilities::realPath(getOutputFile());
  if (!realOutput.empty() && realInput == realOutput) {
    throw CEXCEPTION << "Output MIF would overwrite input.";
  }

  MIF::CMIF mif(getInputFile());
  if (mif.getChannelCount() != 1) {
    throw CEXCEPTION << "Only single-channel MIFs are supported.";
  }

  std::string baseName = getFileName(getInputFile());
  std::string outName = getFileName(getOutputFile());

  CTempFile hdr(getTempDir() + baseName + ".hdr", std::ios_base::out);
  CTempFile img(getTempDir() + baseName + ".img", std::ios_base::out);
  mifToAnalyze75(mif, hdr, img);
  runBET(baseName, outName);

  CTempFile outHdr(getTempDir() + outName + getBetSuffix() + ".hdr", std::ios_base::in);
  CTempFile outImg(getTempDir() + outName + getBetSuffix() + ".img", std::ios_base::in);
  mifSetAnalyzeImg(mif, outImg);
  mif.writeToFile(getOutputFile(), true);
}

void CMIFBet::mifToAnalyze75(const MIF::CMIF &mif, CTempFile &hdr, CTempFile &img) const {
  mifToAnalyzeHdr(mif, hdr);
  mifToAnalyzeImg(mif, img);
}

void CMIFBet::mifToAnalyzeHdr(const MIF::CMIF &mif, CTempFile &hdr) const {
  dsr header = {};
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
  header.dime.pixdim[1] = float(mif.getChannel(1).getPixelSizeX()) / 1000.0f;
  header.dime.pixdim[2] = float(mif.getChannel(1).getPixelSizeY()) / 1000.0f;
  header.dime.pixdim[3] = float(mif.getChannel(1).getSliceThickness()) / 1000.0f;
  header.dime.glmax = std::numeric_limits<int>::max();
  header.dime.glmin = 0;
  header.hist.orient = 3;

  std::string description = "Converted from " + mif.getMIFName();
  strncpy(header.hist.descrip, description.c_str(), sizeof(header.hist.descrip));
  strncpy(header.hk.db_name, getFileName(hdr.getPath()).c_str(), sizeof(header.hk.db_name));

  hdr.write(reinterpret_cast<const char *>(&header), sizeof(dsr));
  hdr.close();
}

void CMIFBet::mifToAnalyzeImg(const MIF::CMIF &mif, CTempFile &img) const {
  long pixelCount = mif.getColumnCount(1) * mif.getRowCount(1);
  for (int s = 1; s <= mif.getSliceCount(1); ++s) {
    CImage<unsigned short> img16(mif.getColumnCount(1), mif.getRowCount(1));
    img16.set(mif.getRawData(1)[s][0], pixelCount);
    int img32[pixelCount];
    img16.toInt().getData(img32, pixelCount);
    img.write(reinterpret_cast<const char *>(img32), sizeof(int) * pixelCount);
  }
  img.close();
}

void CMIFBet::mifSetAnalyzeImg(MIF::CMIF &mif, CTempFile &img) const {
  long pixelCount = mif.getColumnCount(1) * mif.getRowCount(1);
  for (int s = 1; s <= mif.getSliceCount(1); ++s) {
    int buffer32[pixelCount];
    img.read(reinterpret_cast<char *>(buffer32), sizeof(int) * pixelCount);
    CImage<int> img32(mif.getColumnCount(1), mif.getRowCount(1));
    img32.set(buffer32, pixelCount);
    unsigned short buffer16[pixelCount];
    img32.toUShort().getData(buffer16, pixelCount);
    memcpy(mif.getRawData(1)[s][0], buffer16, sizeof(unsigned short) * pixelCount);
  }
}

void CMIFBet::runBET(const std::string &input, const std::string &output) const {
  std::string originalDir = CProcessInfo::getInstance().getWorkingDirectory();
  if (chdir(getTempDir().c_str()) != 0) {
    throw CEXCEPTION << "Cannot enter temp dir: " << strerror(errno);
  }

  std::string command = getBetPath() + " " + input + " " + output + " " + getBetOptions();
  if (system(command.c_str()) != 0) {
    throw CEXCEPTION << "BET failed: " << command;
  }

  if (chdir(originalDir.c_str()) != 0) {
    throw CEXCEPTION << "Cannot return to working dir: " << strerror(errno);
  }
  std::cout << std::endl;
}

std::string CMIFBet::getUsageString() const {
  return "Extracts brain tissue.\n\nUsage: mifbet [-i CONFIG] INPUT -o OUTPUT";
}

void CMIFBet::printErrorExit(const std::string &msg, int code) {
  std::cout << "MIFBet encountered an error:\n    " << msg << "\n\nMIFBet aborted." << std::endl;
  exit(code);
}
