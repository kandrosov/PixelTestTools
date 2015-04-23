/*!
 * \file fix_bare_chip_id.cpp
 * \brief Fix chip id in a ROOT file produced by bare module test.
 * \author Konstantin Androsov (University of Siena, INFN Pisa)
 *
 * Copyright 2015 Konstantin Androsov <konstantin.androsov@gmail.com>
 *
 * This file is part of PixelTestTools.
 *
 * PixelTestTools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * PixelTestTools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PixelTestTools.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <TROOT.h>
#include <TFile.h>
#include <Compression.h>
#include <TClass.h>
#include <TKey.h>
#include <TNamed.h>
#include <TCanvas.h>
#include <TPaletteAxis.h>
#include <TError.h>

namespace {

struct Config {
    std::string inputFileName, outputFileName;
    size_t oldChipId, newChipId;
};

bool ParseProgramArguments(int argc, char* argv[], Config& config)
{
    static const std::string optHelp = "help";
    static const std::string optInputFile = "input";
    static const std::string optOutputFile = "output";
    static const std::string optOldChipId = "old-chip-id";
    static const std::string optNewChipId = "new-chip-id";

    using namespace boost::program_options;

    options_description desc("Available command line arguments");
    desc.add_options()
            (optHelp.c_str(), "print help message")
            (optInputFile.c_str(), value<std::string>(&config.inputFileName)->required(),
             "input ROOT file with bare module test results")
            (optOutputFile.c_str(), value<std::string>(&config.outputFileName)->required(),
             "output ROOT file where to store test results with fixed chip id")
            (optOldChipId.c_str(), value<size_t>(&config.oldChipId)->default_value(0),
             "chip id stored in the original test file")
            (optNewChipId.c_str(), value<size_t>(&config.newChipId)->required(),
             "chip id that will be stored in the output file");

    variables_map variables;

    try {
        store(parse_command_line(argc, argv, desc), variables);
        if(variables.count(optHelp)) {
            std::cout << desc << std::endl;
            return false;
        }
        notify(variables);
    }
    catch(error& e) {
        std::cerr << "ERROR: " << e.what() << ".\n\n" << desc << std::endl;
        return false;
    }
    return true;
}

std::shared_ptr<TFile> CreateRootFile(const std::string& file_name)
{
    std::shared_ptr<TFile> file(TFile::Open(file_name.c_str(), "RECREATE", "", ROOT::kZLIB * 100 + 9));
    if(!file || file->IsZombie()) {
        std::ostringstream ss;
        ss << "File '" << file_name << "' not created.";
        throw std::runtime_error(ss.str());
    }
    return file;
}

std::shared_ptr<TFile> OpenRootFile(const std::string& file_name)
{
    std::shared_ptr<TFile> file(TFile::Open(file_name.c_str(), "READ"));
    if(!file || file->IsZombie()) {
        std::ostringstream ss;
        ss << "File '" << file_name << "' not opened.";
        throw std::runtime_error(ss.str());
    }
    return file;
}

class Program {
public:
    Program(const Config& _config)
        : config(_config), inputFile(OpenRootFile(_config.inputFileName)),
          outputFile(CreateRootFile(_config.outputFileName)) {}

    void Run()
    {
        std::unique_ptr<TCanvas> dummy_canvas(new TCanvas()); // workaround to avoid problems with a dictionary for
        /*gErrorIgnoreLevel = kError;*/                       // TPaletteAxis class.

        CopyDirectory(inputFile.get(), outputFile.get());
    }

private:
    void CopyDirectory(TDirectory *source, TDirectory *destination) const
    {
        TIter nextkey(source->GetListOfKeys());
        for(TKey* key; (key = dynamic_cast<TKey*>(nextkey()));) {
            const std::string class_name = key->GetClassName();
            TClass *cl = gROOT->GetClass(class_name.c_str());
            if (!cl) continue;
            if (cl->InheritsFrom("TDirectory")) {
                TDirectory *subdir_source = static_cast<TDirectory*>(source->Get(key->GetName()));
                TDirectory *subdir_destination;
                subdir_destination = destination->mkdir(subdir_source->GetName());

                CopyDirectory(subdir_source, subdir_destination);
            } else {
                std::unique_ptr<TObject> original_obj(key->ReadObj());
                std::unique_ptr<TObject> obj(original_obj->Clone());
                std::string name = key->GetName();
                if(cl->InheritsFrom("TNamed"))
                    name = FixNamedObject(dynamic_cast<TNamed*>(obj.get()));
                destination->WriteTObject(obj.get(), name.c_str(), "WriteDelete");
            }
        }
        destination->SaveSelf(kTRUE);
    }

    std::string FixNamedObject(TNamed* obj) const
    {
        if(!obj)
            throw std::runtime_error("Named object is null");
        const std::string new_name = ReplaceChipId(obj->GetName());
        const std::string new_title = ReplaceChipId(obj->GetTitle());
        obj->SetNameTitle(new_name.c_str(), new_title.c_str());
        return new_name;
    }

    std::string ReplaceChipId(const std::string& name) const
    {
        const std::string old_chip_name = GetChipName(config.oldChipId);
        const std::string new_chip_name = GetChipName(config.newChipId);
        std::string new_name = name;
        boost::replace_all(new_name, old_chip_name, new_chip_name);
        return new_name;
    }

    static std::string GetChipName(size_t chip_id)
    {
        std::ostringstream ss;
        ss << "C" << chip_id;
        return ss.str();
    }

private:
    Config config;
    std::shared_ptr<TFile> inputFile, outputFile;
};

const int NORMAL_EXIT_CODE = 0;
const int ERROR_EXIT_CODE = 1;
const int PRINT_ARGS_EXIT_CODE = 2;

} // anonymous namespace


int main(int argc, char* argv[])
{
    try {
        Config config;
        if(!ParseProgramArguments(argc, argv, config))
            return PRINT_ARGS_EXIT_CODE;
        Program program(config);
        program.Run();
    } catch(std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return ERROR_EXIT_CODE;
    }
    return NORMAL_EXIT_CODE;
}
